#include "HardWareInnerVboxWbSocket.h"
#include "Tools.h"
#include "./md5/md5.h"
#include <objbase.h>
namespace HardWareNamespace {
	HardWareInnerVboxWbSocket::HardWareInnerVboxWbSocket() :m_systemInfo(NULL), m_webSocket(NULL), m_thread(NULL),m_webSocketFlag(false),
		m_deviceNo(""), m_sessionId("")
	{

	}


	HardWareInnerVboxWbSocket::~HardWareInnerVboxWbSocket()
	{
		Uninit();
		UnInitWebSocket();
	}
	
	std::string  HardWareInnerVboxWbSocket::WSServiceTransferSignString() {
		///
		Json::Value root;
		Json::Value data;
		////////
		int ret = -1;
		long long timeStamp = GetTimeStamp();
		std::wstring randomString = std::to_wstring(GetRandomString());
		root["noncestr"] = WsToS(randomString.c_str());

		std::string noncestrQt = std::to_string(timeStamp);
		std::string epochTimeStr = noncestrQt;
		root["timestamp"] = epochTimeStr;
		std::string signStr = epochTimeStr + WsToS(API_SIGN_KEY);
		signStr += noncestrQt;
		std::string  signStrMd5 = MD5NameSpace::md5(signStr);
		root["sign"] = signStrMd5.c_str();
		root["type"] = "SignIn";
		root["sessionId"] = "";
		////////
		if (m_deviceNo.empty()) {
			std::string deviceNoStr = "";
			//deviceNoStr += GetDeviceId();
			deviceNoStr = GetCpuGUID() ;// GetDeviceMac();
			deviceNoStr += GetDeviceMac();
			//std::cout << "@@@@@@@@@@@mac address1@@@@@@@@@@@:" << deviceNoStr.c_str()<<" size:"<< deviceNoStr.size() << std::endl;
			deviceNoStr = MD5NameSpace::md5(deviceNoStr + "ndxx");
			//std::cout << "@@@@@@@@@@@mac address2@@@@@@@@@@@:" << deviceNoStr.c_str() << " size:" << deviceNoStr.size() << std::endl;
			m_deviceNo = deviceNoStr;
			//////写入本地的文件
			FILE* file = NULL;
			file = fopen("c:\\testDeviceNo.txt", "wt+");
			if (file) {
				fwrite(m_deviceNo.c_str(), m_deviceNo.size(), 1, file);
				fclose(file);
			}
			//std::string testDStr = "123456789";
			//testDStr = MD5NameSpace::md5(testDStr);
			//std::cout << "@@@@@@@@@@@mac address3  testDStr@@@@@@@@@@@:" << testDStr.c_str() << std::endl;
		}
		data["deviceNo"] = m_deviceNo.c_str();
		root["data"] = data;
		////////
		Json::StreamWriterBuilder styled_write;
		styled_write.settings_["indentation"] = "";
		styled_write.settings_["precision"] = 16;
		styled_write.settings_["emitUTF8"] = true;
		std::unique_ptr<Json::StreamWriter> writer(styled_write.newStreamWriter());
		std::ostringstream os;
		writer->write(root, &os);
		return os.str();
	}


	void HardWareInnerVboxWbSocket::ConnectedCallback(std::string msg, int error) {

	}

	void HardWareInnerVboxWbSocket::DisconnectedCallback(std::string msg, int error) {

	}
	
