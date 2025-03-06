#include "StdAfx.h"
#include "PlayerCamera.h"
#include "GamePlugin.h"

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <CryMath/Angle.h>
#include <IViewSystem.h>

#include "Components/Player/Player.h"
#include "CameraModes.h"
#include "Utility/CryWatch.h"

#include <CrySystem/ConsoleRegistration.h>

namespace DoxD
{
#if !defined(_RELEASE)

#define PLAYER_CAMERA_WATCH(...)  do { if (g_pCVars->pl_debug_watch_camera_mode)       CryWatch    ("[PlayerCamera] " __VA_ARGS__); } while(0)
#define PLAYER_CAMERA_LOG(...)    do { if (g_pCVars->pl_debug_log_camera_mode_changes) CryLogAlways("[PlayerCamera] " __VA_ARGS__); } while(0)

#else

#define PLAYER_CAMERA_WATCH(...)  (void)(0)
#define PLAYER_CAMERA_LOG(...)    (void)(0)

#endif

	CPlayerCamera::CPlayerCamera(CPlayerComponentOld* targetPlayer)
		: m_pTargetPlayer(targetPlayer)
		, m_pCameraComponent(nullptr)
		, m_transitionTime(0.0f)
		, m_totalTransitionTime(0.0f)
		, m_previousCameraMode(ECameraModeOld::Invalid)
		, m_transitionCameraMode(ECameraModeOld::Invalid)
		, m_currentCameraMode(ECameraModeOld::Default)
		, m_enteredPartialAnimControlledCameraOnLedge(false)
	{
		memset(m_cameraModes, 0, sizeof(m_cameraModes));
	}

	CPlayerCamera::~CPlayerCamera()
	{
		PLAYER_CAMERA_LOG("Destroying PlayerCamera instance %p", this);

		for (unsigned int i = 0; i < (int)ECameraModeOld::Last; ++i)
		{
			SAFE_DELETE(m_cameraModes[i]);
		}
	}

	void CPlayerCamera::Initialize()
	{
		CCamera camera = gEnv->pSystem->GetViewCamera();
		camera.SetMatrix(m_pTargetPlayer->GetEntity()->GetWorldTM());
		gEnv->pSystem->SetViewCamera(camera);

		m_cameraModes[(int)ECameraModeOld::Default] = new CDefaultCameraMode(m_pCameraComponent);
		m_cameraModes[(int)ECameraModeOld::Examine] = new CFocusCameraMode(m_pCameraComponent);
		m_cameraModes[(int)ECameraModeOld::Vehicle] = new CVehicleCameraMode(m_pCameraComponent);
		m_cameraModes[(int)ECameraModeOld::AnimationControlled] = new CAnimationControlledCameraMode(m_pCameraComponent);

		PLAYER_CAMERA_LOG("Creating PlayerCamera instance %p", this);
	}

	void CPlayerCamera::Reset()
	{
		m_previousCameraMode = ECameraModeOld::Invalid;
		m_transitionCameraMode = ECameraModeOld::Invalid;
		m_currentCameraMode = ECameraModeOld::Default;
	}

	void CPlayerCamera::SetCameraMode(ECameraModeOld newMode, const char* why)
	{
		m_currentCameraMode = newMode;
	}

	void CPlayerCamera::SetCameraModeWithAnimationBlendFactors(ECameraModeOld newMode, const ICameraModeOld::AnimationSettings& animationSettings, const char* why)
	{
		SetCameraMode(newMode, why);

		m_cameraModes[(int)newMode]->SetCameraAnimationFactor(animationSettings);
	}


