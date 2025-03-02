#pragma once

#include <CryGame/CoherentValue.h>
#include <CryPhysics/IPhysics.h>

typedef TBitfield TActorStatusFlags;

enum class EActorStatus : TActorStatusFlags
{
	None = BIT32(0),
	OnLadder = BIT32(1),
	PickAndThrow = BIT32(2),
	VaultLedge = BIT32(3),
	Swimming = BIT32(4),
	LinkedToVehicle = BIT32(5),
	Last = BIT32(6)
};

CRY_CREATE_ENUM_FLAG_OPERATORS(EActorStatus);

struct SActorStats
{
	struct SItemExchangeStats
	{
		EntityId switchingToItemID;
		bool keepHistory;
		bool switchThisFrame; // used only when the character is using an action controller

		SItemExchangeStats() : switchingToItemID(0), keepHistory(false), switchThisFrame(false) {}
	};

	CCoherentValue<float> inAir;	//double purpose, tells if the actor is in air and for how long
	CCoherentValue<float> onGround;	//double purpose, tells if the actor is on ground and for how long
	CCoherentValue<float> speedFlat;

	int movementDir;
	float inFiring;
	float maxAirSpeed; //The maximum speed the actor has reached over the duration of their current jump/fall. Set to 0 when on the ground.

	bool isGrabbed;
	bool isRagDoll;
	bool isInBlendRagdoll; // To signal when the actor is in the FallNPlay Animation Graph state
	bool wasHit;		// to signal if the player is in air due to an impulse

	bool bStealthKilling;
	bool bStealthKilled;
	bool bAttemptingStealthKill;

	CCoherentValue<bool> isHidden;

	EntityId mountedWeaponID;

	SItemExchangeStats exchangeItemStats;

	float feetHeight[2];

	void EnableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags |= reason; }
	void DisableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags &= ~reason; }
	bool HasAnyStatusFlags(TActorStatusFlags reasons = ~0) const { return (m_actorStatusFlags & reasons) != (TBitfield)EActorStatus::None; }
	bool HasAllStatusFlags(TActorStatusFlags reasons) const { return (m_actorStatusFlags & reasons) == reasons; }
	TActorStatusFlags GetStatusFlags() const { return m_actorStatusFlags; }

	SActorStats()
	{
		memset(this, 0, sizeof(SActorStats));
	}

private:
	TActorStatusFlags m_actorStatusFlags;
};

struct SStanceInfo
{
	//dimensions
	float heightCollider;
	float heightPivot;
	float groundContactEps;

	bool useCapsule;

	Vec3 size;

	//view
	Vec3 viewOffset;
	Vec3 leanLeftViewOffset;
	Vec3 leanRightViewOffset;
	Vec3 whileLeanedLeftViewOffset;
	Vec3 whileLeanedRightViewOffset;
	float viewDownYMod;

	Vec3 peekOverViewOffset;
	Vec3 peekOverWeaponOffset;

	//weapon
	Vec3 weaponOffset;
	Vec3 leanLeftWeaponOffset;
	Vec3 leanRightWeaponOffset;
	Vec3 whileLeanedLeftWeaponOffset;
	Vec3 whileLeanedRightWeaponOffset;

	//movement
	float normalSpeed;
	float maxSpeed;

	//misc
	char name[32];

	Vec3 modelOffset;

