#include "Actor.h"

CActor::CActor()
	: m_stance(STANCE_NULL)
	, m_desiredStance(STANCE_NULL)
{ }

//void CActor::Initialize()
//{
//}

bool CActor::UpdateStance()
{
	if (m_stance != m_desiredStance)
	{
		// If character is animated, postpone stance change until state transition is finished (i.e. in steady state).
		//if ((m_pAnimatedCharacter != NULL) && m_pAnimatedCharacter->InStanceTransition())
		//	return false;

		if (!TrySetStance(m_desiredStance))
		{
#ifdef STANCE_DEBUG
			// Only time this should be allowed is when we're already in a valid state or we have no physics... otherwise warn user!
			// (Temporarily only warn about players, although really NPCs shouldn't be left in STANCE_NULL either.)
			if (m_stance == STANCE_NULL && GetEntity()->GetPhysics() && IsPlayer())
			{
				const Vec3 pos = GetEntity()->GetWorldPos();

				switch (m_tryToChangeStanceCounter)
				{
				case kNumFramesUntilDisplayNullStanceWarning + 1:
					break;

				case kNumFramesUntilDisplayNullStanceWarning:
					DesignerWarning(false, "%s '%s' (%g %g %g) can't change stance from %d '%s' to %d '%s' (tried and failed for %u consecutive frames). Possibly a spawn-point intersecting with level geometry?", GetEntity()->GetClass()->GetName(), GetEntity()->GetName(), pos.x, pos.y, pos.z, (int)m_stance, GetStanceName(m_stance), (int)m_desiredStance, GetStanceName(m_desiredStance), m_tryToChangeStanceCounter);
					++m_tryToChangeStanceCounter;
					break;

				default:
					++m_tryToChangeStanceCounter;
					CryLog("%s '%s' (%g %g %g) can't change stance from %d '%s' to %d '%s' [Attempt %u]", GetEntity()->GetClass()->GetName(), GetEntity()->GetName(), pos.x, pos.y, pos.z, (int)m_stance, GetStanceName(m_stance), (int)m_desiredStance, GetStanceName(m_desiredStance), m_tryToChangeStanceCounter);
					break;
				}
			}
#endif
			return false;
		}

		EStance newStance = m_desiredStance;
		EStance oldStance = m_stance;
		m_stance = newStance;

#ifdef STANCE_DEBUG
		if (m_tryToChangeStanceCounter)
		{
			const Vec3 pos = GetEntity()->GetWorldPos();
			CryLog("%s '%s' (%g %g %g) has finally changed stance from %d '%s' to %d '%s'", GetEntity()->GetClass()->GetName(), GetEntity()->GetName(), pos.x, pos.y, pos.z, (int)oldStance, GetStanceName(oldStance), (int)newStance, GetStanceName(m_desiredStance));
			m_tryToChangeStanceCounter = 0;
		}
#endif

		OnStanceChanged(newStance, oldStance);

		// Request new animation stance.
		// AnimatedCharacter has it's own understanding of stance, which might be in conflict.
		// Ideally the stance should be maintained only in one place. Currently the Actor (gameplay) rules.
	//	if (m_pAnimatedCharacter != NULL)
	//		m_pAnimatedCharacter->RequestStance(m_stance, GetStanceInfo(m_stance)->name);

	//	IPhysicalEntity* pPhysEnt = GetEntity()->GetPhysics();
	//	if (pPhysEnt != NULL)
	//	{
	//		pe_action_awake aa;
	//		aa.bAwake = 1;
	//		pPhysEnt->Action(&aa);
	//	}
	//}

		return true;
	}

	return false;
}

void CActor::OnSetStance(EStance desiredStance)
{
	//CRY_ASSERT(desiredStance >= 0 && desiredStance < STANCE_LAST, "%s '%s' desired stance %d is out of range 0-%d", GetEntity()->GetClass()->GetName(), GetEntity()->GetName(), desiredStance, STANCE_LAST - 1);

#if ENABLE_GAME_CODE_COVERAGE || ENABLE_SHARED_CODE_COVERAGE
	if (m_desiredStance != desiredStance)
	{
		switch (m_desiredStance)
		{
		case STANCE_STAND:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStopStand);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStopStand);
			break;

		case STANCE_CROUCH:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStopCrouch);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStopCrouch);
			break;

		case STANCE_SWIM:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStopSwim);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStopSwim);
			break;
		}

		switch (desiredStance)
		{
		case STANCE_STAND:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStartStand);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStartStand);
			break;

		case STANCE_CROUCH:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStartCrouch);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStartCrouch);
			break;

		case STANCE_SWIM:
			CCCPOINT_IF(m_isClient, ActorStance_LocalActorStartSwim);
			CCCPOINT_IF(!m_isClient, ActorStance_OtherActorStartSwim);
			break;
		}
	}
#endif

	m_desiredStance = desiredStance;
}

void CActor::SetStance(EStance desiredStance)
{
	OnSetStance(desiredStance);
}

void CActor::OnStanceChanged(EStance newStance, EStance oldStance)
{
	//CItem* pCurrentItem = GetItem(GetCurrentItemId(false));
	//if (pCurrentItem != NULL)
	//{
	//	pCurrentItem->OnOwnerStanceChanged(newStance);
	//}
}

bool CActor::TrySetStance(EStance stance)
{
	//IPhysicalEntity* pPhysEnt = GetEntity()->GetPhysics();
	int result = 0;
//	if (pPhysEnt)
//	{
//		const SStanceInfo* sInfo = GetStanceInfo(stance);
//#ifdef STANCE_DEBUG
//		if (sInfo == &m_defaultStance)
//		{
//			CryLogAlways("%s trying to set undefined stance (%d).\nPlease update the stances tables in the Lua script accordingly.",
//				GetEntity()->GetName(), stance);
//		}
//#endif
//
//		// don't changes to an invalid stance.
//		if (stricmp(sInfo->name, "null") == 0)
//		{
//			return false;
//		}
//
//		pe_player_dimensions playerDim;
//		playerDim.heightEye = 0.0f;
//		playerDim.heightCollider = sInfo->heightCollider;
//		playerDim.sizeCollider = sInfo->size;
//		playerDim.heightPivot = sInfo->heightPivot;
//		if (m_stance != STANCE_NULL || stance == STANCE_CROUCH)
//		{
//			playerDim.maxUnproj = max(0.25f, sInfo->heightPivot);
//		}
//		else
//		{
//			// If we're coming from a NULL stance, try and be a bit for aggressive at
//			// unprojecting from collisions.
//			// fixes some last minute s/l bugs on Crysis2.
//			playerDim.maxUnproj = max(sInfo->size.x, sInfo->heightPivot);
//			playerDim.maxUnproj = max(0.35f, playerDim.maxUnproj);
//			playerDim.dirUnproj = ZERO;
//		}
//		playerDim.bUseCapsule = sInfo->useCapsule;
//		playerDim.groundContactEps = sInfo->groundContactEps;
//
//		result = pPhysEnt->SetParams(&playerDim);
//	}

	return (result != 0);
}