	void HardWareInnerVboxWbSocket::MessageCallback(std::string message, int error) {
		if (message != "")
		{
			Json::Value root;
			Json::CharReaderBuilder readBuilder;
			std::unique_ptr<Json::CharReader> const reader(readBuilder.newCharReader());
			JSONCPP_STRING errs;
			bool ret = reader->parse(message.c_str(), message.c_str() + message.size(), &root, &errs);
			if (!ret || !errs.empty()) {
				return;
			}
			/////////////
			bool exsit = root.isMember("type");
			if (!exsit) {
				bool exsit = root.isMember("code");
				if (exsit) {
					std::string codeStr = root["code"].asString();
					if (codeStr.compare("0") == 0) {// connect is success
						///please signIn
						std::cout << "connect success!" << std::endl;
						std::string signInStr = WSServiceTransferSignString();
						m_webSocket->Send(signInStr.c_str());
					}
					else {

					}
				}
			}
			else {
				std::string msgType = root["type"].asCString();
				std::string returnCodeStr = root["code"].asCString();
				int returnCode = atoi(returnCodeStr.c_str());
				if (0 != returnCode) {
					return;
				}
				if (!msgType.empty()) {
					bool existData = root.isMember("data");
					if (existData) {
						Json::StreamWriterBuilder styled_write;
						styled_write.settings_["indentation"] = "";
						styled_write.settings_["precision"] = 16;
						styled_write.settings_["emitUTF8"] = true;
						std::unique_ptr<Json::StreamWriter> writer(styled_write.newStreamWriter());
						std::ostringstream os;
						writer->write(root["data"], &os);
						std::string rootJsonStr = os.str();
						//std::cout << " send signIn content:" << rootJsonStr.c_str() << std::endl;
						if (msgType.compare("SignIn") == 0) {//signIn
							this->SignInCloudGameCallback(rootJsonStr.c_str());
						}
					}
				}
			}
		}
	}

	void HardWareInnerVboxWbSocket::FailureCallback(std::string msg, int error) {

	}

	void HardWareInnerVboxWbSocket::InterruptCallback(std::string msg, int error) {

	}



	void HardWareInnerVboxWbSocket::Run(std::string url ,std::string deviceNo) {
		m_webSocketFlag = true;
		m_webSocket = std::make_shared<WsAppConnection>();
		if (m_webSocket.get()) {
			m_webSocket->init(url);
			m_webSocket->connect();
			m_webSocket->SetCallback(this);
		}
		//m_webSocket = std::shared_ptr<WebSocket>(WebSocket::from_url(url));
		//if (!m_webSocket) {
		//	return;
		//}
		///*std::string loginCommand = m_loginCommand.toStdString();
		//ws->send(loginCommand.c_str());*/
		////////
		//while (m_webSocketFlag) {
		//	if(!(m_webSocket->getReadyState() != WebSocket::CLOSED)){
		//		m_webSocket = std::shared_ptr<WebSocket>(WebSocket::from_url(url));
		//		if (!m_webSocket) {
		//			return;
		//		}
		//	}
		//	WebSocket::pointer wsp = &*m_webSocket; // <-- because a unique_ptr cannot be copied into a lambda
		//	m_webSocket->poll();
		//	m_webSocket->dispatch([&](const std::string & message) {
		//		
		//	});
		//}
	}

	void HardWareInnerVboxWbSocket::SignInCloudGameCallback(std::string signInData) {
		if (signInData.empty()) {
			return;
		}
		/*Json::Reader reader;
		Json::Value root;*/
		Json::Value root;
		Json::CharReaderBuilder readBuilder;
		std::unique_ptr<Json::CharReader> const reader(readBuilder.newCharReader());
		//Json::Reader reader;
		JSONCPP_STRING errs;
		bool ret = reader->parse(signInData.c_str(), signInData.c_str() + signInData.size(), &root, &errs);
		if (!ret || !errs.empty()) {
			return ;
		}
		////////////
		if (root.isMember("sessionId")) {
			if (root["sessionId"].isString()) {
				m_sessionId = root["sessionId"].asCString();
				//std::cout << "recieve the sessionId:" << m_sessionId.c_str() << std::endl;
			}
		}
	}

	int HardWareInnerVboxWbSocket::Init() {
		Uninit();
		m_systemInfo = std::make_shared<SystemInfoDll>();
		if (m_systemInfo) {
			m_systemInfo->init();
		}
		return 0;
	}

	int HardWareInnerVboxWbSocket::Uninit() {
		if (m_systemInfo.get()) {
			m_systemInfo->unInit();
		}

		return 0;
	}

