#include "Attachment.h"

namespace DoxD
{
	void IAttachmentType::Serialize(Serialization::IArchive& ar)
	{
		ar(m_typeId, "typeId", "Type Id");
		ar.doc(m_typeName);
	}
	
	void IAttachment::Serialize(Serialization::IArchive& ar)
	{
		ar(m_attachmentType, "attachmentType", "AttachmentType");
		ar(m_isInUse, "isInUse", "Is this attachment in use?");
		ar(m_children, "children", "Children Attachments");
	}

	std::vector<IAttachment> IAttachment::GetChildren()
	{
		return std::vector<IAttachment>();
	}

	void IAttachment::AddAttachment(IAttachment attachment)
	{
		m_children.push_back(attachment);
	}
}