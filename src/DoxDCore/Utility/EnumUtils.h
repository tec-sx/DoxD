#pragma once



//TBitfield GetBitfieldFromString(const char* inString, const char** inArray, int arraySize);
//bool      GetEnumValFromString(const char* inString, const char** inArray, int arraySize, int* outVal);

#if !defined(_RELEASE) || defined(PERFORMANCE_BUILD)
//string GetStringFromBitfield(TBitfield bitfield, const char** inArray, int arraySize);
#else
#define GetStringFromBitfield PLEASE_ONLY_CALL_AutoEnum_GetStringFromBitfield_IN_DEBUG_CODE
#endif