#include "HardWareReport.h"
#include "HardWareReportVBox.h"
namespace HardWareNamespace {
	////////////////////////
	HardWareReportInner::HardWareReportInner() {

	
	}

	HardWareReportInner::~HardWareReportInner() {

	}

	///////////////////
	HardWareReport::HardWareReport()
	{
	}


	HardWareReport::~HardWareReport()
	{
	}
	//////////

	HardWareReport * CreateHardWareReport(HardWareReportType type) {
		return new HardWareReportVBox(type);
	}

	void  ReleaseHardWareReport(HardWareReport * report) {
		if (report) {
			delete report;
			report = NULL;
		}
	}
}
