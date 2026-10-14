// appLauncher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <PAL_platformheader.h>
//#include <winsock2.h> // replaced by PAL header
//#include <shellapi.h> // replaced by PAL header
#include <iostream>


bool connectToPort(int portNum)
{
	int retStatus;
	bool connected = false;

	SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketHandle != INVALID_SOCKET )
	{
		sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_port = htons(portNum);
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		if (connect(socketHandle, (struct sockaddr *) &address, sizeof(address)) != SOCKET_ERROR)
		{
			connected = true;
		}

		retStatus = closesocket(socketHandle);
	}

	return connected;
}

int _tmain(int argc, _TCHAR* argv[])
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 0 );

	err = WSAStartup( wVersionRequested, &wsaData );

	if (err == 0)
	{
		std::cout << "Checking for webServer service... " << std::endl;

		while (!connectToPort(80))
		{
			std::cout << ".";
			Sleep(200);
		}
		//web server is functional now

		std::cout << std::endl << "Checking for hobart service... " << std::endl;

		while (!connectToPort(6000))
		{
			std::cout << ".";
			Sleep(200);
		}

		Sleep (2000);
		//application is functional now
		std::string path ("\"c:/program files/hobart/access scale/scripts/launchVB.vbs\"");

		HINSTANCE res = ShellExecuteA (NULL,"open",path.c_str(),"","",SW_SHOW);
	}

	WSACleanup();
	return 0;
}

