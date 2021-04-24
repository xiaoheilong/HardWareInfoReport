// TestHardWareReport.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "./HardWareReport.h"
#include "resource.h"
#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include"shellapi.h"
#include <tlhelp32.h>
#include <iostream>
#include <sstream>
void Safe_flush(FILE *fp)//清空scanf缓冲区内存 
{
	int ch;
	while ((ch = fgetc(fp)) != EOF && ch != '\n');
}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

bool StartGame(char * gamepath, char * command, int showMode = SW_SHOW) {
	if (gamepath && 0 != strlen(gamepath)) {
		SHELLEXECUTEINFO ShExecInfo_au;
		ShExecInfo_au.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo_au.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo_au.hwnd = NULL;
		ShExecInfo_au.lpVerb = NULL;
		ShExecInfo_au.lpFile = stringToLPCWSTR(gamepath);
		if (command != NULL && 0 != strlen(command))
		{
			ShExecInfo_au.lpParameters = stringToLPCWSTR(command);
		}
		else {
			ShExecInfo_au.lpParameters = NULL;
		}
		ShExecInfo_au.lpDirectory = NULL;
		ShExecInfo_au.nShow = showMode;
		ShExecInfo_au.hInstApp = NULL;
		return ShellExecuteEx(&ShExecInfo_au);
	}
	return false;
}

std::string ws2s(const std::wstring& ws)
{
	if (!ws.size()) {
		return "";
	}
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
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


void ForceQuit() {
	WritePrivateProfileString(L"reportInfo" , L"reportStatus" , L"1" , L".//reportStatus.ini");
}

void ForceRunUpdate() {
	WritePrivateProfileString(L"reportInfo", L"reportStatus", L"0", L".//reportStatus.ini");
}


std::wstring GetServerUrl() {
	wchar_t serverUrl[200];
	memset(serverUrl , 0 , sizeof(serverUrl));
	GetPrivateProfileString(L"reportInfo", L"serverUrl", L"" , serverUrl, sizeof(serverUrl), L".//reportStatus.ini");
	std::wstring result = serverUrl;
	return result;
}

template < class T>
void convertFromString(T &value, const std::string &s) {
	std::stringstream ss(s);
	ss >> value;
}

double GetReportInterTimer() {
	wchar_t time[10];
	memset(time, 0, sizeof(time));
	GetPrivateProfileString(L"reportInfo", L"reportInterTime", L"", time, sizeof(time), L".//reportStatus.ini");
	std::wstring result = time;
	double d = 0;
	std::string tempStr = ws2s(result);
	convertFromString(d, tempStr);
	std::cout << d << std::endl;
	return d;
}

bool ctrlhandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
		// handle the ctrl-c signal.
	case CTRL_C_EVENT:
		printf("ctrl-c event\n\n");
		return(true);
		// ctrl-close: confirm that the user wants to exit.
	case CTRL_CLOSE_EVENT:
		printf("ctrl-close event\n\n");
		ForceQuit();
		return(true);
		// pass other signals to the next handler.
	case CTRL_BREAK_EVENT:
		printf("ctrl-break event\n\n");
		return false;
	case CTRL_LOGOFF_EVENT:
		printf("ctrl-logoff event\n\n");
		return false;
	case CTRL_SHUTDOWN_EVENT:
		printf("ctrl-shutdown event\n\n");
		ForceQuit();
		return false;
	default:
		return false;
	}
}
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

typedef HWND(WINAPI * GETCONSOLEPROC)();
HWND GetConsole()
{
	HWND hRet(NULL);
	BOOL bLoad(FALSE);
	HMODULE hMod = GetModuleHandle(_T("kernel32.dll"));
	if (hMod == NULL)
	{
		hMod = LoadLibrary(_T("kernel32.dll"));
		bLoad = TRUE;
	}
	if (hMod != NULL)
	{
		GETCONSOLEPROC pFun = (GETCONSOLEPROC)GetProcAddress(hMod, "GetConsoleWindow");
		if (pFun != NULL)
		{
			hRet = pFun();
		}
		if (bLoad)
		{
			FreeLibrary(hMod);
		}
	}
	return hRet;
}


static const char m_szName[] = "窗口";
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0); 
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
/////命令行窗口句柄
static HWND m_hWnd = 0;
#define MYWM_NOTIFYICON WM_USER+1


