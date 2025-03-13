#include "StdAfx.h"

#include "TPCameraComponent.h"
#include "CameraManagerComponent.h"
#include "Components/Actor/ActorComponent.h"
#include "Components/Player/PlayerComponent.h"
#include "Components/Player/Input/PlayerInputComponent.h"
#include "Utility/StringUtils.h"
#include <CryGame/GameUtils.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace
{
	static void RegisterTPCameraComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CTPCameraComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterTPCameraComponent)
}

namespace DoxD
{
	void CTPCameraComponent::Initialize()
	{
		m_targetEntityID = GetEntityId();
		m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();
		m_skipInterpolation = true;
	}

	void CTPCameraComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::Update:
			Update();
			UpdateView();
			break;
		}
	}

	void CTPCameraComponent::OnShutDown()
	{ }

	void CTPCameraComponent::Update()
	{
		UpdateZoom();
		UpdateCamera();
	}

	void CTPCameraComponent::UpdateZoom()
	{
		float tempZoomGoal = m_lastZoomGoal + m_pCameraManager->GetZoomDelta() * g_pCVars->m_actionRPGCameraZoomStep;

		m_zoomGoal = clamp_tpl(tempZoomGoal, g_pCVars->m_actionRPGCameraZoomMin, g_pCVars->m_actionRPGCameraZoomMax);
		m_lastZoomGoal = m_zoomGoal;

		Interpolate(m_zoomLevel, m_zoomGoal, g_pCVars->m_actionRPGCameraZoomSpeed, gEnv->pTimer->GetFrameTime());
	}

	void CTPCameraComponent::UpdateCamera()
	{
		auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		auto pPlayer = CPlayerComponent::GetLocalPlayer();
		auto pPlayerInput = pPlayer->GetPlayerInput();

		if (pPlayerInput)
		{
			if (pPlayer->GetInteractionState().IsCameraMovementAllowed())
			{
				m_viewPitch += pPlayerInput->GetMousePitchDelta();
				m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_pCVars->m_actionRPGCameraPitchMin), DEG2RAD(g_pCVars->m_actionRPGCameraPitchMax));
				m_viewYaw += pPlayerInput->GetMouseYawDelta();
			}

			// Yaw should wrap around if it exceeds it's values. This is a bit simplistic, but will work most of the time.
			if (m_viewYaw > gf_PI)
				m_viewYaw -= gf_PI2;
			if (m_viewYaw < -gf_PI)
				m_viewYaw += gf_PI2;

			if (pEntity && !gEnv->IsCutscenePlaying())
			{
				auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
				if (pTargetEntity)
				{
					// Calculate pitch and yaw movements to apply both prior to and after positioning the camera.
					Quat quatPreTransformYawPitch = Quat(Ang3(m_viewPitch, 0.0f, m_viewYaw));
					Quat quatPostTransformYawPitch = Quat::CreateRotationXYZ(Ang3(m_viewPitch * g_pCVars->m_actionRPGCameraReversePitchTilt, 0.0f, 0.0f));
					Vec3 vecTargetAimPosition = GetTargetAimPosition(pTargetEntity);

					// The distance from the view to the target.
					float calculatedZoomDistance = (m_zoomLevel * m_zoomLevel) / (g_pCVars->m_actionRPGCameraZoomMax * g_pCVars->m_actionRPGCameraZoomMax);
					float zoomDistance = g_pCVars->m_actionRPGCameraTargetDistance * calculatedZoomDistance;

					// Calculate the target rotation and SLERP it if we can.
					Quat quatTargetRotationGoal = m_quatTargetRotation * quatPreTransformYawPitch;
					Quat quatTargetRotation;
					float slerpSpeed = gEnv->pTimer->GetFrameTime() * g_pCVars->m_actionRPGCameraSlerpSpeed;
					
					if (m_skipInterpolation)
					{
						m_quatLastTargetRotation = quatTargetRotation = quatTargetRotationGoal;
					}
					else
					{
						quatTargetRotation = Quat::CreateSlerp(m_quatLastTargetRotation, quatTargetRotationGoal, slerpSpeed);
						m_quatLastTargetRotation = quatTargetRotation;
					}

					// Work out where to place the new initial position. We will be using a unit vector facing forward Y
					// as the starting place and applying rotations from the target bone and player camera movements.
					Vec3 viewPositionOffset = Vec3FromString(g_pCVars->m_actionRPGCameraViewPositionOffset->GetString());
					Vec3 vecViewPosition = vecTargetAimPosition + (quatTargetRotation * (FORWARD_DIRECTION * zoomDistance)) + quatTargetRotation * viewPositionOffset;

					// By default, we try and aim the camera at the target, taking into account the current mouse yaw and pitch values.
					// Since the target and aim can actually be the same we need to use a safe version of the normalised quaternion to
					// prevent errors.
					Quat quatViewRotationGoal = Quat::CreateRotationVDir((vecTargetAimPosition - vecViewPosition).GetNormalizedSafe());

					// Use a slerp to smooth out fast camera rotations.
					Quat quatViewRotation;
					if (m_skipInterpolation)
					{
						m_quatLastViewRotation = quatViewRotation = quatViewRotationGoal;
						m_skipInterpolation = false;
					}
					else
					{
						quatViewRotation = Quat::CreateSlerp(m_quatLastViewRotation, quatViewRotationGoal, slerpSpeed);
						m_quatLastViewRotation = quatViewRotation;
					}

					// Apply a final translation to both the view position and the aim position.
					Vec3 aimPositionOffset = Vec3FromString(g_pCVars->m_actionRPGCameraAimPositionOffset->GetString());
					vecViewPosition += quatViewRotation * aimPositionOffset;

					// Gimbal style rotation after it's moved into it's initial position.
					Quat quatOrbitRotation = quatViewRotation * quatPostTransformYawPitch;

					// Perform a collision detection. Note, any collisions will disallow use of interpolated camera movement.
					DetectCollision(vecTargetAimPosition, vecViewPosition);

#if defined(_DEBUG)
					if (g_pCVars->m_actionRPGCameraDebug)
					{
						gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition, 0.1f, ColorB(128, 0, 0));
						gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition + quatTargetRotation * viewPositionOffset, 0.1f, ColorB(0, 128, 0));
					}