	void HardWareInnerVboxWbSocket::InitWebSocket(std::string url) {
		if (url.empty()) {
			return;
		}
		m_thread = std::make_shared<std::thread>(&HardWareInnerVboxWbSocket::Run , this , url , m_deviceNo);
		if (m_thread.get()) {
			m_thread->detach();
		}
	}

	void HardWareInnerVboxWbSocket::UnInitWebSocket() {
		m_webSocketFlag = false;
		if (m_thread.get()) {
			if (m_thread->joinable()) {
				m_thread->join();
			}
		}
		if (m_webSocket.get()) {
			m_webSocket->close();
			m_webSocket.reset();
		}
	}


	Json::Value HardWareInnerVboxWbSocket::HeaderPackets(std::string msgType){
		Json::Value root;
		if (!msgType.empty()) {
			////////
			int ret = -1;
			long long timeStamp = GetTimeStamp();
			std::wstring randomString = std::to_wstring(GetRandomString());
			root["noncestr"] = WsToS(randomString.c_str());

			std::string noncestrQt = std::to_string(timeStamp);
			std::string epochTimeStr = noncestrQt;
			root["timestamp"] = epochTimeStr;
			std::string signStr = epochTimeStr + WsToS(API_SIGN_KEY);
			signStr += noncestrQt;
			std::string  signStrMd5 = MD5NameSpace::md5(signStr);
			root["sign"] = signStrMd5.c_str();
			root["type"] = msgType.c_str();
			root["sessionId"] = m_sessionId;
		}
		////////
		return root;
	}

	int HardWareInnerVboxWbSocket::Report(wchar_t * url) {
		if (!url) {
			return -1;
		}
		/////////初始化websocket , 如果websocket 没有初始化， 那么先初始化 , return
		if (!m_webSocket.get()) {
			std::string websocketUlr = WString2String(std::wstring(url));
			InitWebSocket(websocketUlr);
			return 0;
		}
		/////////
		if (m_systemInfo) {
			///////////信息上报
			Json::Value root;
			root = HeaderPackets("ReportDeviceState");
			if (!root.size()) {
				return -1;
			}
			/////////////////
			Json::Value bodyValue;
			////////device
			std::string valueTemp = "";
			if (m_deviceNo.empty()) {
				//valueTemp = GetDeviceId();
				//valueTemp += GetDeviceId();
				valueTemp = GetCpuGUID();// GetDeviceMac();
				valueTemp += GetDeviceMac();
				valueTemp = MD5NameSpace::md5(valueTemp + "ndxx");
				m_deviceNo = valueTemp;
				///////
			}
			else {
				valueTemp = m_deviceNo;
			}
			bodyValue["deviceNo"] = valueTemp.c_str();
			///////intranetIp  内网ip
			valueTemp.clear();
			valueTemp = GetLocalLanIp();
			bodyValue["intranetIp"] = valueTemp.c_str();
			//////////internetIp 外网
			valueTemp.clear();
			valueTemp = GetInternetIp();
			bodyValue["internetIp"] = valueTemp.c_str();
			//////mac 地址
			valueTemp.clear();
			valueTemp = GetDeviceMac();
			bodyValue["mac"] = valueTemp.c_str();
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
			///////版本上报
			valueTemp.clear();
			valueTemp = "1.0.0.18";
			bodyValue["noteVersion"] = valueTemp.c_str();
			///////
			root["data"] = bodyValue;
			//////////
			Json::StreamWriterBuilder styled_write;
			styled_write.settings_["indentation"] = "";
			styled_write.settings_["precision"] = 16;
			styled_write.settings_["emitUTF8"] = true;
			std::unique_ptr<Json::StreamWriter> writer(styled_write.newStreamWriter());
			std::ostringstream os;
			writer->write(root, &os);
			std::string rootJsonStr = os.str();
			os.clear();
			////////websocket 上报
			if (m_webSocket.get()) {
				std::cout << "send report: " << rootJsonStr.c_str()<<std::flush<<std::endl;
				m_webSocket->Send(rootJsonStr);
			}
			return 0;
		}
		return -1;
	}