int HasAnotherInstance(std::string processName)
{
	int count = 0;
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		//LogEx(UI_ERROR , "GetProcessidFromName Process32First is failure!");
		return false;
	while (1)
	{
		if (count >= 2) {
			break;
		}
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE) {
			//LogEx(g_UI_ERROR, "GetProcessidFromName  result is false!");
			break;
		}
		std::string path1 = processName;
		std::wstring path = s2ws(path1);
		if (lstrcmp(pe.szExeFile, path.c_str()) == 0)
		{
			++count;
		}
	}
	CloseHandle(hSnapshot);
	return count >= 2 ? true : false;
}
#pragma execution_character_set("GB2312")
int main()
{
	if (HasAnotherInstance("TestHardWareReport.exe")) {
		//MessageBoxA(NULL, "TestHardWareReport.exe is Already running!", "error", MB_OK);
		return -1;
	}

	const wchar_t * reportUrl = L"ws://socket1.cccsaas.com:9093";
	std::wstring serverUrl = GetServerUrl();
	if (serverUrl.empty()) {
		serverUrl = reportUrl;
	}
	double interTime = GetReportInterTimer();
	std::cout<<L"url "<<serverUrl.c_str()<<L" time:" << interTime << std::endl;
	HardWareNamespace::HardWareReport * report = HardWareNamespace::CreateHardWareReport(HardWareNamespace::HardWareReportType::TYPE_WEBSOCKET);
	if (report) {
		//const wchar_t * reportUrl = L"http://test.ndxvs.com/sportapi/device/api";
		report->Init(const_cast<wchar_t *>(serverUrl.c_str()) , interTime >0.0 ? interTime :20000);
		report->SetBindUser(L"15574674692");
		//report->SetReportInterTime(3000);
		report->Run();
	}
	ForceRunUpdate();
	//system("pause");
	//char command[200];
	StartGame("protectReportNodeStatus.bat" , "" ,SW_HIDE);
	//////////应用程序图标
	HWND hConsole = GetConsole();
	if (hConsole != NULL)
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
		SendMessage(hConsole, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hConsole, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		if (hIcon) {
			::DestroyIcon(hIcon);
		}
	//	/////////////////托盘图标
	//	NOTIFYICONDATA m_tnid;
	//	m_tnid.cbSize = sizeof(NOTIFYICONDATA);
	//	m_tnid.hWnd = hConsole;
	//	m_tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	//	m_tnid.uCallbackMessage = MYWM_NOTIFYICON;
	//	//用户定义的回调消息 
	//	//std::string szToolTip;
	//	//szToolTip = "reportNodeStatus"; //鼠标在托盘上移动时显示文本
	//	_tcscpy(m_tnid.szTip, _T("reportNodeStatus"));
	//	m_tnid.uID = IDI_ICON2;
	//	//HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
	//	m_tnid.hIcon = hIcon;
	//	::Shell_NotifyIcon(NIM_ADD, &m_tnid);
	//	/*if (hIcon) {
	//		::DestroyIcon(hIcon);
	//	}*/
	//	SendMessage(hConsole, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
	//memset(command , 0 , sizeof(command));
	//while (true) {
		if (!report->IsValid()) {
			report->Init(const_cast<wchar_t *>(serverUrl.c_str()), interTime >0 ? interTime : 20000);
			report->SetBindUser(L"15574674692");
			//report->SetReportInterTime(3000);
			report->Run();
		}
	//	//scanf_s("%s" , command);
	//	std::cout << "intput:" << command << std::endl;
	//	if (0 == strcmp(command, "quit")) {
	//		break;
	//	}
	//	else {
	//		std::cout << "application ding!" << std::endl;
	//		Safe_flush(stdin);
	//	}
	//}
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlhandler, true))
		{
			printf("\nthe control handler is installed.\n");
			printf("\n -- now try pressing ctrl+c or ctrl+break, or");
			printf("\n try logging off or closing the console...\n");
			printf("\n(...waiting in a loop for events...)\n\n");
			while (1) {
				Sleep(100);
			}
		}

	//MSG sMsg;
	//while (int ret = GetMessage(&sMsg, hConsole, 0, 0)) { // 消息循环 
	//	if (ret != -1) {
	//		TranslateMessage(&sMsg);
	//		MessageBoxA(hConsole, "1212", "323", MB_OK);
	//		if (sMsg.lParam == MYWM_NOTIFYICON) {
	//			MessageBoxA(hConsole, "1212", "323", MB_OK);
	//		}
	//		else if (sMsg.lParam == WM_CLOSE) {
	//			MessageBoxA(hConsole, "dasd", "dasdasd", MB_OK);
	//		}
	//		else {
	//			DispatchMessage(&sMsg);
	//		}
	//	}
	//}
	//////
	system("pause");
	ForceQuit();
	HardWareNamespace::ReleaseHardWareReport(report);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
