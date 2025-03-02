// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "GamePlugin.h"

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>

#include <IGameObjectSystem.h>
#include <IGameObject.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include "GameCVars.h"

CGamePlugin* g_pGame = 0;
SCVars* g_pGameCVars = 0;

// CCountdownTimer
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCountdownTimer::CCountdownTimer(FN_COMPLETION_CALLBACK fnComplete) :
	m_fnCompleted(fnComplete),
	m_timeLeft(0.0f)
{}

void CCountdownTimer::Start(float countdownDurationSeconds)
{
	m_timeLeft = countdownDurationSeconds;
}

void CCountdownTimer::Advance(float dt)
{
	if (m_timeLeft > 0.0f)
	{
		m_timeLeft -= dt;

		if (m_timeLeft <= 0.0f)
		{
			if (m_fnCompleted != nullptr)
			{
				m_fnCompleted();
			}
		}
	}
}

bool CCountdownTimer::IsActive()
{
	return m_timeLeft > 0.0f;
}

void CCountdownTimer::Abort()
{
	m_timeLeft = 0.0f;
}

float CCountdownTimer::ToSeconds()
{
	return m_timeLeft;
}

// CGamePlugin
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CGamePlugin::CGamePlugin()
	: m_pConsole(0)
{
	m_pCVars = new SCVars();
	g_pGameCVars = m_pCVars;
	g_pGame = this;
}

CGamePlugin::~CGamePlugin()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
	}

	g_pGame = 0;
	g_pGameCVars = 0;
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");

	m_pConsole = gEnv->pConsole;

	RegisterConsoleVars();
	RegisterConsoleCommands();

	return true;
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
	case ESYSTEM_EVENT_GAME_POST_INIT:
	{
		// Don't need to load the map in editor
		if (!gEnv->IsEditor())
		{
			// Load the example map in client server mode
			gEnv->pConsole->ExecuteString("map lv_sandbox s", false, true);
		}

		CreatePlayer();
	}
	break;

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
					"Crytek GmbH",
					"Components",
					staticAutoRegisterLambda
				)
			);
		}
	}
	break;

	case ESYSTEM_EVENT_LEVEL_UNLOAD:
	{

	}
	break;
	}
}

void CGamePlugin::CreatePlayer()
{
	// Create Player camera entity
	//SEntitySpawnParams cameraSpawnParams;
	//cameraSpawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	//cameraSpawnParams.sName = "PlayerCamera";

	//if (IEntity* pCameraEntity = gEnv->pEntitySystem->SpawnEntity(cameraSpawnParams))
	//{
	//	// Add player camera component to cameraentity
	//	auto pPlayerCameraComponent = pCameraEntity->GetOrCreateComponentClass<DoxD::DefaultComponents::CPlayerCameraComponent>();
	//	
		// Create Player entity

	//SEntitySpawnParams playerSpawnParams;
	//playerSpawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	//playerSpawnParams.sName = "Player";

	//if (IEntity* pPlayerEntity = gEnv->pEntitySystem->SpawnEntity(playerSpawnParams))
	//{
	//	// Add player component to player entity
	//	//auto pPlayerComponent = 
	//		pPlayerEntity->GetOrCreateComponent<DoxD::CPlayerComponent>();

	//	//// Attach camera to player
	//	//pCameraEntity->AddEntityLink("TargetActor", pPlayerEntity->GetId(), pPlayerEntity->GetGuid());
	//}
	//}
}

float CGamePlugin::GetFOV() const
{
	return g_pGameCVars->cm_fov;
}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)