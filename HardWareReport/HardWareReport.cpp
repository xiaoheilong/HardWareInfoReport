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

	HardWareReport * CreateHardWareReport() {
		return new HardWareReportVBox();
	}

	void  ReleaseHardWareReport(HardWareReport * report) {
		if (report) {
			delete report;
			report = NULL;
		}
	}
}
