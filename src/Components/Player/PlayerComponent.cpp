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

	void CPlayerComponent::AttachToCharacter(EntityId characterId)
	{
		// Ensure the object is an actor, then inform it we are now attached.
		if (auto pAttachedEntity = gEnv->pEntitySystem->GetEntity(characterId))
		{
			if (auto pActorComponent = pAttachedEntity->GetComponent<CActorComponent>())
			{
				// Remove any existing attachment.
				if (m_attachedCharacterId != INVALID_ENTITYID)
				{
					if (auto pOldAttachedEntity = gEnv->pEntitySystem->GetEntity(m_attachedCharacterId))
					{
						if (auto pOldActorComponent = pOldAttachedEntity->GetComponent<CActorComponent>())
						{
							// Inform the previous actor we are no longer attached to it now.
							pOldActorComponent->OnPlayerDetach();
						}
					}
				}

				// If the target was an actor then it's safe to complete the attachment.
				m_attachedCharacterId = m_cameraTargetId = characterId;

				// Inform the actor we are attached to it now.
				pActorComponent->OnPlayerAttach(*this);

				// Inform the camera system we are now watching a different entity.
				m_pCameraManager->AttachToEntity(m_cameraTargetId);
			}
		}
	}

	void CPlayerComponent::AttachToHero()
	{
		// #HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
		auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Hero");
		CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
		if (pCharacterEntity)
			AttachToCharacter(pCharacterEntity->GetId());
	}

	ICameraComponent* CPlayerComponent::GetCamera() const
	{
		CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
		return m_pCameraManager->GetCamera();
	}

	void CPlayerComponent::OnRevive()
	{
		if (auto pActorComponent = GetLocalActor())
		{
			pActorComponent->OnRevive();
		}
	}

	void CPlayerComponent::OnClientConnect()
	{
		// We want to make it simple to attach to the default entity 'hero' when entering a game as the local player. This
		// behaviour isn't strictly correct, but it's good enough for now.
		if (IsLocalPlayer())
			AttachToHero();

		OnRevive();
	}


	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}
