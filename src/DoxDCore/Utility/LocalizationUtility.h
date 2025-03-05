#pragma once
namespace DoxD::LocalizationUtility
{
	const char* LocalizeString(const char* text, const char* arg1 = 0, const char* arg2 = 0, const char* arg3 = 0, const char* arg4 = 0);
	void LocalizeString(string& out, const char* text, const char* arg1, const char* arg2, const char* arg3, const char* arg4);
	void LocalizeStringn(char* dest, size_t bufferSizeInBytes, const char* text, const char* arg1 = 0, const char* arg2 = 0, const char* arg3 = 0, const char* arg4 = 0);

	const char* LocalizeNumber(const int number);
	void LocalizeNumber(string& out, const int number);
	void LocalizeNumbern(const char* dest, size_t bufferSizeInBytes, const int number);
	const char* LocalizeNumber(const float number, int decimals);
	void LocalizeNumber(string& out, const float number, int decimals);
	void LocalizeNumbern(const char* dest, size_t bufferSizeInBytes, const float number, int decimals);
};

