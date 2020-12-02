#pragma once
#include "HardWareReport.h"
#include "CurlReportCell.h"
#include <SystemInfoDll.h>
#include <string>
using namespace SystemWMISpace;
namespace HardWareNamespace {
	class HardWareInnerVbox : public HardWareReportInner
	{
	public:
		HardWareInnerVbox();
		~HardWareInnerVbox();
	public:
		virtual int Init();
		virtual int Uninit();
		virtual int Report(wchar_t * url);
		virtual const wchar_t* GetBindUser();
		virtual int SetBindUser(const wchar_t * reportUrl);
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
		int IsRunningGame(wchar_t * game);//�Ƿ�������Ϸ  1 ����  0 û������
	private:
		std::shared_ptr<SystemInfoDll> m_systemInfo;
		std::shared_ptr<CurlReportCell> m_curlReport;
	};
}

