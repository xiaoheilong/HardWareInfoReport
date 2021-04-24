#include "SystemInfoDll.h"
#include "./WMIWBEMINFO.h"
#include "./nvapi/nvapi.h"
#include "./NetworkDeal.h"
#include <tlhelp32.h>
#include <iostream>
//#include "./network/common/network.h"
#include<Python.h>
extern "C"
{
#include "Python.h"
}
//#include <Windows.h>
//#include <tchar.h>
//#include <intrin.h>
//#include <comdef.h>
//#include <Wbemidl.h>
#include <sstream>
#include <mutex>
//#include <atlconv.h>
#define IP_TYPES_INCLUDED
//#include<IPTypes.h>
//#include <powerbase.h>

//#include <winsock2.h>
//#include <iphlpapi.h>
//#include <wininet.h>
//#include <atlconv.h>
//#include <atlbase.h> 
//#include <atlstr.h>
////#include <Ws2tcpip.h>
#include <vector>
using namespace  std;
namespace SystemWMISpace {
	//////////////////
	/////////////////struct
	//typedef  IP_ADAPTER_ADDRESSES_LH  IP_ADAPTER_ADDRESSES;
	//typedef  IP_ADAPTER_ADDRESSES_LH * PIP_ADAPTER_ADDRESSES;
	//////////////////////
	//std::wstring s2ws(const std::string &s)
	//{
	//	size_t i;
	//	std::string curLocale = setlocale(LC_ALL, NULL);
	//	setlocale(LC_ALL, "chs");
	//	const char* _source = s.c_str();
	//	size_t _dsize = s.size() + 1;
	//	wchar_t* _dest = new wchar_t[_dsize];
	//	wmemset(_dest, 0x0, _dsize);
	//	mbstowcs_s(&i, _dest, _dsize, _source, _dsize);
	//	std::wstring result = _dest;
	//	delete[] _dest;
	//	setlocale(LC_ALL, curLocale.c_str());
	//	return result;
	//}
	const  wchar_t * g_pythonHomePath = L"./Python39";
	//const  wchar_t * g_pythonPath = L"./Python39,./Python39/Scripts,./Python39/Lib,./Python39/DLLs,./Python39/Lib/site-packages,./Python39/Lib/site-packages/win32,./Python39/Lib/site-packages/win32/lib,./Python39/Lib/site-packages/Pythonwin";

#define GB2312_ACP 936
	std::string UnEscape(const char* strSource)
	{
		std::string strResult;
		int nDestStep = 0;
		int nLength = strlen(strSource);
		if (!nLength || nLength < 6) return strResult;
		char* pResult = new char[nLength + 1];
		wchar_t* pWbuufer = nullptr;
		if (!pResult)
		{
			pResult = NULL;
			return strResult;
		}
		ZeroMemory(pResult, nLength + 1);
		for (int nPos = 0; nPos < nLength; nPos++)
		{
			if (strSource[nPos] == '\\' && strSource[nPos + 1] == 'u')
			{
				char szTemp[5];
				char szSource[5];
				ZeroMemory(szTemp, 5);
				ZeroMemory(szSource, 5);
				CopyMemory(szSource, (char*)strSource + nPos + 2, 4);
				sscanf_s(szSource, "%04X", szTemp);
				CopyMemory(pResult + nDestStep, szTemp, 4);
				nDestStep += 2;
			}
			else {
				/*	CopyMemory(pResult + nDestStep, &strSource[nPos], 1);
					nDestStep +=1;*/
			}
		}
		nDestStep += 2;
		pWbuufer = new wchar_t[nDestStep];
		if (!pWbuufer)
		{
			delete[] pWbuufer;
			pWbuufer = nullptr;
			return strResult;
		}
		ZeroMemory(pWbuufer, nDestStep);
		CopyMemory(pWbuufer, pResult, nDestStep);
		delete[] pResult;
		pResult = nullptr;
		CHAR* MultPtr = nullptr;
		int MultLen = -1;
		//GB2312_ACP = 936
		MultLen = ::WideCharToMultiByte(GB2312_ACP, WC_COMPOSITECHECK, pWbuufer, -1, NULL, NULL, NULL, NULL);
		MultPtr = new CHAR[MultLen + 1];
		if (MultPtr)
		{
			ZeroMemory(MultPtr, MultLen + 1);
			::WideCharToMultiByte(GB2312_ACP, WC_COMPOSITECHECK, pWbuufer, -1, MultPtr, MultLen, NULL, NULL);
			strResult = MultPtr;
			delete[] MultPtr;
			MultPtr = nullptr;
		}
		delete[] pWbuufer;
		pWbuufer = nullptr;
		return strResult;
	}

	std::string ws2s(const wchar_t* szStr)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
		if (nLen == 0)
		{
			return NULL;
		}
		char* pResult = new char[nLen];
		WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
		std::string goal = pResult;
		delete[]pResult;
		pResult = NULL;
		return goal;
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


