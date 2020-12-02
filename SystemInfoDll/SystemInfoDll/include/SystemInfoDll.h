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

#define EXTERN_C_MODEL  extern "C"
namespace SystemWMISpace {
	EXTERN_C_MODEL class INSTALL_EXPORT SystemInfoDll
	{
	public:
		SystemInfoDll();
		~SystemInfoDll();
	public:
		int init();
		unsigned int getCpuUsagePercentage(); //cpu������  not /100
		float getRamPercentage();  //�ڴ�������  not /100
		float getNvidaGPUPercentage();//��ȡӢΰ���Կ�������  not /100
		float getDiskPercentage();//��ȡ���������� not /100
		float getNetDelay(wchar_t* goalIp);//��ȡ�����ӳ�  ,
	private:
		void * m_wmiWbemInfo;
	};
}

