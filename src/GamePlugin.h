// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryPhysics/RayCastQueue.h>
#include <CryPhysics/IntersectionTestQueue.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>
#include <CryExtension/ClassWeaver.h>

namespace DoxD
{
	struct SCVars;

	// The entry-point of the application
	// An instance of CGamePlugin is automatically created when the library is loaded
	// We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is first called.
	class CGamePlugin
		: public Cry::IEnginePlugin
		, public ISystemEventListener
		, public IGameFrameworkListener
		, public INetworkedClientListener
	{
	public:
		CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
			CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "DoxD", "{0900F201-49F3-4B3C-81D1-0C91CF3C8FDA}"_cry_guid)

			CGamePlugin();
		virtual ~CGamePlugin();

		// Cry::IEnginePlugin
		virtual const char* GetName() const override { return "DoxDCore"; }
		virtual const char* GetCategory() const override { return "Game"; }
		virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
		// ~Cry::IEnginePlugin

		// ISystemEventListener
		virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
		// ~ISystemEventListener

		// IGameFrameworkListener
		virtual void OnPostUpdate(float fDeltaTime) override {};
		virtual void OnSaveGame(ISaveGame* pSaveGame) override {};
		virtual void OnLoadGame(ILoadGame* pLoadGame) override {};
		virtual void OnLevelEnd(const char* nextLevel) override {};
		virtual void OnActionEvent(const SActionEvent& event) override {};
		// ~IGameFrameworkListener

		// INetworkedClientListener
		virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}
		virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
		virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override { return true; };
		virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override {};
		virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
		// ~INetworkedClientListener
		
		// Helper function to get the CGamePlugin instance
		// Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
		static CGamePlugin* GetInstance()
		{
			return cryinterface_cast<CGamePlugin>(CGamePlugin::s_factory.CreateClassInstance().get());
		}

		inline SCVars* GetCVars() { return m_pCVars; }

	private:
		SCVars* m_pCVars;
		IConsole* m_pConsole;
	};
} // namespace DoxD