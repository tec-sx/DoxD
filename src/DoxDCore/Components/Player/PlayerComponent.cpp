#include "PlayerComponent.h"

#include <Components/Actor/ActorComponent.h>
#include <Components/Player/Camera/CameraManagerComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

namespace
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

namespace DoxD
{
	void CPlayerComponent::Initialize()
	{
	}

	void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
	{
	}

	IEntity* CPlayerComponent::GetAttachedEntity() const
	{
		CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "This player is not attached to an entity.");

		return gEnv->pEntitySystem->GetEntity(m_attachedCharacterId);
	}

	CActorComponent* CPlayerComponent::GetAttachedActor() const
	{
		if (auto pAttachedEntity = GetAttachedEntity())
		{
			return pAttachedEntity->GetComponent<CActorComponent>();
		}

		CRY_ASSERT_MESSAGE(false, "Failed to retrieve attached actor.");

		return nullptr;
	}

	ICameraComponent* CPlayerComponent::GetCamera() const
	{
		CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
		return m_pCameraManager->GetCamera();
	}
}
