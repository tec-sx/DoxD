#pragma once

#include <DefaultComponents/Physics/CharacterControllerComponent.h>

namespace DoxD
{
	struct ICameraComponent;
	struct CActorComponent;
	class CPlayerInputComponent;
	class CCameraManagerComponent;

	class CPlayerComponent : public IEntityComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		virtual void ProcessEvent(const SEntityEvent& event) override;
		Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update | EEntityEvent::GameplayStarted; }
		// ~IEntityComponent

	public:
		CPlayerComponent() {}
		virtual ~CPlayerComponent() {}

		static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
		{
			desc.SetGUID(CPlayerComponent::IID());
			desc.SetEditorCategory("Player");
			desc.SetLabel("Player");
			desc.SetDescription("A player.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{58A26B16-EC1D-4700-A76D-38CBB7B2B400}"_cry_guid;
			return id;
		}

		struct SInteractionState
		{
			enum class EInteractionMode
			{
				/** No special states apply. */
				eNoMode,
				/** The player is handling an item in their hands. */
				eHandlingEntity,
				/** The player is zoomed into an entity, examining / using it in some special way e.g. security pad, computer system. */
				eExamineZoom
			};


			bool IsCharacterMovementAllowed() const { return m_allowCharacterMovement; }
			bool IsCharacterRotationAllowed() const { return m_allowCharacterRotation; }
			bool IsCameraMovementAllowed() const { return m_allowCameraMovement; }

			void SetInteractionMode(EInteractionMode playerInteractionMode)
			{
				m_playerInteractionMode = playerInteractionMode;

				switch (m_playerInteractionMode)
				{
				case EInteractionMode::eNoMode:
					m_allowCharacterMovement = true;
					m_allowCharacterRotation = true;
					m_allowCameraMovement = true;
					break;

				case EInteractionMode::eHandlingEntity:
					m_allowCharacterMovement = false;
					m_allowCharacterRotation = false;
					m_allowCameraMovement = false;
					break;

				case EInteractionMode::eExamineZoom:
					m_allowCharacterMovement = false;
					m_allowCharacterRotation = false;
					m_allowCameraMovement = true;
					break;
				}
			}

		private:
			EInteractionMode m_playerInteractionMode{ EInteractionMode::eNoMode };

			void SetAllowCharacterMovement(bool val) { m_allowCharacterMovement = val; }
			void SetAllowCharacterRotation(bool val) { m_allowCharacterRotation = val; }
			void SetAllowCameraMovement(bool val) { m_allowCameraMovement = val; }

			bool m_allowCharacterMovement{ true };
			bool m_allowCharacterRotation{ true };
			bool m_allowCameraMovement{ true };
		};
	public:
		IEntity* GetAttachedEntity() const;
		CActorComponent* GetAttachedActor() const;
		static CActorComponent* GetLocalActor();
		void AttachToCharacter(EntityId characterId);
		void AttachToHero();
		CPlayerInputComponent* GetPlayerInput() const { return m_pPlayerInput; }

		ILINE bool IsLocalPlayer()
		{
			return (gEnv->pGameFramework->GetClientActorId() == GetEntityId());
		}

		ILINE static CPlayerComponent* GetLocalPlayer()
		{
			auto pActor = gEnv->pGameFramework->GetClientEntity();

			return pActor ? pActor->GetComponent<CPlayerComponent>() : nullptr;
		}

		ICameraComponent* GetCamera() const;
		EntityId GetCameraTargetId() const { return m_cameraTargetId; }
		CCameraManagerComponent* GetCameraManager() const { return m_pCameraManager; }
		SInteractionState& GetInteractionState() { return m_playerInteractionState; }

		void OnRevive();
		void OnClientConnect();
	private:
		CCameraManagerComponent* m_pCameraManager{ nullptr };
		CPlayerInputComponent* m_pPlayerInput{ nullptr };

		EntityId m_attachedCharacterId{ INVALID_ENTITYID };
		EntityId m_cameraTargetId{ INVALID_ENTITYID };

		SInteractionState m_playerInteractionState;
	};
}

