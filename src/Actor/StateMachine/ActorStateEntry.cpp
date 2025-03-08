#include <StdAfx.h>

#include "ActorStateEvents.h"
#include <Components/Actor/ActorControllerComponent.h>


namespace DoxD
{
	class CActorStateEntry : private CStateHierarchy<CActorControllerComponent>
	{
		DECLARE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateEntry);
		DECLARE_STATE_CLASS_END(CActorControllerComponent);
	};


	DEFINE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateEntry, eActorState_Entry, Root);
	DEFINE_STATE_CLASS_END(CActorControllerComponent, CActorStateEntry);


	const CActorStateEntry::TStateIndex CActorStateEntry::Root(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
	{
		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
		case eActorEvent_Entry:
			RequestTransitionState(actorControllerComponent, eActorState_Movement);
			break;

		case eActorEvent_EntryAI:
			RequestTransitionState(actorControllerComponent, eActorState_MovementAI);
			break;
		}

		return State_Continue;
	}
}