#pragma once
#define USE_REPORT_DLL
#define INSTA_REPORT_FORMAT_TRANFER
#ifdef USE_REPORT_DLL
#ifdef INSTA_REPORT_FORMAT_TRANFER
#define  INSTALL_EXPORT  __declspec( dllexport )
#else
#define  INSTALL_EXPORT  __declspec(dllimport)
#endif
#else
#define INSTALL_EXPORT
#endif

#define EXTERN_C_MODEL  extern "C"
#include <iostream>
namespace HardWareNamespace {
	std::string WString2String(const std::wstring& ws);
	enum HardWareReportType {
		TYPE_HTTP = 0 ,
		TYPE_WEBSOCKET
	};
	class HardWareReportInner {
	public:
		HardWareReportInner();
		virtual ~HardWareReportInner();
	public:
		virtual int Init() = 0 ;
		virtual int Uninit() = 0;
		//virtual void  SendReportInterTime(unsigned int time) = 0;  // per ms 毫秒
		virtual int Report(wchar_t * url) = 0;
		virtual const wchar_t * GetBindUser() = 0;
		virtual int SetBindUser(const wchar_t * reportUrl) = 0;
	//private:
	//	virtual void Run() = 0; //上报任务执行线程函数
	};
	
	/////开发给外部的接口
	EXTERN_C_MODEL class INSTALL_EXPORT HardWareReport
	{
	public:
		HardWareReport();
		virtual ~HardWareReport();
	public:
		virtual int Init(wchar_t * reportUrl , unsigned int interTime) = 0;
		virtual const wchar_t * GetReportUrl() = 0;
		virtual int SetReportUrl(const wchar_t * reportUrl) = 0;
		virtual const wchar_t * GetBindUser() = 0;
		virtual int SetBindUser(const wchar_t * reportUrl) = 0;
		virtual void  SetReportInterTime(unsigned int time) = 0;  ///per ms   Range[10000 , *]
		virtual unsigned int GetReportInterTime() = 0;
		virtual int Run() = 0;
		virtual int Stop() = 0;
		virtual int UnInit() = 0;
	};
	
	EXTERN_C_MODEL INSTALL_EXPORT HardWareReport * CreateHardWareReport(HardWareReportType type);
	EXTERN_C_MODEL INSTALL_EXPORT void  ReleaseHardWareReport(HardWareReport * report);

}

