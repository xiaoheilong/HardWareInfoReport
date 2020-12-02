#pragma once
namespace Networkspace {
	int getLocalLanIp(wchar_t * lanIp, unsigned int len);
	int getIpAddress(wchar_t *ipBuff, unsigned int len);
	int getLocalMacAddress(wchar_t * macAdress , unsigned int len);
}

