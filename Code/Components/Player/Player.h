// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <array>
#include <numeric>

#include <ICryMannequin.h>
#include <IItemSystem.h>

#include "Components/Actor/Actor.h"

struct IItemParamsNode;

namespace primitives
{
	struct capsule;
}

namespace Cry
{
	namespace DefaultComponents
	{
		class CInputComponent;
		class CCharacterControllerComponent;
		class CAdvancedAnimationComponent;
	}

	namespace Audio::DefaultComponents
	{
		class CListenerComponent;
	}
}

namespace DoxD
{
	class CPlayerCamera;
}

namespace DoxD
{
	struct SAimAccelerationParams
	{
		SAimAccelerationParams();

		float angle_min;
		float angle_max;
	};

	struct SPlayerRotationParams
	{
		enum EAimType
		{
			EAimType_NORMAL,
			EAimType_CROUCH,
			EAimType_SLIDING,
			EAimType_SPRINTING,
			EAimType_SWIM,
			EAimType_MOUNTED_GUN,
			EAimType_TOTAL
		};

		SAimAccelerationParams m_horizontalAims[EAimType_TOTAL];
		SAimAccelerationParams m_verticalAims[EAimType_TOTAL];

		void Reset(const IItemParamsNode* pRootNode);

		inline const SAimAccelerationParams& GetHorizontalAimParams(SPlayerRotationParams::EAimType aimType) const
		{
			CRY_ASSERT((aimType >= 0) && (aimType < EAimType_TOTAL));

			return m_horizontalAims[aimType];
		}

		inline const SAimAccelerationParams& GetVerticalAimParams(SPlayerRotationParams::EAimType aimType) const
		{
			CRY_ASSERT((aimType >= 0) && (aimType < EAimType_TOTAL));

			return m_verticalAims[aimType];
		}
	private:
		void ReadAimParams(const IItemParamsNode* pRootNode, const char* aimTypeName, EAimType aimType);
		void ReadAccelerationParams(const IItemParamsNode* pNode, SAimAccelerationParams* output);
	};

	template<typename T, size_t SAMPLES_COUNT>
	class MovingAverage
	{
		static_assert(SAMPLES_COUNT > 0, "SAMPLES_COUNT shall be larger than zero!");

	public:

		MovingAverage()
			: m_values()
			, m_cursor(SAMPLES_COUNT)
			, m_accumulator()
		{
		}

		MovingAverage& Push(const T& value)
		{
			if (m_cursor == SAMPLES_COUNT)
			{
				m_values.fill(value);
				m_cursor = 0;
				m_accumulator = std::accumulate(m_values.begin(), m_values.end(), T(0));
			}
			else
			{
				m_accumulator -= m_values[m_cursor];
				m_values[m_cursor] = value;
				m_accumulator += m_values[m_cursor];
				m_cursor = (m_cursor + 1) % SAMPLES_COUNT;
			}

			return *this;
		}

		T Get() const
		{
			return m_accumulator / T(SAMPLES_COUNT);
		}

		void Reset()
		{
			m_cursor = SAMPLES_COUNT;
		}

	private:

		std::array<T, SAMPLES_COUNT> m_values;
		size_t m_cursor;

		T m_accumulator;
	};


	////////////////////////////////////////////////////////
	// Represents a player participating in gameplay
	////////////////////////////////////////////////////////
	class CPlayerComponent final : public IEntityComponent, public CActor
	{
	public:
		enum class EPlayerState
		{
			Walking,
			Sprinting
		};

		enum class EPlayerStance
		{
			Standing,
			Crouching
		};

		static constexpr float DEFAULT_SPEED_WALKING = 12;
		static constexpr float DEFAULT_SPEED_RUNNING = 39;
		static constexpr float DEFAULT_JUMP_ENERGY = 3;
		static constexpr EPlayerState DEFAULT_STATE = EPlayerState::Walking;
		static constexpr float DEFAULT_CAPSULE_HEIGHT_CROUCHING = 0.75;
		static constexpr float DEFAULT_CAPSULE_HEIGHT_STANDING = 1.6;
		static constexpr float DEFAULT_CAPSULE_GROUND_OFFSET = 0.2;
		static constexpr EPlayerStance DEFAULT_STANCE = EPlayerStance::Standing;
		static constexpr float DEFAULT_ROTATION_SPEED_WALK = 6;
		static constexpr float DEFAULT_ROTATION_SPEED_RUN = 3;

	public:
		CPlayerComponent();
		virtual ~CPlayerComponent() override;

		virtual void Initialize() override;

		virtual Cry::Entity::EventFlags GetEventMask() const override;
		virtual void ProcessEvent(const SEntityEvent& event) override;

		int GetPhysicalSkipEntities(IPhysicalEntity** pSkipList, const int maxSkipSize) const;

