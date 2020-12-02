#pragma once
#include<string>
#include <functional>
namespace HardWareNamespace {
	class CurlReportCell
	{
	public:
		CurlReportCell();
		~CurlReportCell();
	public:
		int Report(std::string  reportData);
		void SetReportUrl(std::string reportUrl);
		std::string GetReportUrl();
		bool UrlIsValid(std::string url);
		size_t ReadData(char *buffer, size_t size, size_t nitems);
	private:
		std::string m_reportUrl;
		char * m_readDataBuffer;
	};
}

