#include "StdAfx.h"

#include "AttachmentComponent.h"

#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"

namespace
{
	static void RegisterAttachmentComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(DoxD::CAttachmentComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterAttachmentComponent);
}

namespace DoxD
{
	void CAttachmentComponent::Initialize()
	{
		OnResetState();
	}

	void CAttachmentComponent::OnResetState()
	{
	}

	void CAttachmentComponent::AddAttachment(IAttachment attachment)
	{
		m_attachment.AddAttachment(attachment);
	}
} // namespace DoxD