#pragma once
#include "HardWareReport.h"
#include <thread>
#include <mutex>
#include <condition_variable>
namespace HardWareNamespace {
	class HardWareReportVBox :public std::enable_shared_from_this<HardWareReportVBox> ,public HardWareReport
	{
	public:
		HardWareReportVBox(HardWareReportType type);
		~HardWareReportVBox();
	public:
		virtual int Init(wchar_t * reportUrl, unsigned int interTime);
		virtual int SetReportUrl(const wchar_t * reportUrl);
		virtual const wchar_t * GetReportUrl();
		virtual const wchar_t * GetBindUser() ;
		virtual int SetBindUser(const wchar_t * reportUrl) ;
		virtual int Run();
		virtual int Stop();
		virtual int UnInit();
		virtual void  SetReportInterTime(unsigned int time);
		virtual unsigned int GetReportInterTime();
		virtual std::shared_ptr<HardWareReportVBox> GetThis();
		virtual bool IsValid();
	protected:
		virtual int SetCell(std::shared_ptr<HardWareReportInner> cell);
	private:
		void CallBack();
	private:
		std::shared_ptr<HardWareReportInner> m_reportCell;
		std::wstring  m_reportUrl;
		//std::mutex m_mutex;
		//std::condition_variable m_event;
		std::shared_ptr<std::thread> m_thread;
		bool m_threadFlag;
		unsigned int m_reportInterTime;// per ms
		HardWareReportType m_reportType;
	};
}

