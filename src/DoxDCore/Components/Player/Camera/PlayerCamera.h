#pragma once
#include <array>
#include <numeric>

#include "ICameraMode.h"

namespace Cry::DefaultComponents
{
	class CCameraComponent;
	class CInputComponent;
}

namespace DoxD
{
	class CPlayerComponentOld;

	class CPlayerCamera
	{
	public:

		CPlayerCamera(CPlayerComponentOld* targetPlayer);
		~CPlayerCamera();

		void Initialize();

		// Methods
		void SetCameraMode(ECameraModeOld newMode, const char* why);
		void SetCameraModeWithAnimationBlendFactors(ECameraModeOld newMode, const ICameraModeOld::AnimationSettings& animationSettings, const char* why);

		void Update(float frameTime);
		//void PostUpdate(const QuatT& camDelta);

		inline bool IsTransitioning() const { return (m_transitionTime > 0.0f); }

		inline const CPlayerComponentOld* GetPlayer() const { return m_pTargetPlayer; }
		inline Cry::DefaultComponents::CCameraComponent* GetCamera() const { return m_pCameraComponent; }
	protected:

	private:
		// Component references
		Cry::DefaultComponents::CCameraComponent* m_pCameraComponent;
		CPlayerComponentOld* m_pTargetPlayer;

		void Reset();

		ECameraModeOld GetCurrentCameraMode();
		void FilterGroundOnlyShakes(SViewParams& viewParams);

		void UpdateTotalTransitionTime();

		// Runtime Variables
		ICameraModeOld* m_cameraModes[(int)ECameraModeOld::Last];
		ECameraModeOld m_currentCameraMode;
		ECameraModeOld m_previousCameraMode;

		float		m_transitionTime;
		float		m_totalTransitionTime;
		ECameraModeOld m_transitionCameraMode;

		bool m_enteredPartialAnimControlledCameraOnLedge;
	};
}