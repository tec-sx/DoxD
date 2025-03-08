// Copyright 2012 - 2021 Crytek GmbH / Crytek Group.All rights reserved.

/*************************************************************************
-------------------------------------------------------------------------
StringUtils.h

TODO: Move contents of this file into CryEngine/CryCommon/StringUtils.h
and contents of StringUtils.cpp into CryEngine/CryCommon/StringUtils.cpp
*************************************************************************/
#pragma once

#include <CryMath/Cry_Color.h>
#include <CryString/CryString.h>
#include <CryMath/Cry_Math.h>

namespace DoxD
{
	//--------------------------------------------------------------------------------
	// cry_copyStringUntilFindChar: Parameter order is the same as strncpy;
	// additional 'until' parameter defines which additional character should
	// stop the copying. Return value is number of bytes (including NULL)
	// written into 'destination', or 0 if 'until' character not found in
	// first 'bufferLength' bytes of 'source'.
	size_t cry_copyStringUntilFindChar(char* destination, const char* source, size_t bufferLength, char until);

#if !defined(_DEBUG)
#define cry_displayMemInHexAndAscii(...)			(void)(0)
#else

	class ITextOutputHandler
	{
	public:
		virtual void DoOutput(const char* text) = 0;
	};

	class CCryWatchOutputHandler : public ITextOutputHandler
	{
		virtual void DoOutput(const char* text);
	};

	class CCryLogOutputHandler : public ITextOutputHandler
	{
		virtual void DoOutput(const char* text);
	};

	class CCryLogAlwaysOutputHandler : public ITextOutputHandler
	{
		virtual void DoOutput(const char* text);
	};

	//--------------------------------------------------------------------------------
	// cry_displayMemInHexAndAscii outputs (using an ITextOutputHandler subclass) the
	// contents of the first 'size' bytes starting at memory location 'data'.
	void cry_displayMemInHexAndAscii(const char* startEachLineWith, const void* data, int size, ITextOutputHandler& output, const int bytesPerLine = 32);
#endif
	//
	//--------------------------------------------------------------------------------
	// Generates a string in the format X days X hrs X mins X secs, or if useShortForm is set 00:00:00.
	const char* GetTimeString(int secs, bool useShortForm = false, bool includeSeconds = true, bool useSingleLetters = false);
	const char* GetTimeString(float secs, bool useShortForm = false, bool includeSeconds = true, bool useSingleLetters = false);
	
	/**
	Takes a number of seconds as an input and translates that into the corresponding number of days, hours, minutes
	and seconds.
	
	\param    secs               The number of seconds to be converted.
	\param [in,out]    days       The days.
	\param [in,out]    hours      The hours.
	\param [in,out]    minutes    The minutes.
	\param [in,out]    seconds    The seconds.
	*/
	static void ExpandTimeSeconds(int secs, int& days, int& hours, int& minutes, int& seconds);

	inline Vec3 Vec3FromString(string sVector)
	{
		size_t yPos = sVector.find(",");
		size_t zPos = sVector.find(",", yPos + 1);

		Vec3 vector;

		vector.x = (float)atof(sVector.substr(0, yPos));
		vector.y = (float)atof(sVector.substr(yPos + 1, (zPos - yPos) - 1));
		vector.z = (float)atof(sVector.substr(zPos + 1));

		return vector;
	}

	inline string Vec3ToString(Vec3 vector)
	{
		string sVector;
		sVector.Format("%f,%f,%f", vector.x, vector.y, vector.z);

		return sVector;
	}

	inline Quat QuatFromString(string sQuat)
	{
		size_t xPos = sQuat.find(",");
		size_t yPos = sQuat.find(",", xPos + 1);
		size_t zPos = sQuat.find(",", yPos + 1);

		Quat quaternion;

		quaternion.w = (float)atof(sQuat.substr(0, xPos));;
		quaternion.v.x = (float)atof(sQuat.substr(xPos + 1, (yPos - xPos) - 1));
		quaternion.v.y = (float)atof(sQuat.substr(yPos + 1, (zPos - yPos) - 1));
		quaternion.v.z = (float)atof(sQuat.substr(zPos + 1));

		return quaternion;
	}

	inline string QuatToString(Quat quaternion)
	{
		string sQuaternion;
		sQuaternion.Format("%f,%f,%f,%f", quaternion.w, quaternion.v.x, quaternion.v.y, quaternion.v.z);

		return sQuaternion;
	}

	inline ColorF StringToColor(const char* sColor, bool adjustGamma)
	{
		ColorF color(1.f);
		string colorString = sColor;

		for (int i = 0; i < 4; ++i)
		{
			size_t pos = colorString.find_first_of(",");
			if (pos == string::npos)
				pos = colorString.size();

			string sToken = colorString.substr(0, pos);

			float fToken = (float)atof(sToken);

			// Convert to linear space
			if (adjustGamma)
				color[i] = powf(fToken / 255, 2.2f);
			else
				color[i] = fToken;

			if (pos == colorString.size())
				break;
			else
				colorString.erase(0, pos + 1);
		}

		return color;
	}

	inline Vec3 StringToVec3(const char* sVector)
	{
		Vec3 v(ZERO);
		string vecString = sVector;

		for (int i = 0; i < 3; ++i)
		{
			size_t pos = vecString.find_first_of(",");
			if (pos == string::npos)
				pos = vecString.size();

			string sToken = vecString.substr(0, pos);

			float fToken = (float)atof(sToken);

			v[i] = fToken;

			if (pos == vecString.size())
				break;
			else
				vecString.erase(0, pos + 1);
		}

		return v;
	}

	inline Quat StringToQuat(const char* sQuat)
	{
		Quat q(ZERO);
		string quatString = sQuat;

		for (int i = 0; i < 4; ++i)
		{
			size_t pos = quatString.find_first_of(",");
			if (pos == string::npos)
				pos = quatString.size();

			string sToken = quatString.substr(0, pos);

			float fToken = (float)atof(sToken);

			switch (i)
			{
			case 0:
				q.w = fToken;
				break;
			case 1:
				q.v.x = fToken;
				break;
			case 2:
				q.v.y = fToken;
				break;
			case 3:
				q.v.z = fToken;
				break;
			}

			if (pos == quatString.size())
				break;
			else
				quatString.erase(0, pos + 1);
		}

		return q;
	}

	inline uint64 StringToMs(string time)
	{
		size_t secondsPos = time.find(":");
		size_t msPos = time.find(":", secondsPos + 1);

		string sMinutes = time.substr(0, secondsPos);
		string sSeconds = time.substr(secondsPos + 1, (msPos - secondsPos) - 1);
		string sMS = time.substr(msPos + 1);

		uint64 ms = atoi(sMS.c_str()) + (atoi(sSeconds.c_str()) * 1000) + (atoi(sMinutes.c_str()) * 60000);

		return ms;
	}
}