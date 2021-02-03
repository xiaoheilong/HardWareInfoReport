#pragma once
#define USE_WMI_DLL
#define INSTA_WMI_FORMAT_TRANFER
#ifdef USE_WMI_DLL
	#ifdef INSTA_WMI_FORMAT_TRANFER
		#define  INSTALL_EXPORT  __declspec( dllexport )
	#else
		#define  INSTALL_EXPORT  __declspec(dllimport)
	#endif
#else
#define INSTALL_EXPORT
#endif
#include<string>
#define EXTERN_C_MODEL  extern "C"
namespace SystemWMISpace {
	EXTERN_C_MODEL class INSTALL_EXPORT SystemInfoDll
	{
	public:
		SystemInfoDll();
		~SystemInfoDll();
	public:
		int init();
		void unInit();
		float getCpuUsagePercentage(); //cpu利用率  not /100
		float getRamPercentage();  //内存利用率  not /100
		float getNvidaGPUPercentage();//获取英伟达显卡利用率  not /100
		float getDiskPercentage();//获取磁盘利用率 not /100
		float getNetDelay(wchar_t* goalIp);//获取网络延迟  ,
		int getDeviceId(wchar_t * deviceID, unsigned int len);//获取设备机器码  guid
		int getDeviceMac(wchar_t * mac , unsigned int len);//mac 地址
		int getLocalLanIp(wchar_t * lanIp , unsigned int len);//获取内网ip
		int getInternetIp(wchar_t * internetIp, unsigned int len);//获取外网
		int getRegion(wchar_t * region, unsigned int len);//地区
		int getNetworkOperator(wchar_t * operator1, unsigned int len);//网络运营商
		int getNetworkUploadSpeed(wchar_t * speed, unsigned int len);//网络上传速度
		int getNetworkDownloadSpeed(wchar_t * speed, unsigned int len);//网络上传速度
		int getCpuGuid(wchar_t * content , unsigned int len);//获取cpu序列号
		int setBindUser(const wchar_t * user);
		const wchar_t *  getBindUser();//绑定用户
		int isRunningGame(wchar_t * game);//是否运行游戏  1 运行  0 没有运行
	private:
		void * m_wmiWbemInfo;
		void * m_mutex;
		wchar_t * m_userName;
		std::wstring m_deviceId;
		void * m_pModule;
		void * m_getCpuUsagePercentage;
		void * m_getRamPercentage;
		void * m_getDiskPercentage;
		void * m_getInternetIp;
		void * m_getRegion;
		//void * getRegion;
		void * m_getNetworkOperator;
		void * m_getNetworkUploadSpeed;
		void * m_getNetworkDownloadSpeed;
		void * m_getCpuGuid;

	};
}

