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
		float getCpuUsagePercentage(); //cpu������  not /100
		float getRamPercentage();  //�ڴ�������  not /100
		float getNvidaGPUPercentage();//��ȡӢΰ���Կ�������  not /100
		float getDiskPercentage();//��ȡ���������� not /100
		float getNetDelay(wchar_t* goalIp);//��ȡ�����ӳ�  ,
		int getDeviceId(wchar_t * deviceID, unsigned int len);//��ȡ�豸������  guid
		int getDeviceMac(wchar_t * mac , unsigned int len);//mac ��ַ
		int getLocalLanIp(wchar_t * lanIp , unsigned int len);//��ȡ����ip
		int getInternetIp(wchar_t * internetIp, unsigned int len);//��ȡ����
		int getRegion(wchar_t * region, unsigned int len);//����
		int getNetworkOperator(wchar_t * operator1, unsigned int len);//������Ӫ��
		int getNetworkUploadSpeed(wchar_t * speed, unsigned int len);//�����ϴ��ٶ�
		int getNetworkDownloadSpeed(wchar_t * speed, unsigned int len);//�����ϴ��ٶ�
		int getCpuGuid(wchar_t * content , unsigned int len);//��ȡcpu���к�
		int setBindUser(const wchar_t * user);
		const wchar_t *  getBindUser();//���û�
		int isRunningGame(wchar_t * game);//�Ƿ�������Ϸ  1 ����  0 û������
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

