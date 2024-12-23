#pragma once

struct SPowerSprintParams
{
	float foward_angle;
};

struct SJumpAirControl
{
	float air_control_scale;
	float air_resistance_scale;
	float air_inertia_scale;
};

struct SPlayerHealth
{
	float normal_regeneration_rateSP;
	float critical_health_thresholdSP;
	float critical_health_updateTimeSP;
	float normal_threshold_time_to_regenerateSP;

	int   enable_FallAndPlay;
	int	  collision_health_threshold;

	float fallDamage_SpeedSafe;
	float fallDamage_SpeedFatal;
	float fallSpeed_HeavyLand;
	int	  fallDamage_health_threshold;
	int	  debug_FallDamage;
	int	  enableNewHUDEffect;
	int	  minimalHudEffect;
};

struct SPlayerMovement
{
	float powerSprint_targetFov;

	float ground_timeInAirToFall;

	float speedScale;
	float strafe_SpeedScale;
	float sprint_SpeedScale;
	float crouch_SpeedScale;
	
	float heavy_speedScale;
	float heavy_sprintScale;
	float heavy_strafe_speedScale;
	float heavy_crouch_speedScale;

	int sprintStamina_debug;

	float mp_slope_speed_multiplier_uphill;
	float mp_slope_speed_multiplier_downhill;
	float mp_slope_speed_multiplier_minHill;
};

struct SPlayerLedgeClamber
{
	float cameraBlendWeight;
	int	  debugDraw;
	int	  enableVaultFromStanding;
};

struct SPlayerSlideControl
{
	float min_speed_threshold;
	float min_speed;

	float deceleration_speed;
	float min_downhill_threshold;
	float max_downhill_threshold;
	float max_downhill_acceleration;
};

struct SPostEffect
{	// Use same naming convention as the post effects in the engine
	float FilterGrain_Amount;
	float FilterRadialBlurring_Amount;
	float FilterRadialBlurring_ScreenPosX;
	float FilterRadialBlurring_ScreenPosY;
	float FilterRadialBlurring_Radius;
	float Global_User_ColorC;
	float Global_User_ColorM;
	float Global_User_ColorY;
	float Global_User_ColorK;
	float Global_User_Brightness;
	float Global_User_Contrast;
	float Global_User_Saturation;
	float Global_User_ColorHue;
	float HUD3D_Interference;
	float HUD3D_FOV;
};

struct SSilhouette
{
	int			enableUpdate;
	float		r;
	float		g;
	float		b;
	float		a;
	int			enabled;
};

struct SAIPerceptionCVars
{
	int			movement_useSurfaceType;
	float		movement_movingSurfaceDefault;
	float		movement_standingRadiusDefault;
	float		movement_crouchRadiusDefault;
	float		movement_standingMovingMultiplier;
	float		movement_crouchMovingMultiplier;

	float		landed_baseRadius;
	float		landed_speedMultiplier;
};

struct SAICollisions
{
	float minSpeedForFallAndPlay;
	float minMassForFallAndPlay;
	float dmgFactorWhenCollidedByObject;
	int   showInLog;
};

struct SAIThreatModifierCVars
{
	char const* DebugAgentName;

	float SOMIgnoreVisualRatio;
	float SOMDecayTime;

	float SOMMinimumRelaxed;
	float SOMMinimumTense;
	float SOMCrouchModifierRelaxed;
	float SOMCrouchModifierTense;
	float SOMMovementModifierRelaxed;
	float SOMMovementModifierTense;
	float SOMWeaponFireModifierRelaxed;
	float SOMWeaponFireModifierTense;

	float SOMCloakMaxTimeRelaxed;
	float SOMCloakMaxTimeTense;
	float SOMUncloakMinTimeRelaxed;
	float SOMUncloakMinTimeTense;
	float SOMUncloakMaxTimeRelaxed;
	float SOMUncloakMaxTimeTense;
};

struct SCVars
{
	// Client
	float cl_sensitivity;
	float cl_sensitivityController;
	float cl_sensitivityControllerMP;
	int	  cl_invertMouse;
	int	  cl_invertController;
	int	  cl_invertLandVehicleInput;
	int	  cl_invertAirVehicleInput;
	int	  cl_zoomToggle;
	int	  cl_crouchToggle;
	int	  cl_sprintToggle;
	int	  cl_debugSwimming;
	int	  cl_logAsserts;
	float cl_motionBlurVectorScale;
	float cl_motionBlurVectorScaleSprint;

	float cl_shallowWaterDepthLo;
	float cl_shallowWaterDepthHi;

	float cl_idleBreaksDelayTime;

	int	  cl_postUpdateCamera;

	int   p_collisionClassDebug;

	// Player
#if !defined(_RELEASE)
	int	  pl_debug_watch_camera_mode;
	int	  pl_debug_log_camera_mode_changes;
#endif

	float pl_cameraTransitionTime;
	float pl_cameraTransitionTimeLedgeGrabIn;
	float pl_cameraTransitionTimeLedgeGrabOut;

	float pl_inputAccel;
	int	  pl_debug_energyConsumption;
	int	  pl_debug_pickable_items;
	
	// Camera
	float cm_fov;
	int   cm_tpvCameraCollision;
	float cm_tpvCameraCollisionOffset;
	int   cm_tpvDebugAim;
	float cm_tpvDist;
	float cm_tpvDistHorizontal;
	float cm_tpvDistVertical;
	float cm_tpvInterpolationSpeed;
	float cm_tpvMaxAimDist;
	float cm_tpvMinDist;
	float cm_tpvYaw;
	int   cm_debugCamera;

	//Interactive Entity Highlighting
	float g_highlightingMaxDistanceToHighlightSquared;
	float g_highlightingMovementDistanceToUpdateSquared;
	float g_highlightingTimeBetweenForcedRefresh;

	// Vaulting
	float g_ledgeGrabClearHeight;
	float g_ledgeGrabMovingledgeExitVelocityMult;
	float g_vaultMinHeightDiff;
	float g_vaultMinAnimationSpeed;

	// Watch
	int watch_enabled;
	float watch_text_render_start_pos_x;
	float watch_text_render_start_pos_y;
	float watch_text_render_size;
	float watch_text_render_lineSpacing;
	float watch_text_render_fxscale;

	// AI
	float ai_CloakingDelay;
	float ai_CompleteCloakDelay;
	float ai_UnCloakingDelay;

	SAIPerceptionCVars ai_perception;
	SAIThreatModifierCVars ai_threatModifiers;
	SPowerSprintParams	 pl_power_sprint;
	SJumpAirControl pl_jump_control;
	SPlayerHealth pl_health;
	SPlayerMovement pl_movement;
	SPlayerLedgeClamber pl_ledgeClamber;
	SPlayerSlideControl	 pl_sliding_control;
	SAICollisions AICollisions;

	SCVars() { memset(this, 0, sizeof(SCVars)); }
	~SCVars() { ReleaseCVars(); }

	void InitCVars(IConsole* pConsole);
	void ReleaseCVars();

	void InitAIPerceptionCVars(IConsole* pConsole);
	void ReleaseAIPerceptionCVars(IConsole* pConsole);
};