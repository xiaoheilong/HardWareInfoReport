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
		int IsRunningGame(wchar_t * game);//是否运行游戏  1 运行  0 没有运行
	private:
		std::shared_ptr<SystemInfoDll> m_systemInfo;
		std::shared_ptr<CurlReportCell> m_curlReport;
	};
}

