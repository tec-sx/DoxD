#include "ICameraMode.h"

#include <DefaultComponents/Cameras/CameraComponent.h>

namespace DoxD
{
	ICameraModeOld::ICameraModeOld(Cry::DefaultComponents::CCameraComponent* camera)
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

	void ICameraModeOld::ActivateMode(const CPlayerComponentOld& player)
	{
		m_isBlendingOff = false;

		Activate(player);
	}

	void ICameraModeOld::DeactivateMode(const CPlayerComponentOld& player)
	{
		m_isBlendingOff = true;

		Deactivate(player);
	}

	bool ICameraModeOld::CanTransition()
	{
		return false;
	}

	void ICameraModeOld::SetCameraAnimationFactor(const AnimationSettings& animationSettings)
	{ }

	void ICameraModeOld::GetCameraAnimationFactor(float& pos, float& rot)
	{
		pos = 0.0f;
		rot = 0.0f;
	}

	void ICameraModeOld::Activate(const CPlayerComponentOld& player)
	{
	}

	void ICameraModeOld::Deactivate(const CPlayerComponentOld& player)
	{
	}
}