	void CPlayerCamera::Update(float frameTime)
	{
		ECameraModeOld currentCameraMode = GetCurrentCameraMode();
		assert((currentCameraMode >= ECameraModeOld::Default) && (currentCameraMode < ECameraModeOld::Last));

		if (m_currentCameraMode != currentCameraMode)
		{
			SetCameraMode(currentCameraMode, "in update");
		}

		if (m_previousCameraMode != m_currentCameraMode)
		{
			assert(m_previousCameraMode >= ECameraModeOld::Invalid && m_previousCameraMode < ECameraModeOld::Last);
			assert(m_currentCameraMode >= ECameraModeOld::Invalid && m_currentCameraMode < ECameraModeOld::Last);

			if (m_previousCameraMode != ECameraModeOld::Invalid)
			{
				m_cameraModes[(int)m_previousCameraMode]->DeactivateMode(*m_pTargetPlayer);

				if (m_cameraModes[(int)m_previousCameraMode]->CanTransition() && m_cameraModes[(int)m_currentCameraMode]->CanTransition())
				{
					UpdateTotalTransitionTime();

					if (m_transitionCameraMode != m_currentCameraMode)
					{
						m_transitionTime = m_totalTransitionTime;
					}
					else
					{
						//--- We are reversing a transition, reverse the time & carry on running
						m_transitionTime = m_totalTransitionTime - m_transitionTime;
					}
					m_transitionCameraMode = m_previousCameraMode;
				}
				else
				{
					m_totalTransitionTime = 0.0f;
					m_transitionTime = 0.0f;
					m_transitionCameraMode = ECameraModeOld::Invalid;
				}
			}
			m_cameraModes[(int)m_currentCameraMode]->ActivateMode(*m_pTargetPlayer);
			m_previousCameraMode = m_currentCameraMode;
		}

		assert(currentCameraMode >= ECameraModeOld::Invalid);
		assert(m_cameraModes[(int)currentCameraMode]);

		m_transitionTime = max(m_transitionTime - frameTime, 0.0f);

		m_cameraModes[(int)currentCameraMode]->Update(*m_pTargetPlayer, frameTime);
	}

	//void CPlayerCamera::PostUpdate(const QuatT& camDelta)
	//{
	//	//static bool playerWasSlidingLastFrame = false;

	//	//const bool playerIsSliding = m_targetPlayer.IsSliding();

	//	//ECameraModeOld camMode = GetCurrentCameraMode();

	//	float blendFactorTran = 0.0f;
	//	float blendFactorRot = 0.0f;

	//	//playerWasSlidingLastFrame = playerIsSliding;

	//	if ((blendFactorTran > 0.0f) || (blendFactorRot > 0.0f))
	//	{
	//		QuatT camDelta;
	//		camDelta.q.SetSlerp(IDENTITY, camDelta.q, blendFactorRot);
	//		camDelta.t = camDelta.t * blendFactorTran;

	//		Matrix34 camRotDelta;
	//		{
	//			CCamera camera = gEnv->pSystem->GetViewCamera();
	//			Matrix34 newMat;
	//			Matrix34 camMat = camera.GetMatrix();

	//			if (g_pGameCVars->cl_postUpdateCamera == 1)
	//			{
	//				camRotDelta.SetIdentity();
	//				camRotDelta.SetTranslation(camDelta.t);
	//			}
	//			else
	//			{
	//				camRotDelta.Set(Vec3Constants<f32>::fVec3_One, camDelta.q, camDelta.t);
	//			}

	//			if (g_pGameCVars->cl_postUpdateCamera == 3)
	//			{
	//				newMat = camRotDelta * camMat;
	//			}
	//			else
	//			{
	//				newMat = camMat * camRotDelta;
	//			}
	//			camera.SetMatrix(newMat);

	//			gEnv->pSystem->SetViewCamera(camera);
	//		}

