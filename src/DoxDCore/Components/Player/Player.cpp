// Copyright 2016-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"

#include <CryEntitySystem/IEntityComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>
#include <DefaultComponents/Cameras/CameraComponent.h>
#include <CryGame/GameUtils.h>

#include "Components/Player/Camera/PlayerCamera.h"
#include "Components/SpawnPoint.h"

#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

namespace
{
	static void RegisterPlayerComponentOld(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CPlayerComponentOld));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponentOld);
}

namespace DoxD
{
	SAimAccelerationParams::SAimAccelerationParams()
		: angle_min(-80.0f)
		, angle_max(80.0f)
	{ }

	void SPlayerRotationParams::Reset(const IItemParamsNode* pRootNode)
	{
		const IItemParamsNode* pParamNode = pRootNode->GetChild("PlayerRotation");

		pParamNode = pRootNode->GetChild("PlayerRotation3rdPerson");

		if (pParamNode)
		{
			ReadAimParams(pParamNode, "Normal", EAimType_NORMAL);
			ReadAimParams(pParamNode, "Crouch", EAimType_CROUCH);
			ReadAimParams(pParamNode, "Sliding", EAimType_SLIDING);
			ReadAimParams(pParamNode, "Sprinting", EAimType_SPRINTING);
			ReadAimParams(pParamNode, "Swim", EAimType_SWIM);
			ReadAimParams(pParamNode, "MountedGun", EAimType_MOUNTED_GUN);
		}
	}

	void SPlayerRotationParams::ReadAimParams(const IItemParamsNode* pRootNode, const char* aimTypeName, EAimType aimType)
	{
		const IItemParamsNode* pAimNode = pRootNode->GetChild(aimTypeName);

		if (pAimNode)
		{
			ReadAccelerationParams(pAimNode->GetChild("Horizontal"), &m_horizontalAims[aimType]);
			ReadAccelerationParams(pAimNode->GetChild("Vertical"), &m_verticalAims[aimType]);
		}
	}

	void SPlayerRotationParams::ReadAccelerationParams(const IItemParamsNode* pNode, SAimAccelerationParams* output)
	{
		if (pNode)
		{
			pNode->GetAttribute("angle_min", output->angle_min);
			pNode->GetAttribute("angle_max", output->angle_max);
		}
	}

	// Player Component
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CPlayerComponentOld::CPlayerComponentOld()
		: m_pInputComponent(nullptr)
		, m_pCharacterController(nullptr)
		, m_pAnimationComponent(nullptr)
		, m_pAudioListener(nullptr)
		, m_pPlayerCamera(nullptr)
		, m_currentYaw(IDENTITY)
		, m_currentPitch(0.f)
		, m_movementDelta(ZERO)
		, m_lastFrameDesiredRotation(IDENTITY)

	{
		m_pPlayerCamera = new CPlayerCamera(this);
	}

	CPlayerComponentOld::~CPlayerComponentOld()
	{
		SAFE_DELETE(m_pPlayerCamera);
	}

	void CPlayerComponentOld::Initialize()
	{
		// Initialize Components
		//----------------------
		m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
		m_pAudioListener = m_pEntity->GetOrCreateComponent<Cry::Audio::DefaultComponents::CListenerComponent>();
		m_pAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();

		// Initialize Camera
		//----------------------
		m_pPlayerCamera->Initialize();

		// Initialize Character
		//---------------------
		// Set the player geometry, this also triggers physics proxy creation
		m_pAnimationComponent->SetMannequinAnimationDatabaseFile("Animations/Mannequin/ADB/FirstPerson.adb");
		m_pAnimationComponent->SetCharacterFile("Objects/Characters/PuffyDelite/puffydelite.cdf");

		m_pAnimationComponent->SetControllerDefinitionFile("Animations/Mannequin/ADB/FirstPersonControllerDefinition.xml");
		m_pAnimationComponent->SetDefaultScopeContextName("ThirdPersonCharacter");

		// Queue the idle fragment to start playing
		m_pAnimationComponent->SetDefaultFragmentName("Idle");

		// Disable root motion
		m_pAnimationComponent->SetAnimationDrivenMotion(true);

		m_pAnimationComponent->LoadFromDisk();

		// Acquire fragment and tag identifiers to avoid doing so each update
		m_idleFragmentId = m_pAnimationComponent->GetFragmentId("Idle");
		m_walkFragmentId = m_pAnimationComponent->GetFragmentId("Walk");
		m_rotateTagId = m_pAnimationComponent->GetTagId("Rotate");

		Reset();

		m_pEntity->SetWorldTM(CSpawnPointComponent::GetFirstSpawnPointTransform());
	}

