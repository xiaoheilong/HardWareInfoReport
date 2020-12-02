#include "NetworkDeal.h"
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
using namespace std;
namespace Networkspace {
#define NOIPROUTETABLE 0
#define SUCCESS 1
#define  IPROUTETABLEZERO -1
	///////////////////////////获取当前正在使用的网卡的ip
	class LocalIpInfo
	{
	public:
		LocalIpInfo() {}
	public:
		inline int GetCurrentUsingIp(std::string& strLoalhostIp)
		{
			PMIB_IPFORWARDTABLE pIpRouteTable = GetIpForwardTable(TRUE);
			if (pIpRouteTable != NULL)
			{
				DWORD i, dwCurrIndex;
				struct in_addr inadDest;
				struct in_addr inadMask;
				struct in_addr inadGateway;
				char szDestIp[128] = { 0 };
				char szMaskIp[128] = { 0 };
				char szGatewayIp[128] = { 0 };
				DWORD IfIndex = 0;
				DWORD ForwardMetric1 = 0;
				if (pIpRouteTable->dwNumEntries > 0)
				{
					int i = 0;
					for (i = 0; i < pIpRouteTable->dwNumEntries; i++)
					{
						dwCurrIndex = pIpRouteTable->table[i].dwForwardIfIndex;
						// 目的地址  
						inadDest.s_addr = pIpRouteTable->table[i].dwForwardDest;
						strcpy_s(szDestIp, sizeof(szDestIp), inet_ntoa(inadDest));
						// 子网掩码  
						inadMask.s_addr = pIpRouteTable->table[i].dwForwardMask;
						strcpy_s(szMaskIp, sizeof(szDestIp), inet_ntoa(inadMask));
						// 网关地址  
						inadGateway.s_addr = pIpRouteTable->table[i].dwForwardNextHop;
						strcpy_s(szGatewayIp, sizeof(szDestIp), inet_ntoa(inadGateway));
						if ((strcmp(szDestIp, "0.0.0.0") == 0) && (strcmp(szMaskIp, "0.0.0.0") == 0))
						{

							if (i == 0)
							{
								ForwardMetric1 = pIpRouteTable->table[i].dwForwardMetric1;
								IfIndex = pIpRouteTable->table[i].dwForwardIfIndex;
								struct in_addr inadDest;
								inadDest.s_addr = pIpRouteTable->table[i].dwForwardDest;
							}

							else if (ForwardMetric1 > pIpRouteTable->table[i].dwForwardMetric1)
							{
								ForwardMetric1 = pIpRouteTable->table[i].dwForwardMetric1;
								IfIndex = pIpRouteTable->table[i].dwForwardIfIndex;
								struct in_addr inadDest;
								inadDest.s_addr = pIpRouteTable->table[i].dwForwardDest;
							}
						}
					}
				}
				else
				{
					FreeIpForwardTable(pIpRouteTable);
					return IPROUTETABLEZERO;
				}
				FreeIpForwardTable(pIpRouteTable);

				////////////
				if (IfIndex > 0)
				{
					DWORD ipdwSize = 0;
					DWORD ipdwRetval;
					PBYTE m_pBuffer = new BYTE[MAX_PATH];
					ULONG m_ulSize = MAX_PATH;
					MIB_IPADDRTABLE *pIPAddrTable;
					DWORD m_dwResult;
					PMIB_IPADDRTABLE pAddrTable;
					PMIB_IPADDRROW pAddrRow;
					in_addr ia;
					GetIpAddrTable((PMIB_IPADDRTABLE)m_pBuffer, &m_ulSize, TRUE);
					delete[] m_pBuffer;
					m_pBuffer = new BYTE[m_ulSize];
					if (NULL != m_pBuffer)
					{
						m_dwResult = GetIpAddrTable((PMIB_IPADDRTABLE)m_pBuffer, &m_ulSize, TRUE);
						if (m_dwResult == NO_ERROR)
						{
							pAddrTable = (PMIB_IPADDRTABLE)m_pBuffer;

							for (int x = 0; x < pAddrTable->dwNumEntries; x++)
							{
								pAddrRow = (PMIB_IPADDRROW) &(pAddrTable->table[x]);

								ia.S_un.S_addr = pAddrRow->dwAddr;
								char IPMsg[100] = { 0 };
								if (IfIndex == pAddrRow->dwIndex)
								{
									LPCSTR psz = inet_ntoa(ia);
									if (psz)
									{
										strLoalhostIp = psz;
									}
									delete[] m_pBuffer;
									return SUCCESS;
								}
							}
						}
						else
						{

						}
						delete[] m_pBuffer;
					}
				}
			}
			else
			{
				FreeIpForwardTable(pIpRouteTable);
				return NOIPROUTETABLE;
			}
			return NOIPROUTETABLE;
		}
	private:

