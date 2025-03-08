#include "StdAfx.h"

#include "ConditionDistanceToEntity.h"
#include <CrySerialization/IArchive.h>

namespace DoxD
{
	CConditionDistanceToEntity::CConditionDistanceToEntity()
	{
	}

	CConditionDistanceToEntity::CConditionDistanceToEntity(const string& actorName)
		: m_entityName(actorName)
	{ }

	bool CConditionDistanceToEntity::IsMet(DRS::IResponseInstance* pResponseInstance)
	{
		IEntity* pTargetEntity = gEnv->pEntitySystem->FindEntityByName(m_entityName.c_str());
		if (pTargetEntity)
		{
			IEntity* pSourceEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
			if (pSourceEntity)
			{
				return pTargetEntity->GetWorldPos().GetSquaredDistance(pSourceEntity->GetWorldPos()) < m_squaredDistance;
			}
		}

		return false;
	}


	void CConditionDistanceToEntity::Serialize(Serialization::IArchive& ar)
	{
		float distance = sqrt(m_squaredDistance);
		ar(distance, "Distance", "^> Distance");
		m_squaredDistance = distance * distance;
		ar(m_entityName, "EntityName", "^EntityName");
	}


	string CConditionDistanceToEntity::GetVerboseInfo() const
	{
		return m_entityName + "' < than " + CryStringUtils::toString(sqrt(m_squaredDistance)).c_str();
	}
}