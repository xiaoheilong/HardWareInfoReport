#include "HardWareInnerVbox.h"
#include "Tools.h"
#include <json/config.h>
#include <json/value.h>
#include <json/writer.h>
#include <iostream>
namespace HardWareNamespace {
	const wchar_t * g_PingUrl = L"baidu.com";


	std::string WString2String(const std::wstring& ws)
	{
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


	HardWareInnerVbox::HardWareInnerVbox():m_systemInfo(NULL), m_curlReport(NULL)
	{
	}


	HardWareInnerVbox::~HardWareInnerVbox()
	{
		Uninit();
	}


	int HardWareInnerVbox::Init() {

		m_systemInfo = std::make_shared<SystemInfoDll>();
		if (m_systemInfo) {
			m_systemInfo->init();
		}
		m_curlReport = std::make_shared<CurlReportCell>();
		return 0;
	}

	int HardWareInnerVbox::Uninit() {
		if (m_systemInfo) {
			m_systemInfo->unInit();
		}
		return 0;
	}


	int HardWareInnerVbox::Report(wchar_t * url) {
		if (m_systemInfo) {
			///////////信息上报
			Json::Value headerValue;
			headerValue["service"] = "deviceService";
			headerValue["method"] = "reportDeviceState";
			headerValue["timestamp"] = GetTimeStamp();
			std::string nocestr = std::to_string(GetRandomString());
			headerValue["noncestr"] = nocestr.c_str();
			headerValue["sign"] = GetSignString().c_str();
			headerValue["version"] = "1.0.0";
			headerValue["platform"] = "pc";
			headerValue["language"] = "cn";
			
			Json::Value bodyValue;
			////////device
			std::string valueTemp = "";
			valueTemp = GetDeviceId();
			bodyValue["deviceNo"] = valueTemp.c_str();
			///////intranetIp  内网ip
			valueTemp.clear();
			valueTemp = GetLocalLanIp();
			bodyValue["intranetIp"] = valueTemp.c_str();
			//////////internetIp 外网
			valueTemp.clear();
			valueTemp = GetInternetIp();
			bodyValue["internetIp"] = valueTemp.c_str();
			////// 地区
			valueTemp.clear();
			valueTemp = GetRegion();
			bodyValue["region"] = valueTemp.c_str();
			/////cpu占用率
			bodyValue["cpuUsage"] = GetCpuUsagePercentage();
			/////gpu占用率
			bodyValue["gpuUsage"] = GetNvidaGPUPercentage();
			/////内存占用率
			bodyValue["ramUsage"] = GetRamPercentage();
			/////磁盘占用率
			bodyValue["diskUsage"] = GetDiskPercentage();
			/////网络运营商
			valueTemp.clear();
			valueTemp = GetNetworkOperator();
			bodyValue["networkOperator"] = valueTemp.c_str();
			/////网络上传速度
			valueTemp.clear();
			valueTemp = GetNetworkUploadSpeed();
			bodyValue["networkSpeed"] = valueTemp.c_str();
			/////网络延迟
			bodyValue["networkLatency"] = GetNetDelay(const_cast<wchar_t *>(g_PingUrl));
			//Json::StyledWriter styled_writer;
			Json::Value root;
			root["header"] = headerValue;
			root["body"] = bodyValue;
			Json::StreamWriterBuilder styled_write;
			styled_write.settings_["indentation"] = "";
			styled_write.settings_["precision"] = 16;
			styled_write.settings_["emitUTF8"] = true;
			std::unique_ptr<Json::StreamWriter> writer(styled_write.newStreamWriter());
			std::ostringstream os;
			writer->write(root, &os);
			std::string rootJsonStr = os.str();
			os.clear();
			////////curl http上报
			std::cout << rootJsonStr.c_str() << std::endl;
			std::string urlS =  WString2String(url);
			m_curlReport->SetReportUrl(urlS);
			m_curlReport->Report(rootJsonStr);
			return 0;
		}
		return -1;
	}


	////////////
	float HardWareInnerVbox::GetCpuUsagePercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getCpuUsagePercentage();
		}
		return 0.0;
	}

	float HardWareInnerVbox::GetRamPercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getRamPercentage();
		}
		return -1;
	}

	float HardWareInnerVbox::GetNvidaGPUPercentage() {

		if (m_systemInfo) {
			return m_systemInfo->getNvidaGPUPercentage();
		}
		return -1;
	}

	float HardWareInnerVbox::GetDiskPercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getDiskPercentage();
		}
		return -1;
	}

	float HardWareInnerVbox::GetNetDelay(wchar_t * goalIp) {
		if (m_systemInfo) {
			return m_systemInfo->getNetDelay(goalIp);
		}
		return 0.0;
	}

	std::string HardWareInnerVbox::GetDeviceId() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getDeviceId(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetDeviceMac() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getDeviceMac(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetLocalLanIp() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getLocalLanIp(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return  strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetInternetIp() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getInternetIp(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetRegion() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getRegion(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetNetworkOperator() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getNetworkOperator(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetNetworkUploadSpeed() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getNetworkUploadSpeed(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVbox::GetNetworkDownloadSpeed() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getNetworkDownloadSpeed(str, strSize);
			std::string strS = WString2String(std::wstring(str));
			return strS;
		}
		return "";
	}

	int HardWareInnerVbox::SetBindUser(const wchar_t * user) {
		if (m_systemInfo) {
			m_systemInfo->setBindUser(const_cast<wchar_t *>(user));
			return 0;
		}
		return -1;
	}

	const wchar_t* HardWareInnerVbox::GetBindUser() {
		if (m_systemInfo) {
			return m_systemInfo->getBindUser();
		}
		return L"";
	}

	int HardWareInnerVbox::IsRunningGame(wchar_t * game) {
		if (m_systemInfo) {
			return m_systemInfo->isRunningGame(game);
		}
		return -1;
	}

}
