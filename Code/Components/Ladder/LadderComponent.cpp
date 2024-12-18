#include "StdAfx.h"
#include "LadderComponent.h"
#include "GamePlugin.h"
#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/StaticInstanceList.h>



static void RegisterLadderComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CLadderComponent));
		// Functions
		{
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterLadderComponent)

void CLadderComponent::Initialize()
{
	Reset();
}

void CLadderComponent::ProcessEvent(const SEntityEvent& event)
{
	if (event.event == ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED)
	{
		Reset();
	}
}

Cry::Entity::EventFlags CLadderComponent::GetEventMask() const
{
	return ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED;
}

void CLadderComponent::Reset()
{
	if (m_poleGeometryPath.value.size() > 0)
	{
		// Set Up Poles
		GetEntity()->LoadGeometry(POLE_L_GEOMETRY_SLOT, m_poleGeometryPath.value.c_str());
		GetEntity()->LoadGeometry(POLE_R_GEOMETRY_SLOT, m_poleGeometryPath.value.c_str());

		auto poleTransform = Vec3(m_poleDistance, 0.f, 0.f);
		Matrix34 leftPoleMatrix = Matrix34(m_poleScale, Quat::CreateIdentity(), poleTransform);
		Matrix34 rightPoleMatrix = Matrix34(m_poleScale, Quat::CreateIdentity(), poleTransform * -1);

		GetEntity()->SetSlotLocalTM(POLE_L_GEOMETRY_SLOT, leftPoleMatrix);
		GetEntity()->SetSlotLocalTM(POLE_R_GEOMETRY_SLOT, rightPoleMatrix);

		//m_poleGeom = gEnv->p3DEngine->LoadStatObj(m_poleGeometryPath.value.c_str());
		//m_material = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(m_materialPath.value.c_str(), false);
			Vec3 barScale = Vec3(m_barSize, m_barSize, m_poleDistance);
			Quat barRotation = Quat(90.f, GetEntity()->GetForwardDir());
			Vec3 barPosition = Vec3(0.f, 0.f, m_barDistance);
			Matrix34 barMatrix = Matrix34(barScale, barRotation, barPosition);
			
			int barSlot = GetEntity()->LoadGeometry(BAR_GEOMETRY_SLOT, m_barGeometryPath.value.c_str());
			GetEntity()->SetSlotLocalTM(barSlot, barMatrix);

		for (int i = 0; i < 5; i++)
		{
			// Might be a problem with world/local
		}

	}
	else
	{
		FreeEntitySlot();
	}
}
