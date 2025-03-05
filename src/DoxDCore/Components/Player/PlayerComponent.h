#pragma once

#include <DefaultComponents/Physics/CharacterControllerComponent.h>

namespace DoxD
{
	class CActorComponent;

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
			static CryGUID id = "{B3309FD2-F8D4-4171-8BA9-31206ADC020F}"_cry_guid;
			return id;
		}

		IEntity* GetAttachedEntity() const;
		CActorComponent* GetAttachedActor() const;

		ILINE static CPlayerComponent* GetActivePlayer()
		{
			auto actorId = gEnv->pGameFramework->GetClientActorId();
			auto pActor = gEnv->pEntitySystem->GetEntity(actorId);
			CRY_ASSERT_MESSAGE(pActor, "The local actor Id %d did not return an entity.", actorId);

			return pActor->GetComponent<CPlayerComponent>();
		}

	private:
		EntityId m_attachedCharacterId{ INVALID_ENTITYID };
	};
}

