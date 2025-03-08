#pragma once

#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <Utility/StateMachine.h>
#include <Actor/Animation/ActorAnimation.h>
#include <Components/Actor/ActorComponent.h>
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Components/Animation/ActorAnimationComponent.h>

namespace DoxD
{
	struct CActorComponent;

	struct SActorStance
	{
	public:
		EActorStance GetStance() const { return m_stance; }
		void SetStance(EActorStance stance) { m_stance = stance; }
		EActorPosture GetPosture() const { return m_posture; }
		void SetPosture(EActorPosture posture) { m_posture = posture; }

	private:

		/** The actor is currently in this stance, or moving into this stance. */
		EActorStance m_stance{ EActorStance::standing };

		/** The actor's posture, which should indicate their state of mind, or game conditions they presently have e.g. sapped. */
		EActorPosture m_posture{ EActorPosture::neutral };
	};


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

		bool GetShouldJump() { return m_shouldJump; };
		void SetShouldJump(bool shouldJump) { m_shouldJump = shouldJump; };

		EActorStance GetStance() const { return m_actorStance.GetStance(); }
		void SetStance(EActorStance stance) { m_actorStance.SetStance(stance); }
		EActorPosture GetPosture() const { return m_actorStance.GetPosture(); }
		void SetPosture(EActorPosture posture) { m_actorStance.SetPosture(posture); }

		void OnActionCrouchToggle();
		void OnActionCrawlToggle();
		void OnActionSitToggle();
		void OnActionJogToggle() { m_isJogging = !m_isJogging; };
		void OnActionSprintStart() { m_isSprinting = true; };
		void OnActionSprintStop() { m_isSprinting = false; };
		bool IsSprinting() const { return m_isSprinting; }
		bool IsJogging() const { return m_isJogging; }

		const CActorComponent* GetActor() { return m_pActorComponent; };

		float durationInAir{ 0.0f };
		float durationOnGround{ 0.0f };

	private:
		CActorAnimationComponent* m_pActorAnimationComponent{ nullptr };
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
		TagID m_rotateTagId{ TAG_ID_INVALID };
		Vec3 m_movementRequest{ ZERO };
		Quat m_lookOrientation{ IDENTITY };
		float m_yawAngularVelocity{ 0.0f };
		float m_movingDuration{ 0.0f };
		bool m_isSprinting{ false };
		bool m_isJogging{ false };
		SActorStance m_actorStance;
		TagState m_mannequinTagsClear{ TAG_STATE_EMPTY };
		TagState m_mannequinTagsSet{ TAG_STATE_EMPTY };
	};
}

