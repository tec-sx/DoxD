#pragma once
#include "Actor/Fate.h"
#include <CryAISystem/IAgent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>
#include <Components/Player/Input/PlayerInputComponent.h>
#include "Contracts/IActor.h"
#include "Attachments/Attachment.h"

namespace DoxD
{
	class CPlayerComponent;
	class CAttachmentComponent;

	/** Equipment slots **/
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_HEAD, "Actor Head", 0x617985533E2A4E5D, 0xB43CD7D2CFC60571)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_FACE, "Actor Face", 0x10BA29FEA6F14F48, 0xAC475D386AD3637D)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_NECK, "Actor Neck", 0x98CF570E51B343AE, 0xA566097392EEA395)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_SHOULDERS, "Actor Shoulders", 0x0435E8CD9FA84215, 0x9FEEBDED107A3482)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_CHEST, "Actor Chest", 0x7E232140AFC04AF0, 0x95A200219E4B5031)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_BACK, "Actor Back", 0xDF40E37A38CB4530, 0xB421E09F4972A025)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_LEFTARM, "Actor Left Arm", 0xB6B761D420F445CA, 0xB8195FCAC86B0E7D)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_RIGHTARM, "Actor Right Arm", 0x5536709863DE489C, 0xA94FA3900B546241)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_LEFTHAND, "Actor Left Hand", 0xA1EB42095D804E6B, 0xABDB1052EBC22704)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_RIGHTHAND, "Actor Right Hand", 0xC9225FCED1E34228, 0xBA2B35347EB4E30D)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_WAIST, "Actor Waist", 0x7631F1B70DC14B27, 0x9B3D14368B2EE2DE)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_LEFTLEG, "Actor Left Leg", 0x6C0F6F7269504D25, 0x97F96FBCB54BA444)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_RIGHTLEG, "Actor Right Leg", 0xB349158398FD4B19, 0xB0F42137CF694919)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_LEFTFOOT, "Actor Left Foot", 0x8880849B8D5E4A28, 0xB18F39A02772920B)
	DECLARE_ATTACHMENT_TYPE(SLOT_ACTOR_RIGHTFOOT, "Actor Right Foot", 0xB3619CEF068F4388, 0xB49947F598CF321D)

	enum EActorClassType
	{
		EACT_ACTOR = 0,
		EACT_PLAYER,
		EACT_CHARACTER,
		EACT_PET,
	};

	class CActorComponent : public IEntityComponent, public IActor, public CPlayerInputComponent::IInputEventListener
	{
		// CPlayerInputComponent::IInputEventListener
		virtual void OnInputEscape(int activationMode) override;
		virtual void OnInputInteraction(int activationMode) override;
		// ~CPlayerInputComponent::IInputEventListener



		// IEntityComponent
		virtual void Initialize() override;
		virtual void ProcessEvent(const SEntityEvent& event) override;
		virtual Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Initialize | EEntityEvent::Update | EEntityEvent::Remove; }
		// ~IEntityComponent
		
		virtual void Update(SEntityUpdateContext* pCtx);

	public:
		CActorComponent() {};
		virtual ~CActorComponent() = default;
		
		static void ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc)
		{
			desc.SetGUID(CActorComponent::IID());
			desc.SetEditorCategory("Actors");
			desc.SetLabel("Actor");
			desc.SetDescription("No description.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

			//desc.AddMember(&CActorComponent::m_actorGeometry, 'geot', "ActorGeometry", "Actor Geometry", "", "");
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{95A26BF9-0AC4-4593-A7CB-C2E0E864BD41}"_cry_guid;
			return id;
		}
		
		const Vec3& GetVelocity() const { return m_pCharacterControllerComponent->GetVelocity(); }
		Vec3 GetMoveDirection() const { return m_pCharacterControllerComponent->GetMoveDirection(); }
		virtual const bool IsPlayer() const override;
		virtual CPlayerComponent* GetPlayer() const override { return m_pPlayer; };
		virtual ICharacterInstance* GetCharacter() const override;

		virtual const Vec3 GetLocalEyePos() const { return { 0.0f, 0.0f, 1.82f }; };
		virtual Vec3 GetLocalLeftHandPos() const { return Vec3(ZERO); };
		virtual Vec3 GetLocalRightHandPos() const { return Vec3(ZERO); };
		static CActorComponent* GetActor(EntityId entityId)
		{
			auto pActor = gEnv->pEntitySystem->GetEntity(entityId);
			if (pActor)
				return pActor->GetComponent<CActorComponent>();

			return nullptr;
		};

		virtual void OnActionItemUse() override;
		virtual void OnActionItemPickup() override;
		virtual void OnActionItemDrop() override;
		virtual void OnActionItemToss() override;

		virtual void OnActionInspectStart() override;
		virtual void OnActionInspecting() override;
		virtual void OnActionInspectEnd() override;

		virtual void OnActionInteractionStart() override;
		virtual void OnActionInteractionTick() override;
		virtual void OnActionInteractionEnd() override;

		virtual void OnActionCrouchToggle() override;
		virtual void OnActionCrawlToggle() override;
		virtual void OnActionSitToggle() override;
		virtual void OnActionJogToggle() override;
		virtual void OnActionSprintStart() override;
		virtual void OnActionSprintStop() override;

		virtual bool IsSprinting() const override;
		virtual bool IsJogging() const override;

		virtual void InteractionStart(IInteraction* pInteraction) override;
		virtual void InteractionTick(IInteraction* pInteraction) override;
		virtual void InteractionEnd(IInteraction* pInteraction) override;

		virtual void QueueAction(IAction& pAction) override;
		virtual IActionController* GetActionController() const override;
		virtual const SActorMannequinParams* GetMannequinParams() const override;

		virtual void OnPlayerPossess(CPlayerComponentOld& player) override;
		virtual void OnPlayerUnpossess() override;


		virtual void OnKill() override;
		virtual void OnDeath() override;
		virtual void OnRevive() override;
		virtual void OnJump() override;

		virtual void OnEnterVehicle(const char* vehicleClassName, const char* seatName, bool bThirdPerson) override;
		virtual void OnExitVehicle() override;

		virtual void OnHealthChanged(float newHealth) override;
		virtual void OnSanityChanged(float newSanity) override;

		virtual void OnItemPickedUp(EntityId itemId) override;
		virtual void OnItemUsed(EntityId itemId) override;
		virtual void OnItemDropped(EntityId itemId) override;

		virtual void OnSprintStaminaChanged(float newStamina) override;

		protected:
			Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent{ nullptr };

			Schematyc::CharacterFileName m_charcterGeometry;

			virtual void OnResetState();

			void SetIK() const;
			bool SetLookingIK(const bool isLooking, const Vec3& lookTarget) const;

		private:	
			CPlayerComponent* m_pPlayer{ nullptr };
			CAttachmentComponent* m_pAttachmentComponent{ nullptr };
			Cry::Audio::DefaultComponents::CListenerComponent* m_pAudioListenerComponent{ nullptr };

			CFate m_fate;
	};
}

