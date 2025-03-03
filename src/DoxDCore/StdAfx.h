// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_EnginePlugin
#define GAME_API   DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include "Utility/CryWatch.h"

#include "GamePlugin.h"
#include "GameCVars.h"

extern struct SCVars* g_pGameCVars;

#ifdef USE_PCH
	#include <algorithm>
	#include <atomic>
	#include <functional>
	#include <limits>
	#include <list>
	#include <map>
	#include <memory>
	#include <queue>
	#include <set>
	#include <vector>
#endif // USE_PCH

// Definitions
typedef uint32 TBitfield;

namespace DoxD
{
#if !defined(_RELEASE)
#define INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
#endif // _RELEASE

	template<class T>
	static IEntityClass* RegisterEntityWithDefaultComponent(const char* name, const char* editorCategory = "", const char* editorIcon = "", bool bIconOnTop = false)
	{
		IEntityClassRegistry::SEntityClassDesc clsDesc;
		clsDesc.sName = name;

		clsDesc.editorClassInfo.sCategory = editorCategory;
		clsDesc.editorClassInfo.sIcon = editorIcon;
		clsDesc.editorClassInfo.bIconOnTop = bIconOnTop;

		struct CObjectCreator
		{
			static IEntityComponent* Create(IEntity* pEntity, SEntitySpawnParams& params, void* pUserData)
			{
				return pEntity->GetOrCreateComponentClass<T>();
			}
		};

		clsDesc.pUserProxyCreateFunc = &CObjectCreator::Create;
		clsDesc.flags |= ECLF_INVISIBLE;

		return gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(clsDesc);
	}
}