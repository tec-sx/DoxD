#pragma once

#include <Utility/StateMachine.h>
//#include "Actor/Animation/ActorAnimation.h"
#include "Components/Player/Input/PlayerInputComponent.h"
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
//#include <Components/Animation/ActorAnimationComponent.h>

namespace DoxD
{
	class CActorComponent;

	class CActorControllerComponent : public IEntityComponent
	{
	protected:
		// Declaration of the state machine that controls actor movement.
		DECLARE_STATE_MACHINE(CActorControllerComponent, Movement);

		// IEntityComponent
		void Initialize() override;
		void ProcessEvent(const SEntityEvent& event) override;
		Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update | EEntityEvent::PrePhysicsUpdate; }
		// ~IEntityComponent

		virtual void Update(SEntityUpdateContext* pCtx);
		virtual void PrePhysicsUpdate();

		virtual void UpdateMovementRequest(float frameTime);
		virtual void UpdateLookDirectionRequest(float frameTime);
		virtual void UpdateAnimation(float frameTime);

	public:
		CActorControllerComponent() {}
		virtual ~CActorControllerComponent() { StateMachineReleaseMovement(); }

		static void ReflectType(Schematyc::CTypeDesc<CActorControllerComponent>& desc)
		{
			desc.SetGUID(CActorControllerComponent::IID());
			desc.SetEditorCategory("Actors");
			desc.SetLabel("Actor Controller");
			desc.SetDescription("Actor controller.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

			desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CActorComponent::IID());
			//desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, "{3CD5DDC5-EE15-437F-A997-79C2391537FE}"_cry_guid);
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{D75A8B46-158F-461C-B93E-035217C6B481}"_cry_guid;
			return id;
		}

		virtual void OnResetState();

		bool IsAIControlled() const { return m_isAIControlled; };
		const bool IsPlayer() const;
		const virtual Vec3 GetLocalEyePos() const;

		virtual void AddVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->AddVelocity(velocity); }
		virtual void SetVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->SetVelocity(velocity); }

		const Vec3& GetVelocity() const { return m_pCharacterControllerComponent->GetVelocity(); }
		Vec3 GetMoveDirection() const { return m_pCharacterControllerComponent->GetMoveDirection(); }

		float virtual GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const;

	private:
		Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent{ nullptr };

		CActorComponent* m_pActorComponent{ nullptr };

		bool m_isAIControlled{ false };
		Vec3 m_lookTarget{ ZERO };
		bool m_useLookTarget{ false };
		bool m_useLookIK{ true };
		Vec3 m_aimTarget{ ZERO };
		bool m_useAimIK{ true };
		bool m_shouldJump{ false };
		bool m_canJump{ false };
	};
}

