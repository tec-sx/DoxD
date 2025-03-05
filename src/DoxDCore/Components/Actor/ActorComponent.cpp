#include "ActorComponent.h"

#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

namespace
{
	static void RegisterActorComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CActorComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorComponent);
}

namespace DoxD
{
	void CActorComponent::Initialize()
	{
	}

	void CActorComponent::ProcessEvent(const SEntityEvent& event)
	{
	}

	void CActorComponent::OnInputEscape(int activationMode)
	{
		CryLogAlways("CActorComponent::OnInputEscape");
	}

	void CActorComponent::OnInputInteraction(int activationMode)
	{
	}

	void CActorComponent::Update(SEntityUpdateContext* pCtx)
	{
	}

	const bool CActorComponent::IsPlayer() const
	{
		return false;
	}

	ICharacterInstance* CActorComponent::GetCharacter() const
	{
		return nullptr;
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

	IActionController* CActorComponent::GetActionController() const
	{
		return nullptr;
	}

	const SActorMannequinParams* CActorComponent::GetMannequinParams() const
	{
		return nullptr;
	}

	void CActorComponent::OnPlayerPossess(CPlayerComponentOld& player)
	{
	}

	void CActorComponent::OnPlayerUnpossess()
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

	void CActorComponent::SetIK() const
	{
	}

	bool CActorComponent::SetLookingIK(const bool isLooking, const Vec3& lookTarget) const
	{
		return false;
	}
}
