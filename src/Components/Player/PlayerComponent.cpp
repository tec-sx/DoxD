#include "StdAfx.h"

#include "PlayerComponent.h"
#include <Components/Actor/ActorComponent.h>
#include <Components/Player/Camera/CameraManagerComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <Components/Player/Input/PlayerInputComponent.h>

namespace DoxD
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	void CPlayerComponent::Initialize()
	{
		CryLogAlways("The player Id is %d - should be 30583.", GetEntityId());

		m_pCameraManager = m_pEntity->CreateComponent<CCameraManagerComponent>();
		m_pPlayerInput = m_pEntity->CreateComponent<CPlayerInputComponent>();
	}

	void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::GameplayStarted:
		{
			// Revive the entity when gameplay starts
			if (auto pActorComponent = GetLocalActor())
			{
				pActorComponent->OnRevive();
			}
		}
		break;

		case EEntityEvent::Update:
		{
			//SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam [0];
		}
		break;
		}
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

	CActorComponent* CPlayerComponent::GetLocalActor()
	{
		auto pPlayer = CPlayerComponent::GetLocalPlayer();
		if (pPlayer)
			return pPlayer->GetAttachedActor();

		CRY_ASSERT_MESSAGE(false, "Attempt to retrieve local character failed. Check your logic.");

		return nullptr;
	}

	ICameraComponent* CPlayerComponent::GetCamera() const
	{
		CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
		return m_pCameraManager->GetCamera();
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}
