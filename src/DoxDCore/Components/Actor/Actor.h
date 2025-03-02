#pragma once
#include <CryAISystem/IAgent.h> // Used for EStance
#include "ActorDefinitions.h"

class CActor
{
public:

	CActor();
	virtual ~CActor() {}

	// Stance
	inline EStance GetStance() const { return m_stance.Value(); }

	inline const SStanceInfo* GetStanceInfo(EStance stance) const
	{
		if (stance < 0 || stance > STANCE_LAST)
		{
			return &m_defaultStance;
		}
		else
		{
			return &m_stances[stance];
		}
	}

	//get actor status
	virtual SActorStats* GetActorStats() { return 0; };
	virtual const SActorStats* GetActorStats() const { return 0; };
	SActorParams& GetActorParams() { return m_params; };
	const SActorParams& GetActorParams() const { return m_params; };

protected:
	CCoherentValue<EStance> m_stance;
	EStance m_desiredStance;

	SStanceInfo m_defaultStance;
	SStanceInfo m_stances[STANCE_LAST];

	SActorParams m_params;

	// Stance
	virtual bool UpdateStance();
	void OnSetStance(EStance desiredStance);
	virtual void SetStance(EStance desiredStance);
	virtual void OnStanceChanged(EStance newStance, EStance oldStance);
	virtual bool TrySetStance(EStance stance);
};

