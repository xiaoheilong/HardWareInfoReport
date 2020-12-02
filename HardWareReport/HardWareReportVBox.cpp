#include "HardWareReportVBox.h"
#include "Tools.h"
#include "HardWareInnerVbox.h"
#include <iostream>
namespace HardWareNamespace {


	HardWareReportVBox::HardWareReportVBox(): m_reportCell(NULL), m_reportUrl(L"") , m_thread(NULL), m_threadFlag(false)
	{

	}


	HardWareReportVBox::~HardWareReportVBox()
	{
		m_reportCell.reset();
		m_reportUrl = L"";
	}


	int HardWareReportVBox::Init(wchar_t * reportUrl, unsigned int interTime) {
		int ret = 0;
		ret = SetReportUrl(reportUrl);
		std::shared_ptr<HardWareReportInner> cell = std::make_shared<HardWareInnerVbox>();
		SetCell(cell);
		cell->Init();
		SetReportInterTime(interTime);
		///////////
		m_threadFlag = true;
		m_thread = std::make_shared<std::thread>( &HardWareReportVBox::Run, this);
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

	int HardWareReportVBox::Run() {
		
		while (m_threadFlag) {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_event.wait_for(lock ,std::chrono::milliseconds(m_reportInterTime));
			//////////////执行上报动作
			if (m_reportUrl.size()) {
				m_reportCell->Report(const_cast<wchar_t *>(m_reportUrl.c_str()));
				std::cout << "report status" << std::endl;
			}
		}
		return 0;
	}

	int HardWareReportVBox::UnInit() {
		if (m_reportCell) {
			m_reportCell->Uninit();
		}
		return 0;
	}

	void  HardWareReportVBox::SetReportInterTime(unsigned int time) {
		/*if (m_reportInterTime < 10000) {
			m_reportInterTime = 10000;
		}
		else */ {
			m_reportInterTime = time;
		}
		//std::unique_lock<std::mutex> lock(m_mutex);
		//m_event.notify_one();
	}

	unsigned int HardWareReportVBox::GetReportInterTime() {
		return m_reportInterTime;
	}

	std::shared_ptr<HardWareReportVBox> HardWareReportVBox::GetThis() {
		return shared_from_this();
	}

}
