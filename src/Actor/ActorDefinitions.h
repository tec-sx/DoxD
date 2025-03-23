#pragma once
#include <CryGame/CoherentValue.h>
#include "IAnimationGraph.h"
#include "IAnimatedCharacter.h"
#include "IMovementController.h"
#include <CryCore/CryFlags.h>
#include <CryPhysics/IPhysics.h>
#include "Utility/AutoEnum.h"

namespace DoxD
{
	struct SActorPhysics
	{
		enum EActorPhysicsFlags
		{
			Flying = BIT(0),
			WasFlying = BIT(1),
			Stuck = BIT(2)
		};


		SActorPhysics()
			: angVelocity(ZERO)
			, velocity(ZERO)
			, velocityDelta(ZERO)
			, velocityUnconstrained(ZERO)
			, velocityUnconstrainedLast(ZERO)
			, gravity(ZERO)
			, groundNormal(0, 0, 1)
			, speed(0)
			, groundHeight(0.0f)
			, mass(80.0f)
			, lastFrameUpdate(0)
			, groundMaterialIdx(-1)
			, groundColliderId(0)
		{
		}

		void Serialize(TSerialize ser, EEntityAspects aspects);

		CCryFlags<uint32> flags;

		Vec3 angVelocity;
		Vec3 velocity;
		Vec3 velocityDelta;
		Vec3 velocityUnconstrained;
		Vec3 velocityUnconstrainedLast;
		Vec3 gravity;
		Vec3 groundNormal;

		float speed;
		float groundHeight;
		float mass;

		int lastFrameUpdate;
		int groundMaterialIdx;
		EntityId groundColliderId;

	};

	typedef TBitfield TActorStatusFlags;

#define ActorStatusFlagList(func)                \
	func(kActorStatus_onLadder)                    \
	func(kActorStatus_stealthKilling)              \
	func(kActorStatus_stealthKilled)               \
	func(kActorStatus_attemptingStealthKill)       \
	func(kActorStatus_pickAndThrow)                \
	func(kActorStatus_vaultLedge)                  \
	func(kActorStatus_swimming)                    \
	func(kActorStatus_linkedToVehicle)             \

	AUTOENUM_BUILDFLAGS_WITHZERO(ActorStatusFlagList, kActorStatus_none);

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
		float maxAirSpeed; //The maximum speed the actor has reached over the duration of their current jump/fall. Set to 0 when on the ground.

		bool isGrabbed;
		bool isRagDoll;
		bool isInBlendRagdoll; // To signal when the actor is in the FallNPlay Animation Graph state
		bool wasHit;		   // to signal if the player is in air due to an impulse

		CCoherentValue<bool> isHidden;

		EntityId mountedToolID;

		SItemExchangeStats exchangeItemStats;

		float feetHeight[2];

		void EnableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags |= reason; }
		void DisableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags &= ~reason; }
		bool HasAnyStatusFlags(TActorStatusFlags reasons = ~0) const { return (m_actorStatusFlags & reasons) != kActorStatus_none; }
		bool HasAllStatusFlags(TActorStatusFlags reasons) const { return (m_actorStatusFlags & reasons) == reasons; }
		TActorStatusFlags GetStatusFlags() const { return m_actorStatusFlags; }

		SActorStats()
		{
			memset(this, 0, sizeof(SActorStats));
		}

	private:
		TActorStatusFlags m_actorStatusFlags;
	};
}
