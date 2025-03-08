#include "StdAfx.h"

#include "Snaplock.h"

namespace DoxD
{
	void ISnaplockType::Serialize(Serialization::IArchive& ar)
	{
		ar(m_typeId, "typeId", "Type Id");
		ar.doc(m_typeName);
	}
	
	void ISnaplock::Serialize(Serialization::IArchive& ar)
	{
		ar(m_snaplockType, "snaplockType", "SnaplockType");
		ar(m_isInUse, "isInUse", "Is this snaplock in use?");
		ar(m_children, "children", "Children Snaplocks");
	}

	std::vector<ISnaplock> ISnaplock::GetChildren()
	{
		return std::vector<ISnaplock>();
	}

	void ISnaplock::AddSnaplock(ISnaplock snaplock)
	{
		m_children.push_back(snaplock);
	}
}