#endif

					// Track our last position.
					m_vecLastPosition = vecViewPosition;

					// Return the newly calculated pose.
					m_cameraMatrix = Matrix34::Create(Vec3(1.0f), quatOrbitRotation, vecViewPosition + quatOrbitRotation * m_pCameraManager->GetViewOffset());

					return;
				}
			}
		}

		// The calulation failed somewhere, so just use an default matrix.
		m_cameraMatrix = Matrix34::Create(Vec3(1.0f), IDENTITY, ZERO);

		// If we made it here, we will want to avoid interpolating, since the last frame result will be wrong in some way.
		m_skipInterpolation = true;

		// Camera spring arm system ported from UE

//		// Update desired arm location
//		const Quat desiredRotationQuat = Quat::CreateSlerp(m_lastFrameDesiredRotation, player.GetLookOrientation(), m_cameraRotationLagSpeed * frameTime);
//		const Ang3 desiredRotationYPR = CCamera::CreateAnglesYPR(Matrix33(desiredRotationQuat));
//
//		m_lastFrameDesiredRotation = desiredRotationQuat;
//
//		// Get the spring arm 'origin', the target we want to look at
//		Vec3 armOrigin = player.GetEntity()->GetWorldPos() + Vec3(0, 0, g_pCVars->cm_tpCameraDistanceVertical);
//
//		// We lag the target, not the actual camera position, so rotating the camera around does not have lag
//		Vec3 desiredLocation = armOrigin;
//
//		// Do location lag
//		desiredLocation = Vec3::CreateLerp(m_lastFrameDesiredLocation, desiredLocation, m_cameraLagSpeed * frameTime);
//
//		m_lastFrameArmOrigin = armOrigin;
//		m_lastFrameDesiredLocation = desiredLocation;
//
//#ifndef _RELEASE
//		if (g_pCVars->cm_debugCamera)
//		{
//			IRenderAuxGeom* pRender = gEnv->pRenderer->GetIRenderAuxGeom();
//
//			pRender->DrawSphere(armOrigin, .03f, Col_Green, false);
//			pRender->DrawSphere(desiredLocation, .015f, Col_Green, false);
//
//			const Vec3 toOrigin = armOrigin - desiredLocation;
//
//			pRender->DrawLine(desiredLocation, Col_Green, desiredLocation + toOrigin * 0.5f, Col_Green, 3.f);
//			pRender->DrawLine(desiredLocation + toOrigin * 0.5f, Col_Green, armOrigin, Col_Green, 3.f);
//		}
//#endif	
//
//		// Now offset camera position back along our rotation
//		desiredLocation -= CCamera::CreateViewdir(desiredRotationYPR) * m_targetArmLength;
//
//		// Add socket offset in local space
//		desiredLocation += Matrix34(desiredRotationQuat).TransformVector(m_socketOffset);
//
//		const Vec3 cameraDirection = (Vec3Constants<float>::fVec3_OneY * desiredRotationQuat.GetInverted()) * m_targetArmLength;
//		const Vec3 crosshairPosition = desiredLocation + cameraDirection;
//
//		// Handle camera collisions
//		if (g_pCVars->cm_tpCameraCollision)
//		{
//			IPhysicalEntity* pSkipEnts[5];
//			const int nSkipEnts = player.GetPhysicalSkipEntities(pSkipEnts, CRY_ARRAY_COUNT(pSkipEnts));
//
//			primitives::sphere spherePrim;
//			spherePrim.center = crosshairPosition;
//			spherePrim.r = g_pCVars->cm_tpCameraCollisionOffset;
//
//			float fDist = gEnv->pPhysicalWorld->PrimitiveWorldIntersection(
//				spherePrim.type, &spherePrim, -cameraDirection, ent_all, nullptr, 0, geom_colltype0, nullptr, nullptr, 0, pSkipEnts, nSkipEnts);
//
//			if (fDist <= 0.0f)
//			{
//				fDist = m_targetArmLength;
//			}
//
//			fDist = max(g_pCVars->cm_tpMinDist, fDist);
//
//			Interpolate(m_lastTpvDistance, fDist, g_pCVars->m_cameraInterpolationSpeed, frameTime);
//			desiredLocation = crosshairPosition - (cameraDirection.GetNormalized() * m_lastTpvDistance);
//		}
//
//		// Form a transform for new world transform for camera
//		CCamera camera = gEnv->pSystem->GetViewCamera();
//		Matrix34 finalTransformTM = Matrix34::CreateTranslationMat(desiredLocation) * Matrix34(desiredRotationQuat);
//
//		camera.SetMatrix(finalTransformTM);
//
//		gEnv->pSystem->SetViewCamera(camera);
	}

	void CTPCameraComponent::AttachToEntity(EntityId entityID)
	{
		m_targetEntityID = entityID;

		// No interpolation, since the camera needs to jump into position.
		m_skipInterpolation = true;

		// Resolve the entity ID if possible.
		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		
		if (pTargetEntity)
		{
			// Give the camera an initial orientation based on the entity's rotation.
			m_quatTargetRotation = Quat::CreateRotationVDir(pTargetEntity->GetForwardDir());
		}
		else
		{
			// Use a sensible default value.
			m_quatTargetRotation = Quat(IDENTITY);
		}

		// Reset this.
		m_quatLastTargetRotation = m_quatTargetRotation;
	}

	void CTPCameraComponent::OnActivate()
	{
		m_EventFlags |= EEntityEvent::Update;
		GetEntity()->UpdateComponentEventMask(this);
		ResetCamera();

		// Avoid interpolation after activating the camera, there is no-where to interpolate from.
		m_skipInterpolation = true;
	}

	void CTPCameraComponent::OnDeactivate()
	{
		m_EventFlags &= ~EEntityEvent::Update;
		GetEntity()->UpdateComponentEventMask(this);
	}
	
	void CTPCameraComponent::ResetCamera()
	{
		// Default is for zoom level to be as close as possible to fully zoomed in. When toggling from first to third
		// person camera, this gives the least amount of jerking. 
		m_lastZoomGoal = m_zoomGoal = m_zoomLevel = g_pCVars->m_actionRPGCameraZoomMin;

		// Pitch and yaw can default to their mid-points.
		m_viewPitch = (DEG2RAD(g_pCVars->m_actionRPGCameraPitchMax) + DEG2RAD(g_pCVars->m_actionRPGCameraPitchMin)) / 2;

		// We'll start from behind our character as a default.
		if (auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID))
		{
			// The camera should default to being behind the actor.
			Vec3 backward = pTargetEntity->GetForwardDir();
			backward.Flip();
			m_viewYaw = CCamera::CreateAnglesYPR(backward).x;
		}
		else
		{
			m_viewYaw = 180.0f;
		}
	}
	
	Vec3 CTPCameraComponent::GetTargetAimPosition(IEntity* const pEntity)
	{
		Vec3 position{ AverageEyePosition };

		if (pEntity)
		{
			// If we are attached to an entity that is an actor we can use their eye position.
			Vec3 localEyePosition = position;
			auto pActor = CActorComponent::GetActor(m_targetEntityID);
			if (pActor)
				localEyePosition = pActor->GetLocalEyePos();

			// Pose is based on entity position and the eye position.
			// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
			position = pEntity->GetPos() + localEyePosition;

#if defined(_DEBUG)
			if (g_pCVars->m_actionRPGCameraDebug)
			{
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.04f, ColorB(0, 0, 255, 255));
			}
#endif
		}

		return position;
	}

	bool CTPCameraComponent::DetectCollision(const Vec3& Goal, Vec3& CameraPosition)
	{
		bool updatedCameraPosition = false;

		// Skip the target actor for this.
		ray_hit rayhit;
		static IPhysicalEntity* pSkipEnts[10];
		pSkipEnts[0] = gEnv->pEntitySystem->GetEntity(m_targetEntityID)->GetPhysics();

		// Perform the ray cast.
		int hits = gEnv->pPhysicalWorld->RayWorldIntersection(Goal,
			CameraPosition - Goal,
			ent_static | ent_sleeping_rigid | ent_rigid | ent_independent | ent_terrain,
			rwi_stop_at_pierceable | rwi_colltype_any,
			&rayhit,
			1, pSkipEnts, 2);

		if (hits)
		{
			CameraPosition = rayhit.pt;
			updatedCameraPosition = true;
		}

		return updatedCameraPosition;
	}
}