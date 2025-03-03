#pragma once

#include "Attachments/Attachment.h"

namespace DoxD
{
	class CAttachmentComponent : public IEntityComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		// ~IEntityComponent
	public:
		CAttachmentComponent() = default;
		virtual ~CAttachmentComponent() = default;

		static void ReflectType(Schematyc::CTypeDesc<CAttachmentComponent>& desc)
		{
			desc.SetGUID(CAttachmentComponent::IID());
			desc.SetEditorCategory("Attachments");
			desc.SetLabel("Attachment");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags(IEntityComponent::EFlags::Transform);
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{CD211079-A759-4257-B605-44DC6AB31E20}"_cry_guid;
			return id;
		}

		virtual void OnResetState();

		void AddAttachment(IAttachment attachment);

	private:
		IAttachment m_attachment{ SLOT_ROOT, false };
		string m_definitionFile;
	};
} // namespace DoxD

