#include "StdAfx.h"

#include "ActorControllerComponent.h"
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include "Components/Player/PlayerComponent.h"

namespace
{
	static void RegisterActorControllerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CActorControllerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorControllerComponent);
}

namespace DoxD
{

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
	}

	void CActorControllerComponent::UpdateLookDirectionRequest(float frameTime)
	{
	}

	void CActorControllerComponent::UpdateAnimation(float frameTime)
	{
	}


	const bool CActorControllerComponent::IsPlayer() const
	{
		return m_pActorComponent->IsPlayer();
	}

	const Vec3 CActorControllerComponent::GetLocalEyePos() const
	{
		return m_pActorComponent->GetLocalEyePos();
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
}