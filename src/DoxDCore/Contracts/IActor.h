#pragma once

#include <CrySchematyc/Reflection/TypeDesc.h>

struct ICharacterInstance;

namespace DoxD
{
	struct IActionController;
	struct SActorMannequinParams;
	class IAction;
	class CPlayerComponent;
	struct IInteraction;

	struct IActor
	{
		virtual const bool IsPlayer() const = 0;
		virtual CPlayerComponent* GetPlayer() const = 0;
		virtual ICharacterInstance* GetCharacter() const = 0;

		virtual const Vec3 GetLocalEyePos() const { return { 0.0f, 0.0f, 1.82f }; };
		virtual Vec3 GetLocalLeftHandPos() const { return Vec3(ZERO); };
		virtual Vec3 GetLocalRightHandPos() const { return Vec3(ZERO); };
		
		virtual void OnActionItemUse() = 0;
		virtual void OnActionItemPickup() = 0;
		virtual void OnActionItemDrop() = 0;
		virtual void OnActionItemToss() = 0;

		virtual void OnActionInspectStart() = 0;
		virtual void OnActionInspecting() = 0;
		virtual void OnActionInspectEnd() = 0;

		virtual void OnActionInteractionStart() = 0;
		virtual void OnActionInteractionTick() = 0;
		virtual void OnActionInteractionEnd() = 0;

		virtual void OnActionCrouchToggle() = 0;
		virtual void OnActionCrawlToggle() = 0;
		virtual void OnActionSitToggle() = 0;
		virtual void OnActionJogToggle() = 0;
		virtual void OnActionSprintStart() = 0;
		virtual void OnActionSprintStop() = 0;

		virtual bool IsSprinting() const = 0;
		virtual bool IsJogging() const = 0;

		virtual void InteractionStart(IInteraction* pInteraction) = 0;
		virtual void InteractionTick(IInteraction* pInteraction) = 0;
		virtual void InteractionEnd(IInteraction* pInteraction) = 0;

		virtual void QueueAction(IAction& pAction) = 0;
		virtual IActionController* GetActionController() const = 0;
		virtual const SActorMannequinParams* GetMannequinParams() const = 0;

		virtual void OnPlayerPossess(CPlayerComponentOld& player) = 0;
		virtual void OnPlayerUnpossess() = 0;


		virtual void OnKill() = 0;
		virtual void OnDeath() = 0;
		virtual void OnRevive() = 0;
		virtual void OnJump() = 0;

		virtual void OnEnterVehicle(const char* vehicleClassName, const char* seatName, bool bThirdPerson) = 0;
		virtual void OnExitVehicle() = 0;

		virtual void OnHealthChanged(float newHealth) = 0;
		virtual void OnSanityChanged(float newSanity) = 0;

		virtual void OnItemPickedUp(EntityId itemId) = 0;
		virtual void OnItemUsed(EntityId itemId) = 0;
		virtual void OnItemDropped(EntityId itemId) = 0;

		virtual void OnSprintStaminaChanged(float newStamina) = 0;
	};
	DECLARE_SHARED_POINTERS(IActor);

}
