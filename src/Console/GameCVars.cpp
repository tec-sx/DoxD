#include "StdAfx.h"

#include "Console/GameCVars.h"
#include <CrySystem/ConsoleRegistration.h>

namespace DoxD
{
	SCVars* g_pCVars;

	static void OnFOVChanged(ICVar* pPass)
	{
		g_pCVars->cm_fov = clamp_tpl<float>(g_pCVars->cm_fov, 25.f, 80.f);
	}


	void SCVars::RegisterCVars(IConsole* pConsole)
	{
		// Controls
		REGISTER_CVAR(ctl_mouseSensitivity, 1.0f, VF_DUMPTODISK, "Set mouse sensitivity!");
		REGISTER_CVAR(ctl_invertMouse, 0, VF_DUMPTODISK, "mouse invert?");
		REGISTER_CVAR(ctl_zoomToggle, 0, VF_DUMPTODISK, "To make the zoom key work as a toggle");
		REGISTER_CVAR(ctl_crouchToggle, 1, VF_DUMPTODISK, "To make the crouch key work as a toggle");
		REGISTER_CVAR(ctl_sprintToggle, 1, VF_DUMPTODISK, "To make the sprint key work as a toggle");
		
		REGISTER_CVAR(cl_logAsserts, 0, 0, "1 = log assert only. 2 = log assert and callstack");

		REGISTER_CVAR(cl_motionBlurVectorScale, 1.5f, VF_CHEAT, "Default motion blur vector scale");
		REGISTER_CVAR(cl_motionBlurVectorScaleSprint, 3.0f, VF_CHEAT, "Motion blur vector scale while sprinting");

		OnFOVChanged(NULL);

		REGISTER_CVAR(cl_shallowWaterDepthLo, 0.5f, 0, "Shallow water depth low (below has zero slowdown)");
		REGISTER_CVAR(cl_shallowWaterDepthHi, 1.2f, 0, "Shallow water depth high (above has full slowdown)");

		REGISTER_CVAR(cl_idleBreaksDelayTime, 8.0f, VF_CHEAT, "Time delay between idle breaks");

		REGISTER_CVAR(cl_postUpdateCamera, 2, VF_CHEAT, "Apply post animation updates to the camera (0 = none, 1 = position, 2 = position&rotation)");

		REGISTER_CVAR(p_collisionClassDebug, 0, VF_CHEAT, "Show debug info about CollisionClass data for physics objects.");

		// Player
		REGISTER_CVAR(pl_cameraTransitionTime, 0.25f, VF_CHEAT, "Time over which to transition the camera between modes");
		REGISTER_CVAR(pl_cameraTransitionTimeLedgeGrabIn, 0.0f, VF_CHEAT, "Time over which to transition into the animated camera mode going into ledge grab");
		REGISTER_CVAR(pl_cameraTransitionTimeLedgeGrabOut, 0.25f, VF_CHEAT, "Time over which to transition into the animated camera mode going out of the ledge grab");

		REGISTER_CVAR(pl_inputAccel, 30.0f, 0, "Movement input acceleration");
		REGISTER_CVAR(pl_debug_energyConsumption, 0, VF_CHEAT, "Display debug energy consumption rate info");
		REGISTER_CVAR(pl_debug_pickable_items, 0, 0, "Display information about pickable item at which the player is about to interact with");

		// Camera General
		REGISTER_CVAR_CB(cm_fov, 55.0f, 0, "field of view.", OnFOVChanged);
		REGISTER_CVAR(cm_tpCameraCollision, 1, 0, "enable camera collision in 3rd person view");
		REGISTER_CVAR(cm_tpCameraCollisionOffset, 0.3f, 0, "offset that is used to move the 3rd person camera out of collisions");
		REGISTER_CVAR(cm_tpDebugAim, 0, 0, "show aim debug helpers in 3rd person view");
		REGISTER_CVAR(cm_tpCameraDistance, 2.0f, 0, "camera distance in 3rd person view");
		REGISTER_CVAR(cm_tpCameraDistanceHorizontal, 0.5f, 0, "horizontal camera distance in 3rd person view");
		REGISTER_CVAR(cm_tpCameraDistanceVertical, 1.5f, 0, "vertical camera distance in 3rd person view");
		REGISTER_CVAR(cm_tpMaxAimDist, 1000.0f, 0, "maximum range in which the player can aim at an object");
		REGISTER_CVAR(cm_tpMinDist, 0.5f, 0, "minimum distance in 3rd person view");
		REGISTER_CVAR(cm_tpYaw, 0, 0, "camera angle offset in 3rd person view");
#ifndef _RELEASE
		REGISTER_CVAR(cm_debugCamera, 0, VF_CHEAT, "Enables camera debug mode.");
#endif
		REGISTER_CVAR(m_cameraInterpolationSpeed, 5.0f, 0, "interpolation speed when camera hits an object.");
		
		// CameraManager
		m_cameraManagerDebugViewOffset = REGISTER_STRING("camera_manager_debug_view_offset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned.");
		
		// Third person camera
		REGISTER_CVAR2("camera_actionrpg_Debug", &m_actionRPGCameraDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("camera_actionrpg_PitchMin", &m_actionRPGCameraPitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_actionrpg_PitchMax", &m_actionRPGCameraPitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_actionrpg_TargetDistance", &m_actionRPGCameraTargetDistance, 5.0f, VF_CHEAT, "Preferred distance in metres from the view camera to the target.");
		REGISTER_CVAR2("camera_actionrpg_ReversePitchTilt", &m_actionRPGCameraReversePitchTilt, 0.1f, VF_CHEAT, "A factor that applies pitch in the direction the camera is facing. The effect of this is to tilt it towards or away from the orbit centre.");
		REGISTER_CVAR2("camera_actionrpg_SlerpSpeed", &m_actionRPGCameraSlerpSpeed, 20.0f, VF_CHEAT, "The slerp speed. The speed at which we apply spherical linear interpolation to the camera rotations.");
		REGISTER_CVAR2("camera_actionrpg_ZoomMin", &m_actionRPGCameraZoomMin, 0.2f, VF_CHEAT, "The minimum value for camera zoom.");
		REGISTER_CVAR2("camera_actionrpg_ZoomMax", &m_actionRPGCameraZoomMax, 1.0f, VF_CHEAT, "The maximum value for camera zoom.");
		REGISTER_CVAR2("camera_actionrpg_ZoomStep", &m_actionRPGCameraZoomStep, 0.02f, VF_CHEAT, "Each zoom event in or out will alter the zoom factor, m_zoom, by this amount. Use lower values for more steps and higher values to zoom in / out faster with less steps.");
		REGISTER_CVAR2("camera_actionrpg_ZoomSpeed", &m_actionRPGCameraZoomSpeed, 10.0f, VF_CHEAT, "When the zoom changes we interpolate between it's last value and the goal value. This provides for smoother movement on camera zooms. Higher values will interpolate faster than lower values.");
		m_actionRPGCameraViewPositionOffset = REGISTER_STRING("camera_actionrpg_view_position_offset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");
		m_actionRPGCameraAimPositionOffset = REGISTER_STRING("camera_actionrpg_aim_position_offset", "0.45, -0.5, 0.0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");

		// Examine Camera.
		REGISTER_CVAR2("camera_examine_debug", &m_examineCameraDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("camera_examine_yaw_min", &m_examineCameraYawMin, -85.0f, VF_CHEAT, "Minimum yaw angle of the camera (degrees)");
		REGISTER_CVAR2("camera_examine_yaw_max", &m_examineCameraYawMax, 85.0f, VF_CHEAT, "Maximum yaw angle of the camera (degrees)");
		REGISTER_CVAR2("camera_examine_pitch_min", &m_examineCameraPitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_examine_pitch_max", &m_examineCameraPitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");

		// Interactive Entity Highlighting
		REGISTER_CVAR(g_highlightingMaxDistanceToHighlightSquared, 100.f, 0, "How close interactive entities should be before highlighting");
		REGISTER_CVAR(g_highlightingMovementDistanceToUpdateSquared, 0.025f, 0, "How far the player needs to move before reassessing nearby interactive entities");
		REGISTER_CVAR(g_highlightingTimeBetweenForcedRefresh, 1.f, 0, "Maximum time before entities are re-checked for highlight requirements");

		// Components
		REGISTER_CVAR2("component_keyring_debug", &m_componentKeyringDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("component_equipment_debug", &m_componentEquipmentDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("component_character_attributes_debug", &m_componentCharacterAttributesDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("component_awareness_debug", &m_componentAwarenessDebug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("component_inventory_debug", &m_componentInventoryDebug, 0, VF_CHEAT, "Allow debug display.");

		// Vaulting
		REGISTER_CVAR(g_ledgeGrabClearHeight, 0.2f, 0, "Distance a player must clear a ledge grab entity by to avoid doing a ledge grab");
		REGISTER_CVAR(g_ledgeGrabMovingledgeExitVelocityMult, 1.5f, 0, "On exiting a grab onto a moving ledge a movement will be added to the players exit velocity in the direction of the ledge movement");
		REGISTER_CVAR(g_vaultMinHeightDiff, 1.2f, 0, "Minimum height difference between a player and a ledge for a vault to be possible");
		REGISTER_CVAR(g_vaultMinAnimationSpeed, 0.7f, 0, "When vaulting at slow speeds the resulting animation speed will never go beneath this value");

		REGISTER_CVAR(pl_ledgeClamber.debugDraw, 0, VF_CHEAT, "Turn on debug drawing of Ledge Clamber");
		REGISTER_CVAR(pl_ledgeClamber.cameraBlendWeight, 1.0f, VF_CHEAT, "FP camera blending weight when performing ledge grab action. Do NOT change this from 1.0f lightly!");
		REGISTER_CVAR(pl_ledgeClamber.enableVaultFromStanding, 1, VF_CHEAT, "0 = No vault from standing; 1 = Vault when push towards obstacle and press jump; 2 = Vault when press jump; 3 = Vault when push towards obstacle");

		// Watch
		REGISTER_CVAR(watch_enabled, 1, 0, "On-screen watch text is enabled/disabled");
		REGISTER_CVAR(watch_text_render_start_pos_x, 35.0f, 0, "On-screen watch text render start x position");
		REGISTER_CVAR(watch_text_render_start_pos_y, 180.0f, 0, "On-screen watch text render start y position");
		REGISTER_CVAR(watch_text_render_size, 1.75f, 0, "On-screen watch text render size");
		REGISTER_CVAR(watch_text_render_lineSpacing, 9.3f, 0, "On-screen watch text line spacing (to cram more text on screen without shrinking the font)");
		REGISTER_CVAR(watch_text_render_fxscale, 13.0f, 0, "Draw2d label to IFFont x scale value (for calcing sizes).");

		// AI Threat Moifiers
		REGISTER_CVAR2("ai_SOMDebugName", &ai_threatModifiers.DebugAgentName, "none", 0, "Debug the threat modifier for the given AI");
		REGISTER_CVAR2("ai_SOMIgnoreVisualRatio", &ai_threatModifiers.SOMIgnoreVisualRatio, 0.35f, 0,
			"Ratio from [0,1] where the AI will ignore seeing an enemy (keep the threat at none).\n"
			"Usage: ai_SOMIgnoreVisualRatio 0.35\n"
			"Default is 35%. Putting this at a higher value will result in the AI first considering a visual target to be non-threatening for a percentage of the SOM time period");
		REGISTER_CVAR2("ai_SOMDecayTime", &ai_threatModifiers.SOMDecayTime, 1.0f, 0,
			"How long it takes to decay one bar when visual sighting is lost on the target\n"
			"Usage: ai_SOMDecayTime 1\n"
			"Default is 1 second. A higher value means the AI will take longer to go aggressive if they see the target again");

		REGISTER_CVAR2("ai_SOMMinimumRelaxed", &ai_threatModifiers.SOMMinimumRelaxed, 2.0f, 0,
			"Minimum time that must pass before AI will see the enemy while relaxed, regardless of distance.\n"
			"Usage: ai_SOMMinimumRelaxed 2.0\n"
			"Default is 2.0 seconds. A lower value causes the AI to potentially react to the enemy sooner.");
		REGISTER_CVAR2("ai_SOMMinimumTense", &ai_threatModifiers.SOMMinimumTense, 0.5f, 0,
			"Minimum time that must pass before AI will see the enemy while alarmed, regardless of distance.\n"
			"Usage: ai_SOMMinimumTense 0.5\n"
			"Default is 0.5 seconds. A lower value causes the AI to potentially react to the enemy sooner.");
		REGISTER_CVAR2("ai_SOMCrouchModifierRelaxed", &ai_threatModifiers.SOMCrouchModifierRelaxed, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy if the enemy is crouched while relaxed.\n"
			"Usage: ai_SOMCrouchModifierRelaxed 1.0\n"
			"Default is 1.0. A higher value will cause the AI to not see the enemy for a longer period of time while the enemy is crouched.");
		REGISTER_CVAR2("ai_SOMCrouchModifierTense", &ai_threatModifiers.SOMCrouchModifierTense, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy if the enemy is crouched while alarmed.\n"
			"Usage: ai_SOMCrouchModifierTense 1.0\n"
			"Default is 1.0. A higher value will cause the AI to not see the enemy for a longer period of time while the enemy is crouched.");
		REGISTER_CVAR2("ai_SOMMovementModifierRelaxed", &ai_threatModifiers.SOMMovementModifierRelaxed, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy based on the enemy's movement speed while relaxed.\n"
			"Usage: ai_SOMMovementModifierRelaxed 1.0\n"
			"Default is 1.0. A lower value will cause the AI to see the enemy sooner relative to how fast the enemy is moving - the faster, the sooner.");
		REGISTER_CVAR2("ai_SOMMovementModifierTense", &ai_threatModifiers.SOMMovementModifierTense, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy based on the enemy's movement speed while alarmed.\n"
			"Usage: ai_SOMMovementModifierTense 1.0\n"
			"Default is 1.0. A lower value will cause the AI to see the enemy sooner relative to how fast the enemy is moving - the faster, the sooner.");
		REGISTER_CVAR2("ai_SOMWeaponFireModifierRelaxed", &ai_threatModifiers.SOMWeaponFireModifierRelaxed, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy based on if the enemy is firing their weapon.\n"
			"Usage: ai_SOMWeaponFireModifierRelaxed 0.1\n"
			"Default is 0.1. A lower value will cause the AI to see the enemy sooner if the enemy is firing their weapon.");
		REGISTER_CVAR2("ai_SOMWeaponFireModifierTense", &ai_threatModifiers.SOMWeaponFireModifierTense, 1.0f, 0,
			"Modifier applied to time before the AI will see the enemy based on if the enemy is firing their weapon.\n"
			"Usage: ai_SOMWeaponFireModifierTense 0.1\n"
			"Default is 0.1. A lower value will cause the AI to see the enemy sooner if the enemy is firing their weapon.");

		REGISTER_CVAR2("ai_SOMCloakMaxTimeRelaxed", &ai_threatModifiers.SOMCloakMaxTimeRelaxed, 5.0f, 0,
			"Time the AI cannot react to a cloaked target when the target is close enough to be seen while relaxed.\n"
			"Usage: ai_SOMCloakMaxTimeRelaxed 5.0\n"
			"Default is 5.0. A lower value will cause the AI to see the enemy sooner when cloaked relative to how close the enemy is.");
		REGISTER_CVAR2("ai_SOMCloakMaxTimeTense", &ai_threatModifiers.SOMCloakMaxTimeTense, 5.0f, 0,
			"Time the AI cannot react to a cloaked target when the target is close enough to be seen while alarmed.\n"
			"Usage: ai_SOMCloakMaxTimeTense 5.0\n"
			"Default is 5.0. A lower value will cause the AI to see the enemy sooner when cloaked relative to how close the enemy is.");
		REGISTER_CVAR2("ai_SOMUncloakMinTimeRelaxed", &ai_threatModifiers.SOMUncloakMinTimeRelaxed, 1.0f, 0,
			"Minimum time the AI cannot react to a target uncloaking while relaxed.\n"
			"Usage: ai_SOMUncloakMinTimeRelaxed 1.0\n"
			"Default is 1.0. A lower value will cause the AI to see the enemy sooner when uncloaking.");
		REGISTER_CVAR2("ai_SOMUncloakMinTimeTense", &ai_threatModifiers.SOMUncloakMinTimeTense, 1.0f, 0,
			"Minimum time the AI cannot react to a target uncloaking while alarmed.\n"
			"Usage: ai_SOMUncloakMinTimeTense 1.0\n"
			"Default is 1.0. A lower value will cause the AI to see the enemy sooner when uncloaking.");
		REGISTER_CVAR2("ai_SOMUncloakMaxTimeRelaxed", &ai_threatModifiers.SOMUncloakMaxTimeRelaxed, 5.0f, 0,
			"Time the AI cannot react to a target uncloaking when the target is at their max sight range while relaxed.\n"
			"Usage: ai_SOMUncloakMaxTimeRelaxed 5.0\n"
			"Default is 5.0. A lower value will cause the AI to see the enemy sooner when uncloaking relative to how close the enemy is.");
		REGISTER_CVAR2("ai_SOMUncloakMaxTimeTense", &ai_threatModifiers.SOMUncloakMaxTimeTense, 5.0f, 0,
			"Time the AI cannot react to a target uncloaking when the target is at their max sight range while alarmed.\n"
			"Usage: ai_SOMUncloakMaxTimeTense 5.0\n"
			"Default is 5.0. A lower value will cause the AI to see the enemy sooner when uncloaking relative to how close the enemy is.");
		REGISTER_CVAR2("ai_CloakingDelay", &ai_CloakingDelay, 0.55f, 0,
			"Time it takes for AI perception to become effected by cloak.\n"
			"Usage: ai_CloakingDelay 0.7\n"
			"Default is 1.5.");
		REGISTER_CVAR2("ai_UnCloakingTime", &ai_UnCloakingDelay, 0.75f, 0,
			"Time it takes the player to uncloak, after which he becomes visible to the AI.\n"
			"Usage: ai_UnCloakingTime 0.9\n"
			"Default is 0.5.");
		REGISTER_CVAR2("ai_CompleteCloakDelay", &ai_CompleteCloakDelay, 2.0f, 0,
			"Time it takes the AI to decide cloaked target's position is lost.\n"
			"Usage: ai_CompleteCloakDelay 2.0\n"
			"Default is 0.5.");

		// Actions
		REGISTER_CVAR(pl_power_sprint.foward_angle, 45.0f, 0, "Power sprint: Stick angle threshold for sprinting (0.0 - 90.0f");

		REGISTER_CVAR(pl_jump_control.air_control_scale, 1.0f, 0, "Scales base air control while in air");
		REGISTER_CVAR(pl_jump_control.air_resistance_scale, 1.3f, 0, "Scales base air resitance while in air");
		REGISTER_CVAR(pl_jump_control.air_inertia_scale, 0.3f, 0, "Scales inertia while in air");

		// Health
		REGISTER_CVAR(pl_health.normal_regeneration_rateSP, 250.0f, 0, "Regeneration rate in points per second");
		REGISTER_CVAR(pl_health.critical_health_thresholdSP, 10.0f, 0, "If player health percentage drops below this threshold it's considered critical.");
		REGISTER_CVAR(pl_health.critical_health_updateTimeSP, 5.0f, 0, "Interval used to keep critical health msg shown (SP)");
		REGISTER_CVAR(pl_health.normal_threshold_time_to_regenerateSP, 4.0f, 0, "Time between hits to start regeneration");

		REGISTER_CVAR(pl_health.enable_FallAndPlay, 1, 0, "Enables/Disables fall&play for the player");
		REGISTER_CVAR(pl_health.collision_health_threshold, 41, 0, "Collision damage will never put the player below this health");

		REGISTER_CVAR(pl_health.fallDamage_SpeedSafe, 12.0f, VF_CHEAT, "Safe fall speed.");
		REGISTER_CVAR(pl_health.fallDamage_SpeedFatal, 17.0f, VF_CHEAT, "Fatal fall");
		REGISTER_CVAR(pl_health.fallSpeed_HeavyLand, 14.0f, VF_CHEAT, "Fall speed to play heavyLand");
		REGISTER_CVAR(pl_health.fallDamage_health_threshold, 1, VF_CHEAT, "Falling damage will never pu the player below this health, unless falling above fatal speed");
		REGISTER_CVAR(pl_health.debug_FallDamage, 0, VF_CHEAT, "Enables console output of fall damage information.");
		REGISTER_CVAR(pl_health.enableNewHUDEffect, 1, VF_CHEAT, "Enables new Health/Hits HUD effect (only on level reload)");
		REGISTER_CVAR(pl_health.minimalHudEffect, 0, VF_CHEAT, "only shows a bit of blood on screen when player is hit. Use this with g_godMode on.");

		// Movement
		REGISTER_CVAR(pl_movement.heavy_speedScale, 0.75f, 0, "Base speed multiplier in non combat mode while carrying a heavy (ripped-off) weapon");
		REGISTER_CVAR(pl_movement.heavy_sprintScale, 1.5f, 0, "Sprint speed multiplier in non combat mode while carrying a heavy (ripped-off) weapon");
		REGISTER_CVAR(pl_movement.heavy_strafe_speedScale, 0.9f, 0, "Strafe speed multiplier in non combat mode while carrying a heavy (ripped-off) weapon");
		REGISTER_CVAR(pl_movement.heavy_crouch_speedScale, 1.0f, 0, "Crouch speed multiplier in non combat mode while carrying a heavy (ripped-off) weapon");
		REGISTER_CVAR(pl_movement.powerSprint_targetFov, 55.0f, 0, "Fov while sprinting in power mode");

		REGISTER_CVAR(pl_movement.ground_timeInAirToFall, 0.3f, VF_CHEAT, "Amount of time (in seconds) the player/ai can be in the air and still be considered on ground");
		REGISTER_CVAR(pl_movement.speedScale, 1.2f, 0, "General speed scale");
		REGISTER_CVAR(pl_movement.strafe_SpeedScale, 0.9f, 0, "Strafe speed scale");
		REGISTER_CVAR(pl_movement.sprint_SpeedScale, 1.8f, 0, "Sprint speed scale");
		REGISTER_CVAR(pl_movement.crouch_SpeedScale, 1.0f, 0, "Crouch speed scale");
		REGISTER_CVAR(pl_movement.sprintStamina_debug, false, VF_CHEAT, "For MP characters (eg. Assault defenders). Display debug values for the sprint stamina");
		REGISTER_CVAR(pl_movement.mp_slope_speed_multiplier_uphill, 1.0f, 0, "Changes how drastically slopes affect a players movement speed when moving uphill. Lower = less effect.");
		REGISTER_CVAR(pl_movement.mp_slope_speed_multiplier_downhill, 1.0f, 0, "Changes how drastically slopes affect a players movement speed when moving downhill. Lower = less effect.");
		REGISTER_CVAR(pl_movement.mp_slope_speed_multiplier_minHill, 0.f, 0, "Minimum threshold for the slope steepness before speed is affected (in degrees).");

		// Sliding
		REGISTER_CVAR(pl_sliding_control.min_speed_threshold, 4.0f, 0, "Min sprinting movement speed to trigger sliding");
		REGISTER_CVAR(pl_sliding_control.min_speed, 2.0f, 0, "Speed threshold at which sliding will stop automatically");
		REGISTER_CVAR(pl_sliding_control.deceleration_speed, 4.0f, 0, "Deceleration while sliding (in m/s)");

		REGISTER_CVAR(pl_sliding_control.min_downhill_threshold, 5.0f, 0, "Slope angle threshold for down hill sliding");
		REGISTER_CVAR(pl_sliding_control.max_downhill_threshold, 50.0f, 0, "Slope angle threshold at which we reach maximum downhill acceleration");
		REGISTER_CVAR(pl_sliding_control.max_downhill_acceleration, 15.0f, 0, "Extra speed added to sliding, when going down hill (scaled linearly to slope)");

		// AI Collisions
		REGISTER_CVAR(AICollisions.minSpeedForFallAndPlay, 10.0f, 0, "Min collision speed that can cause a fall and play");
		REGISTER_CVAR(AICollisions.minMassForFallAndPlay, 5.0f, 0, "Min collision mass that can cause a fall and play");
		REGISTER_CVAR(AICollisions.dmgFactorWhenCollidedByObject, 1.0f, 0, "generic multiplier applied to the dmg calculated when an object impacts into an AI.");
		REGISTER_CVAR(AICollisions.showInLog, 0, 0, "Logs collisions ( 0=no Log, 1=Only collisions with damage, 2=All collisions");

#if !defined(_RELEASE)
		REGISTER_CVAR(pl_debug_watch_camera_mode, 0, VF_CHEAT, "Display on-screen text showing player's current camera mode (requires 'watch_enabled')");
		REGISTER_CVAR(pl_debug_log_camera_mode_changes, 0, VF_CHEAT, "Write CPlayerCamera-related messages to game log");
#endif
	}

	void SCVars::UnregisterCVars()
	{
		IConsole* pConsole = gEnv->pConsole;

		// Client
		pConsole->UnregisterVariable("ctl_mouseSensitivity", true);
		pConsole->UnregisterVariable("ctl_invertMouse", true);
		pConsole->UnregisterVariable("ctl_zoomToggle", true);
		pConsole->UnregisterVariable("ctl_crouchToggle", true);
		pConsole->UnregisterVariable("ctl_sprintToggle", true);

		pConsole->UnregisterVariable("cl_logAsserts", true);
		pConsole->UnregisterVariable("cl_motionBlurVectorScale", true);
		pConsole->UnregisterVariable("cl_motionBlurVectorScaleSprint", true);

		pConsole->UnregisterVariable("cl_shallowWaterDepthLo", true);
		pConsole->UnregisterVariable("cl_shallowWaterDepthHi", true);

		pConsole->UnregisterVariable("cl_idleBreaksDelayTime", true);

		pConsole->UnregisterVariable("cl_postUpdateCamera", true);

		pConsole->UnregisterVariable("p_collisionClassDebug", true);

		// Player
		pConsole->UnregisterVariable("pl_cameraTransitionTime", true);
		pConsole->UnregisterVariable("pl_cameraTransitionTimeLedgeGrabIn", true);
		pConsole->UnregisterVariable("pl_cameraTransitionTimeLedgeGrabOut", true);

		pConsole->UnregisterVariable("pl_inputAccel", true);
		pConsole->UnregisterVariable("pl_debug_energyConsumption", true);
		pConsole->UnregisterVariable("pl_debug_pickable_items", true);

		// Camera
		pConsole->UnregisterVariable("cm_fov", true);
		pConsole->UnregisterVariable("cm_tpCameraCollision", true);
		pConsole->UnregisterVariable("cm_tpCameraCollisionOffset", true);
		pConsole->UnregisterVariable("cm_tpDebugAim", true);

		pConsole->UnregisterVariable("cm_tpCameraDistance", true);
		pConsole->UnregisterVariable("cm_tpCameraDistanceHorizontal", true);
		pConsole->UnregisterVariable("cm_tpCameraDistanceVertical", true);
		pConsole->UnregisterVariable("cm_tpMaxAimDistance", true);
		pConsole->UnregisterVariable("cm_tpMinDistance", true);
		pConsole->UnregisterVariable("cm_tpYaw", true);
#ifndef _RELEASE
		pConsole->UnregisterVariable("cm_debugCamera", true);
#endif
		pConsole->UnregisterVariable("m_cameraInterpolationSpeed", true);

		// Camera Manager
		pConsole->UnregisterVariable("m_cameraManagerDebugViewOffset", true);

		// Third person camers
		pConsole->UnregisterVariable("m_actionRPGCameraDebug", true);
		pConsole->UnregisterVariable("m_actionRPGCameraPitchMin", true);
		pConsole->UnregisterVariable("m_actionRPGCameraPitchMax", true);
		pConsole->UnregisterVariable("m_actionRPGCameraTargetDistance", true);
		pConsole->UnregisterVariable("m_actionRPGCameraReversePitchTilt", true);
		pConsole->UnregisterVariable("m_actionRPGCameraSlerpSpeed", true);
		pConsole->UnregisterVariable("m_actionRPGCameraZoomMin", true);
		pConsole->UnregisterVariable("m_actionRPGCameraZoomMax", true);
		pConsole->UnregisterVariable("m_actionRPGCameraZoomStep", true);
		pConsole->UnregisterVariable("m_actionRPGCameraZoomSpeed", true);
		pConsole->UnregisterVariable("m_actionRPGCameraViewPositionOffset", true);
		pConsole->UnregisterVariable("m_actionRPGCameraAimPositionOffset", true);

		// Examine Camera.
		pConsole->UnregisterVariable("m_examineCameraDebug", true);
		pConsole->UnregisterVariable("m_examineCameraYawMin", true);
		pConsole->UnregisterVariable("m_examineCameraYawMax", true);
		pConsole->UnregisterVariable("m_examineCameraPitchMin", true);
		pConsole->UnregisterVariable("m_examineCameraPitchMax", true);

		//Interactive Entity Highlighting
		pConsole->UnregisterVariable("g_highlightingMaxDistanceToHighlightSquared", true);
		pConsole->UnregisterVariable("g_highlightingMovementDistanceToUpdateSquared", true);
		pConsole->UnregisterVariable("g_highlightingTimeBetweenForcedRefresh", true);

		// Components
		pConsole->UnregisterVariable("m_componentKeyringDebug", true);
		pConsole->UnregisterVariable("m_componentEquipmentDebug", true);
		pConsole->UnregisterVariable("m_componentCharacterAttributesDebug", true);
		pConsole->UnregisterVariable("m_componentAwarenessDebug", true);
		pConsole->UnregisterVariable("m_componentInventoryDebug", true);

		//Vaulting
		pConsole->UnregisterVariable("g_ledgeGrabClearHeight", true);
		pConsole->UnregisterVariable("g_ledgeGrabMovingledgeExitVelocityMult", true);
		pConsole->UnregisterVariable("g_vaultMinHeightDiff", true);
		pConsole->UnregisterVariable("g_vaultMinAnimationSpeed", true);

		pConsole->UnregisterVariable("pl_ledgeClamber.debugDraw", true);
		pConsole->UnregisterVariable("pl_ledgeClamber.cameraBlendWeight", true);
		pConsole->UnregisterVariable("pl_ledgeClamber.enableVaultFromStanding", true);

		// Watch
		pConsole->UnregisterVariable("watch_enabled", true);
		pConsole->UnregisterVariable("watch_text_render_start_pos_x", true);
		pConsole->UnregisterVariable("watch_text_render_start_pos_y", true);
		pConsole->UnregisterVariable("watch_text_render_size", true);
		pConsole->UnregisterVariable("watch_text_render_lineSpacing", true);
		pConsole->UnregisterVariable("watch_text_render_fxscale", true);

		// AI Threat Moifiers
		pConsole->UnregisterVariable("ai_SOMCrouchModifierRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMCrouchModifierTense", true);
		pConsole->UnregisterVariable("ai_SOMMovementModifierRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMMovementModifierTense", true);
		pConsole->UnregisterVariable("ai_SOMWeaponFireModifierRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMWeaponFireModifierTense", true);
		pConsole->UnregisterVariable("ai_SOMCloakMaxTimeRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMCloakMaxTimeTense", true);
		pConsole->UnregisterVariable("ai_SOMUncloakMinTimeRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMUncloakMinTimeTense", true);
		pConsole->UnregisterVariable("ai_SOMUncloakMaxTimeRelaxed", true);
		pConsole->UnregisterVariable("ai_SOMUncloakMaxTimeTense", true);
		pConsole->UnregisterVariable("ai_CloakingDelay", true);
		pConsole->UnregisterVariable("ai_UnCloakingTime", true);
		pConsole->UnregisterVariable("ai_CompleteCloakDelay", true);

		// Actions
		pConsole->UnregisterVariable("pl_power_sprint.foward_angle", true);
		pConsole->UnregisterVariable("pl_jump_control.air_control_scale", true);
		pConsole->UnregisterVariable("pl_jump_control.air_resistance_scale", true);
		pConsole->UnregisterVariable("pl_jump_control.air_inertia_scale", true);

		// Health

		pConsole->UnregisterVariable("pl_health.normal_regeneration_rateSP", true);
		pConsole->UnregisterVariable("pl_health.critical_health_thresholdSP", true);
		pConsole->UnregisterVariable("pl_health.critical_health_updateTimeSP", true);
		pConsole->UnregisterVariable("pl_health.normal_threshold_time_to_regenerateSP", true);

		pConsole->UnregisterVariable("pl_health.enable_FallAndPlay", true);
		pConsole->UnregisterVariable("pl_health.collision_health_threshold", true);

		pConsole->UnregisterVariable("pl_health.fallDamage_SpeedSafe", true);
		pConsole->UnregisterVariable("pl_health.fallDamage_SpeedFatal", true);
		pConsole->UnregisterVariable("pl_health.fallSpeed_HeavyLand", true);
		pConsole->UnregisterVariable("pl_health.fallDamage_health_threshold", true);
		pConsole->UnregisterVariable("pl_health.debug_FallDamage", true);
		pConsole->UnregisterVariable("pl_health.enableNewHUDEffect", true);
		pConsole->UnregisterVariable("pl_health.minimalHudEffect", true);

		// Movement
		pConsole->UnregisterVariable("pl_movement.heavy_speedScale", true);
		pConsole->UnregisterVariable("pl_movement.heavy_sprintScale", true);
		pConsole->UnregisterVariable("pl_movement.heavy_strafe_speedScale", true);
		pConsole->UnregisterVariable("pl_movement.heavy_crouch_speedScale", true);
		pConsole->UnregisterVariable("pl_movement.powerSprint_targetFov", true);

		pConsole->UnregisterVariable("pl_movement.ground_timeInAirToFall", true);
		pConsole->UnregisterVariable("pl_movement.speedScale", true);
		pConsole->UnregisterVariable("pl_movement.sprint_SpeedScale", true);
		pConsole->UnregisterVariable("pl_movement.strafe_SpeedScale", true);
		pConsole->UnregisterVariable("pl_movement.crouch_SpeedScale", true);
		pConsole->UnregisterVariable("pl_movement.sprintStamina_debug", true);
		pConsole->UnregisterVariable("pl_movement.mp_slope_speed_multiplier_uphill", true);
		pConsole->UnregisterVariable("pl_movement.mp_slope_speed_multiplier_downhill", true);
		pConsole->UnregisterVariable("pl_movement.mp_slope_speed_multiplier_minHill", true);

		// Sliding
		pConsole->UnregisterVariable("pl_sliding_control.min_speed_threshold", true);
		pConsole->UnregisterVariable("pl_sliding_control.min_speed", true);
		pConsole->UnregisterVariable("pl_sliding_control.deceleration_speed", true);

		pConsole->UnregisterVariable("pl_sliding_control.min_downhill_threshold", true);
		pConsole->UnregisterVariable("pl_sliding_control.max_downhill_threshold", true);
		pConsole->UnregisterVariable("pl_sliding_control.max_downhill_acceleration", true);

		// AI Collisions
		pConsole->UnregisterVariable("AICollisions.minSpeedForFallAndPlay", true);
		pConsole->UnregisterVariable("AICollisions.minMassForFallAndPlay", true);
		pConsole->UnregisterVariable("AICollisions.dmgFactorWhenCollidedByObject", true);
		pConsole->UnregisterVariable("AICollisions.showInLog", true);

#if !defined(_RELEASE)
		pConsole->UnregisterVariable("pl_debug_watch_camera_mode", true);
		pConsole->UnregisterVariable("pl_debug_log_camera_mode_changes", true);
#endif
	}

	void SCVars::InitAIPerceptionCVars(IConsole* pConsole)
	{
		REGISTER_CVAR(ai_perception.movement_useSurfaceType, 0, VF_CHEAT, "Toggle if surface type should be used to get the base radius instead of cvars");
		REGISTER_CVAR(ai_perception.movement_movingSurfaceDefault, 1.0f, VF_CHEAT, "Default value for movement speed effect on footstep radius (overridden by surface type)");
		REGISTER_CVAR(ai_perception.movement_standingRadiusDefault, 4.0f, VF_CHEAT, "Default value for standing footstep sound radius (overridden by surface type)");
		REGISTER_CVAR(ai_perception.movement_crouchRadiusDefault, 2.0f, VF_CHEAT, "Default value for crouching footstep sound radius multiplier (overridden by surface type)");
		REGISTER_CVAR(ai_perception.movement_standingMovingMultiplier, 2.5f, VF_CHEAT, "Multiplier for standing movement speed effect on footstep sound radius");
		REGISTER_CVAR(ai_perception.movement_crouchMovingMultiplier, 2.0f, VF_CHEAT, "Multiplier for crouched movement speed effect on footstep sound radius");

		REGISTER_CVAR(ai_perception.landed_baseRadius, 5.0f, VF_CHEAT, "Base radius for the AI sound generated when player lands");
		REGISTER_CVAR(ai_perception.landed_speedMultiplier, 1.5f, VF_CHEAT, "Multiplier applied to fall speed which is added on to the radius for the AI sound generated when player lands");
	}

	void SCVars::ReleaseAIPerceptionCVars(IConsole* pConsole)
	{
		pConsole->UnregisterVariable("ai_perception.movement_useSurfaceType", true);
		pConsole->UnregisterVariable("ai_perception.movement_movingSurfaceDefault", true);
		pConsole->UnregisterVariable("ai_perception.movement_standingRadiusDefault", true);
		pConsole->UnregisterVariable("ai_perception.movement_crouchRadiusDefault", true);
		pConsole->UnregisterVariable("ai_perception.movement_standingMovingMultiplier", true);
		pConsole->UnregisterVariable("ai_perception.movement_crouchMovingMultiplier", true);
		pConsole->UnregisterVariable("ai_perception.landed_baseRadius", true);
		pConsole->UnregisterVariable("ai_perception.landed_speedMultiplier", true);
	}
}
