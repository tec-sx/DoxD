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
		m_pActorAnimationComponent = m_pEntity->GetOrCreateComponent<CActorAnimationComponent>();
		m_pCharacterControllerComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		m_pActorControllerComponent = m_pEntity->GetOrCreateComponent<CActorControllerComponent>();

		/*m_pInventoryComponent = m_pEntity->GetOrCreateComponent<CInventoryComponent>();
		m_pEquipmentComponent = m_pEntity->GetOrCreateComponent<CEquipmentComponent>();*/

		m_pDrsComponent = crycomponent_cast<IEntityDynamicResponseComponent*> (m_pEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));
		m_pSnaplockComponent = m_pEntity->GetOrCreateComponent<CSnaplockComponent>();

		//m_pAwarenessComponent = m_pEntity->GetOrCreateComponent<CEntityAwarenessComponent>();

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
		CRY_ASSERT(m_pActorAnimationComponent);

		return m_pActorAnimationComponent->GetCharacter();
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

		auto pCharacter = m_pActorAnimationComponent->GetCharacter();
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
		auto pCharacter = m_pActorAnimationComponent->GetCharacter();

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
		auto pCharacter = m_pActorAnimationComponent->GetCharacter();

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

	IActionController* CActorComponent::GetActionController() const
	{
		return nullptr;
	}

	const SActorMannequinParams* CActorComponent::GetMannequinParams() const
	{
		return nullptr;
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

	void CActorComponent::QueueAction(IAction& pAction)
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
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorComponent);
}
