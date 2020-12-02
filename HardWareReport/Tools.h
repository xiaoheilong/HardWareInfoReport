#pragma once
#include<string>
namespace HardWareNamespace {
	int JugeUrlIsValid(std::wstring url);
	int JugeJsonIsValid(std::wstring jsonStr);
	long long GetTimeStamp();
	uint64_t GetRandomString();
	std::string GetSignString();
	std::wstring SToWs(const std::string &s);
	std::string  WsToS(const wchar_t* szStr);
}
