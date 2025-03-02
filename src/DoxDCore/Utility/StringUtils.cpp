//#include "StdAfx.h"
//#include "StringUtils.h"
//#include "CryWatch.h"
//
//size_t cry_copyStringUntilFindChar(char* destination, const char* source, size_t bufferLength, char until)
//{
//	size_t reply = 0;
//
//	CRY_ASSERT(destination);
//	CRY_ASSERT(source);
//
//	if (bufferLength)
//	{
//		size_t i;
//		for (i = 0; source[i] && source[i] != until && (i + 1) < bufferLength; ++i)
//		{
//			destination[i] = source[i];
//		}
//		destination[i] = '\0';
//		reply = (source[i] == until) ? (i + 1) : 0;
//	}
//
//	return reply;
//}
//
////void cry_displayMemInHexAndAscii(const char* startEachLineWith, const void* data, int size, ITextOutputHandler& output, const int bytesPerLine)
////{
////}
//
//void CCryWatchOutputHandler::DoOutput(const char* text)
//{
//	CryWatch("%s", text);
//}
//
//void CCryLogOutputHandler::DoOutput(const char* text)
//{
//	CryLog("%s", text);
//}
//
//void CCryLogAlwaysOutputHandler::DoOutput(const char* text)
//{
//	CryLogAlways("%s", text);
//}