		// Reflect type to set a unique identifier for this component
		static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
		{
			desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
			desc.SetComponentFlags(IEntityComponent::EFlags::Singleton);

			desc.AddMember(&CPlayerComponent::m_walkSpeed, 'pws', "playerwalkspeed", "Player Walk Speed", "Sets the Player Walking Speed", DEFAULT_SPEED_WALKING);
			desc.AddMember(&CPlayerComponent::m_runSpeed, 'prs', "playerrunspeed", "Player Run Speed", "Sets the Player Running Speed", DEFAULT_SPEED_RUNNING);
			desc.AddMember(&CPlayerComponent::m_jumpHeight, 'pjh', "playerjumpheight", "Player Jump Height", "Sets the Player Jump Height", DEFAULT_JUMP_ENERGY);
			desc.AddMember(&CPlayerComponent::m_rotationSpeedWalking, 'rspw', "playerrotationspeedwalking", "Player Walking Rotation Speed", "Sets the player rotation speed when walking", DEFAULT_ROTATION_SPEED_WALK);
			desc.AddMember(&CPlayerComponent::m_rotationSpeedRunning, 'rspr', "playerrotationspeedrunning", "Player Running Rotation Speed", "Sets the player rotation speed when running", DEFAULT_ROTATION_SPEED_RUN);
			desc.AddMember(&CPlayerComponent::m_capsuleHeightCrouching, 'capc', "capsuleheightcrouching", "Capsule Crouching Height", "Height of collision capsule while crouching", DEFAULT_CAPSULE_HEIGHT_CROUCHING);
			desc.AddMember(&CPlayerComponent::m_capsuleHeightStanding, 'caps', "capsuleheightstanding", "Capsule Standing Height", "Height of collision capsule while standing", DEFAULT_CAPSULE_HEIGHT_STANDING);
			desc.AddMember(&CPlayerComponent::m_capsuleGroundOffset, 'capo', "capsulegroundoffset", "Capsule Ground Offset", "Offset of the capsule from the entity floor", DEFAULT_CAPSULE_GROUND_OFFSET);
		}

		const Quat& GetLookOrientation() const { return m_lookOrientation; }
		const SPlayerRotationParams& GetPlayerRotationParams() const { return m_playerRotationParams; }
		
		SPlayerRotationParams::EAimType GetCurrentAimType() const;

		// Stance
		Vec3 GetStanceViewOffset(EStance stance, const float* pLeanAmt = NULL, bool withY = false, const bool useWhileLeanedOffsets = false) const;

	protected:
		void Reset();
		void RecenterCollider();
		void InitializeInput();

		void UpdateLookDirectionRequest(float frameTime);
		void UpdateMovement(float frameTime);
		void UpdateAnimation(float frameTime);

		void TryUpdateStance();
		bool IsCapsuleIntersectingGeometry(const primitives::capsule& capsule) const;

		// Component properties
		float m_capsuleHeightCrouching = DEFAULT_CAPSULE_HEIGHT_CROUCHING;
		float m_capsuleHeightStanding = DEFAULT_CAPSULE_HEIGHT_STANDING;
		float m_capsuleGroundOffset = DEFAULT_CAPSULE_GROUND_OFFSET;
		float m_walkSpeed = DEFAULT_SPEED_WALKING;
		float m_runSpeed = DEFAULT_SPEED_RUNNING;
		float m_jumpHeight = DEFAULT_JUMP_ENERGY;
		float m_rotationSpeedWalking = DEFAULT_ROTATION_SPEED_WALK;
		float m_rotationSpeedRunning = DEFAULT_ROTATION_SPEED_RUN;

		FragmentID m_idleFragmentId;
		FragmentID m_walkFragmentId;
		TagID m_rotateTagId;
	private:
		CPlayerCamera* m_pPlayerCamera;
		
		// Component references
		Cry::DefaultComponents::CInputComponent* m_pInputComponent;
		Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController;
		Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAnimationComponent;
		Cry::Audio::DefaultComponents::CListenerComponent* m_pAudioListener;

		// Runtime variables
		Vec2 m_movementDelta;
		EPlayerState m_currentPlayerState;

		Quat m_currentYaw;
		float m_currentPitch;
		SPlayerRotationParams m_playerRotationParams;

		Vec2 m_mouseDeltaRotation;
		MovingAverage<Vec2, 10> m_mouseDeltaSmoothingFilter;
		
		Quat m_lookOrientation;
		Quat m_lastFrameDesiredRotation;

		float m_horizontalAngularVelocity;
		MovingAverage<float, 10> m_averagedHorizontalAngularVelocity;


		EPlayerStance m_currentStance;
		EPlayerStance m_desiredStance;
		Vec3 m_cameraEndOffset;

		FragmentID m_activeFragmentId;
	};
}
