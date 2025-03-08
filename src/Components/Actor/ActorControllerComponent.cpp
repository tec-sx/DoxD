#include <StdAfx.h>

#include "ActorControllerComponent.h"
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include "Components/Player/PlayerComponent.h"
#include "Components/Player/Camera/ICameraComponent.h"
#include "Actor/StateMachine/ActorStateEvents.h"

namespace DoxD
{
	DEFINE_STATE_MACHINE(CActorControllerComponent, Movement);

	static void RegisterActorControllerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorControllerComponent));
		}
	}

	void CActorControllerComponent::Initialize()
	{
		// Mesh and animation.
		//m_pActorAnimationComponent = m_pEntity->GetOrCreateComponent<CActorAnimationComponent>();

		// Character movement controller.
		m_pCharacterControllerComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		m_pActorComponent = m_pEntity->GetOrCreateComponent<CActorComponent>();
		CRY_ASSERT_MESSAGE(m_pActorComponent, "The actor controller component must be paired with an actor component.");

		StateMachineInitMovement();
	}

	void CActorControllerComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::LevelStarted:
		case EEntityEvent::Reset:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;

		case EEntityEvent::Update:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
			Update(pCtx);
		}
		break;

		case EEntityEvent::PrePhysicsUpdate:
			PrePhysicsUpdate();
			break;
		}
	}

	void CActorControllerComponent::OnResetState()
	{
		m_lookTarget.zero();
		m_aimTarget.zero();
		m_useLookTarget = false;
		m_useLookIK = true;
		m_useAimIK = true;

		StateMachineResetMovement();
		StateMachineHandleEventMovement(eActorEvent_Entry);
	}

	void CActorControllerComponent::Update(SEntityUpdateContext* pCtx)
	{
		const float frameTime = pCtx->fFrameTime;

		UpdateMovementRequest(frameTime);
		UpdateLookDirectionRequest(frameTime);
		UpdateAnimation(frameTime);

		const bool shouldDebug = false;

		StateMachineUpdateMovement(pCtx->fFrameTime, shouldDebug);
		StateMachineHandleEventMovement(SStateEventUpdate(pCtx->fFrameTime));
	}

	void CActorControllerComponent::PrePhysicsUpdate()
	{
		// TODO: HACK: BROKEN: This stuff was commented out in the character pre-physics. Some of it might belong here now.

		//	// The routine will need to be rewritten to work with actors only, or we need a new one that does the actor, that
		//	// is called by a character version of this.
		//	CActorStateUtility::UpdatePhysicsState(*this, m_actorPhysics, frameTime);
		
		//	//#ifdef STATE_DEBUG
		//	//		if (g_pGameCVars->pl_watchPlayerState >= (bIsClient ? 1 : 2))
		//	//		{
		//	//			// NOTE: outputting this info here is 'was happened last frame' not 'what was decided this frame' as it occurs before the prePhysicsEvent is dispatched
		//	//			// also IsOnGround and IsInAir can possibly both be false e.g. - if you're swimming
		//	//			// May be able to remove this log now the new HSM debugging is in if it offers the same/improved functionality
		//	//			CryWatch("%s stance=%s flyMode=%d %s %s%s%s%s", GetEntity()->GetEntityTextDescription(), GetStanceName(GetStance()), m_actorState.flyMode, IsOnGround() ? "ON-GROUND" : "IN-AIR", IsViewFirstPerson() ? "FIRST-PERSON" : "THIRD-PERSON", IsDead() ? "DEAD" : "ALIVE", m_actorState.isScoped ? " SCOPED" : "", m_actorState.isInBlendRagdoll ? " FALLNPLAY" : "");
		//	//		}
		//	//#endif
		
		//	// Push the pre-physics event down to our state machine.
		//	const SActorPrePhysicsData prePhysicsData(frameTime);
		//	const SStateEventActorMovementPrePhysics prePhysicsEvent(&prePhysicsData);
		//	StateMachineHandleEventMovement(STATE_DEBUG_APPEND_EVENT(prePhysicsEvent));
		
		//	// Bring the animation state of the character into line with it's requested state.
		//	UpdateAnimationState();
	}

	void CActorControllerComponent::UpdateMovementRequest(float frameTime)
	{
		m_movementRequest = ZERO;

		if (!m_pCharacterControllerComponent->IsOnGround())
		{
			m_movingDuration = 0.0f;
			return;
		}

		if (auto* pPlayer = m_pActorComponent->GetPlayer())
		{
			/**
			The request needs to take into account both the direction of the camera and the direction of
			the movement i.e. left is alway left relative to the camera. TODO: What will it take to make this
			use AddVelocity instead? The values don't seem to match with what I'd expect to input to it.
			**/
			auto* pPlayerInput = pPlayer->GetPlayerInput();
			float moveSpeed = GetMovementBaseSpeed(pPlayerInput->GetMovementDirectionFlags());
			if ((moveSpeed > FLT_EPSILON) && (pPlayerInput->IsMovementRequested()))
			{
				m_movingDuration += frameTime;
				m_movementRequest = pPlayerInput->GetMovementDirection(pPlayer->GetCamera()->GetRotation()) * moveSpeed;
				SetVelocity(m_movementRequest);
			}
			else
			{
				// I'm forcing the velocity to zero if we're not actively controlling the character. This naive
				// approach is probably wrong, but it works for now. 
				SetVelocity(ZERO);
			}
		}
	}

	void CActorControllerComponent::UpdateLookDirectionRequest(float frameTime)
	{
		const float angularVelocityMax = g_PI2 * 1.5f;
		const float catchupSpeed = g_PI2 * 1.2f;

		if (auto* pPlayer = m_pActorComponent->GetPlayer())
		{
			auto* pPlayerInput = pPlayer->GetPlayerInput();

			// Only allow the character to rotate if they are moving.
			if (m_movementRequest.len() > FLT_EPSILON)
			{
				 Ang3 facingDir = CCamera::CreateAnglesYPR(m_movementRequest.GetNormalizedFast());

				// Use their last orientation as their present direction.
				// NOTE: I tried it with GetEntity()->GetWorldTM() but that caused crazy jitter issues.
				Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

				// We add in some extra rotation to 'catch up' to the direction they are being moved. This will perform a gradual
				// turn on the actor over several frames.
				float rotationDelta{ 0.0f };
				if (std::abs(facingDir.x - ypr.x) > g_PI)
					rotationDelta = ypr.x - facingDir.x;
				else
					rotationDelta = facingDir.x - ypr.x;

				// Catchup allows us to step towards the goal direction in even steps using a set angular velocity.
				float catchUp{ 0.0f };
				if (std::abs(rotationDelta) > FLT_EPSILON)
				{
					if (rotationDelta > 0.0f)
						catchUp = std::min(rotationDelta, catchupSpeed * frameTime);
					else
						catchUp = std::max(rotationDelta, -catchupSpeed * frameTime);
				}

				// Update angular velocity metrics.
				m_yawAngularVelocity = CLAMP(pPlayerInput->GetMouseYawDelta() + catchUp, -angularVelocityMax * frameTime, angularVelocityMax * frameTime);

				// Yaw.
				ypr.x += m_yawAngularVelocity;

				// Roll (zero it).
				ypr.z = 0;

				// Update the preferred direction we face.
				m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
			}
		}
	}

	void CActorControllerComponent::UpdateAnimation(float frameTime)
	{
		if (auto* pPlayer = m_pActorComponent->GetPlayer())
		{
			// Radians / sec
			const float angularVelocityMin = 0.174f;

			// Update tags and motion parameters used for turning
			const bool isTurning = std::abs(m_yawAngularVelocity) > angularVelocityMin;

			// Resolve the animation tags.
			// HACK: This should be done once on init or on entity changed events or similar. It fails hard if the init order
			// is switched with CActorAnimationComponent.
			if ((m_rotateTagId == TAG_ID_INVALID) && (strlen(m_pActorAnimationComponent->GetControllerDefinitionFile()) > 0))
				m_rotateTagId = m_pActorAnimationComponent->GetTagId("Rotate");

			// Set the tag, if it exists.
			if (m_rotateTagId != TAG_ID_INVALID)
				m_pActorAnimationComponent->SetTagWithId(m_rotateTagId, isTurning);

			if (isTurning)
			{
				// Expect the turning motion to take approximately one second.
				// TODO: Get to work on making this happen more like Blade and Soul.
				const float turnDuration = 1.0f;
				m_pActorAnimationComponent->SetMotionParameter(eMotionParamID_TurnAngle, m_yawAngularVelocity * turnDuration);
			}

			// Update entity rotation as the player turns. We only want to affect Z-axis rotation, zero pitch and roll.
			// TODO: is there a case where we want to avoid zeroing out pitch and roll? for example swimming or flying?
			Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
			ypr.y = 0;
			ypr.z = 0;
			const Quat correctedOrientation = Quat(CCamera::CreateOrientationYPR(ypr));

			// Send updated transform to the entity, only orientation changes.
			GetEntity()->SetPosRotScale(GetEntity()->GetWorldPos(), correctedOrientation, Vec3(1, 1, 1));
		}
	}

	float CActorControllerComponent::GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const
	{
		const static float walkBaseSpeed{ 2.1f };
		const static float jogBaseSpeed{ 4.2f };
		const static float runBaseSpeed{ 6.3f };
		const static float crawlBaseSpeed{ 1.2f };
		const static float proneBaseSpeed{ 0.4f };
		const static float crouchBaseSpeed{ 1.2f };
		float baseSpeed{ 0.0f };
		float dirScale{ 1.0f };

		switch (GetStance())
		{
		case EActorStance::standing:
			// Work out a base for walking, jogging or sprinting.
			if (IsSprinting())
			{
				baseSpeed = runBaseSpeed;
			}
			else
			{
				if (IsJogging())
					baseSpeed = jogBaseSpeed;
				else
					baseSpeed = walkBaseSpeed;
			}
			break;

		case EActorStance::crawling:
			baseSpeed = crawlBaseSpeed;
			break;

		case EActorStance::crouching:
			baseSpeed = crouchBaseSpeed;
			break;

		case EActorStance::swimming:
			baseSpeed = walkBaseSpeed;
			break;

		default:
			// Don't let them control movement.
			baseSpeed = 0.0f;
			break;
		}

		switch (movementDirectionFlags)
		{
		case (TInputFlags)EInputFlag::Forward:
			dirScale = 1.0f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.9f;
			break;

		case (TInputFlags)EInputFlag::Right:
		case (TInputFlags)EInputFlag::Left:
			dirScale = 0.85f;
			break;

		case (TInputFlags)EInputFlag::Backward:
		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.71f;
			break;

		default:
			dirScale = 0.0f;
			break;
		}

		return baseSpeed * dirScale;
	}

	// TODO: So wrong in every way. Need some more state checking before deciding which stance to move to when exiting
	// crouching, though this is a pretty decent default. Needs checks to ensure you can't crouch while swimming / falling /
	// etc - maybe put this into the class itself? 
	void CActorControllerComponent::OnActionCrouchToggle()
	{
		if (GetStance() == EActorStance::crouching)
			SetStance(EActorStance::standing);
		else
			SetStance(EActorStance::crouching);
	}

	void CActorControllerComponent::OnActionCrawlToggle()
	{
		if (GetStance() == EActorStance::crawling)
			SetStance(EActorStance::standing);
		else
			SetStance(EActorStance::crawling);
	}

	void CActorControllerComponent::OnActionSitToggle()
	{
		if (GetStance() == EActorStance::sittingFloor)
			SetStance(EActorStance::standing);
		else
			SetStance(EActorStance::sittingFloor);
	}

	const bool CActorControllerComponent::IsPlayer() const
	{
		return m_pActorComponent->IsPlayer();
	}

	const Vec3 CActorControllerComponent::GetLocalEyePos() const
	{
		return m_pActorComponent->GetLocalEyePos();
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorControllerComponent);
}