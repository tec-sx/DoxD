#include "CameraModes.h"

#include "Components/Player/Player.h"
#include "Components/Camera/PlayerCamera.h"
#include "Utility/CryWatch.h"

#include <CryMath/Angle.h>
#include <Cry3DEngine/I3DEngine.h>                       
#include <CryAISystem/IAgent.h>                          
#include <CryAISystem/IAISystem.h>                       
#include <CryAnimation/ICryAnimation.h>                  
#include <CryCore/StlUtils.h>                            
#include <CryEntitySystem/IEntity.h>                     
#include <CryEntitySystem/IEntitySystem.h>               
#include <CryGame/CoherentValue.h>                       
#include <CryGame/GameUtils.h>
#include <CryGame/IGameFramework.h>                      
#include <CryMath/Cry_Camera.h>                          
#include <CryMath/Cry_Geo.h>                             
#include <CryPhysics/physinterface.h>
#include <CryPhysics/primitives.h>

#include <DefaultComponents/Cameras/CameraComponent.h>

#include <CryRenderer/IRenderAuxGeom.h>                      
#include <CryRenderer/Tarray.h>

namespace DoxD
{
	// Default Camera Mode
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CDefaultCameraMode::CDefaultCameraMode(Cry::DefaultComponents::CCameraComponent* camera)
		: ICameraMode(camera)
	{
		m_targetArmLength = g_pGameCVars->cm_tpvDist;
		m_lastTpvDistance = m_targetArmLength;
		m_cameraLagSpeed = 10;
		m_cameraRotationLagSpeed = 8;
		m_cameraLagMaxDistance = 0;
	}

	void CDefaultCameraMode::Update(const CPlayerComponent& player, float frameTime)
	{
		// Update desired arm location
		const Quat desiredRotationQuat = Quat::CreateSlerp(m_lastFrameDesiredRotation, player.GetLookOrientation(), m_cameraRotationLagSpeed * frameTime);
		const Ang3 desiredRotationYPR = CCamera::CreateAnglesYPR(Matrix33(desiredRotationQuat));

		m_lastFrameDesiredRotation = desiredRotationQuat;

		// Get the spring arm 'origin', the target we want to look at
		Vec3 armOrigin = player.GetEntity()->GetWorldPos() + Vec3(0, 0, g_pGameCVars->cm_tpvDistVertical);

		// We lag the target, not the actual camera position, so rotating the camera around does not have lag
		Vec3 desiredLocation = armOrigin;

		// Do location lag
		desiredLocation = Vec3::CreateLerp(m_lastFrameDesiredLocation, desiredLocation, m_cameraLagSpeed * frameTime);

		m_lastFrameArmOrigin = armOrigin;
		m_lastFrameDesiredLocation = desiredLocation;

#ifndef _RELEASE
		if (g_pGameCVars->cm_debugCamera)
		{
			IRenderAuxGeom* pRender = gEnv->pRenderer->GetIRenderAuxGeom();

			pRender->DrawSphere(armOrigin, .03f, Col_Green, false);
			pRender->DrawSphere(desiredLocation, .015f, Col_Green, false);

			const Vec3 toOrigin = armOrigin - desiredLocation;

			pRender->DrawLine(desiredLocation, Col_Green, desiredLocation + toOrigin * 0.5f, Col_Green, 3.f);
			pRender->DrawLine(desiredLocation + toOrigin * 0.5f, Col_Green, armOrigin, Col_Green, 3.f);
		}
#endif	

		// Now offset camera position back along our rotation
		desiredLocation -= CCamera::CreateViewdir(desiredRotationYPR) * m_targetArmLength;

		// Add socket offset in local space
		desiredLocation += Matrix34(desiredRotationQuat).TransformVector(m_socketOffset);
		
		const Vec3 cameraDirection = (Vec3Constants<float>::fVec3_OneY * desiredRotationQuat.GetInverted()) * m_targetArmLength;
		const Vec3 crosshairPosition = desiredLocation + cameraDirection;
		
		// Handle camera collisions
		if (g_pGameCVars->cm_tpvCameraCollision)
		{
			IPhysicalEntity* pSkipEnts[5];
			const int nSkipEnts = player.GetPhysicalSkipEntities(pSkipEnts, CRY_ARRAY_COUNT(pSkipEnts));

			primitives::sphere spherePrim;
			spherePrim.center = crosshairPosition;
			spherePrim.r = g_pGameCVars->cm_tpvCameraCollisionOffset;

			float fDist = gEnv->pPhysicalWorld->PrimitiveWorldIntersection(
				spherePrim.type, &spherePrim, -cameraDirection, ent_all, nullptr, 0, geom_colltype0, nullptr, nullptr, 0, pSkipEnts, nSkipEnts);

			if (fDist <= 0.0f)
			{
				fDist = m_targetArmLength;
			}

			fDist = max(g_pGameCVars->cm_tpvMinDist, fDist);

			Interpolate(m_lastTpvDistance, fDist, g_pGameCVars->cm_tpvInterpolationSpeed, frameTime);
			desiredLocation = crosshairPosition - (cameraDirection.GetNormalized() * m_lastTpvDistance);
		}	

		// Form a transform for new world transform for camera
		CCamera camera = gEnv->pSystem->GetViewCamera();
		Matrix34 finalTransformTM = Matrix34::CreateTranslationMat(desiredLocation) * Matrix34(desiredRotationQuat);

		camera.SetMatrix(finalTransformTM);
		
		gEnv->pSystem->SetViewCamera(camera);
	}

	// Animation Controled Camera Mode
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CAnimationControlledCameraMode::CAnimationControlledCameraMode(Cry::DefaultComponents::CCameraComponent* camera)
		: ICameraMode(camera)
	{}

	void CAnimationControlledCameraMode::Update(const CPlayerComponent& player, float frameTime)
	{
	}


	// Focus Camera Mode
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CFocusCameraMode::CFocusCameraMode(Cry::DefaultComponents::CCameraComponent* camera)
		: CAnimationControlledCameraMode(camera)
		, m_fFocusDistance(0)
		, m_fFocusRange(0)
	{ }

	CFocusCameraMode::~CFocusCameraMode()
	{
	}

	void CFocusCameraMode::Update(const CPlayerComponent& player, float frameTime)
	{
	}

	void CFocusCameraMode::Activate(const CPlayerComponent& player)
	{
	}

	void CFocusCameraMode::Deactivate(const CPlayerComponent& player)
	{
	}

	bool CFocusCameraMode::IsEntityInFrustrum(EntityId entityId, const CPlayerComponent& player) const
	{
		return false;
	}

	void CFocusCameraMode::Init(const CPlayerComponent* subject, const EntityId killerEid)
	{
	}


	// Vehicle Camera Mode
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CVehicleCameraMode::CVehicleCameraMode(Cry::DefaultComponents::CCameraComponent* camera)
		: ICameraMode(camera)
	{
	}

	void CVehicleCameraMode::Update(const CPlayerComponent& player, float frameTime)
	{
	}
}