#pragma once

#include "Contracts/IEntityInteraction.h"


namespace DoxD
{
	class CEntityInteractionComponent;


	class CDRSInteractionComponent
		: public IEntityComponent
		, public IInteractionDRS
	{
	public:
		CDRSInteractionComponent() {}
		virtual ~CDRSInteractionComponent() {}

		static void ReflectType(Schematyc::CTypeDesc<CDRSInteractionComponent>& desc);

		static CryGUID& IID()
		{
			static CryGUID id = "{25963340-C513-405A-9C44-3CC6EDE66B5A}"_cry_guid;
			return id;
		}

		struct SDRSProperties
		{
			inline bool operator==(const SDRSProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
			inline bool operator!=(const SDRSProperties& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

			Schematyc::CSharedString key;
			Schematyc::CSharedString value;

			static void ReflectType(Schematyc::CTypeDesc<SDRSProperties>& desc)
			{
				desc.SetGUID("{C07C367C-106F-4FD4-B7D5-E40C1A21F9F3}"_cry_guid);
				desc.AddMember(&SDRSProperties::key, 'key', "Key", "Key", "DRS Key", "");
				desc.AddMember(&SDRSProperties::value, 'valu', "Value", "Value", "DRS Value", "");
			}
		};

		// IEntityComponent
		void Initialize() override;
		void ProcessEvent(const SEntityEvent& event) override;
		// ~IEntityComponent

		// IDRSInteractionEntityComponent
		virtual void OnResetState() final;
		// IDRSInteractionEntityComponent

		// IInteractionDRS
		void OnInteractionDRS() override;
		// ~IInteractionDRS

	private:
		CEntityInteractionComponent* m_interactor{ nullptr };
		Schematyc::CSharedString m_drsResponse;
		Schematyc::CArray<SDRSProperties> m_drsProperties;
	};

	bool Serialize(Serialization::IArchive& archive, CDRSInteractionComponent::SDRSProperties& value, const char* szName, const char* szLabel);
}