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

	//const bool CActorComponent::IsPlayer() const
	//{
	//	return false;
	//}

	//CPlayerComponent* CActorComponent::GetPlayer() const
	//{
	//	return nullptr;
	//}

	//ICharacterInstance* CActorComponent::GetCharacter() const
	//{
	//	return nullptr;
	//}

	void CActorComponent::Update(SEntityUpdateContext* pCtx)
	{
	}
}