	Cry::Entity::EventFlags CPlayerComponentOld::GetEventMask() const
	{
		return Cry::Entity::EEvent::GameplayStarted
			| Cry::Entity::EEvent::Update
			| Cry::Entity::EEvent::Reset
			| Cry::Entity::EEvent::EditorPropertyChanged
			| Cry::Entity::EEvent::PhysicalTypeChanged;
	}

	void CPlayerComponentOld::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case Cry::Entity::EEvent::GameplayStarted:
		{
			RecenterCollider();
			Reset();
		}
		break;
		case Cry::Entity::EEvent::Update:
		{
			const float frameTime = event.fParam[0];

			TryUpdateStance();
			UpdateMovement(frameTime);

			UpdateLookDirectionRequest(frameTime);
			UpdateAnimation(frameTime);

			m_pPlayerCamera->Update(frameTime);
			m_pAudioListener->SetOffset(gEnv->pSystem->GetViewCamera().GetPosition());
		}
		break;
		case Cry::Entity::EEvent::PhysicalTypeChanged:
		{
			RecenterCollider();
		}
		break;
		case Cry::Entity::EEvent::EditorPropertyChanged:
		case Cry::Entity::EEvent::Reset:
		{
			Reset();
		}
		break;
		}
	}

	SPlayerRotationParams::EAimType CPlayerComponentOld::GetCurrentAimType() const
	{
		//EStance playerStance = GetStance();

		//if (IsSliding())
		//{
		//	return SPlayerRotationParams::EAimType_SLIDING;
		//}
		//else if (IsSprinting())
		//{
		//	return SPlayerRotationParams::EAimType_SPRINTING;
		//}
		//else if (playerStance == STANCE_CROUCH)
		//{
		//	return SPlayerRotationParams::EAimType_CROUCH;
		//}
		//else if (IsSwimming())
		//{
		//	return SPlayerRotationParams::EAimType_SWIM;
		//}
		//else if (GetCurrentItem() && static_cast<CItem*>(GetCurrentItem())->IsMounted())
		//{
		//	return SPlayerRotationParams::EAimType_MOUNTED_GUN;
		//}

		return SPlayerRotationParams::EAimType_NORMAL;
	}

	Vec3 CPlayerComponentOld::GetStanceViewOffset(EStance stance, const float* pLeanAmt, bool withY, const bool useWhileLeanedOffsets) const
	{
		return Vec3();
	}

	void CPlayerComponentOld::Reset()
	{
		InitializeInput();

		// Reset player states
		m_currentPlayerState = EPlayerState::Walking;
		m_currentStance = EPlayerStance::Standing;
		m_desiredStance = m_currentStance;

		// Reset Input/Controller data
		m_movementDelta = ZERO;
		m_currentYaw = Quat::CreateRotationZ(m_pEntity->GetWorldRotation().GetRotZ());
		m_currentPitch = 0.f;

		m_mouseDeltaRotation = ZERO;
		m_mouseDeltaSmoothingFilter.Reset();
		m_lookOrientation = IDENTITY;
		m_horizontalAngularVelocity = 0.0f;
		m_averagedHorizontalAngularVelocity.Reset();

		// Reset camera lerp end position to standing camera position
		//m_cameraEndOffset = m_cameraOffsetStanding;

		m_activeFragmentId = FRAGMENT_ID_INVALID;

		m_stance = STANCE_NULL;
		m_stance = STANCE_NULL;
	}

	void CPlayerComponentOld::RecenterCollider()
	{
		static bool skip = false;
		if (skip)
		{
			skip = false;
			return;
		}

		auto pCharacterController = m_pEntity->GetComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		if (pCharacterController == nullptr)
		{
			return;
		}

		const auto& physParams = pCharacterController->GetPhysicsParameters();
		float heightOffset = physParams.m_height * 0.5f;

		if (physParams.m_bCapsule)
		{
			heightOffset = heightOffset * 0.5f + physParams.m_radius * 0.5f;
		}

		pCharacterController->SetTransformMatrix(Matrix34(IDENTITY, Vec3(0.f, 0.f, 0.005f + heightOffset)));

		skip = true;

		pCharacterController->Physicalize();
	}

	void CPlayerComponentOld::InitializeInput()
	{
		// Keyboard
		m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { m_movementDelta.y = value; });
		m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);

		m_pInputComponent->RegisterAction("player", "movebackward", [this](int activationMode, float value) { m_movementDelta.y = -value; });
		m_pInputComponent->BindAction("player", "movebackward", eAID_KeyboardMouse, eKI_S);

		m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { m_movementDelta.x = value; });
		m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);

		m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { m_movementDelta.x = -value; });
		m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);

		m_pInputComponent->RegisterAction("player", "sprint", [this](int activationMode, float value)
			{
				if (activationMode & eAAM_OnPress)
				{
					m_currentPlayerState = EPlayerState::Sprinting;
				}
				else if (activationMode & eAAM_OnRelease)
				{
					m_currentPlayerState = EPlayerState::Walking;
				}
			});
		m_pInputComponent->BindAction("player", "sprint", eAID_KeyboardMouse, eKI_LShift);

		m_pInputComponent->RegisterAction("player", "jump", [this](int         activationMode, float value)
			{
				if (m_pCharacterController->IsOnGround())
				{
					m_pCharacterController->AddVelocity(Vec3(0, 0, m_jumpHeight));
				}
			});
		m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);

		m_pInputComponent->RegisterAction("player", "crouch", [this](int activationMode, float value)
			{
				if (activationMode & (int)eAAM_OnPress)
				{
					m_desiredStance = EPlayerStance::Crouching;
				}
				else if (activationMode & (int)eAAM_OnRelease)
				{
					m_desiredStance = EPlayerStance::Standing;
				}
			});
		m_pInputComponent->BindAction("player", "crouch", eAID_KeyboardMouse, eKI_LCtrl);

		// Mouse Input
		m_pInputComponent->RegisterAction("player", "yaw", [this](int activationMode, float value) { m_mouseDeltaRotation.y = -value; });
		m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseY);

		m_pInputComponent->RegisterAction("player", "pitch", [this](int activationMode, float value) { m_mouseDeltaRotation.x = -value; });
		m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseX);
	}

	void CPlayerComponentOld::UpdateLookDirectionRequest(float frameTime)
	{
		const float rotationSpeed = 0.002f;
		const float rotationLimitsMinPitch = -0.84f;
		const float rotationLimitsMaxPitch = 1.5f;

		// Apply smoothing filter to the mouse input
		m_mouseDeltaRotation = m_mouseDeltaSmoothingFilter.Push(m_mouseDeltaRotation).Get();

		if (!m_mouseDeltaRotation.IsZero())
		{
			// Update angular velocity metrics
			m_horizontalAngularVelocity = (m_mouseDeltaRotation.x * rotationSpeed) / frameTime;
			m_averagedHorizontalAngularVelocity.Push(m_horizontalAngularVelocity);

			// Start with updating look orientation from the latest input
			Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

			// Yaw
			ypr.x += m_mouseDeltaRotation.x * rotationSpeed;

			// Pitch
			// TODO: Perform soft clamp here instead of hard wall, should reduce rot speed in this direction when close to limit.
			ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * rotationSpeed, rotationLimitsMinPitch, rotationLimitsMaxPitch);

			// Roll (skip)
			ypr.z = 0;

			m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));

			// Reset the mouse delta accumulator every frame
			m_mouseDeltaRotation = ZERO;
		}
	}

	void CPlayerComponentOld::UpdateMovement(float frameTime)
	{
		if (!m_pCharacterController->IsOnGround())
			return;

		Vec3 velocity = Vec3(m_movementDelta.x, m_movementDelta.y, 0.0f);
		velocity.Normalize();
		const float playerMoveSpeed = m_currentPlayerState == EPlayerState::Sprinting ? m_runSpeed : m_walkSpeed;
		m_pCharacterController->AddVelocity(m_pEntity->GetWorldRotation() * velocity * playerMoveSpeed * frameTime);
	}

	void CPlayerComponentOld::UpdateAnimation(float frameTime)
	{
		const float angularVelocityTurningThreshold = 0.174; // [rad/s]

		// Update tags and motion parameters used for turning
		//const bool isTurning = std::abs(m_averagedHorizontalAngularVelocity.Get()) > angularVelocityTurningThreshold;
		//m_pAnimationComponent->SetTagWithId(m_rotateTagId, isTurning);
		//if (isTurning)
		//{
		//	// TODO: This is a very rough predictive estimation of eMotionParamID_TurnAngle that could easily be replaced with accurate reactive motion 
		//	// if we introduced IK look/aim setup to the character's model and decoupled entity's orientation from the look direction derived from mouse input.

		//	const float turnDuration = 1.0f; // Expect the turning motion to take approximately one second.
		//	m_pAnimationComponent->SetMotionParameter(eMotionParamID_TurnAngle, m_horizontalAngularVelocity * turnDuration);
		//}

		// Update active fragment
		const FragmentID& desiredFragmentId = m_pCharacterController->IsWalking() ? m_walkFragmentId : m_idleFragmentId;
		if (m_activeFragmentId != desiredFragmentId)
		{
			m_activeFragmentId = desiredFragmentId;
			m_pAnimationComponent->QueueFragmentWithId(m_activeFragmentId);
		}

		// Update entity rotation as the player turns
		// We only want to affect Z-axis rotation, zero pitch and roll

		
		if (!m_pCharacterController->GetVelocity().IsZero())
		{
			Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
			ypr.y = 0;
			ypr.z = 0;
			const float rotationSpeed = m_currentPlayerState == EPlayerState::Sprinting ? m_rotationSpeedRunning : m_rotationSpeedWalking;
			const Quat targetRotation = Quat(CCamera::CreateOrientationYPR(ypr));
			const Quat finalRotation = Quat::CreateSlerp(m_lastFrameDesiredRotation, targetRotation, rotationSpeed * frameTime);
			
			m_lastFrameDesiredRotation = finalRotation;

			// Send updated transform to the entity, only orientation changes
			GetEntity()->SetPosRotScale(GetEntity()->GetWorldPos(), finalRotation, Vec3(1, 1, 1));
		}
	}

	void CPlayerComponentOld::TryUpdateStance()
	{
		if (m_desiredStance == m_currentStance)
			return;

		IPhysicalEntity* pPhysEnt = m_pEntity->GetPhysicalEntity();

		if (pPhysEnt == nullptr)
			return;

		const float radius = m_pCharacterController->GetPhysicsParameters().m_radius * 0.5f;

		float height = 0.f;
		//Vec3 camOffset = ZERO;

		switch (m_desiredStance)
		{
		case EPlayerStance::Crouching:
		{
			height = m_capsuleHeightCrouching;
			//camOffset = m_cameraOffsetCrouching;
		}
		break;
		case EPlayerStance::Standing:
		{
			height = m_capsuleHeightStanding;
			//camOffset = m_cameraOffsetStanding;

			primitives::capsule capsule;

			capsule.axis.Set(0, 0, 1);

			capsule.center = m_pEntity->GetWorldPos() + Vec3(0, 0, m_capsuleGroundOffset + radius + height * 0.5f);
			capsule.r = radius;
			capsule.hh = height * 0.5f;

			if (IsCapsuleIntersectingGeometry(capsule))
			{
				return;
			}
		}
		break;
		}

		pe_player_dimensions playerDimensions;
		pPhysEnt->GetParams(&playerDimensions);

		playerDimensions.heightCollider = m_capsuleGroundOffset + radius + height * 0.5f;
		playerDimensions.sizeCollider = Vec3(radius, radius, height * 0.5f);
		//m_cameraEndOffset = camOffset;
		m_currentStance = m_desiredStance;

		pPhysEnt->SetParams(&playerDimensions);
	}

	bool CPlayerComponentOld::IsCapsuleIntersectingGeometry(const primitives::capsule& capsule) const
	{
		IPhysicalEntity* pPhysEnt = m_pEntity->GetPhysicalEntity();

		if (pPhysEnt == nullptr)
			return false;

		IPhysicalWorld::SPWIParams pwiParams;

		pwiParams.itype = capsule.type;
		pwiParams.pprim = &capsule;

		pwiParams.pSkipEnts = &pPhysEnt;
		pwiParams.nSkipEnts = 1;

		intersection_params intersectionParams;
		intersectionParams.bSweepTest = false;
		pwiParams.pip = &intersectionParams;

		const int contactCount = static_cast<int>(gEnv->pPhysicalWorld->PrimitiveWorldIntersection(pwiParams));
		return contactCount > 0;
	}

	int CPlayerComponentOld::GetPhysicalSkipEntities(IPhysicalEntity** pSkipList, const int maxSkipSize) const
	{
		int skipCount = 0;

		if (maxSkipSize > 0)
		{
			// For now we just care about pick and throw objects
			//EntityId pickAndThrowHeldObjectId = GetPickAndThrowEntity();
			//if (IsInPickAndThrowMode() && pickAndThrowHeldObjectId)
			//{
			//	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(pickAndThrowHeldObjectId);
			//	if (pEntity)
			//	{
			//		IPhysicalEntity* pPhysEnt = pEntity->GetPhysics();
			//		if (pPhysEnt)
			//		{
			//			++skipCount;
			//			*pSkipList = pPhysEnt;
			//		}
			//	}
			//}
			//else
			//{
			IPhysicalEntity* pPlayerPhysics = GetEntity()->GetPhysics();
			if (pPlayerPhysics)
			{
				if (skipCount < maxSkipSize)
				{
					pSkipList[skipCount] = pPlayerPhysics;
					++skipCount;
				}
			}
			ICharacterInstance* pChar = GetEntity()->GetCharacter(0);
			if (pChar)
			{
				ISkeletonPose* pPose = pChar->GetISkeletonPose();
				if (pPose)
				{
					if (skipCount < maxSkipSize)
					{
						pSkipList[skipCount] = pPose->GetCharacterPhysics();
						++skipCount;
					}
				}
			}
		}
		// }
		return skipCount;
	}
}