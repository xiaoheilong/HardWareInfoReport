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
		float GetCpuUsagePercentage(); //cpu������  not /100
		float GetRamPercentage();  //�ڴ�������  not /100
		float GetNvidaGPUPercentage();//��ȡӢΰ���Կ�������  not /100
		float GetDiskPercentage();//��ȡ���������� not /100
		float GetNetDelay(wchar_t * goalIp);//��ȡ�����ӳ�  ,  ms
		std::string GetDeviceId();//��ȡ�豸������  guid
		std::string GetDeviceMac();//mac ��ַ
		std::string GetLocalLanIp();//��ȡ����ip
		std::string GetInternetIp();//��ȡ����
		std::string GetRegion();//����
		std::string GetNetworkOperator();//������Ӫ��
		std::string GetNetworkUploadSpeed();//�����ϴ��ٶ�
		std::string GetNetworkDownloadSpeed();//�����ϴ��ٶ�
		std::string GetCpuGUID(); //��ȡcpu���к�
		int IsRunningGame(wchar_t * game);//�Ƿ�������Ϸ  1 ����  0 û������

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

