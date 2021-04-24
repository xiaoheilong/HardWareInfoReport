#include "HardWareReportVBox.h"
#include "Tools.h"
#include "HardWareInnerVbox.h"
#include "HardWareInnerVboxWbSocket.h"
#include <iostream>
#include <windows.h>
namespace HardWareNamespace {


	int NSSleep(int intel)  // ms
	{
		HANDLE hTimer = NULL;
		LARGE_INTEGER liDueTime;

		liDueTime.QuadPart = -1 * intel * 10000;

		// Create a waitable timer.
		hTimer = CreateWaitableTimer(NULL, TRUE, L"WaitableTimer");
		if (!hTimer)
		{
			printf("CreateWaitableTimer failed (%d)\n", GetLastError());
			return 1;
		}

		// Set a timer to wait for 10 seconds.
		if (!SetWaitableTimer(
			hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			printf("SetWaitableTimer failed (%d)\n", GetLastError());
			return 2;
		}

		// Wait for the timer.
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			printf("WaitForSingleObject failed (%d)\n", GetLastError());

		return 0;
	}


	HardWareReportVBox::HardWareReportVBox(HardWareReportType type): m_reportCell(NULL), m_reportUrl(L"") , m_thread(NULL), m_threadFlag(false),
		m_reportType(type)
	{

	}


	HardWareReportVBox::~HardWareReportVBox()
	{
		std::cout << "start close HardWareReportVBox!" << std::endl;
		m_reportCell.reset();
		m_reportUrl = L"";
		std::cout << "end close HardWareReportVBox!" << std::endl;
	}


	int HardWareReportVBox::Init(wchar_t * reportUrl, unsigned int interTime) {
		int ret = 0;
		ret = SetReportUrl(reportUrl);
		std::shared_ptr<HardWareReportInner> cell;
		switch (m_reportType) {
		case HardWareReportType::TYPE_HTTP: {
			cell = std::make_shared<HardWareInnerVbox>();
		}
			break;
		case HardWareReportType::TYPE_WEBSOCKET: {
			cell = std::make_shared<HardWareInnerVboxWbSocket>();
		}
			break;
		default: {

		}
			break;
		}
		SetCell(cell);
		cell->Init();
		SetReportInterTime(interTime);
		///////////
		m_threadFlag = true;
		m_thread = std::make_shared<std::thread>( &HardWareReportVBox::CallBack, this);
		m_thread->detach();
		return ret;
	}

	int HardWareReportVBox::SetReportUrl(const wchar_t * reportUrl) {
	/*	if (!JugeUrlIsValid(reportUrl)) {
			return -1;
		}*/
		m_reportUrl = reportUrl;
		return 0;
	}

	const wchar_t * HardWareReportVBox::GetReportUrl() {
		return  m_reportUrl.c_str();
	}

	int HardWareReportVBox::SetCell(std::shared_ptr<HardWareReportInner> cell) {
		m_reportCell = cell;
		return 0;
	}

	const wchar_t * HardWareReportVBox::GetBindUser() {
		if (m_reportCell) {
			return m_reportCell->GetBindUser();
		}

		return NULL;
	}

	int HardWareReportVBox::SetBindUser(const wchar_t * reportUrl) {
		if (!reportUrl) {
			return -1;
		}
		m_reportCell->SetBindUser(reportUrl);
		return 0;
	}

	void HardWareReportVBox::CallBack() {
		while (1) {
			//std::unique_lock<std::mutex> lock(m_mutex);
			//m_event.wait_for(lock, std::chrono::milliseconds(m_reportInterTime));
			//////////////执行上报动作
			NSSleep(m_reportInterTime);
			if (m_reportUrl.size() && m_threadFlag) {
				std::cout << "report status------------------------------" << std::flush<<std::endl;
				if (m_reportCell.get()) {
					m_reportCell->Report(const_cast<wchar_t *>(m_reportUrl.c_str()));
				}
				else {
					m_threadFlag = false;
					break;
				}
			}
		}
	}

	bool HardWareReportVBox::IsValid() {
		return m_reportCell.get();
	}

	int HardWareReportVBox::Run() {
		{
			//std::unique_lock<std::mutex> lock(m_mutex);
			m_threadFlag = true;
		}
		//m_event.notify_one();
		return 0;
	}

	int HardWareReportVBox::Stop() {
		{
			//std::unique_lock<std::mutex> lock(m_mutex);
			m_threadFlag = false;
		}
		//m_event.notify_one();
		return 0;
	}

	int HardWareReportVBox::UnInit() {
		if (m_reportCell) {
			m_reportCell->Uninit();
		}
		return 0;
	}

	void  HardWareReportVBox::SetReportInterTime(unsigned int time) {
		if (m_reportInterTime < 10000) {
			m_reportInterTime = 10000;
		}
		else  {
			m_reportInterTime = time;
		}
	}

	unsigned int HardWareReportVBox::GetReportInterTime() {
		return m_reportInterTime;
	}

	std::shared_ptr<HardWareReportVBox> HardWareReportVBox::GetThis() {
		return shared_from_this();
	}

}
