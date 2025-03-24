#include "StdAfx.h"

#include "DRSInteractionComponent.h"
#include "Components/Interaction/EntityInteractionComponent.h"
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>

namespace DoxD
{
	static void RegisterDRSInteractionComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CDRSInteractionComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterDRSInteractionComponent)

	void CDRSInteractionComponent::ReflectType(Schematyc::CTypeDesc<CDRSInteractionComponent>& desc)
	{
		desc.SetGUID(CDRSInteractionComponent::IID());
		desc.SetEditorCategory("Interaction");
		desc.SetLabel("DRS Interaction");
		desc.SetDescription("No description.");
		desc.SetIcon("icons:ObjectTypes/light.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::None });

		// Mark the entity interaction component as a hard requirement.
		desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CEntityInteractionComponent::IID());

		desc.AddMember(&CDRSInteractionComponent::m_drsResponse, 'resp', "DRSResponse", "DRSResponse", "Verb to pass into DRS e.g. interaction_play_audio.", "");
		desc.AddMember(&CDRSInteractionComponent::m_drsProperties, 'prop', "DRSProperties", "DRS Properties", "A list of properties to be passed to the DRS entity.", Schematyc::CArray<SDRSProperties>{});
	}


	void CDRSInteractionComponent::Initialize()
	{
		// Add new verbs to the interactor.
		if (m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>())
		{
			m_interactor->AddInteraction(std::make_shared<CInteractionDRS>(this));
		}

		// Reset the entity.
		OnResetState();
	}


	void CDRSInteractionComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case EEntityEvent::LevelStarted:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;
		}
	}


	void CDRSInteractionComponent::OnResetState()
	{
	}


	void CDRSInteractionComponent::OnInteractionDRS()
	{
		if (!m_drsResponse.empty())
		{
			auto pTargetEntity = GetEntity();
			auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pTargetEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));
			DRS::IVariableCollectionSharedPtr pContextVariableCollection = gEnv->pDynamicResponseSystem->CreateContextCollection();

			pContextVariableCollection->CreateVariable("Verb", CHashedString(m_drsResponse.c_str()));

			for (int i = 0 ; i < m_drsProperties.Size() ; i++)
			{
				SDRSProperties& prop = m_drsProperties.At(i);
				pContextVariableCollection->CreateVariable(CHashedString(prop.key.c_str()), CHashedString(prop.value.c_str()));
			}

			pDrsProxy->GetResponseActor()->QueueSignal(m_drsResponse.c_str(), pContextVariableCollection);
		}
	}


	bool Serialize(Serialization::IArchive& ar, CDRSInteractionComponent::SDRSProperties& value, const char* szName, const char* szLabel)
	{
		ar(value.key, "key", "Key");
		ar.doc("Key");

		// HACK: This is broken if there is more than one member added. I don't expect a working collection type until 5.5 or 5.6.
		// Consider this broken until then and *don't use this collection to store values*.
		ar(value.value, "value", "Value");
		ar.doc("Value");

		return true;
	}
}