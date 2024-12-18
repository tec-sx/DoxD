// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryPhysics/RayCastQueue.h>
#include <CryPhysics/IntersectionTestQueue.h>
#include <CryEntitySystem/IEntityClass.h>

struct SCVars;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CCountdownTimer
{
public:
	typedef void (*FN_COMPLETION_CALLBACK)();

private:
	float                  m_timeLeft;
	FN_COMPLETION_CALLBACK m_fnCompleted;

public:
	CCountdownTimer(FN_COMPLETION_CALLBACK fnComplete);
	void  Start(float countdownDurationSeconds);
	void  Advance(float dt);
	bool  IsActive();
	void  Abort();
	float ToSeconds();
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// The entry-point of the application
// An instance of CGamePlugin is automatically created when the library is loaded
// We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is first called.
class CGamePlugin 
	: public Cry::IEnginePlugin
	, public ISystemEventListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "DoxD", "{0900F201-49F3-4B3C-81D1-0C91CF3C8FDA}"_cry_guid)

	CGamePlugin();
	virtual ~CGamePlugin();
	
	// Cry::IEnginePlugin
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// Helper function to get the CGamePlugin instance
	// Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CGamePlugin* GetInstance()
	{
		return cryinterface_cast<CGamePlugin>(CGamePlugin::s_factory.CreateClassInstance().get());
	}

	virtual void RegisterConsoleVars();
	virtual void RegisterConsoleCommands();
	virtual void UnregisterConsoleCommands();

	inline SCVars* GetCVars() { return m_pCVars; }
	float GetFOV() const;

private:
	SCVars* m_pCVars;
	IConsole* m_pConsole;

private:
	void CreatePlayer();
};

extern CGamePlugin* g_pGame;