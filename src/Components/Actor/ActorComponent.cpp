#include "StdAfx.h"

#include "ActorComponent.h"
#include "Actor/ActorDefinitions.h"
#include "Actor/Animation/ActorAnimation.h"
#include "Actor/Animation/Actions/ActorAnimationActionLocomotion.h"
#include "Actor/Animation/Actions/ActorAnimationActionLookPose.h"
#include "Actor/StateMachine/ActorStateEvents.h"
#include "Components/Snaplocks/SnaplockComponent.h"
#include "Components/Player/PlayerComponent.h"
#include "Components/Player/Camera/ICameraComponent.h"
#include "Components/Interaction/EntityInteractionComponent.h"
#include "Components/Interaction/EntityAwarenessComponent.h"
#include "Animation/ProceduralContext/ProceduralContextLook.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <ICryMannequin.h>
#include <IGameObject.h>

namespace DoxD
{
	static void RegisterActorComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorComponent));
		}
	}

	void CActorComponent::ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc)
	{
		desc.SetGUID(CActorComponent::IID());
		desc.SetEditorCategory("Actors");
		desc.SetLabel("Actor");
		desc.SetDescription("No description.");
		desc.SetIcon("icons:ObjectTypes/light.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

		//desc.AddMember(&CActorComponent::m_actorGeometry, 'geot', "ActorGeometry", "Actor Geometry", "", "");
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

		const IDefaultSkeleton& defaultSkeleton = pCharacter->GetIDefaultSkeleton();
		const int32 leftEyeJointID = defaultSkeleton.GetJointIDByName("c_eye_l");
		const int32 rightEyeJointID = defaultSkeleton.GetJointIDByName("c_eye_r");

		Vec3 eyeLeftPosition = GetEntity()->GetRotation() * pCharacter->GetISkeletonPose()->GetAbsJointByID(leftEyeJointID).t;
		Vec3 eyeRightPosition = GetEntity()->GetRotation() * pCharacter->GetISkeletonPose()->GetAbsJointByID(rightEyeJointID).t;

		eyePosition = (eyeLeftPosition + eyeRightPosition) / 2.0f;

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
		const bool shouldHandle = (m_pProceduralContextLook != nullptr);

		if (shouldHandle)
		{
			m_pProceduralContextLook->SetLookTarget(lookTarget);
			m_pProceduralContextLook->SetIsLookingGame(isLooking);
		}

		return shouldHandle;
	}

	void CActorComponent::OnInputInteraction(int activationMode)
	{
		switch (activationMode)
		{
		case eAAM_OnPress:
			CryLogAlways("CActorComponent::OnInputInteraction - Press");
			OnActionInteractionStart();
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CActorComponent::OnInputInteraction - Release");
			break;
		}
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
		// You shouldn't be allowed to start another interaction before the last one is completed.
		if (b_isBusyInInteraction)
			return;

		if (m_pAwarenessComponent)
		{
			auto results = m_pAwarenessComponent->GetNearDotFiltered();
			if (results.size() > 0)
			{
				m_interactionEntityId = results[0];
				auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

				// HACK: Another test - this time of setting an emote.
				//auto emoteAction = new CActorAnimationActionEmote(g_emoteMannequinParams.tagIDs.Awe);
				//QueueAction(*emoteAction);

				if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
				{
					// There's an interactor component, so this is an interactive entity.
					// #TODO: We should really only process an 'interact' verb - not simply the first entry.
					auto verbs = pInteractor->GetVerbs();
					if (verbs.size() > 0)
					{
						// Display the verbs in a cheap manner.
						CryLogAlways("VERBS");
						int index{ 1 };
						for (auto& verb : verbs)
						{
							CryLogAlways("%d) %s", index, verb.c_str());
							index++;
						}

						auto verb = verbs[0];

						// #HACK: Another test - just calling the interaction directly instead.
						m_pInteraction = pInteractor->GetInteraction(verb).lock();
						CryLogAlways("Player started interacting with: %s", m_pInteraction->GetVerbUI().c_str());
						m_pInteraction->OnInteractionStart(*this);
					}
				}
			}
		}
	}

	void CActorComponent::OnActionInteractionTick()
	{
		if (m_pInteraction)
		{
			CryWatch("Interacting with: %s", m_pInteraction->GetVerbUI().c_str());
			m_pInteraction->OnInteractionTick(*this);
		}
		else
		{
			CryWatch("Interacting with nothing");
		}
	}
	void CActorComponent::OnActionInteractionEnd()
	{
		if (m_pInteraction)
		{
			CryLogAlways("Player stopped interacting with: %s", m_pInteraction->GetVerbUI().c_str());
			m_pInteraction->OnInteractionComplete(*this);
		}
		else
		{
			CryLogAlways("Player stopped interacting with nothing");
		}
	}

	void CActorComponent::InteractionStart(IInteraction* pInteraction)
	{
		b_isBusyInInteraction = true;
	}

	void CActorComponent::InteractionTick(IInteraction* pInteraction)
	{
	}

	void CActorComponent::InteractionEnd(IInteraction* pInteraction)
	{
		// No longer valid.
		b_isBusyInInteraction = false;
		m_pInteraction = nullptr;
		m_interactionEntityId = INVALID_ENTITYID; // HACK: FIX: This seems weak, look for a better way to handle keeping an entity Id for later.
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
		m_pActorControllerComponent->AddVelocity(Vec3(0, 0, 5.f));
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
		m_pProceduralContextLook = nullptr;

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
			//m_pAnimationComponent->SetDefaultScopeContextName("NPCCharacter");
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
				if (CActorAnimationActionLookPose::IsSupported(pContext)) // HACK: These tests are causing crashes on the second run through.
					//&& CActorAnimationActionLooking::IsSupported(pContext))
				{
					const auto pX = m_pAnimationComponent->GetActionController()->FindOrCreateProceduralContext(CProceduralContextLook::GetCID());
					m_pProceduralContextLook = static_cast<CProceduralContextLook*>(pX);

					QueueAction(*new CActorAnimationActionLookPose());
					//QueueAction(*new CActorAnimationActionLooking());
				}
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorComponent);
}
