//#include <winnls.h>
#include "Tools.h"
#include "./md5/md5.h"
#include <sys/timeb.h>
#include <sys/types.h>
#include <ctime>
#include <cstdio>

#include <winsock2.h>
#include <iphlpapi.h>
#include <wininet.h>
#include <atlconv.h>
#include <atlbase.h> 
#include <atlstr.h>
#include <Ws2tcpip.h>
#include<cstring>
#include<iostream>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")

namespace HardWareNamespace {
	extern const wchar_t * API_SIGN_KEY = L"CGW1uQ1StIRE0MBVrQg7wcYQjgC2wRlLmAQ1ZofF8S2XKWMv0ZS587lnXZeW9bA1QFo9veTBswC2nAnrMoBUec30mxuWCF0A2h3dOwDcRLjfVrTri73ynYn3gSLBedDSn0S";
	int JugeUrlIsValid(std::wstring url) {

		return 0;
	}

	int JugeJsonIsValid(std::wstring jsonStr) {

		return 0;
	}

	long long GetTimeStamp() {
		struct timeb t;
		ftime(&t);
		return 1000 * t.time + t.millitm;
	}

	uint64_t GetRandomString(){
		int a = 45468453, b=23476354;
		//scanf("%d %d", &a, &b);
		srand((unsigned)time(NULL));
		uint64_t p = 1.0 * rand() / RAND_MAX * (a - b) + a;
		return  p ;
	}

	std::string GetSignString() {
		long long timeStamp = GetTimeStamp();
		std::wstring randomString = std::to_wstring(GetRandomString());
		std::wstring seedWStr = std::to_wstring(timeStamp) + API_SIGN_KEY + randomString;
		std::string seedStr = WsToS(seedWStr.c_str());
		std::string md5Str = MD5NameSpace::md5sum6(seedStr);
		return md5Str/*SToWs(md5Str)*/;
	}

	std::string WsToS(const wchar_t* szStr)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
		if (nLen == 0)
		{
			return NULL;
		}
		char* pResult = new char[nLen];
		WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
		std::string goal = pResult;
		delete []pResult;
		pResult = NULL;
		return goal;
	}

	std::wstring SToWs(const std::string &s)
	{
		size_t i;
		std::string curLocale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "chs");
		const char* _source = s.c_str();
		size_t _dsize = s.size() + 1;
		wchar_t* _dest = new wchar_t[_dsize];
		wmemset(_dest, 0x0, _dsize);
		mbstowcs_s(&i, _dest, _dsize, _source, _dsize);
		std::wstring result = _dest;
		delete[] _dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}
}
