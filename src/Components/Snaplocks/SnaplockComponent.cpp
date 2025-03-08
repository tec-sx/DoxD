#include "StdAfx.h"

#include "SnaplockComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"

namespace
{
	static void RegisterSnaplockComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CSnaplockComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSnaplockComponent);
}

namespace DoxD
{
	void CSnaplockComponent::Initialize()
	{
		OnResetState();
	}

	void CSnaplockComponent::OnResetState()
	{
	}

	void CSnaplockComponent::AddSnaplock(ISnaplock snaplock)
	{
		m_snaplock.AddSnaplock(snaplock);
	}
} // namespace DoxD