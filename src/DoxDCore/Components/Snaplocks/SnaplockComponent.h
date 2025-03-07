#pragma once

#include "Snaplocks/Snaplock.h"

namespace DoxD
{
	class CSnaplockComponent : public IEntityComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		// ~IEntityComponent
	public:
		CSnaplockComponent() = default;
		virtual ~CSnaplockComponent() = default;

		static void ReflectType(Schematyc::CTypeDesc<CSnaplockComponent>& desc)
		{
			desc.SetGUID(CSnaplockComponent::IID());
			desc.SetEditorCategory("Snaplocks");
			desc.SetLabel("Snaplocks");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags(IEntityComponent::EFlags::Transform);
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{CD211079-A759-4257-B605-44DC6AB31E20}"_cry_guid;
			return id;
		}

		virtual void OnResetState();

		void AddSnaplock(ISnaplock snaplock);

	private:
		ISnaplock m_snaplock{ SLOT_ROOT, false };
		string m_definitionFile;
	};
} // namespace DoxD

