#include "CurlReportCell.h"
#include "Tools.h"
#include <cstdlib>
#include <cerrno>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <json/config.h>
#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>
#include <iostream>
namespace HardWareNamespace {

	CurlReportCell::CurlReportCell():m_reportUrl(""), m_readDataBuffer(NULL)
	{
		cURLpp::initialize(CURL_GLOBAL_ALL);
	}


	CurlReportCell::~CurlReportCell()
	{
		m_reportUrl.clear();
		if (m_readDataBuffer) {
			delete[] m_readDataBuffer;
			m_readDataBuffer = NULL;
		}
		cURLpp::terminate();
	}
	char *data1 = NULL;
	size_t readData(char *buffer, size_t size, size_t nitems)
	{
		strncpy(buffer, data1, size * nitems);
		if (data1) {
			std::cout << "recv:" << data1 << std::endl;
		}
		return size * nitems;
	}

	int CurlReportCell::Report(std::string reportData) {
		std::cout << "enter Report step1!url:"<< m_reportUrl << std::endl;
		if (reportData.size() < 0 || m_reportUrl.size() < 0) {
			std::cout << "enter Report step2!" << std::endl;
			return -1;
		}
		std::cout << "enter Report step3!" << std::endl;
		/////////////½âÎöreportData json´®¸ñÊ½

		Json::Value root;
		Json::CharReaderBuilder readBuilder;
		std::unique_ptr<Json::CharReader> const reader(readBuilder.newCharReader());
		//Json::Reader reader;
		JSONCPP_STRING errs;
		bool ret = reader->parse(reportData.c_str(), reportData.c_str() + reportData.size() , &root , &errs);
		std::cout << "enter Report step4!" << std::endl;
		if (!ret || !errs.empty()) {
			std::cout << "enter Report step4!" << std::endl;
			return -1;
		}
		if (!root.isMember("header") || !root.isMember("body")) {
			std::cout << "enter Report step6!" << std::endl;
			return -1;
		}
		//std::cout << "header:" << root["header"] << "  body:" << root["body"] << std::endl;
		//Json::StyledWriter styled_writer;
		std::cout << "enter Report step7!" << std::endl;


		Json::StreamWriterBuilder styled_write;
		styled_write.settings_["indentation"] = "";
		styled_write.settings_["precision"] = 16;
		styled_write.settings_["emitUTF8"] = true;
		std::unique_ptr<Json::StreamWriter> writer(styled_write.newStreamWriter());
		std::ostringstream os;
		writer->write(root["header"],&os);
		std::string header = os.str();
		os.str("");
		writer->write(root["body"], &os);
		std::string body = os.str();
		///////////
		try {
			curlpp::Cleanup cleaner;
			curlpp::Easy request;

			request.setOpt(new curlpp::options::Url(m_reportUrl));
			request.setOpt(new curlpp::options::Verbose(true));
			request.setOpt(new curlpp::options::Encoding("GBK"));
			// Forms takes ownership of pointers!
			std::list<std::string> headers;
			headers.push_back("Content-Type: multipart/form-data;charset=GBK");
			////sprintf(buf, "Content-Length: %d", size);
			//headers.push_back(buf);
			request.setOpt(new curlpp::options::HttpHeader(headers));


			curlpp::Forms formParts;

			formParts.push_back(new curlpp::FormParts::Content("header", header.c_str()));
			formParts.push_back(new curlpp::FormParts::Content("body", body.c_str()));
			//std::function<size_t(char *, size_t, size_t)> readDataFunc = std::bind(&CurlReportCell::ReadData, this, std::placeholders::_1, std::placeholders::_2, \
				std::placeholders::_3);
			std::cout << "enter Report step8!" << std::endl;
			request.setOpt(new curlpp::options::ReadFunction(curlpp::types::ReadFunctionFunctor(readData)));
			request.setOpt(new curlpp::options::HttpPost(formParts));
			//curlpp::FormPart content = formParts.back(;
			std::cout << "request once" << std::endl;
			request.perform();
			std::cout << "enter Report step9!" << std::endl;
		}catch (curlpp::LogicError & e) {
		std::cout << e.what() << std::endl;
	}
	catch (curlpp::RuntimeError & e) {
		std::cout << e.what() << std::endl;
	}
		return 0;
	}

	void CurlReportCell::SetReportUrl(std::string reportUrl) {
		m_reportUrl = reportUrl;
	}

	std::string CurlReportCell::GetReportUrl() {
		return m_reportUrl;
	}

	bool CurlReportCell::UrlIsValid(std::string url) {
		if (url.empty()) {
			return false;
		}
		m_reportUrl = url;
	}

	size_t CurlReportCell::ReadData(char *buffer, size_t size, size_t nitems) {
		if (m_readDataBuffer) {
			delete[] m_readDataBuffer;
			m_readDataBuffer = NULL;
		}
		m_readDataBuffer = new char[size * nitems + 1];
		strncpy(buffer,m_readDataBuffer, size * nitems);
		std::cout<<"recv:" << m_readDataBuffer << std::endl;
		return size * nitems;
	}

}
