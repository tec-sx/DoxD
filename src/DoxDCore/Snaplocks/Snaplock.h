#pragma once

#include <CryExtension/CryGUID.h>

namespace DoxD
{
	struct ISnaplock;

	DECLARE_SHARED_POINTERS(ISnaplock);

#define DECLARE_ATTACHMENT_TYPE(class_name, type_name, guid_hi, guid_lo) const static ISnaplockType class_name { ISnaplockType(type_name, CryGUID { guid_hi, guid_lo }) };


/** Each type needs to be registered with the Snaplock system. */
	struct ISnaplockType
	{
		ISnaplockType() = default;
		ISnaplockType(string typeName, CryGUID typeId) : 
			m_typeName(typeName), 
			m_typeId(typeId)
		{}
		virtual ~ISnaplockType() = default;

		bool operator==(const ISnaplockType& rhs) const { return m_typeId == rhs.m_typeId; }
		bool operator<(const ISnaplockType& rhs) const { return m_typeId < rhs.m_typeId; }

		virtual void Serialize(Serialization::IArchive& ar);

		const CryGUID& GetTypeId() const { return m_typeId; }
		const string GetName() const { return m_typeName; }

	private:
		string m_typeName;
		CryGUID m_typeId{ CryGUID::Null() };
	};


	struct ISnaplock
	{
		ISnaplock() = default;
		ISnaplock(ISnaplockType snaplockType, bool isRoot = false) :
			m_snaplockType(snaplockType),
			m_isRoot(isRoot)
		{}
		virtual ~ISnaplock() = default;

		virtual void Serialize(Serialization::IArchive& ar);

		const ISnaplockType& GetType() const { return m_snaplockType; }
		std::vector<ISnaplock> GetChildren();
		void AddSnaplock(ISnaplock snaplock);

	private:
		ISnaplockType m_snaplockType;
		bool m_isRoot{ false };
		bool m_isInUse{ false };
		std::vector<ISnaplock> m_children;
	};


	DECLARE_ATTACHMENT_TYPE(SLOT_ROOT, "Root", 0xBA5EBA5EBA5EBA5E, 0xBA5EBA5EBA5EBA5E)
}