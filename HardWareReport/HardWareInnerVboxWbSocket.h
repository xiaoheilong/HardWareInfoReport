#pragma once
#include "HardWareReport.h"
#include "./easywsclient.hpp"
#include "websocketConnection.h"
#include <SystemInfoDll.h>
#include <string>
#include <thread>
#include <json/config.h>
#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>
#include <iostream>
//#include <memory>
using namespace SystemWMISpace;
using easywsclient::WebSocket;
using namespace WebSocketNamsSpace;
namespace HardWareNamespace {
	extern const wchar_t * g_PingUrl;
	class HardWareInnerVboxWbSocket:public HardWareReportInner , public OutterInterfaceConnection
	{
	public:
		HardWareInnerVboxWbSocket();
		~HardWareInnerVboxWbSocket();
	public:
		virtual int Init();
		virtual int Uninit() ;
		virtual int Report(wchar_t * url);
		virtual const wchar_t * GetBindUser();
		virtual int SetBindUser(const wchar_t * reportUrl);
		std::string  WSServiceTransferSignString();
	protected:
		virtual void ConnectedCallback(std::string msg, int error);
		virtual void DisconnectedCallback(std::string msg, int error);
		virtual void MessageCallback(std::string msg, int error);
		virtual void FailureCallback(std::string msg, int error) ;
		virtual void InterruptCallback(std::string msg, int error);
	private:
		float GetCpuUsagePercentage(); //cpu利用率  not /100
		float GetRamPercentage();  //内存利用率  not /100
		float GetNvidaGPUPercentage();//获取英伟达显卡利用率  not /100
		float GetDiskPercentage();//获取磁盘利用率 not /100
		float GetNetDelay(wchar_t * goalIp);//获取网络延迟  ,  ms
		std::string GetDeviceId();//获取设备机器码  guid
		std::string GetDeviceMac();//mac 地址
		std::string GetLocalLanIp();//获取内网ip
		std::string GetInternetIp();//获取外网
		std::string GetRegion();//地区
		std::string GetNetworkOperator();//网络运营商
		std::string GetNetworkUploadSpeed();//网络上传速度
		std::string GetNetworkDownloadSpeed();//网络上传速度
		std::string GetCpuGUID(); //获取cpu序列号
		int IsRunningGame(wchar_t * game);//是否运行游戏  1 运行  0 没有运行

	protected:
		void Run(std::string url , std::string deviceNo);
		void InitWebSocket(std::string url);
		void UnInitWebSocket();

		void SignInCloudGameCallback(std::string signInData);

		Json::Value  HeaderPackets(std::string msgType);
	private:
		std::shared_ptr<SystemInfoDll> m_systemInfo;
		std::shared_ptr<WsAppConnection> m_webSocket;  ///websocket 
		std::shared_ptr<std::thread>  m_thread;  //websocket
		bool m_webSocketFlag;
	public:
		std::string m_deviceNo;
		std::string m_sessionId;
	};

}

