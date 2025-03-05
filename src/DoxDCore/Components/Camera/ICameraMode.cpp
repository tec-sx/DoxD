#include "ICameraMode.h"

#include <DefaultComponents/Cameras/CameraComponent.h>

namespace DoxD
{
	ICameraMode::ICameraMode(Cry::DefaultComponents::CCameraComponent* camera)
		: m_pCamera(camera)
		, m_isBlendingOff(false)
		, m_targetArmLength(0)
		, m_socketOffset(ZERO)
		, m_targetOffset(ZERO)
		, m_probeSize(0)
		, m_cameraLagSpeed(0)
		, m_cameraRotationLagSpeed(0)
		, m_cameraLagMaxDistance(0)
		, m_lastFrameDesiredLocation(ZERO)
		, m_lastFrameArmOrigin(ZERO)
		, m_lastFrameDesiredRotation(IDENTITY)
	{ }

	void ICameraMode::ActivateMode(const CPlayerComponentOld& player)
	{
		m_isBlendingOff = false;

		Activate(player);
	}

	void ICameraMode::DeactivateMode(const CPlayerComponentOld& player)
	{
		m_isBlendingOff = true;

		Deactivate(player);
	}

	bool ICameraMode::CanTransition()
	{
		return false;
	}

	void ICameraMode::SetCameraAnimationFactor(const AnimationSettings& animationSettings)
	{ }

	void ICameraMode::GetCameraAnimationFactor(float& pos, float& rot)
	{
		pos = 0.0f;
		rot = 0.0f;
	}

	void ICameraMode::Activate(const CPlayerComponentOld& player)
	{
	}

	void ICameraMode::Deactivate(const CPlayerComponentOld& player)
	{
	}
}