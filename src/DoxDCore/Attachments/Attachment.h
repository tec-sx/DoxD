#pragma once

#include <CryExtension/CryGUID.h>

namespace DoxD
{
	struct IAttachment;

	DECLARE_SHARED_POINTERS(IAttachment);

#define DECLARE_ATTACHMENT_TYPE(class_name, type_name, guid_hi, guid_lo) const static IAttachmentType class_name { IAttachmentType(type_name, CryGUID { guid_hi, guid_lo }) };


/** Each type needs to be registered with the Attachment system. */
	struct IAttachmentType
	{
		IAttachmentType() = default;
		IAttachmentType(string typeName, CryGUID typeId) : 
			m_typeName(typeName), 
			m_typeId(typeId)
		{}
		virtual ~IAttachmentType() = default;

		bool operator==(const IAttachmentType& rhs) const { return m_typeId == rhs.m_typeId; }
		bool operator<(const IAttachmentType& rhs) const { return m_typeId < rhs.m_typeId; }

		virtual void Serialize(Serialization::IArchive& ar);

		const CryGUID& GetTypeId() const { return m_typeId; }
		const string GetName() const { return m_typeName; }

	private:
		string m_typeName;
		CryGUID m_typeId{ CryGUID::Null() };
	};


	struct IAttachment
	{
		IAttachment() = default;
		IAttachment(IAttachmentType attachmentType, bool isRoot = false) :
			m_attachmentType(attachmentType),
			m_isRoot(isRoot)
		{}
		virtual ~IAttachment() = default;

		virtual void Serialize(Serialization::IArchive& ar);

		const IAttachmentType& GetType() const { return m_attachmentType; }
		std::vector<IAttachment> GetChildren();
		void AddAttachment(IAttachment attachment);

	private:
		IAttachmentType m_attachmentType;
		bool m_isRoot{ false };
		bool m_isInUse{ false };
		std::vector<IAttachment> m_children;
	};


	DECLARE_ATTACHMENT_TYPE(SLOT_ROOT, "Root", 0xBA5EBA5EBA5EBA5E, 0xBA5EBA5EBA5EBA5E)
}