	inline Vec3	GetViewOffsetWithLean(float lean, float peekOver, bool useWhileLeanedOffsets = false) const
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverViewOffset - viewOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return viewOffset + a * ((useWhileLeanedOffsets ? whileLeanedLeftViewOffset : leanLeftViewOffset) - viewOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return viewOffset + a * ((useWhileLeanedOffsets ? whileLeanedRightViewOffset : leanRightViewOffset) - viewOffset) + peekOffset;
		}
		return viewOffset + peekOffset;
	}

	inline Vec3	GetWeaponOffsetWithLean(float lean, float peekOver, bool useWhileLeanedOffsets = false) const
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverWeaponOffset - weaponOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return weaponOffset + a * ((useWhileLeanedOffsets ? whileLeanedLeftWeaponOffset : leanLeftWeaponOffset) - weaponOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return weaponOffset + a * ((useWhileLeanedOffsets ? whileLeanedRightWeaponOffset : leanRightWeaponOffset) - weaponOffset) + peekOffset;
		}
		return weaponOffset + peekOffset;
	}

	static inline Vec3 GetOffsetWithLean(float lean, float peekOver, const Vec3& sOffset, const Vec3& sLeftLean, const Vec3& sRightLean, const Vec3& sPeekOffset)
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (sPeekOffset - sOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return sOffset + a * (sLeftLean - sOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return sOffset + a * (sRightLean - sOffset) + peekOffset;
		}
		return sOffset + peekOffset;
	}


	// Returns the size of the stance including the collider and the ground location.
	inline AABB	GetStanceBounds() const
	{
		AABB	aabb(-size, size);
		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Include ground position
		aabb.Add(Vec3(0, 0, 0));

		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	// Returns the size of the collider of the stance.
	inline AABB	GetColliderBounds() const
	{
		AABB	aabb(-size, size);
		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	SStanceInfo()
		: heightCollider(0.0f)
		, heightPivot(0.0f)
		, groundContactEps(0.025f)
		, useCapsule(false)
		, size(0.5f, 0.5f, 0.5f)
		, viewOffset(ZERO)
		, leanLeftViewOffset(ZERO)
		, leanRightViewOffset(ZERO)
		, whileLeanedLeftViewOffset(ZERO)
		, whileLeanedRightViewOffset(ZERO)
		, viewDownYMod(0.0f)
		, peekOverViewOffset(ZERO)
		, peekOverWeaponOffset(ZERO)
		, weaponOffset(ZERO)
		, leanLeftWeaponOffset(ZERO)
		, leanRightWeaponOffset(ZERO)
		, whileLeanedLeftWeaponOffset(ZERO)
		, whileLeanedRightWeaponOffset(ZERO)
		, normalSpeed(0.0f)
		, maxSpeed(0.0f)
		, modelOffset(ZERO)
		, name("null")
	{}
};

struct SViewLimitParams
{

	SViewLimitParams() { ClearViewLimit(eVLS_None, true); }

	enum EViewLimitState
	{
		eVLS_None,
		eVLS_Ladder,
		eVLS_Slide,
		eVLS_Item,
		eVLS_Ledge,
		eVLS_Stage
	};

	void SetViewLimit(Vec3 direction, float rangeH, float rangeV, float rangeUp, float rangeDown, EViewLimitState limitType)
	{
		vLimitDir = direction;
		vLimitRangeH = rangeH;
		vLimitRangeV = rangeV;
		vLimitRangeVUp = rangeUp;
		vLimitRangeVDown = rangeDown;
		vLimitType = limitType;
	}

	void ClearViewLimit(EViewLimitState limitType, bool force = false)
	{
		if (limitType == vLimitType || force)
		{
			vLimitDir.zero();
			vLimitRangeH = 0.f;
			vLimitRangeV = 0.f;
			vLimitRangeVUp = 0.f;
			vLimitRangeVDown = 0.f;
			vLimitType = eVLS_None;
		}
	}

	inline Vec3 GetViewLimitDir() const { return vLimitDir; }
	inline float GetViewLimitRangeH() const { return vLimitRangeH; }
	inline float GetViewLimitRangeV() const { return vLimitRangeV; }
	inline float GetViewLimitRangeVUp() const { return vLimitRangeVUp; }
	inline float GetViewLimitRangeVDown() const { return vLimitRangeVDown; }
	inline EViewLimitState GetViewLimitState() const { return vLimitType; }

private:
	Vec3 vLimitDir;
	float vLimitRangeH;
	float vLimitRangeV;
	float vLimitRangeVUp;
	float vLimitRangeVDown;
	EViewLimitState vLimitType;
};

struct SAITurnParams
{
	SAITurnParams()
		: minimumAngle(DEG2RAD(5.0f))
		, minimumAngleForTurnWithoutDelay(DEG2RAD(30.0f))
		, maximumDelay(1.0f)
	{}

	float minimumAngle;                    // (radians; >= 0) Angle deviation that is needed before turning is even considered
	float minimumAngleForTurnWithoutDelay; // (radians; >= minimumAngle) Angle deviation that is needed before turning happens immediately, without delay.
	float maximumDelay;                    // (seconds; >= 0) Maximum delay before we turn. This delay is used when the deviation is the minimumAngle and is scaled down to zero when the deviation is minimumAngleForTurnWithoutDelay.
};

struct SActorParams
{
	enum ESpeedMultiplierReason
	{
		eSMR_Internal,
		eSMR_GameRules,
		eSMR_Item,
		eSMR_SuitDisruptor,

		eSMR_COUNT,
	};

	SViewLimitParams viewLimits;

	Vec3 mountedWeaponCameraTarget;

	float viewFoVScale;

	float internalSpeedMult;
	float speedMultiplier[eSMR_COUNT];
	float meeleHitRagdollImpulseScale;

	float lookFOVRadians;
	float lookInVehicleFOVRadians;
	float aimFOVRadians;
	float maxLookAimAngleRadians;
	bool allowLookAimStrafing; // whether or not the character should turn his body a bit towards the look/aim target when moving
	int cornerSmoother; // default:1; 0 = none, 1 = C2 method: SmoothCD of movement angle, 2 = C3 method: spline prediction

	float fallNPlayStiffness_scale;

	float sprintMultiplier;
	float crouchMultiplier;
	float strafeMultiplier;
	float backwardMultiplier;

	float jumpHeight;

	float leanShift;
	float leanAngle;

	float aimIKFadeDuration;

	float proceduralLeaningFactor;

	string footstepEffectName;
	string remoteFootstepEffectName;
	string foleyEffectName;

	string footstepIndGearAudioSignal_Walk;
	string footstepIndGearAudioSignal_Run;
	bool footstepGearEffect;

	bool canUseComplexLookIK;
	string lookAtSimpleHeadBoneName;
	uint32 aimIKLayer; // [0..15] default: 1
	uint32 lookIKLayer; // [0..15] default: 15

	struct SDynamicAimPose
	{
		string leftArmAimPose;
		string rightArmAimPose;
		string bothArmsAimPose;
	};
	SDynamicAimPose idleDynamicAimPose;
	SDynamicAimPose runDynamicAimPose;
	float bothArmsAimHalfFOV;			// half the FOV in radians for using both arms, in radians, at pitch 0
	float bothArmsAimPitchFactor;	// increase of the half-FOV (in radians) per radian of pitch
	bool useDynamicAimPoses;

	SAITurnParams AITurnParams;

	float stepThresholdDistance;	// threshold distance, that is needed before stepping is even considered
	float stepThresholdTime;			// the current distance deviation needs to be over the stepThresholdDistance for longer than this time before the character steps

	float maxDeltaAngleRateNormal;
	float maxDeltaAngleRateAnimTarget;
	float maxDeltaAngleMultiplayer;
	float maxDeltaAngleRateJukeTurn;
	EStance defaultStance;

	bool smoothedZTurning;


	SActorParams()
		: mountedWeaponCameraTarget(ZERO)
		, viewFoVScale(1.0f)
		, lookFOVRadians(gf_PI)
		, lookInVehicleFOVRadians(gf_PI)
		, aimFOVRadians(gf_PI)
		, maxLookAimAngleRadians(gf_PI)
		, allowLookAimStrafing(true)
		, internalSpeedMult(1.0f)
		, meeleHitRagdollImpulseScale(1.0f)
		, fallNPlayStiffness_scale(1200)
		, sprintMultiplier(1.25f)
		, crouchMultiplier(1.0f)
		, strafeMultiplier(0.9f)
		, backwardMultiplier(0.9f)
		, jumpHeight(1.0f)
		, leanShift(0.5f)
		, leanAngle(11.0f)
		, aimIKFadeDuration(0.6f)
		, footstepGearEffect(false)
		, canUseComplexLookIK(true)
		, aimIKLayer(1)
		, lookIKLayer(15)
		, cornerSmoother(1) // C2 method
		, proceduralLeaningFactor(0.8f) // Crysis1 value
		, bothArmsAimHalfFOV(DEG2RAD(10.0f))
		, bothArmsAimPitchFactor(1.0f)
		, useDynamicAimPoses(false)
		, stepThresholdDistance(0.15f) // Crysis1 value
		, stepThresholdTime(1.0f) // Crysis1 value
		, maxDeltaAngleRateNormal(DEG2RAD(180.f))
		, maxDeltaAngleRateAnimTarget(DEG2RAD(360.f))
		, maxDeltaAngleMultiplayer(DEG2RAD(3600.f))
		, maxDeltaAngleRateJukeTurn(DEG2RAD(720.f))
		, defaultStance(STANCE_STAND)
		, smoothedZTurning(false)
	{
		for (int smr = 0; smr < eSMR_COUNT; ++smr)
			speedMultiplier[smr] = 1.0f;
	}
};