	/////////////////////
	////////////
	float HardWareInnerVboxWbSocket::GetCpuUsagePercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getCpuUsagePercentage();
		}
		return 0.0;
	}

	float HardWareInnerVboxWbSocket::GetRamPercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getRamPercentage();
		}
		return -1;
	}

	float HardWareInnerVboxWbSocket::GetNvidaGPUPercentage() {

		if (m_systemInfo) {
			return m_systemInfo->getNvidaGPUPercentage();
		}
		return -1;
	}

	float HardWareInnerVboxWbSocket::GetDiskPercentage() {
		if (m_systemInfo) {
			return m_systemInfo->getDiskPercentage();
		}
		return -1;
	}

	float HardWareInnerVboxWbSocket::GetNetDelay(wchar_t * goalIp) {
		if (m_systemInfo) {
			return m_systemInfo->getNetDelay(goalIp);
		}
		return 0.0;
	}

	std::string HardWareInnerVboxWbSocket::GetDeviceId() {
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

	std::string HardWareInnerVboxWbSocket::GetDeviceMac() {
		//std::cout << "GetDeviceMac step1" << std::endl;
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getDeviceMac(str, strSize);
			//std::cout << "mac address1:" << str << std::endl;
			std::string strS = WString2String(std::wstring(str));
			//std::cout << "mac address2:" << strS << std::endl;
			return strS;
		}
		//std::cout << "GetDeviceMac step1" << std::endl;
		return "";
	}

	std::string HardWareInnerVboxWbSocket::GetLocalLanIp() {
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

	std::string HardWareInnerVboxWbSocket::GetInternetIp() {
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

	std::string HardWareInnerVboxWbSocket::GetRegion() {
		if (m_systemInfo) {
			wchar_t str[50];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getRegion(str, strSize);

			std::string strS =  WString2String(std::wstring(str));
			//std::cout << "getRegion:" << strS.c_str() << std::endl;
			return strS;
		}
		return "";
	}

	std::string HardWareInnerVboxWbSocket::GetNetworkOperator() {
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

	std::string HardWareInnerVboxWbSocket::GetNetworkUploadSpeed() {
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

	std::string HardWareInnerVboxWbSocket::GetNetworkDownloadSpeed() {
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

	std::string HardWareInnerVboxWbSocket::GetCpuGUID() {
		if (m_systemInfo) {
			wchar_t str[100];
			int strSize = sizeof(str);
			memset(str, 0, strSize);
			m_systemInfo->getCpuGuid(str, strSize);
			//
			//CoInitialize(NULL);
			//GUID guid;
			//CoCreateGuid(&guid);
			//WCHAR wszGuid[39];
			//StringFromGUID2(guid, wszGuid, 39);

			//std::wstring guidStr = str;
			//std::cout << "cpuguid:" << str << std::endl;
			//std::cout << "wszGuid:" << wszGuid << std::endl;
			//guidStr += wszGuid;
			//std::cout << "total mix string:" << guidStr.c_str() << std::endl;
			////WideCharToMultiByte(CP_ACP, 0, wszGuid, -1, localName, 128, NULL, NULL);
			//CoUninitialize();
			////////
			std::string strS = WString2String(str);
			return strS;
		}
		return "";
	}

	int HardWareInnerVboxWbSocket::SetBindUser(const wchar_t * user) {
		if (m_systemInfo) {
			m_systemInfo->setBindUser(const_cast<wchar_t *>(user));
			return 0;
		}
		return -1;
	}

	const wchar_t* HardWareInnerVboxWbSocket::GetBindUser() {
		if (m_systemInfo) {
			return m_systemInfo->getBindUser();
		}
		return L"";
	}

	int HardWareInnerVboxWbSocket::IsRunningGame(wchar_t * game) {
		if (m_systemInfo) {
			return m_systemInfo->isRunningGame(game);
		}
		return -1;
	}

}
