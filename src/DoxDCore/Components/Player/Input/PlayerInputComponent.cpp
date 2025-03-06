#include "PlayerInputComponent.h"

#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <CryMath/Cry_Math.h>
#include <Components/Actor/ActorComponent.h>
#include "Components/Player/PlayerComponent.h"
#include <Console/GameCVars.h>

namespace
{
	static void RegisterPlayerInputComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CPlayerInputComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerInputComponent)
}

namespace DoxD
{
	void CPlayerInputComponent::Initialize()
	{
		m_pPlayer = m_pEntity->GetComponent<CPlayerComponentOld>();

		if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
		{
			RegisterActionMaps();
		}
	}
	void CPlayerInputComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::Update:
			Update();
			break;
		}
	}

	void CPlayerInputComponent::RegisterActionMaps()
	{
		m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

		// Escape.
		m_pInputComponent->RegisterAction("player", "special_esc", [this](int activationMode, float value) { OnActionEscape(activationMode, value); });
		m_pInputComponent->BindAction("player", "special_esc", eAID_KeyboardMouse, EKeyId::eKI_Escape);

		// Interaction.
		m_pInputComponent->RegisterAction("player", "player_interaction", [this](int activationMode, float value) { OnActionInteraction(activationMode, value); });
		m_pInputComponent->BindAction("player", "player_interaction", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);

		// Move left.
		m_pInputComponent->RegisterAction("player", "move_left", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Left, activationMode); });
		m_pInputComponent->BindAction("player", "move_left", eAID_KeyboardMouse, EKeyId::eKI_A);

		// Move right.
		m_pInputComponent->RegisterAction("player", "move_right", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Right, activationMode); });
		m_pInputComponent->BindAction("player", "move_right", eAID_KeyboardMouse, EKeyId::eKI_D);

		// Move forward.
		m_pInputComponent->RegisterAction("player", "move_forward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Forward, activationMode); });
		m_pInputComponent->BindAction("player", "move_forward", eAID_KeyboardMouse, EKeyId::eKI_W);

		// Move backward.
		m_pInputComponent->RegisterAction("player", "move_backward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Backward, activationMode); });
		m_pInputComponent->BindAction("player", "move_backward", eAID_KeyboardMouse, EKeyId::eKI_S);

		// Mouse yaw and pitch handlers.
		m_pInputComponent->RegisterAction("player", "rota_teyaw", [this](int activationMode, float value) { OnActionRotateYaw(activationMode, value); });
		m_pInputComponent->BindAction("player", "rotate_yaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

		m_pInputComponent->RegisterAction("player", "rotate_pitch", [this](int activationMode, float value) { OnActionRotatePitch(activationMode, value); });
		m_pInputComponent->BindAction("player", "rotate_pitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

		// Jump.
		m_pInputComponent->RegisterAction("player", "move_jump", [this](int activationMode, float value) { OnActionJump(activationMode, value); });
		m_pInputComponent->BindAction("player", "move_jump", eAID_KeyboardMouse, EKeyId::eKI_Space);

		// Walk, jog, sprint.
		m_pInputComponent->RegisterAction("player", "move_walkjog", [this](int activationMode, float value) { OnActionWalkJog(activationMode, value); });
		m_pInputComponent->BindAction("player", "move_walkjog", eAID_KeyboardMouse, EKeyId::eKI_LCtrl);

		m_pInputComponent->RegisterAction("player", "move_sprint", [this](int activationMode, float value) { OnActionSprintToggle(activationMode, value); });
		m_pInputComponent->BindAction("player", "move_sprint", eAID_KeyboardMouse, EKeyId::eKI_LShift);

		// Stances under player control.
		m_pInputComponent->RegisterAction("player", "stance_crouch", [this](int activationMode, float value) { OnActionCrouchToggle(activationMode, value); });
		m_pInputComponent->BindAction("player", "stance_crouch", eAID_KeyboardMouse, EKeyId::eKI_C);

		m_pInputComponent->RegisterAction("player", "stance_crawl", [this](int activationMode, float value) { OnActionCrawlToggle(activationMode, value); });
		m_pInputComponent->BindAction("player", "stance_crawl", eAID_KeyboardMouse, EKeyId::eKI_H);

		m_pInputComponent->RegisterAction("player", "stance_sit", [this](int activationMode, float value) { OnActionSitToggle(activationMode, value); });
		m_pInputComponent->BindAction("player", "stance_sit", eAID_KeyboardMouse, EKeyId::eKI_B);

		// Interact with an object.
		m_pInputComponent->RegisterAction("player", "item_use", [this](int activationMode, float value) { OnActionItemUse(activationMode, value); });
		m_pInputComponent->BindAction("player", "item_use", eAID_KeyboardMouse, EKeyId::eKI_F);

		m_pInputComponent->RegisterAction("player", "item_pickup", [this](int activationMode, float value) { OnActionItemPickup(activationMode, value); });
		m_pInputComponent->BindAction("player", "item_pickup", eAID_KeyboardMouse, EKeyId::eKI_G);

		m_pInputComponent->RegisterAction("player", "item_drop", [this](int activationMode, float value) { OnActionItemDrop(activationMode, value); });
		m_pInputComponent->BindAction("player", "item_drop", eAID_KeyboardMouse, EKeyId::eKI_M);

		m_pInputComponent->RegisterAction("player", "item_toss", [this](int activationMode, float value) { OnActionItemToss(activationMode, value); });
		m_pInputComponent->BindAction("player", "item_toss", eAID_KeyboardMouse, EKeyId::eKI_X);

		// Numpad.
		m_pInputComponent->RegisterAction("player", "np_0", [this](int activationMode, float value) { OnNumpad(activationMode, 0); });
		m_pInputComponent->BindAction("player", "np_0", eAID_KeyboardMouse, EKeyId::eKI_NP_0);
		m_pInputComponent->RegisterAction("player", "np_1", [this](int activationMode, float value) { OnNumpad(activationMode, 1); });
		m_pInputComponent->BindAction("player", "np_1", eAID_KeyboardMouse, EKeyId::eKI_NP_1);
		m_pInputComponent->RegisterAction("player", "np_2", [this](int activationMode, float value) { OnNumpad(activationMode, 2); });
		m_pInputComponent->BindAction("player", "np_2", eAID_KeyboardMouse, EKeyId::eKI_NP_2);
		m_pInputComponent->RegisterAction("player", "np_3", [this](int activationMode, float value) { OnNumpad(activationMode, 3); });
		m_pInputComponent->BindAction("player", "np_3", eAID_KeyboardMouse, EKeyId::eKI_NP_3);
		m_pInputComponent->RegisterAction("player", "np_4", [this](int activationMode, float value) { OnNumpad(activationMode, 4); });
		m_pInputComponent->BindAction("player", "np_4", eAID_KeyboardMouse, EKeyId::eKI_NP_4);
		m_pInputComponent->RegisterAction("player", "np_5", [this](int activationMode, float value) { OnNumpad(activationMode, 5);; });
		m_pInputComponent->BindAction("player", "np_5", eAID_KeyboardMouse, EKeyId::eKI_NP_5);
		m_pInputComponent->RegisterAction("player", "np_6", [this](int activationMode, float value) { OnNumpad(activationMode, 6); });
		m_pInputComponent->BindAction("player", "np_6", eAID_KeyboardMouse, EKeyId::eKI_NP_6);
		m_pInputComponent->RegisterAction("player", "np_7", [this](int activationMode, float value) { OnNumpad(activationMode, 7); });
		m_pInputComponent->BindAction("player", "np_7", eAID_KeyboardMouse, EKeyId::eKI_NP_7);
		m_pInputComponent->RegisterAction("player", "np_8", [this](int activationMode, float value) { OnNumpad(activationMode, 8); });
		m_pInputComponent->BindAction("player", "np_8", eAID_KeyboardMouse, EKeyId::eKI_NP_8);
		m_pInputComponent->RegisterAction("player", "np_9", [this](int activationMode, float value) { OnNumpad(activationMode, 9); });
		m_pInputComponent->BindAction("player", "np_9", eAID_KeyboardMouse, EKeyId::eKI_NP_9);

		// Inspection.
		m_pInputComponent->RegisterAction("player", "inspect_start", [this](int activationMode, float value) { OnActionInspectStart(activationMode, value); });
		m_pInputComponent->BindAction("player", "inspect_start", eAID_KeyboardMouse, EKeyId::eKI_J);

		m_pInputComponent->RegisterAction("player", "inspect", [this](int activationMode, float value) { OnActionInspect(activationMode, value); });
		m_pInputComponent->BindAction("player", "inspect", eAID_KeyboardMouse, EKeyId::eKI_K);

		m_pInputComponent->RegisterAction("player", "inspect_end", [this](int activationMode, float value) { OnActionInspectEnd(activationMode, value); });
		m_pInputComponent->BindAction("player", "inspect_end", eAID_KeyboardMouse, EKeyId::eKI_L);
	}

	void CPlayerInputComponent::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
	{
		switch (type)
		{
		case EInputFlagType::Hold:
		{
			if (activationMode & eIS_Pressed)
			{
				m_inputFlags |= flags;
			}
			else if (activationMode & eIS_Released)
			{
				m_inputFlags &= ~flags;
			}
		}
		break;

		case EInputFlagType::Toggle:
		{
			if (activationMode & eIS_Released)
			{
				// Toggle the bit(s)
				m_inputFlags ^= flags;
			}
		}
		break;
		}
	}

	void CPlayerInputComponent::Update()
	{
		m_mousePitchDelta = std::abs(m_mousePitchDelta) >= m_pitchFilter ? m_mousePitchDelta : 0.0f;
		m_mouseYawDelta = std::abs(m_mouseYawDelta) >= m_yawFilter ? m_mouseYawDelta : 0.0f;
	}

	Vec3 CPlayerInputComponent::GetMovementDirection(const Quat& baseRotation)
	{
		bool allowMovement = true;
		Quat quatRelativeDirection;
		Vec3 vecMovement = Vec3(ZERO);

		switch (m_inputFlags)
		{
		case (TInputFlags)EInputFlag::Forward:
			quatRelativeDirection = Quat::CreateIdentity();
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(45.0f));
			break;

		case (TInputFlags)EInputFlag::Right:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(90.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(135.0f));
			break;

		case (TInputFlags)EInputFlag::Backward:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(180.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(225.0f));
			break;

		case (TInputFlags)EInputFlag::Left:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(270.0f));
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(315.0f));
			break;

		default:
			quatRelativeDirection = Quat::CreateIdentity();
			allowMovement = false;
			break;
		}

		// Create a vector based on key direction. This is computed in local space for the base rotation.
		if (allowMovement)
		{
			vecMovement = Vec3(baseRotation.GetFwdX(), baseRotation.GetFwdY(), 0.0f).GetNormalized() * quatRelativeDirection;
		}

		return vecMovement;
	}

	void CPlayerInputComponent::OnActionRotateYaw(int activationMode, float value)
	{
		float ctl_mouseSensitivity = CGamePlugin::GetInstance() ? g_pCVars->ctl_mouseSensitivity : 1.f;
		float mouseSensitivity = 0.00032f * max(0.01f, ctl_mouseSensitivity * m_mousePitchYawSensitivity);

		m_mouseYawDelta -= value * mouseSensitivity;
	}

	void CPlayerInputComponent::OnActionRotatePitch(int activationMode, float value)
	{
		bool gameIsInitialized = CGamePlugin::GetInstance() != nullptr;
		float ctl_mouseSensitivity = gameIsInitialized ? g_pCVars->ctl_mouseSensitivity : 1.f;

		if (gameIsInitialized && g_pCVars->ctl_invertMouse)
		{
			value = -value;
		}

		float mouseSensitivity = 0.00032f * max(0.01f, ctl_mouseSensitivity * m_mousePitchYawSensitivity);

		float invertYAxis = m_mouseInvertPitch ? -1.0f : 1.0f;
		m_mousePitchDelta += value * mouseSensitivity * invertYAxis;
	}

	void CPlayerInputComponent::OnActionEscape(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			for (auto it : m_listeners)
			{
				it->OnInputEscape(activationMode);
			}
		}
	}

	void CPlayerInputComponent::OnActionInteraction(int activationMode, float value)
	{
		for (auto it : m_listeners)
		{
			it->OnInputInteraction(activationMode);
		}
	}

	void CPlayerInputComponent::OnActionJump(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnJump();
		}
	}

	void CPlayerInputComponent::OnActionCrouchToggle(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionCrouchToggle();
		}
	}

	void CPlayerInputComponent::OnActionCrawlToggle(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionCrawlToggle();
		}
	}

	void CPlayerInputComponent::OnActionSitToggle(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionSitToggle();
		}
	}

	void CPlayerInputComponent::OnActionSprintToggle(int activationMode, float value)
	{
		if (activationMode == eAAM_OnRelease)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionSprintStop();
		}
		else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionSprintStart();
		}
	}

	void CPlayerInputComponent::OnActionWalkJog(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionJogToggle();
		}
	}

	void CPlayerInputComponent::OnActionItemUse(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionItemUse();
		}
	}

	void CPlayerInputComponent::OnActionItemPickup(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionItemPickup();
		}
	}

	void CPlayerInputComponent::OnActionItemDrop(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionItemDrop();
		}
	}

	void CPlayerInputComponent::OnActionItemToss(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionItemToss();
		}
	}

	void CPlayerInputComponent::OnNumpad(int activationMode, int buttonId)
	{
		if (activationMode == eAAM_OnPress)
		{
			CryLogAlways("OnNumpad");
		}
	}

	void CPlayerInputComponent::OnActionInspectStart(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionInspectStart();
		}
	}

	void CPlayerInputComponent::OnActionInspect(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionInspecting();
		}
	}

	void CPlayerInputComponent::OnActionInspectEnd(int activationMode, float value)
	{
		if (activationMode == eAAM_OnPress)
		{
			CPlayerComponent::GetActivePlayer()->GetAttachedActor()->OnActionInspectEnd();
		}
	}
}