	int SetPythonEnv(const  wchar_t * pythonHomePath) {
		if (!pythonHomePath) {
			return -1;
		}
		char szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
		(strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串
		std::wstring folder = s2ws(szFilePath);
		std::wstring path = folder + pythonHomePath;
		Py_SetPythonHome(path.c_str());
		//Py_SetPath(pythonHomePath);
		return 0;
	}

	bstr_t buildQueryString(const wchar_t* wmiClass, vector<LPCWSTR> attrs) {

		WCHAR queryString[256] = { 0 };
		wcscpy(queryString, L"SELECT ");
		auto it = attrs.begin();
		for (auto it = attrs.begin(); it != attrs.end(); it++) {
			wcscat(queryString, *it);
			if (!((attrs.end() - it == 1))) {
				wcscat(queryString, L", ");
			}
		}
		wcscat(queryString, L" FROM ");
		wcscat(queryString, wmiClass);
		return bstr_t(queryString);
	}

	bstr_t buildQueryString(const wchar_t* wmiClass,
		vector<LPCWSTR> attrs,
		const wchar_t* whereClause) {
		bstr_t mainQuery = buildQueryString(wmiClass, attrs);
		bstr_t mainQueryWithWhereClause = mainQuery;
		if (whereClause != NULL) {
			mainQueryWithWhereClause += SysAllocString(L" WHERE ");
			mainQueryWithWhereClause += SysAllocString(whereClause);
		}
		return mainQueryWithWhereClause;
	}


	IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator * pLoc,
		IWbemServices * pSvc, BSTR stringQuery) {
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			stringQuery,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);
		//test this behavior
		if (FAILED(hres)) {
			/*GenericMessageOK()
				.withMessage(L"Fatal error: Query to operating system failed")
				->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
				->display();*/

			//pSvc->Release();
			//pLoc->Release();
			//CoUninitialize();
			return NULL;
		}
		else {
			return pEnumerator;
		}
	}

	float getRamTotal(WMIWBEMINFO * wmiWbemInfo) {
		if (!wmiWbemInfo) {
			return 0.0;
		}
		vector<LPCWSTR> queryAttrs;
		queryAttrs.push_back(L"Capacity");
		IEnumWbemClassObject* pEnumerator = executeWQLQuery(
			wmiWbemInfo->getHres(),
			wmiWbemInfo->getWbemLocator(),
			wmiWbemInfo->getWbemServices(), buildQueryString(L"Win32_PhysicalMemory", queryAttrs));

		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;

		float accumulatedRAM = 0;
		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}
			VARIANT vtProp;
			hr = pclsObj->Get(L"Capacity", 0, &vtProp, 0, 0);

			float cap;
			float capacity;

			wstring temp;
			//TCHAR tempChar[100];
			temp = vtProp.bstrVal;
			//_tcscpy(tempChar, temp.c_str());
			//swscanf(tempChar, L"%lf", &cap);
			cap = stof(temp);
			cap /= 1024 * 1024;
			accumulatedRAM += cap;
			VariantClear(&vtProp);
		}
		//TCHAR capacityStrBuff[100];
		//_stprintf(capacityStrBuff, _T("%.2lf"), accumulatedRAM);
		//return wstring(capacityStrBuff);
		return accumulatedRAM;
	}


	SystemInfoDll::SystemInfoDll():m_wmiWbemInfo(NULL) , m_mutex(new std::mutex()), m_userName(NULL), m_deviceId(L""), m_pModule(NULL)
	{
		m_getCpuUsagePercentage = NULL;
		m_getRamPercentage = NULL;
		m_getDiskPercentage = NULL;
		m_getInternetIp = NULL;
		m_getRegion = NULL;
		//getRegion = NULL;
		m_getNetworkOperator = NULL;
		m_getNetworkUploadSpeed = NULL;
		m_getNetworkDownloadSpeed = NULL;
		m_getCpuGuid = NULL;
	}


	SystemInfoDll::~SystemInfoDll()
	{
		unInit();
		if (m_mutex) {
			std::mutex * mutex = (std::mutex *)(m_mutex);
			if (mutex) {
				delete mutex;
				mutex = NULL;
			}
			m_mutex = NULL;
		}
		if (m_userName) {
			delete[] m_userName;
			m_userName = NULL;
		}
	}

	int SystemInfoDll:: init() {
		int ret = 0;
		if (!m_mutex) {
			return -1;
		}
		std::mutex * mutex = (std::mutex *)(m_mutex);
		std::unique_lock<std::mutex> lock(*mutex);
		m_wmiWbemInfo = WMIWBEMINFO::getWMIWBEMINFOInstance();
		////////python env
		SetPythonEnv(g_pythonHomePath);
		if (!Py_IsInitialized())
		{
			Py_Initialize();
		}
		if (!Py_IsInitialized())
		{
			return -1;
		}
		m_pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		if (m_pModule == NULL)
		{
			return -1;
		}
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('./')");
		PyObject * pModule = NULL;
		pModule = (PyObject *)(m_pModule);
		m_getCpuUsagePercentage =PyObject_GetAttrString(pModule, "getCpuPercent");
		m_getRamPercentage = PyObject_GetAttrString(pModule, "getMemoryPercent");//
		m_getDiskPercentage = PyObject_GetAttrString(pModule, "getDiskInfo");//;
		m_getRegion = PyObject_GetAttrString(pModule, "getRegion");
		m_getNetworkOperator = PyObject_GetAttrString(pModule, "getInternetOperator");
		m_getNetworkUploadSpeed = PyObject_GetAttrString(pModule, "getNetworkDownLoadFlow");
		m_getNetworkDownloadSpeed = PyObject_GetAttrString(pModule, "getNetworkUpLoadFlow");
		m_getCpuGuid = PyObject_GetAttrString(pModule, "getCpuProcessorId");
		//////////
		return  ret;
	}

	void SystemInfoDll::unInit() {
		if (Py_IsInitialized())
		{
			/////////////////release the function of python scrip
			PyObject * pModule = (PyObject *)(m_pModule);
			////////////
			if (m_getCpuUsagePercentage) {
				Py_DECREF(m_getCpuUsagePercentage);
				m_getCpuUsagePercentage = NULL;
			}
			if (m_getRamPercentage) {
				Py_DECREF(m_getRamPercentage);
				m_getRamPercentage = NULL;
			}
			if (m_getDiskPercentage) {
				Py_DECREF(m_getDiskPercentage);
				m_getDiskPercentage = NULL;
			}
			//m_getInternetIp = PyObject_GetAttrString(pModule, "getDiskInfo");
			if (m_getRegion) {
				Py_DECREF(m_getRegion);
				m_getRegion = NULL;
			}
			if (m_getNetworkOperator) {
				Py_DECREF(m_getNetworkOperator);
				m_getNetworkOperator = NULL;
			}

			if (m_getNetworkUploadSpeed) {
				Py_DECREF(m_getNetworkUploadSpeed);
				m_getNetworkUploadSpeed = NULL;
			}

			if (m_getNetworkDownloadSpeed) {
				Py_DECREF(m_getNetworkDownloadSpeed);
				m_getNetworkDownloadSpeed = NULL;
			}

			if (m_getCpuGuid) {
				Py_DECREF(m_getCpuGuid);
				m_getCpuGuid = NULL;
			}
			////////////
			if (pModule) {
				Py_DECREF(pModule);
				m_pModule = NULL;
			}
			try {
				Py_Finalize(); // 与初始化对应
			}
			catch (...) {
				std::cout << "Py_Finalize has a exception!" << std::endl;
			}
		}
		if (!m_mutex) {
			return ;
		}
		std::mutex * mutex = (std::mutex *)(m_mutex);
		std::unique_lock<std::mutex> lock(*mutex);
		if (m_wmiWbemInfo) {
			WMIWBEMINFO *wmiWbemInfo = (WMIWBEMINFO *)(m_wmiWbemInfo);
			delete wmiWbemInfo;
			wmiWbemInfo = NULL;
			m_wmiWbemInfo = NULL;
		}
	}

	unsigned int numberOfLogicalProcessors(WMIWBEMINFO * wmiWbemInfo) {
		if (!wmiWbemInfo) {
			return 0;
		}
		//int totalProcessorTime = getCpuUsagePercentageEx();
		vector<LPCWSTR> queryAttrs;
		queryAttrs.push_back(L"NumberOfLogicalProcessors");
		IEnumWbemClassObject* pEnumerator = executeWQLQuery
		(wmiWbemInfo->getHres(),
			wmiWbemInfo->getWbemLocator(),
			wmiWbemInfo->getWbemServices(),
			//buildQueryString(L"Win32_PerfFormattedData_PerfOS_Processor", queryAttrs, L"Name='_Total'"));
			//buildQueryString(L"Win32_PerfFormattedData_PerfOS_Processor", queryAttrs, L"Name!='_Total'"));
			buildQueryString(L"Win32_ComputerSystem", queryAttrs));
		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;
		//if -1, then WMI for some reason didn't get the data from the tables
		int cpuLogicCounts = 0;
		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				int errorCode = GetLastError();
				break;
			}

			VARIANT vtProp;

			hr = pclsObj->Get(queryAttrs.at(0), 0, &vtProp, 0, 0);
			if (WBEM_S_NO_ERROR != hr) {
				if (pclsObj) {
					VariantClear(&vtProp);
					pclsObj->Release(); pclsObj = NULL;
				}
				throw std::exception("Failed to get info from logic cores table");
				break;
			}
			cpuLogicCounts = vtProp.cyVal.Lo;
			VariantClear(&vtProp);
			pclsObj->Release(); pclsObj = NULL;
		}
		return cpuLogicCounts;
	}

	float SystemInfoDll::getCpuUsagePercentage() {
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		//if (pModule == NULL)
		//{
		//	return 0.0;
		//}
		/*pModule = (PyObject *)(m_pModule);
		if (pModule == NULL)
		{
			return 0.0;
		}*/
		getIpInfo = (PyObject *)m_getCpuUsagePercentage;// (PyObject *)m_getCpuUsagePercentage;// PyObject_GetAttrString(pModule, "getCpuPercent");//这里是要调用的函数名
		if (!getIpInfo) {
			return 0.0;
		}
		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		if (!pRet) {
			return 0.0;
		}
		float result = 0.0;
		PyArg_Parse(pRet, "f", &result);//转换返回类型;
		Py_DECREF(pRet);
		//Py_DECREF(getIpInfo);
		//Py_DECREF(pModule);
		return result;
	}

	float SystemInfoDll::getRamPercentage() {
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		/*pModule = (PyObject *)(m_pModule);
		if (pModule == NULL)
		{
			return 0.0;
		}*/
		getIpInfo = (PyObject *)(m_getRamPercentage);// PyObject_GetAttrString(pModule, "getMemoryPercent");//这里是要调用的函数名
		if (!getIpInfo) {
			return 0.0;
		}
		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		if (!pRet) {
			//Py_DECREF(getIpInfo);
			return 0.0;
		}
		float result = 0.0;
		PyArg_Parse(pRet, "f", &result);//转换返回类型;
		Py_DECREF(pRet);
		//Py_DECREF(getIpInfo);
		return result;
	}

	float SystemInfoDll::getNvidaGPUPercentage() {
		NvAPI_Status ret = NVAPI_OK;
		int i = 0;

		NvDisplayHandle hDisplay_a[NVAPI_MAX_PHYSICAL_GPUS * 2] = { 0 };

		ret = NvAPI_Initialize();

		if (!ret == NVAPI_OK) {
			NvAPI_ShortString str;
			NvAPI_GetErrorMessage(ret, str);
			printf("NVAPI NvAPI_Initialize: %s\n", str);
			return 0.0;
		}

		NvAPI_ShortString ver;

		NvAPI_GetInterfaceVersionString(ver);
		//printf("NVAPI Version: %s\n", ver);

		NvU32 cnt;

		NvPhysicalGpuHandle phys;

		ret = NvAPI_EnumPhysicalGPUs(&phys, &cnt);

		if (!ret == NVAPI_OK) {
			NvAPI_ShortString string;
			NvAPI_GetErrorMessage(ret, string);
			printf("NVAPI NvAPI_EnumPhysicalGPUs: %s\n", string);
			return  0.0;
		}

		NvAPI_ShortString name;

		NV_GPU_THERMAL_SETTINGS thermal;

		ret = NvAPI_GPU_GetFullName(phys, name);
		if (!ret == NVAPI_OK) {
			NvAPI_ShortString string;
			NvAPI_GetErrorMessage(ret, string);
			printf("NVAPI NvAPI_GPU_GetFullName: %s\n", string);
			return  0.0;
		}

		//printf("Name: %s\n", name);
		thermal.version = NV_GPU_THERMAL_SETTINGS_VER;
		ret = NvAPI_GPU_GetThermalSettings(phys, 0, &thermal);

		if (!ret == NVAPI_OK) {
			NvAPI_ShortString string;
			NvAPI_GetErrorMessage(ret, string);
			printf("NVAPI NvAPI_GPU_GetThermalSettings: %s\n", string);
			return 0.0;
		}

		//printf("Temp: %u C\n", static_cast<unsigned>(thermal.sensor[0].currentTemp));

		NV_GPU_DYNAMIC_PSTATES_INFO_EX infoEx;
		infoEx.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
		ret = NvAPI_GPU_GetDynamicPstatesInfoEx(phys, &infoEx);

		if (!ret == NVAPI_OK) {
			NvAPI_ShortString string;
			NvAPI_GetErrorMessage(ret, string);
			printf("NVAPI NvAPI_GPU_GetDynamicPstatesInfoEx: %s\n", string);
			return 0.0;
		}
		float goalPercentage = 0;
		int percentageCount = 0;
		for (int i = 0; i < NVAPI_MAX_GPU_UTILIZATIONS; ++i) {
			if (infoEx.utilization[i].bIsPresent) {
				++percentageCount;
				goalPercentage += infoEx.utilization[i].percentage;
			}
		}
		goalPercentage = goalPercentage / percentageCount;
		return goalPercentage;
	}

	float getFreeSpaceOfDiskEnum(WMIWBEMINFO * wmiWbemInfo, std::wstring partitionId) {
		//if (!wmiwbeminfo) {
		//	return 0.0;
		//}
		//std::wstring sqlstr = l"associators of {win32_diskpartition.deviceid='" + partitionid + l"'"  \
		//	+ l"} where assocclass = win32_logicaldisktopartition";
		//ienumwbemclassobject* penumerator = executewqlquery(wmiwbeminfo->gethres(),
		//	wmiwbeminfo->getwbemlocator(),
		//	wmiwbeminfo->getwbemservices(),
		//	bstr_t(sqlstr.c_str()));
		//iwbemclassobject* pclsobj = null;
		//ulong ureturn = 0;
		//float freespacesize = 0;
		//while (penumerator) {
		//	hresult hr = penumerator->next(wbem_infinite, 1,
		//		&pclsobj, &ureturn);

		//	if (0 == ureturn) {
		//		break;
		//	}
		//	////////////////freespace
		//	variant vtprop;
		//	wstring freespacestr;
		//	float tempvalue = 0.0;
		//	hr = pclsobj->get(l"freespace", 0, &vtprop, 0, 0);
		//	if (vtprop.bstrval) {
		//		freespacestr = vtprop.bstrval;
		//		tempvalue = (float)stof(freespacestr);
		//		//tempvalue /= (1024.0 * 1024.0 * 1024.0);
		//		freespacesize += tempvalue;
		//	}
		//	variantclear(&vtprop);

		//	pclsobj->release();
		//}
		//if (penumerator) {
		//	penumerator->release();
		//}
		//return freespacesize;
		

		return 0.0;
	}


	float getFreeSpaceOfDiskByDeviceID(WMIWBEMINFO * wmiWbemInfo, std::wstring deviceId) {
		if (!wmiWbemInfo) {
			return 0.0;
		}
		std::wstring sqlStr = L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='" + deviceId + L"'"  \
			+ L"} WHERE AssocClass = Win32_DiskDriveToDiskPartition";
		IEnumWbemClassObject* pEnumerator = executeWQLQuery(wmiWbemInfo->getHres(),
			wmiWbemInfo->getWbemLocator(),
			wmiWbemInfo->getWbemServices(),
			bstr_t(sqlStr.c_str()));
		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;
		float freeSpace = 0;
		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}

			VARIANT vtProp;
			wstring deviceId;
			wstring capacityStr;
			wstring  name;
			///deviceID
			hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
			if (vtProp.bstrVal) {
				deviceId = vtProp.bstrVal;
				freeSpace += getFreeSpaceOfDiskEnum(wmiWbemInfo ,deviceId);
			}

			//////////size
			/*hr = pclsObj->Get(L"Size", 0, &vtProp, 0, 0);
			long long capacityNew = 0;
			if (vtProp.bstrVal) {
				capacityStr = vtProp.bstrVal;
				capacityNew = (long long)stoll(capacityStr);
				capacityNew /= (1024 * 1024 * 1024);
			}*/
			////////////////
			VariantClear(&vtProp);

			pclsObj->Release();
		}
		if (pEnumerator) {
			pEnumerator->Release();
		}
		return freeSpace;
	}



	float SystemInfoDll::getDiskPercentage() {
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		/*pModule = (PyObject *)(m_pModule);
		if (pModule == NULL)
		{
			return 0.0;
		}*/
		getIpInfo = (PyObject *)m_getDiskPercentage;// PyObject_GetAttrString(pModule, "getDiskInfo");//这里是要调用的函数名
		if (!getIpInfo) {
			return 0.0;
		}
		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		if (!pRet) {
			//Py_DECREF(getIpInfo);
			return 0.0;
		}
		float result = 0.0;
		PyArg_Parse(pRet, "f", &result);//转换返回类型;
		Py_DECREF(pRet);
		//Py_DECREF(getIpInfo);
		return result;
	}

	float SystemInfoDll::getNetDelay(wchar_t *goalIp) {
		if (!goalIp) {
			return 0.0;
		}
		if (!m_wmiWbemInfo) {
			return 0;
		}
		WMIWBEMINFO *wmiWbemInfo = (WMIWBEMINFO *)(m_wmiWbemInfo);

		std::wstring sqlStr = L"Select ResponseTime from Win32_PingStatus Where Address = '" + std::wstring(goalIp) + L"'";
		float totalResponseTime = 0.0;
		for (int i = 0; i < 4; ++i) {
			IEnumWbemClassObject* pEnumerator = executeWQLQuery(wmiWbemInfo->getHres(),
				wmiWbemInfo->getWbemLocator(),
				wmiWbemInfo->getWbemServices(),
				bstr_t(sqlStr.c_str()));
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			while (pEnumerator) {
				HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
					&pclsObj, &uReturn);

				if (0 == uReturn) {
					break;
				}

				VARIANT vtProp;
				float responseTime;
				///deviceID
				hr = pclsObj->Get(L"ResponseTime", 0, &vtProp, 0, 0);
				if (vtProp.bstrVal) {
					responseTime = vtProp.iVal;
					totalResponseTime += responseTime;
				}

				VariantClear(&vtProp);

				pclsObj->Release();
			}
			if (pEnumerator) {
				pEnumerator->Release();
			}
		}
		return totalResponseTime / 4.0;
	}

	int SystemInfoDll::getDeviceId(wchar_t * deviceID, unsigned int len) {
		if (!deviceID) {
			return -1;
		}
		if (m_deviceId.empty()) {
			GUID guid;
			CoCreateGuid(&guid);
			char cBuffer[64] = { 0 };
			sprintf_s(cBuffer, sizeof(cBuffer),
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				guid.Data1, guid.Data2,
				guid.Data3, guid.Data4[0],
				guid.Data4[1], guid.Data4[2],
				guid.Data4[3], guid.Data4[4],
				guid.Data4[5], guid.Data4[6],
				guid.Data4[7]);
			m_deviceId = s2ws(cBuffer);
			if (m_deviceId.empty()) {
				return -1;
			}
			if (len < m_deviceId.size()) {
				return -1;
			}
		}
		if (!m_deviceId.empty()) {
			wmemcpy(deviceID, m_deviceId.c_str(), m_deviceId.size());
			return 0;
		}
		return -1;
	}

	int SystemInfoDll::getDeviceMac(wchar_t * mac , unsigned int len) {
		if (!mac) {
			return -1;
		}
		int ret = Networkspace::getLocalMacAddress(mac,len);
		return ret;
	}

	int SystemInfoDll::getLocalLanIp(wchar_t * lanIp, unsigned int len) {
		if (!lanIp) {
			return -1;
		}
		int ret = Networkspace::getLocalLanIp(lanIp, len);
		return 0;
	}

	int  SystemInfoDll::getInternetIp(wchar_t * internetIp, unsigned int len) {
		if (!internetIp) {
			return -1;
		}
		//wchar_t * goalIp = new wchar_t[250];
		int ret = Networkspace::getIpAddress(internetIp , len);
		return ret;
	}

	int SystemInfoDll::getRegion(wchar_t * region, unsigned int len) {
		if (!region) {
			return -1;
		}
		if (20 > len){

			return -1;
		}
		//SetPythonEnv(g_pythonHomePath);
		//Py_Initialize();
		//if (!Py_IsInitialized())
		//{
		//	//std::cout<< L"初始化失败!" << std::endl;;
		//	return -1;
		//}
		
		//PyRun_SimpleString("import sys");
		//PyRun_SimpleString("sys.path.append('./Release/')");//这一步很重要，修改Python路径
		//PyRun_SimpleString("sys.path.append('./')");
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		//pModule = (PyObject *)(m_pModule);
		//if (pModule == NULL)
		//{
		//	///std::cout << "没找到getOutterIp.py!" << endl;
		//	//Py_Finalize(); // 与初始化对应
		//	return -1;
		//}
		getIpInfo = (PyObject *)m_getRegion;// PyObject_GetAttrString(pModule, "getRegion");//这里是要调用的函数名
		//testFunc = PyObject_GetAttrString(pModule, "getTest");
		wchar_t internetIp[50];
		memset(internetIp, 0, sizeof(internetIp));
		int retInter = getInternetIp(internetIp ,50);
		if (0 != retInter) {
			//Py_DECREF(testFunc);
			//Py_DECREF(getIpInfo);
			//Py_Finalize(); // 与初始化对应
			return -1;
		}
		std::string userIp = ws2s(internetIp);
		std::string internetIp1 = R"()" + userIp;
		std::string str1 = "";
		std::string str2 = "";
		//PyObject* args = Py_BuildValue("s", inwternetIp.c_str());//给python函数参数赋值
		wchar_t *result = NULL;
		try {
			PyObject *pArgs = PyTuple_New(1);//函数调用的参数传递均是以元组的形式打包的,2表示参数个数
			PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", internetIp1.c_str()));//0--序号,i表示创建int型变量
			//经常崩溃在此处
			PyObject* pRet = PyObject_CallObject(getIpInfo, pArgs);//调用函数
			//PyObject* pRet1 = PyObject_CallObject(testFunc , NULL);
			//char *result1 = NULL;
			PyArg_Parse(pRet, "u", &result);//转换返回类型
			//PyObject* bytes = PyUnicode_AsUTF8String(result);
			//Py_DECREF(bytes);
			if (result) {
				//str1 = result;
				//std::cout << "this is test region" << result << std::endl;
				//str2 = UnEscape(str1.c_str());
			}
			if (pRet) {
				Py_DECREF(pRet);
			}
			if (pArgs) {
				Py_DECREF(pArgs);
			}
		}
		catch (...) {
			std::cout << "getRegion is failure! error:"<< GetLastError() << std::endl;
			return -1;
		}
		if (!result) {
			return -1;
		}
		///////////
		std::wstring tempStr = result;// s2ws(str1.c_str());
		if (len < tempStr.size()) {
			return 0;
		}
		if (!tempStr.empty()) {
			wmemcpy(region, tempStr.c_str(), wcslen(tempStr.c_str()));
			return 0;
		}
		else {
			return -1;
		}
	}

	int SystemInfoDll::getNetworkOperator(wchar_t * operator1, unsigned int len) {
		if (!operator1) {
			return -1;
		}
		try {
			//SetPythonEnv(g_pythonHomePath);
			//Py_Initialize();
			//if (!Py_IsInitialized())
			//{
			//	//std::cout<< L"初始化失败!" << std::endl;;
			//	return -1;
			//}

			//PyRun_SimpleString("import sys");
			////PyRun_SimpleString("sys.path.append('./Release/')");//这一步很重要，修改Python路径
			//PyRun_SimpleString("sys.path.append('./')");
			PyObject * pModule = NULL;//声明变量
			PyObject * getIpInfo = NULL;// 声明变量
			//PyObject * testFunc = NULL;// 声明变量
			//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
			//pModule = (PyObject *)(m_pModule);
			//if (pModule == NULL)
			//{
			//	///std::cout << "没找到getOutterIp.py!" << endl;
			//	//Py_Finalize(); // 与初始化对应
			//	return -1;
			//}
			getIpInfo = (PyObject *)m_getNetworkOperator;// PyObject_GetAttrString(pModule, "getInternetOperator");//这里是要调用的函数名
			//testFunc = PyObject_GetAttrString(pModule, "getTest");
			wchar_t internetIp[50];
			memset(internetIp, 0, sizeof(internetIp));
			int retInter = getInternetIp(internetIp, 50);
			if (0 != retInter) {
				//Py_DECREF(testFunc);
				if (getIpInfo) {
					//Py_DECREF(getIpInfo);
				}
				//Py_Finalize(); // 与初始化对应
				return -1;
			}
			std::string userIp = ws2s(internetIp);
			std::string internetIp1 = R"()" + userIp;
			std::string str1 = "";
			wchar_t *result = NULL;
			//PyObject* args = Py_BuildValue("s", internetIp.c_str());//给python函数参数赋值
			try {
				PyObject *pArgs = PyTuple_New(1);//函数调用的参数传递均是以元组的形式打包的,2表示参数个数
				PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", internetIp1.c_str()));//0--序号,i表示创建int型变量

				PyObject* pRet = PyObject_CallObject(getIpInfo, pArgs);//调用函数
				
				PyArg_Parse(pRet, "u", &result);//转换返回类型
				if (result)
				{
					//str1 = result;
				}

				//Py_DECREF(pRet1);
				if (pRet) {
					Py_DECREF(pRet);
				}
				if (pArgs) {
					Py_DECREF(pArgs);
				}
			}
			catch (...) {
				std::cout << "getNetworkOperator is failure!error="<<GetLastError() << std::endl;
				return -1;
			}
			//Py_DECREF(testFunc);
			/*if (getIpInfo) {
				Py_DECREF(getIpInfo);
			}*/
			//Py_Finalize(); // 与初始化对应
			/////
			if (!result) {
				return -1;
			}
			std::wstring  tempStr = result;// s2ws(str1.c_str());
			if (len < tempStr.size()) {
				return 0;
			}
			if (!tempStr.empty()) {
				wmemcpy(operator1, tempStr.c_str(), wcslen(tempStr.c_str()));
				return 0;
			}
			else {
				return -1;
			}
		}
		catch (exception e) {
			return -1;
		}
	}

	int SystemInfoDll::getNetworkUploadSpeed(wchar_t * speed, unsigned int len) {
		if (!speed) {
			return -1;
		}
		//SetPythonEnv(g_pythonHomePath);
		//Py_Initialize();
		//if (!Py_IsInitialized())
		//{
		//	//std::cout<< L"初始化失败!" << std::endl;;
		//	return -1;
		//}

		//PyRun_SimpleString("import sys");
		////PyRun_SimpleString("sys.path.append('./Release/')");//这一步很重要，修改Python路径
		//PyRun_SimpleString("sys.path.append('./')");
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		//pModule = (PyObject *)(m_pModule);
		//if (pModule == NULL)
		//{
		//	///std::cout << "没找到getOutterIp.py!" << endl;
		//	//Py_Finalize(); // 与初始化对应
		//	return -1;
		//}
		getIpInfo = (PyObject *)m_getNetworkUploadSpeed;// PyObject_GetAttrString(pModule, "getNetworkDownLoadFlow");//这里是要调用的函数名
		//testFunc = PyObject_GetAttrString(pModule, "getTest");
		//PyObject* args = Py_BuildValue("s", internetIp.c_str());//给python函数参数赋值
		//PyObject *pArgs = PyTuple_New(1);//函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		//PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", internetIp1.c_str()));//0--序号,i表示创建int型变量

		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		char *result = NULL;
		PyArg_Parse(pRet, "s", &result);//转换返回类型
		std::string str1 = "";
		if (result) {
			str1 = result;
		}
		//Py_DECREF(pRet1);
		if (pRet) {
			Py_DECREF(pRet);
		}
		//Py_DECREF(pArgs);
		//Py_DECREF(testFunc);
		/*if (getIpInfo) {
			Py_DECREF(getIpInfo);
		}*/
		//Py_Finalize(); // 与初始化对应
		/////
		std::wstring  tempStr = s2ws(str1.c_str());
		if (len < tempStr.size()) {
			return -1;
		}
		if (!tempStr.empty()) {
			wmemcpy(speed, tempStr.c_str(), wcslen(tempStr.c_str()));
			return 0;
		}
		else {
			return -1;
		}

		return 0;
	}

	int SystemInfoDll::getNetworkDownloadSpeed(wchar_t * speed, unsigned int len){
		if (!speed) {
			return -1;
		}
		//SetPythonEnv(g_pythonHomePath);
		//Py_Initialize();
		//if (!Py_IsInitialized())
		//{
		//	//std::cout<< L"初始化失败!" << std::endl;;
		//	Py_Finalize(); // 与初始化对应
		//	return -1;
		//}

		//PyRun_SimpleString("import sys");
		////PyRun_SimpleString("sys.path.append('./Release/')");//这一步很重要，修改Python路径
		//PyRun_SimpleString("sys.path.append('./')");
		PyObject * pModule = NULL;//声明变量
		PyObject * getIpInfo = NULL;// 声明变量
		//PyObject * testFunc = NULL;// 声明变量
		//pModule = PyImport_ImportModule("getOutterIp");//这里是要调用的文件名hello.py
		//pModule = (PyObject *)(m_pModule);
		//if (pModule == NULL)
		//{
		//	///std::cout << "没找到getOutterIp.py!" << endl;
		//	//Py_Finalize(); // 与初始化对应
		//	return -1;
		//}
		getIpInfo = (PyObject *)m_getNetworkDownloadSpeed;// PyObject_GetAttrString(pModule, "getNetworkUpLoadFlow");//这里是要调用的函数名
		//testFunc = PyObject_GetAttrString(pModule, "getTest");
		//PyObject* args = Py_BuildValue("s", internetIp.c_str());//给python函数参数赋值
		//PyObject *pArgs = PyTuple_New(1);//函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		//PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", internetIp1.c_str()));//0--序号,i表示创建int型变量

		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		char *result = NULL;
		PyArg_Parse(pRet, "s", &result);//转换返回类型
		std::string str1 = "";
		if (result) {
			str1 = result;
		}

		//Py_DECREF(pRet1);
		if (pRet) {
			Py_DECREF(pRet);
		}
		//Py_DECREF(pArgs);
		//Py_DECREF(testFunc);
		/*if (getIpInfo) {
			Py_DECREF(getIpInfo);
		}*/
		//Py_Finalize(); // 与初始化对应
		/////
		std::wstring  tempStr = s2ws(str1.c_str());
		if (len < tempStr.size()) {
			return -1;
		}
		if (!tempStr.empty()) {
			wmemcpy(speed, tempStr.c_str(), wcslen(tempStr.c_str()));
			return 0;
		}
		else {
			return -1;
		}

		return 0;
	}


	int SystemInfoDll::getCpuGuid(wchar_t * content, unsigned int len) {
		if (!content) {
			return -1;
		}
		PyObject * getIpInfo = NULL;// 声明变量
		getIpInfo = (PyObject *)(m_getCpuGuid);
		if (!getIpInfo) {
			return -1;
		}
		PyObject* pRet = PyObject_CallObject(getIpInfo, NULL);//调用函数
		char *result = NULL;
		PyArg_Parse(pRet, "s", &result);//转换返回类型
		std::string str1 = "";
		if (result) {
			str1 = result;
		}
		if (pRet) {
			Py_DECREF(pRet);
		}
		
		std::wstring  tempStr = s2ws(str1.c_str());
		if (len < tempStr.size()) {
			return -1;
		}
		if (!tempStr.empty()) {
			wmemcpy(content, tempStr.c_str(), wcslen(tempStr.c_str()));
			return 0;
		}
		else {
			return -1;
		}

		return 0;
	}

	int SystemInfoDll::setBindUser(const wchar_t * user) {
		if (!user) {
			return -1;
		}
		int sizeUser = 0;
		if (m_userName) {
			delete m_userName;
			m_userName = NULL;
		}
		sizeUser = wcslen(user) + sizeof(wchar_t);
		m_userName = new wchar_t[sizeUser];
		wmemcpy(m_userName, user, sizeUser);
		return 0;
	}

	int findProcess(std::wstring process_name)
	{
		int count = 0;//进程计数 
		PROCESSENTRY32 pe32;

		pe32.dwSize = sizeof(PROCESSENTRY32);
		HANDLE process_snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//创建进程快照句柄

		if (process_snapshot_handle == INVALID_HANDLE_VALUE) return -1;//创建句柄失败

		bool is_exist = Process32First(process_snapshot_handle, &pe32);//找第一个
		while (is_exist)
		{
			if (!_wcsicmp(process_name.c_str(), pe32.szExeFile))
				count++;//进程名不区分大小写
			is_exist = Process32Next(process_snapshot_handle, &pe32);//找下一个
		}

		return count;
	}

	const wchar_t * SystemInfoDll::getBindUser() {
		return m_userName;
	}

	int SystemInfoDll::isRunningGame(wchar_t * game) {
		int ret = -1;
		if (game) {
			if (findProcess(game) > 0) {
				ret = 0;
			}
		}
		return ret;
	}

}
