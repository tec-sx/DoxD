#include "StdAfx.h"

#include "ActorComponent.h"
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <ICryMannequin.h>
#include <IGameObject.h>
#include <Actor/Animation/ActorAnimation.h>
#include <Actor/Animation/Actions/ActorAnimationActionLocomotion.h>
#include <Actor/StateMachine/ActorStateEvents.h>
#include <Components/Actor/ActorControllerComponent.h>
#include <Components/Snaplocks/SnaplockComponent.h>
#include <Components/Player/PlayerComponent.h>
#include <Components/Player/Camera/ICameraComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>

namespace DoxD
{
	static void RegisterActorComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorComponent));
		}
	}

	void CActorComponent::Initialize()
	{
		m_pAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();
		m_pCharacterControllerComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		m_pActorControllerComponent = m_pEntity->GetOrCreateComponent<CActorControllerComponent>();

		/*m_pInventoryComponent = m_pEntity->GetOrCreateComponent<CInventoryComponent>();
		m_pEquipmentComponent = m_pEntity->GetOrCreateComponent<CEquipmentComponent>();*/

		m_pDrsComponent = crycomponent_cast<IEntityDynamicResponseComponent*> (m_pEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));
		m_pSnaplockComponent = m_pEntity->GetOrCreateComponent<CSnaplockComponent>();
		m_pAwarenessComponent = m_pEntity->GetOrCreateComponent<CEntityAwarenessComponent>();

		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_HEAD, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_FACE, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_NECK, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_SHOULDERS, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_CHEST, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_BACK, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_LEFTARM, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_RIGHTARM, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_LEFTHAND, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_RIGHTHAND, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_WAIST, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_LEFTLEG, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_RIGHTLEG, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_LEFTFOOT, false));
		m_pSnaplockComponent->AddSnaplock(ISnaplock(SLOT_ACTOR_RIGHTFOOT, false));

		// Default is for a character to be controlled by AI.
		// m_isAIControlled = true;
		// m_isAIControlled = false;

		// Tells this instance to trigger areas.
		m_pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

		// Are we the local player?
		if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
		{
			// Tells this instance to trigger areas and that it's the local player.
			// BUG: HACK: We may not be the local player! That flag likely needs toggling as we enter and exit control of the actor.
			m_pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);
			CryLogAlways("CActorComponent::HandleEvent(): Entity \"%s\" became the local character!", m_pEntity->GetName());
		}

		// Reset the entity.
		OnResetState();
	}

	void CActorComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::Initialize:
			break;

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
			break;
		}
		}
	}

	void CActorComponent::Update(SEntityUpdateContext* pCtx)
	{
		const float frameTime = pCtx->fFrameTime;

		if (m_pPlayer && m_pPlayer->IsLocalPlayer())
		{
			m_pAudioListenerComponent->SetOffset(m_pPlayer->GetCamera()->GetPosition() - GetEntity()->GetWorldPos());
		}

		// HACK: This belongs in pre-physics...I think.
		SetIK();
	}

	void CActorComponent::OnInputEscape(int activationMode)
	{
		CryLogAlways("CActorComponent::OnInputEscape");
	}


	ICharacterInstance* CActorComponent::GetCharacter() const
	{
		CRY_ASSERT(m_pAnimationComponent);

		return m_pAnimationComponent->GetCharacter();
	}

	TagID CActorComponent::GetStanceTagId(EActorStance actorStance) const
	{
		TagID tagId{ TAG_ID_INVALID };

		switch (actorStance)
		{
		case EActorStance::crawling:
			tagId = m_actorMannequinParams->tagIDs.Crawling;
			break;

		case EActorStance::prone:
			tagId = m_actorMannequinParams->tagIDs.Prone;
			break;

		case EActorStance::crouching:
			tagId = m_actorMannequinParams->tagIDs.Crouching;
			break;

		case EActorStance::falling:
			tagId = m_actorMannequinParams->tagIDs.Falling;
			break;

		case EActorStance::flying:
			tagId = m_actorMannequinParams->tagIDs.Flying;
			break;

		case EActorStance::landing:
			tagId = m_actorMannequinParams->tagIDs.Landing;
			break;

		case EActorStance::sittingChair:
			tagId = m_actorMannequinParams->tagIDs.SittingChair;
			break;

		case EActorStance::sittingFloor:
			tagId = m_actorMannequinParams->tagIDs.SittingFloor;
			break;

		case EActorStance::sleeping:
			tagId = m_actorMannequinParams->tagIDs.Sleeping;
			break;

		case EActorStance::spellcasting:
			tagId = m_actorMannequinParams->tagIDs.Spellcasting;
			break;

		case EActorStance::standing:
			tagId = m_actorMannequinParams->tagIDs.Standing;
			break;

		case EActorStance::swimming:
			tagId = m_actorMannequinParams->tagIDs.Swimming;
			break;
		}

		return tagId;
	}

	TagID CActorComponent::GetPostureTagId(EActorPosture actorPosture) const
	{
		TagID tagId{ TAG_ID_INVALID };

		switch (actorPosture)
		{
		case EActorPosture::aggressive:
			tagId = m_actorMannequinParams->tagIDs.Aggressive;
			break;

		case EActorPosture::alerted:
			tagId = m_actorMannequinParams->tagIDs.Alerted;
			break;

		case EActorPosture::bored:
			tagId = m_actorMannequinParams->tagIDs.Bored;
			break;

		case EActorPosture::dazed:
			tagId = m_actorMannequinParams->tagIDs.Dazed;
			break;

		case EActorPosture::depressed:
			tagId = m_actorMannequinParams->tagIDs.Depressed;
			break;

		case EActorPosture::distracted:
			tagId = m_actorMannequinParams->tagIDs.Distracted;
			break;

		case EActorPosture::excited:
			tagId = m_actorMannequinParams->tagIDs.Excited;
			break;

		case EActorPosture::interested:
			tagId = m_actorMannequinParams->tagIDs.Interested;
			break;

		case EActorPosture::neutral:
			tagId = m_actorMannequinParams->tagIDs.Neutral;
			break;

		case EActorPosture::passive:
			tagId = m_actorMannequinParams->tagIDs.Passive;
			break;

		case EActorPosture::suspicious:
			tagId = m_actorMannequinParams->tagIDs.Suspicious;
			break;

		case EActorPosture::unaware:
			tagId = m_actorMannequinParams->tagIDs.Unaware;
			break;
		}

		return tagId;
	}

	void CActorComponent::OnPlayerAttach(CPlayerComponent& player)
	{
		m_pPlayer = &player;
		m_pPlayer->GetPlayerInput()->AddEventListener(this);
		m_pAudioListenerComponent = m_pEntity->GetOrCreateComponent<Cry::Audio::DefaultComponents::CListenerComponent>();

		// Default assumption is we now control the character.
		//m_isAIControlled = false;

		OnResetState();
	}

	void CActorComponent::OnPlayerDetach()
	{
		m_pPlayer->GetPlayerInput()->RemoveEventListener(this);
		m_pEntity->RemoveComponent<Cry::Audio::DefaultComponents::CListenerComponent>();

		// #TODO: Detach the camera.

		// #TODO: We can remove the entity awareness component if that's desirable.

		// #TODO: handle transitioning this character back into the loving hands of the AI.
		//m_isAIControlled = true;

		m_pPlayer = nullptr;
	}

	const bool CActorComponent::IsPlayer() const
	{
		if (m_pPlayer)
			return m_pPlayer->IsLocalPlayer();

		return false;
	}

	const Vec3 CActorComponent::GetLocalEyePos() const
	{
		Vec3 eyePosition{ 0.0f, 0.0f, 1.82f };

		auto pCharacter = m_pAnimationComponent->GetCharacter();
		if (!pCharacter)
			return eyePosition;

		const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			const auto eyeLeft = pAttachmentManager->GetIndexByName("c_eye_l");
			const auto eyeRight = pAttachmentManager->GetIndexByName("c_eye_r");
			Vec3 eyeLeftPosition;
			Vec3 eyeRightPosition;
			int eyeFlags = 0;

			// Is there a left eye?
			const IAttachment* pEyeLeftAttachment = pAttachmentManager->GetInterfaceByIndex(eyeLeft);
			if (pEyeLeftAttachment)
			{
				eyeLeftPosition = GetEntity()->GetRotation() * pEyeLeftAttachment->GetAttModelRelative().t;
				eyeFlags |= 0x01;
			}

			// Is there a right eye?
			const IAttachment* pEyeRightAttachment = pAttachmentManager->GetInterfaceByIndex(eyeRight);
			if (pEyeRightAttachment)
			{
				eyeRightPosition = GetEntity()->GetRotation() * pEyeRightAttachment->GetAttModelRelative().t;
				eyeFlags |= 0x02;
			}

			static bool alreadyWarned{ false };
			switch (eyeFlags)
			{
			case 0:
				// Failure, didn't find any eyes.
				// This will most likely spam the log. Disable it if it's annoying.
				if (!alreadyWarned)
				{
					CryLogAlways("Character does not have 'c_eye_l' or 'c_eye_r' defined.");
					alreadyWarned = true;
				}
				break;

			case 1:
				// Left eye only.
				eyePosition = eyeLeftPosition;
				break;

			case 2:
				// Right eye only.
				eyePosition = eyeRightPosition;
				break;

			case 3:
				// Both eyes, position between the two points.
				eyePosition = (eyeLeftPosition + eyeRightPosition) / 2.0f;
				break;
			}
		}

		return eyePosition;
	}

	Vec3 CActorComponent::GetLocalLeftHandPos() const
	{
		const Vec3 handPosition{ -0.2f, 0.3f, 1.3f };
		auto pCharacter = m_pAnimationComponent->GetCharacter();

		if (pCharacter)
		{
			const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
			if (pAttachmentManager)
			{
				const auto handBone = pAttachmentManager->GetIndexByName("hand_l");
				const IAttachment* pAttachment = pAttachmentManager->GetInterfaceByIndex(handBone);
				if (pAttachment)
				{
					return GetEntity()->GetRotation() * pAttachment->GetAttModelRelative().t;
				}
			}
		}

		return handPosition;
	}

	Vec3 CActorComponent::GetLocalRightHandPos() const
	{
		const Vec3 handPosition{ 0.2f, 0.3f, 1.3f };
		auto pCharacter = m_pAnimationComponent->GetCharacter();

		if (pCharacter)
		{
			const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
			if (pAttachmentManager)
			{
				const auto handBone = pAttachmentManager->GetIndexByName("hand_r");
				const IAttachment* pAttachment = pAttachmentManager->GetInterfaceByIndex(handBone);
				if (pAttachment)
				{
					return GetEntity()->GetRotation() * pAttachment->GetAttModelRelative().t;
				}
			}
		}

		return handPosition;
	}

	void CActorComponent::SetIK() const
	{
		// TEST: If the actor is looking at something, let's apply the IK.
		if (m_pAwarenessComponent && m_pAwarenessComponent->GetRayHit())
		{
			// TEST: Just look straight ahead.
			SetLookingIK(true, m_pAwarenessComponent->GetRayHitPosition());
		}
		else
		{
			// Don't allow look IK.
			SetLookingIK(false, Vec3{ ZERO });
		}
	}

	bool CActorComponent::SetLookingIK(const bool isLooking, const Vec3& lookTarget) const
	{
		//const bool shouldHandle = (m_pProceduralContextLook != nullptr);

		//if (shouldHandle)
		//{
		//	m_pProceduralContextLook->SetLookTarget(lookTarget);
		//	m_pProceduralContextLook->SetIsLookingGame(isLooking);
		//}

		//return shouldHandle;

		return false;
	}

	void CActorComponent::OnInputInteraction(int activationMode)
	{
	}

	void CActorComponent::OnActionItemUse()
	{
	}
	void CActorComponent::OnActionItemPickup()
	{
	}
	void CActorComponent::OnActionItemDrop()
	{
	}
	void CActorComponent::OnActionItemToss()
	{
	}
	void CActorComponent::OnActionInspectStart()
	{
	}
	void CActorComponent::OnActionInspecting()
	{
	}
	void CActorComponent::OnActionInspectEnd()
	{
	}
	void CActorComponent::OnActionInteractionStart()
	{
	}
	void CActorComponent::OnActionInteractionTick()
	{
	}
	void CActorComponent::OnActionInteractionEnd()
	{
	}
	void CActorComponent::OnActionCrouchToggle()
	{
	}
	void CActorComponent::OnActionCrawlToggle()
	{
	}
	void CActorComponent::OnActionSitToggle()
	{
	}

	void CActorComponent::OnActionJogToggle()
	{
	}

	void CActorComponent::OnActionSprintStart()
	{
	}

	void CActorComponent::OnActionSprintStop()
	{
	}

	bool CActorComponent::IsSprinting() const
	{
		return false;
	}

	bool CActorComponent::IsJogging() const
	{
		return false;
	}

	void CActorComponent::InteractionStart(IInteraction* pInteraction)
	{
	}

	void CActorComponent::InteractionTick(IInteraction* pInteraction)
	{
	}

	void CActorComponent::InteractionEnd(IInteraction* pInteraction)
	{
	}

	void CActorComponent::OnKill()
	{
	}

	void CActorComponent::OnDeath()
	{
	}

	void CActorComponent::OnRevive()
	{
		OnResetState();
		m_pActorControllerComponent->OnResetState();
	}

	void CActorComponent::OnJump()
	{
	}

	void CActorComponent::OnEnterVehicle(const char* vehicleClassName, const char* seatName, bool bThirdPerson)
	{
	}

	void CActorComponent::OnExitVehicle()
	{
	}

	void CActorComponent::OnHealthChanged(float newHealth)
	{
	}

	void CActorComponent::OnSanityChanged(float newSanity)
	{
	}

	void CActorComponent::OnItemPickedUp(EntityId itemId)
	{
	}

	void CActorComponent::OnItemUsed(EntityId itemId)
	{
	}

	void CActorComponent::OnItemDropped(EntityId itemId)
	{
	}

	void CActorComponent::OnSprintStaminaChanged(float newStamina)
	{
	}

	void CActorComponent::OnResetState()
	{
		// HACK: This prevents a weird crash when getting the context a second time.
		m_pProceduralContextLook = nullptr;

		//if (m_pActorAnimationComponent)
		//{
		//	m_pActorAnimationComponent->SetControllerDefinitionFile("Animations/Mannequin/ADB/FirstPersonControllerDefinition.xml");
		//	m_pActorAnimationComponent->SetDefaultScopeContextName("ThirdPersonCharacter");
		//	m_pActorAnimationComponent->SetDefaultFragmentName("Idle");
		//}

		if (m_pAnimationComponent && m_pAnimationComponent->GetActionController() != nullptr)
		{
			// The actor animation component is responsible for maintaining the context.
			const auto& pContext = m_pAnimationComponent->GetActionController()->GetContext();

			// The mannequin tags for an actor will need to be loaded. Because these are found in the controller definition,
			// they are potentially different for every actor. 
			m_actorMannequinParams = GetMannequinUserParams<SActorMannequinParams>(pContext);

			// It's a good idea to flush out any current actions, then resume processing.
			m_pAnimationComponent->GetActionController()->Flush();
			m_pAnimationComponent->GetActionController()->Resume();

			// Select a character definition based on first / third person mode. Hard coding the default scope isn't a great
			// idea, but it's good enough for now. 
			m_pAnimationComponent->SetDefaultScopeContextName("Char3P");
			//m_pActorAnimationComponent->SetCharacterFile(m_geometryThirdPerson.value);

			// Queue the locomotion action, which switches fragments and tags as needed for actor locomotion.
			auto locomotionAction = new CActorAnimationActionLocomotion();
			QueueAction(*locomotionAction);

			// Third person views allow a little extra control.

				// Aim actions.
				//if (CActorAnimationActionAimPose::IsSupported(pContext)
				//	&& CActorAnimationActionAiming::IsSupported(pContext))
				//{
				//	m_pProceduralContextAim = static_cast<CProceduralContextAim*>(m_pActorAnimationComponent->FindOrCreateProceduralContext(CProceduralContextAim::GetCID()));
				//	QueueAction(*new CActorAnimationActionAimPose());
				//	QueueAction(*new CActorAnimationActionAiming());
				//}

				//// Set the scope tag for look pose.
				//auto& animContext = m_pActorAnimationComponent->GetContext();
				//animContext.state.Set(m_actorMannequinParams->tagIDs.ScopeLookPose, true);

				//// Look actions.
				////if (CActorAnimationActionLookPose::IsSupported(pContext) // HACK: These tests are causing crashes on the second run through.
				//	//&& CActorAnimationActionLooking::IsSupported(pContext))
				//{
				//	const auto pX = m_pActorAnimationComponent->FindOrCreateProceduralContext(CProceduralContextLook::GetCID());
				//	m_pProceduralContextLook = static_cast<CProceduralContextLook*>(pX);

				//	QueueAction(*new CActorAnimationActionLookPose());
				//	QueueAction(*new CActorAnimationActionLooking());
				//}
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorComponent);
}
