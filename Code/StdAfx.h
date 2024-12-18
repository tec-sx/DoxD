// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_EnginePlugin
#define GAME_API   DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>

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