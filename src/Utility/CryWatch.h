// Copyright 2012-2021 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
-------------------------------------------------------------------------
CryWatch.cpp

Description:
- basic onscreen watch
- in game.dll till it matures and can be moved into the engine

-------------------------------------------------------------------------
History:
-	[03/08/2009] : Created by James Bamford

*************************************************************************/

#pragma once

namespace DoxD
{
#if !defined(_RELEASE)
#define CRY_WATCH_ENABLED			 (1)
#else
#define CRY_WATCH_ENABLED			 (0)
#endif

#define CryWatch(...) CryWatchFunc(string().Format(__VA_ARGS__))
#define CryWatchLog(...) CryWatchLogFunc(string().Format(__VA_ARGS__))

#if CRY_WATCH_ENABLED

	int CryWatchFunc(const char* message);
	int CryWatchLogFunc(const char* message);
	void CryWatch3DAdd(const char* text, const Vec3& posIn, float lifetime = 2.f, const Vec3* velocity = nullptr, float gravity = 3.f);
	void CryWatch3DReset();
	void CryWatch3DTick(float dt);

#else

#define CryWatchFunc(message)          (0)
#define CryWatchLogFunc(message)          (0)
#define CryWatch3DAdd(...)             ((void)0)
#define CryWatch3DReset()              ((void)0)
#define CryWatch3DTick(dt)             ((void)0)

#endif // CRY_WATCH_ENABLED
}