	//		// Update player camera space position with blending 
	//		IEntity* pPlayerEntity = m_pTargetPlayer->GetEntity();
	//		if (pPlayerEntity)
	//		{
	//			const int slotIndex = 0;
	//			uint32 entityFlags = pPlayerEntity->GetSlotFlags(slotIndex);
	//			if (entityFlags & ENTITY_SLOT_RENDER_NEAREST)
	//			{
	//				Vec3 cameraSpacePos;
	//				pPlayerEntity->GetSlotCameraSpacePos(slotIndex, cameraSpacePos);
	//				cameraSpacePos = camRotDelta.GetInverted() * cameraSpacePos;
	//				pPlayerEntity->SetSlotCameraSpacePos(slotIndex, cameraSpacePos);
	//			}
	//		}
	//	}
	//}

	ECameraModeOld CPlayerCamera::GetCurrentCameraMode()
	{
		//Benito
		//Note: Perhaps the mode should be set instead from the current player state, instead of checking stats every frame
		//NB: Have started working towards this! [TF]

		//const SPlayerStats* pPlayerStats = static_cast<const SPlayerStats*>(m_targetPlayer.GetActorStats());
		//assert(pPlayerStats);

		////if (g_pGameCVars->g_deathCamSP.enable && m_ownerPlayer.IsDead() && !pPlayerStats->isRagDoll && !pPlayerStats->isThirdPerson
		////	&& m_ownerPlayer.GetHitDeathReactions() && !m_ownerPlayer.GetHitDeathReactions()->IsInReaction())
		////{
		////	return ECameraModeOld_Death;
		////}

		//if (pPlayerStats->isRagDoll)
		//{
		//	return ECameraModeOld::AnimationControlled;
		//}

		//if (m_targetPlayer.GetLinkedVehicle())
		//{
		//	return ECameraModeOld::Vehicle;
		//}

		//// TEMP: Any camera mode selected by the above code also needs to be turned OFF by this function.
		////
		//switch (m_currentCameraMode)
		//{
		//case ECameraModeOld::AnimationControlled:
		////case ECameraModeOld::Death:
		//case ECameraModeOld::Vehicle:
		//{
			return ECameraModeOld::Default;
		//}
		//}

		//return m_currentCameraMode;
	}

	void CPlayerCamera::FilterGroundOnlyShakes(SViewParams& viewParams)
	{
		//if (viewParams.groundOnly)
		//{
		//	const SActorStats* pActorStats = m_pTargetPlayer->GetActorStats();
		//	assert(pActorStats);
		//	if (pActorStats->inAir > 0.0f)
		//	{
		//		float factor = pActorStats->inAir;
		//		Limit(factor, 0.0f, 0.2f);
		//		factor = 1.0f - (factor * 5.0f);
		//		factor = factor * factor;

		//		viewParams.currentShakeQuat.SetSlerp(IDENTITY, viewParams.currentShakeQuat, factor);
		//		viewParams.currentShakeShift *= factor;
		//	}
		//}
	}

	void CPlayerCamera::UpdateTotalTransitionTime()
	{
		//bool alreadySetTotalTransitionTime = false;

		//if (m_currentCameraMode == ECameraModeOld_PartialAnimationControlled)
		//{
		//	if (m_targetPlayer.IsOnLedge())
		//	{
		//		m_enteredPartialAnimControlledCameraOnLedge = true;

		//		// entering anim control from ledge
		//		m_totalTransitionTime = g_pGameCVars->pl_cameraTransitionTimeLedgeGrabIn;
		//		alreadySetTotalTransitionTime = true;
		//	}
		//	else
		//	{
		//		m_enteredPartialAnimControlledCameraOnLedge = false;
		//	}
		//}
		//else if (m_previousCameraMode == ECameraModeOld_PartialAnimationControlled)
		//{
		//	if (m_enteredPartialAnimControlledCameraOnLedge)
		//	{
		//		// leaving anim control from ledge
		//		m_totalTransitionTime = g_pGameCVars->pl_cameraTransitionTimeLedgeGrabOut;
		//		alreadySetTotalTransitionTime = true;
		//	}
		//}

		//if (!alreadySetTotalTransitionTime)
		//{
			m_totalTransitionTime = g_pCVars->pl_cameraTransitionTime;
		//}
	}
}