#pragma once

#include <IActionMapManager.h>
#include <Utility/StateMachine.h>
#include <Utility/AutoEnum.h>

namespace DoxD
{
	enum EActorStateEvent
	{
		eActorEvent_PrePhysicsUpdate = eStateEVent_Custom,
		eActorEvent_Update,
		eActorEvent_BacameLocalPlayer,
		eActorEvent_RagdollPhysicalized,
		eActorEvent_Entry,
		eActorEvent_EntryAI,
		eActorEvent_Dead,
		eActorEvent_Ground,
		eActorEvent_Fall,
		eActorEvent_Jump,
		///< #TODO: investigate this. Seems to help determine if you're on the ground or falling.
		eActorEvent_GroundColliderChanged,
		eActorEvent_Input,
		eActorEvent_Fly,
		eActorEvent_WeaponChanged,
		eActorEvent_StanceChanged,
		eActorEvent_Cutscene,
		eActorEvent_InteractiveAction,

		// Required to use ladders.
		// #TODO: Would it be possible for AI / Pets / Mounts to use ladders?
		eActorEvent_Ladder,
		eActorEvent_LadderLeave,
		eActorEvent_LadderPosition,

		// Vehicles / characters?
		eActorEvent_Attach,
		eActorEvent_Detach,

		// I believe these are used for viewing or skipping some form of player intro video. Unsure at present.
		eActorEvent_IntroStart,
		eActorEvent_IntroFinished,

		// Used with co-operative animations, for instance stealth kills.
		eActorEvent_StealthKill,
		eActorEvent_CoopAnimationFinished,

		///< Don't use, except to chain more events to the end of this list.
		eActorEvent_Last,
	};

#define eActorStateFlags(f) \
	f(eActorStateFlags_PrePhysicsUpdateAfterEnter) \
	f(eActorStateFlags_DoUpdate) \
	f(eActorStateFlags_IsUpdating) \
	f(eActorStateFlags_Ground) \
	f(eActorStateFlags_Jump) \
	f(eActorStateFlags_Sprinting) \
	f(eActorStateFlags_SprintPressed) \
	f(eActorStateFlags_Swimming) \
	f(eActorStateFlags_InAir) \
	f(eActorStateFlags_PhysicsSaysFlying) \
	f(eActorStateFlags_CurrentItemIsHeavy) \
	f(eActorStateFlags_InteractiveAction) \
	f(eActorStateFlags_OnLadder)

	AUTOENUM_BUILDFLAGS_WITHZERO(eActorStateFlags, eActorStateFlags_None);

	/*enum EActorStateFlags : TBitfield
	{
		eActorStateFlags_None = BIT32(0),
		eActorStateFlags_PrePhysicsUpdateAfterEnter = BIT32(1),
		eActorStateFlags_DoUpdate = BIT32(2),
		eActorStateFlags_IsUpdating = BIT32(3),
		eActorStateFlags_Ground = BIT32(4),
		eActorStateFlags_Jump = BIT32(5),
		eActorStateFlags_Sprinting = BIT32(6),
		eActorStateFlags_SprintPressed = BIT32(7),
		eActorStateFlags_Swimming = BIT32(8),
		eActorStateFlags_InAir = BIT32(9),
		eActorStateFlags_PhysicsSaysFlying = BIT32(10),
		eActorStateFlags_CurrentItemIsHeavy = BIT32(11),
		eActorStateFlags_InteractiveAction = BIT32(12),
		eActorStateFlags_OnLadder = BIT32(13),
	};*/

	//CRY_CREATE_ENUM_FLAG_OPERATORS(EActorStateFlags);

	enum EActorStates
	{
		eActorState_Entry = eState_First,
		eActorState_Movement,
		eActorState_MovementAI,
		eActorState_Animation,
		eActorState_Linked,
	};

	struct SActorPrePhysicsData
	{
		const float m_frameTime;

		SActorPrePhysicsData(float frameTime)
			: m_frameTime(frameTime)
		{
		}

	private:
		// DO NOT IMPLEMENT!!!
		SActorPrePhysicsData();
	};


	struct SStateEventActorMovementPrePhysics : public SStateEvent
	{
		SStateEventActorMovementPrePhysics(const SActorPrePhysicsData* pPrePhysicsData) : SStateEvent(eActorEvent_PrePhysicsUpdate)
		{
			AddData(pPrePhysicsData);
		}

		const SActorPrePhysicsData& GetPrePhysicsData() const { return *static_cast<const SActorPrePhysicsData*> (GetData(0).GetPtr()); }

	private:
		// DO NOT IMPLEMENT!!!
		SStateEventActorMovementPrePhysics();
	};


	struct SStateEventUpdate : public SStateEvent
	{
		SStateEventUpdate(const float frameTime) : SStateEvent(eActorEvent_Update)
		{
			AddData(frameTime);
		}

		const float GetFrameTime() const { return GetData(0).GetFloat(); }

	private:
		// DO NOT IMPLEMENT!!!
		SStateEventUpdate();
	};


	struct SStateEventFly : public SStateEvent
	{
		SStateEventFly(const uint8 flyMode) : SStateEvent(eActorEvent_Fly)
		{
			AddData(flyMode);
		}

		const int GetFlyMode() const
		{
			return GetData(0).GetInt();
		}

	private:
		// DO NOT IMPLEMENT!
		SStateEventFly();
	};


