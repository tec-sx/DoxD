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
#include "Components/Player/PlayerComponent.h"
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
			gEnv->pGameFramework->AddNetworkedClientListener(*this);

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
				gEnv->pConsole->ExecuteString("map 01_bar", false, true);
			}
		}
		break;
		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			// HACK: TEST: I need a convenient time to write back the Simulation so I can examine it. This will do for now.
			//ECS::Simulation.SavePrototypeData();

			// In the editor, we wait until now before attempting to connect to the local player. This is to ensure all the
			// entities are already loaded and initialised. It works differently in game mode. 
			if (gEnv->IsEditor())
			{
				if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
					pPlayer->OnClientConnect();
			}
			break;
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
		{
		}
		break;
		}
	}

	bool CGamePlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
	{
		// Connection received from a client, create a player entity and component
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
		spawnParams.sName = "Player";
		spawnParams.id = LOCAL_PLAYER_ENTITY_ID;
		spawnParams.nFlags |= ENTITY_FLAG_LOCAL_PLAYER;

		// Spawn the player entity
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			// Set the local player entity channel id, and bind it to the network so that it can support Multiplayer contexts
			pPlayerEntity->GetNetEntity()->SetChannelId(channelId);
			pPlayerEntity->GetNetEntity()->BindToNetwork();

			// Create the player component instance
			pPlayerEntity->GetOrCreateComponent<CPlayerComponent>();
		}

		return true;
	}

	bool CGamePlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
	{
		// In game mode, we can attempt to connect to the local player at this point. This is because all the entities
		// should already be loaded and initialised by now. It works differently in the editor. 
		if (!gEnv->IsEditor())
		{
			if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
				pPlayer->OnClientConnect();
		}

		return true;
	}

	CRYREGISTER_SINGLETON_CLASS(CGamePlugin)
}