#pragma once

#include <StdAfx.h>

namespace DoxD
{
	class CPlayerComponent;
}

namespace Cry::DefaultComponents
{
	class CCameraComponent;
}

namespace DoxD
{
	enum class ECameraMode
	{
		Invalid,
		Default,
		Focus,
		Vehicle,
		AnimationControlled,
		Last
	};


	struct ICameraMode
	{
		struct AnimationSettings
		{
			AnimationSettings()
				: positionFactor(0.f)
				, rotationFactor(0.f)
				, applyResult(false)
				, stableBlendOff(false)
			{}

			float positionFactor;
			float rotationFactor;
			bool applyResult;
			bool stableBlendOff;
		};

		ICameraMode(Cry::DefaultComponents::CCameraComponent* camera);
		virtual ~ICameraMode() {};

		void ActivateMode(const CPlayerComponent& player);
		void DeactivateMode(const CPlayerComponent& player);

		virtual void Update(const CPlayerComponent& player, float frameTime) = 0;

		virtual bool CanTransition();
		virtual void SetCameraAnimationFactor(const AnimationSettings& animationSettings);
		virtual void GetCameraAnimationFactor(float& pos, float& rot);

		inline const bool IsBlendingOff() const
		{
			return m_isBlendingOff;
		}

		inline Cry::DefaultComponents::CCameraComponent* GetCamera() { return m_pCamera; }

	protected:
		virtual void Activate(const CPlayerComponent& player);
		virtual void Deactivate(const CPlayerComponent& player);

		bool m_isBlendingOff;
		
		/** Natural length of the spring arm when there are no collisions */
		float m_targetArmLength;

		/** offset at end of spring arm; use this instead of the relative offset of the attached component to ensure the line trace works as desired */
		Vec3 m_socketOffset;

		/** Offset at start of spring, applied in world space.Use this if you want a world - space offset from the parent component instead of the usual relative - space offset. */
		Vec3 m_targetOffset;

		/** How big should the query probe sphere be */
		float m_probeSize;

		float m_cameraLagSpeed;
		float m_cameraRotationLagSpeed;
		float m_cameraLagMaxDistance;

		// Runtime Variables
		Vec3 m_lastFrameDesiredLocation;
		Vec3 m_lastFrameArmOrigin;
		Quat m_lastFrameDesiredRotation;

	private:
		Cry::DefaultComponents::CCameraComponent* m_pCamera;
	};

}