		inline void FreeIpForwardTable(PMIB_IPFORWARDTABLE pIpRouteTab)
		{
			if (pIpRouteTab != NULL)
			{
				::GlobalFree(pIpRouteTab);
				pIpRouteTab == NULL;
			}
		}
		inline PMIB_IPFORWARDTABLE GetIpForwardTable(BOOL bOrder)
		{
			PMIB_IPFORWARDTABLE pIpRouteTab = NULL;
			DWORD dwActualSize = 0;

			// 查询所需缓冲区的大小  
			if (::GetIpForwardTable(pIpRouteTab, &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
			{
				// 为MIB_IPFORWARDTABLE结构申请内存  
				pIpRouteTab = (PMIB_IPFORWARDTABLE)::GlobalAlloc(GPTR, dwActualSize);
				// 获取路由表  
				if (::GetIpForwardTable(pIpRouteTab, &dwActualSize, bOrder) == NO_ERROR)
					return pIpRouteTab;
				::GlobalFree(pIpRouteTab);
			}
			return NULL;
		}
	};
	//////
	char *getUniCastIP(PIP_ADAPTER_UNICAST_ADDRESS pUnicast) {
		SOCKET_ADDRESS actualAddress = pUnicast->Address;
		LPSOCKADDR sock = actualAddress.lpSockaddr;
		sockaddr_in *act = (sockaddr_in*)sock;
		char *uniCastAddress = inet_ntoa(act->sin_addr);
		return uniCastAddress;
	}


	std::wstring s2ws(const std::string &s)
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




	int getLocalLanIp(wchar_t * lanIp, unsigned int len) {
		if (!lanIp) {
			return -1;
		}
		LocalIpInfo local_Address_info;
		std::string strLocalIP;
		local_Address_info.GetCurrentUsingIp(strLocalIP);
		std::wstring temp = s2ws(strLocalIP);
		if (len < temp.size()) {
			return -1;
		}
		wmemcpy(lanIp, temp.c_str(), temp.size());
		//std::cout << strLocalIP.c_str() << std::endl;
		return 0;
	}


	int getIpAddress(wchar_t *internetIp, unsigned int len) {
		if (!internetIp) {
			return -1;
		}
		std::wstring result = L"";
		//#ifndef _DEBUG
		HINTERNET hInternet, hFile;
		DWORD rSize;
		char *buffer = new char[250];

		hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

		if ((hFile = InternetOpenUrl(hInternet, L"https://api.ipify.org", NULL, 0, INTERNET_FLAG_RELOAD, 0)) != NULL) {
			InternetReadFile(hFile, buffer, 250, &rSize);
			buffer[rSize] = '\0';
			result = s2ws(buffer);
			InternetCloseHandle(hFile);
		}
		else {
			result = L"0.0.0.0";
		}
		InternetCloseHandle(hInternet);
		if (len < result.size()) {
			return -1;
		}
		wmemcpy(internetIp, result.c_str(), result.size());
		return 0;

	}

	bool getAdapterState(DWORD index)
	{
		MIB_IFROW Info;    // 存放获取到的Adapter参数
		memset(&Info, 0, sizeof(MIB_IFROW));
		Info.dwIndex = index; // dwIndex是需要获取的Adapter的索引，可以通过GetAdaptersInfo和其他相关函数获取
		if (GetIfEntry(&Info) != NOERROR)
		{
			printf("ErrorCode = %d\n", GetLastError());
			return false;
		}
		if (Info.dwOperStatus == IF_OPER_STATUS_NON_OPERATIONAL
			|| Info.dwOperStatus == IF_OPER_STATUS_UNREACHABLE
			|| Info.dwOperStatus == IF_OPER_STATUS_DISCONNECTED

			|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTING)
			return false;
		else if (Info.dwOperStatus == IF_OPER_STATUS_OPERATIONAL

			|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTED)
			return true;
	}

	int getLocalMacAddress(wchar_t * macAdress , unsigned int len) {
		//////////
		wchar_t localIp[50];
		memset(localIp, 0, sizeof(localIp));
		int ret = -1;
		ret = getLocalLanIp(localIp , 50);
		if (0 != ret || 0 == wcslen(localIp)) {
			return ret;
		}
		PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
		PIP_ADAPTER_ADDRESSES pAddresses = NULL;
		DWORD dwRetValAddr = 0;
		UINT i;
		ULONG outBufLen = 0x3A98;

		pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
		if (pAddresses == NULL) {
			printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
			return -1;
		}
		dwRetValAddr = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
		USES_CONVERSION;
		if (dwRetValAddr == NO_ERROR) {
			pCurrAddresses = pAddresses;
			while (pCurrAddresses) {
				if (pCurrAddresses->IfType != 0x18 && pCurrAddresses->IfIndex != 0x83 \
					&& pCurrAddresses->OperStatus == IfOperStatusUp) {
					PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
					wstring ipAddr = s2ws(getUniCastIP(pUnicast));
					if (getAdapterState(pCurrAddresses->IfIndex) && 0 == ipAddr.compare(localIp)) {
						wchar_t macAddress[50];
						memset(macAddress, 0, sizeof(macAddress));
						wsprintf(macAddress, _T("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X"), pCurrAddresses->PhysicalAddress[0], pCurrAddresses->PhysicalAddress[1],
							pCurrAddresses->PhysicalAddress[2], pCurrAddresses->PhysicalAddress[3], pCurrAddresses->PhysicalAddress[4], pCurrAddresses->PhysicalAddress[5]);
						if (len < wcslen(macAddress)) {
							return - 1;
						}
						wmemcpy(macAdress, macAddress, wcslen(macAddress) + 1);
						return 0;
					}
				}
				pCurrAddresses = pCurrAddresses->Next;
			}
		}
		return -1;
	}
}