	struct SStateEventInteractiveAction : public SStateEvent
	{
		SStateEventInteractiveAction(const EntityId objectId, bool bUpdateVisibility, int interactionIndex = 0, float actionSpeed = 1.0f)
			: SStateEvent(eActorEvent_InteractiveAction)
		{
			AddData(static_cast<int> (objectId));
			AddData(bUpdateVisibility);
			AddData(actionSpeed);
			AddData(static_cast<int> (interactionIndex));
		}


		SStateEventInteractiveAction(const char* interactionName, bool bUpdateVisibility, float actionSpeed = 1.0f)
			: SStateEvent(eActorEvent_InteractiveAction)
		{
			AddData(interactionName);
			AddData(bUpdateVisibility);
			AddData(actionSpeed);
		}


		int GetObjectEntityID() const
		{
			if (GetData(0).m_type == SStateEventData::eSEDT_int)
			{
				return GetData(0).GetInt();
			}
			return 0;
		}


		const char* GetObjectInteractionName() const
		{
			if (GetData(0).m_type == SStateEventData::eSEDT_voidptr)
			{
				return static_cast<const char*> (GetData(0).GetPtr());
			}
			return nullptr;
		}


		int GetInteractionIndex() const
		{
			if (GetData(3).m_type == SStateEventData::eSEDT_int)
			{
				return GetData(3).GetInt();
			}
			return 0;
		}


		bool GetShouldUpdateVisibility() const
		{
			return GetData(1).GetBool();
		}


		float GetActionSpeed() const
		{
			return GetData(2).GetFloat();
		}
	};


	struct SInputEventData
	{
		enum class EInputEvent
		{
			jump,
			slide,
			sprint,
			buttonMashingSequence,
		};

		SInputEventData(EInputEvent inputEvent)
			: m_inputEvent(inputEvent), m_entityId(0), m_actionId(""), m_activationMode(0), m_value(0.0f)
		{
		}

		SInputEventData(EInputEvent inputEvent, EntityId entityId, const ActionId& actionId, int activationMode, float value)
			: m_inputEvent(inputEvent), m_entityId(entityId), m_actionId(actionId), m_activationMode(activationMode), m_value(value)
		{
		}

		EInputEvent m_inputEvent;
		const EntityId m_entityId;
		const ActionId m_actionId;
		const int m_activationMode;
		const float m_value;
	};


	struct SStateEventActorInput : public SStateEvent
	{
		SStateEventActorInput(const SInputEventData* pInputEvent) : SStateEvent(eActorEvent_Input)
		{
			AddData(pInputEvent);
		}

		const SInputEventData& GetInputEventData() const { return *static_cast<const SInputEventData*> (GetData(0).GetPtr()); }
	};


	struct SStateEventStanceChanged : public SStateEvent
	{
		SStateEventStanceChanged(const int targetStance) : SStateEvent(eActorEvent_StanceChanged)
		{
			AddData(targetStance);
		}

		ILINE int GetStance() const { return GetData(0).GetInt(); }
	};


	struct SStateEventJump : public SStateEvent
	{
		enum EData
		{
			eD_PrePhysicsData = 0,
			eD_VerticalSpeedMofidier
		};

		SStateEventJump(const SActorPrePhysicsData& data, const float verticalSpeedModifier = 0.0f) : SStateEvent(eActorEvent_Jump)
		{
			AddData(&data);
			AddData(verticalSpeedModifier);
		}

		ILINE const SActorPrePhysicsData& GetPrePhysicsEventData() const { return *static_cast<const SActorPrePhysicsData*> (GetData(eD_PrePhysicsData).GetPtr()); }
		ILINE float GetVerticalSpeedModifier() const { return (GetData(eD_VerticalSpeedMofidier).GetFloat()); }
	};


	struct SStateEventCutScene : public SStateEvent
	{
		explicit SStateEventCutScene(const bool enable) : SStateEvent(eActorEvent_Cutscene)
		{
			AddData(enable);
		}

		ILINE bool IsEnabling() const { return (GetData(0).GetBool()); }
	};


	struct SStateEventGroundColliderChanged : public SStateEvent
	{
		explicit SStateEventGroundColliderChanged(const bool bChanged) : SStateEvent(eActorEvent_GroundColliderChanged)
		{
			AddData(bChanged);
		}

		ILINE const bool OnGround() const { return GetData(0).GetBool(); }
	};


	enum ELadderLeaveLocation
	{
		eLLL_First = 0,
		eLLL_Top = eLLL_First,
		eLLL_Bottom,
		eLLL_Drop,
		eLLL_Count,
	};


	struct SStateEventLadder : public SStateEvent
	{
		SStateEventLadder(IEntity* pLadder) : SStateEvent(eActorEvent_Ladder)
		{
			AddData(pLadder);
		}

		IEntity* GetLadder() const { return (IEntity*)(GetData(0).GetPtr()); }
	};


	struct SStateEventLeaveLadder : public SStateEvent
	{
		SStateEventLeaveLadder(ELadderLeaveLocation leaveLocation) : SStateEvent(eActorEvent_LadderLeave)
		{
			AddData((int)leaveLocation);
		}

		ELadderLeaveLocation GetLeaveLocation() const { return (ELadderLeaveLocation)(GetData(0).GetInt()); }
	};


	struct SStateEventLadderPosition : public SStateEvent
	{
		SStateEventLadderPosition(float heightFrac) : SStateEvent(eActorEvent_LadderPosition)
		{
			AddData(heightFrac);
		}

		float GetHeightFrac() const { return GetData(0).GetFloat(); }
	};
}
