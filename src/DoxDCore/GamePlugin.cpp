// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "GamePlugin.h"

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <CrySystem/ISystem.h>
#include <IGameObjectSystem.h>
#include <IGameObject.h>
#include "DynamicResponseSystem/ConditionDistanceToEntity.h"

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include "Console/GameCVars.h"
namespace DoxD
{
	CGamePlugin::CGamePlugin()
		: m_pConsole(0)
	{
		m_pCVars = new SCVars();
		g_pCVars = m_pCVars;
		g_pGame = this;
	}

	CGamePlugin::~CGamePlugin()
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
		gEnv->pGameFramework->UnregisterListener(this);

		if (gEnv->pSchematyc)
		{
			gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
		}

		m_pCVars->UnregisterCVars();
	}

	bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
	{
		// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");

		m_pConsole = gEnv->pConsole;

		assert(m_pConsole);

		if (m_pCVars)
		{
			m_pCVars->RegisterCVars(m_pConsole);
		}

		return true;
	}

	void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
	{
		switch (event)
		{
		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			// Register all components that belong to this plug-in
			auto staticAutoRegisterLambda = [](Schematyc::IEnvRegistrar& registrar)
				{
					// Call all static callback registered with the CRY_STATIC_AUTO_REGISTER_WITH_PARAM
					Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);
				};

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CGamePlugin::GetCID(),
						"EntityComponents",
						"TecSX",
						"Components",
						staticAutoRegisterLambda
					)
				);
			}
		}
		break;

		// Called when the game framework has initialized and we are ready for game logic to start
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			gEnv->pGameFramework->RegisterListener(this, "CGame", FRAMEWORKLISTENERPRIORITY_GAME);

			// We need to register the procedural contexts.
			//IProceduralClipFactory& proceduralClipFactory = gEnv->pGameFramework->GetMannequinInterface().GetProceduralClipFactory();
			//mannequin::RegisterProceduralClipsForModule(proceduralClipFactory);

			// #TODO: this is also null at this point in init...move as well.
			if (gEnv->pDynamicResponseSystem)
			{
				// Register the custom DRS actions and conditions.
				REGISTER_DRS_CUSTOM_CONDITION(CConditionDistanceToEntity);
				//REGISTER_DRS_CUSTOM_ACTION(CActionClose);
				//REGISTER_DRS_CUSTOM_ACTION(CActionLock);
				//REGISTER_DRS_CUSTOM_ACTION(CActionOpen);
				//REGISTER_DRS_CUSTOM_ACTION(CActionPlayAnimation);
				//REGISTER_DRS_CUSTOM_ACTION(CActionSwitch);
				//REGISTER_DRS_CUSTOM_ACTION(CActionUnlock);
			}

			// Don't need to load the map in editor
			if (!gEnv->IsEditor())
			{
				// Load the example map in client server mode
				gEnv->pConsole->ExecuteString("map lv_sandbox", false, true);
			}
		}
		break;

		case ESYSTEM_EVENT_LEVEL_UNLOAD:
		{

		}
		break;
		}
	}

	float CGamePlugin::GetFOV() const
	{
		return g_pCVars->cm_fov;
	}

	CRYREGISTER_SINGLETON_CLASS(CGamePlugin)
}