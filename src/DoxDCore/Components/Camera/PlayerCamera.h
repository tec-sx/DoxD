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
		void SetCameraMode(ECameraMode newMode, const char* why);
		void SetCameraModeWithAnimationBlendFactors(ECameraMode newMode, const ICameraMode::AnimationSettings& animationSettings, const char* why);

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

		ECameraMode GetCurrentCameraMode();
		void FilterGroundOnlyShakes(SViewParams& viewParams);

		void UpdateTotalTransitionTime();

		// Runtime Variables
		ICameraMode* m_cameraModes[(int)ECameraMode::Last];
		ECameraMode m_currentCameraMode;
		ECameraMode m_previousCameraMode;

		float		m_transitionTime;
		float		m_totalTransitionTime;
		ECameraMode m_transitionCameraMode;

		bool m_enteredPartialAnimControlledCameraOnLedge;
	};
}