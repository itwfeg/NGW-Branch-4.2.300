/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ServerSocket.h"
#include "Configuration.h"
#include "Integer.h"


ServerSocket::ServerSocket()
{
	bNonBlocking = false;
	init();
}

ServerSocket::ServerSocket(int port)
{
	init();
	setPort(port);
	/* bind socket to local address */
	sBind();
	/* create queue for client connection requests */
	sListen();
}

void ServerSocket::init()
{
	address.sin_addr.s_addr = htonl(INADDR_ANY);
}


void ServerSocket::sBind()
{
	if (isValid())
	{
		if (::bind(socketHandle, (struct sockaddr *) &address, socketAddressSize) == ERROR_CODE) // MSVC2010
		{
			perror ("bind");
			cmClose(); 
		}
	}
}

void ServerSocket::sListen()
{
	if (isValid())
	{
		if (listen(socketHandle, MAX_NUMBER_OF_CONNECTIONS) == ERROR_CODE) 
		{ 
			perror("listen"); 
			cmClose(); 
		}
	}
}

void ServerSocket::setNonBlocking()
{
	// turn on non-blocking i/o
	int data = TRUE;
#ifdef LINUX_FIX
	int ret = PAL_absclass::PAL_ioctlsocket(socketHandle, PAL_FIONBIO, (u_long *)&data);
#else
	int ret = ioctlsocket(socketHandle, FIONBIO, (u_long *)&data);
#endif      // LINUX_FIX

	bNonBlocking = true;
	HobString hsdata(Integer::toString(data));
	Debug::trace("Socket", HobString("set blocking io: FIONBIO - ret=") + HobString(Integer::toString(ret)) +
												HobString(" data=") + hsdata);
}


Socket *ServerSocket::sAccept()
{
	int connectedSocketHandle = -1;
#ifdef LINUX_FIX
#if defined ( __linux__ )
	socklen_t sizer = sizeof(struct sockaddr_in);
#else
	int sizer = sizeof(struct sockaddr_in);
#endif      // __linux__
#else
	int sizer = sizeof(struct sockaddr_in);
#endif      // LINUX_FIX

	struct sockaddr_in clientAddress;

	if (!isValid())
		return (Socket *)NULL;

	Debug::trace("Socket", HobString("ServerSocket.sAccept - Listening for connections on socket ") + 
									HobString(getAddress()) + HobString(Integer::toString(getPort())));

	connectedSocketHandle = (int) accept(socketHandle, (struct sockaddr *) &clientAddress, &sizer);
	if (connectedSocketHandle == ERROR_CODE)
	{
		if (!bNonBlocking)
		{
			perror("accept"); 
			cmClose();
		}
		return (Socket *)NULL;
	}
	else
	{
		Socket *newClientSocket = new Socket();
		newClientSocket->clone(connectedSocketHandle, &clientAddress);
		//newClientSocket->setAddress(inet_ntoa(clientAddress.sin_addr.s_addr));
		//newClientSocket->setPort(ntohs(clientAddress.sin_port));

		Debug::trace("Socket", HobString("ServerSocket.sAccept - Accepted client connection on socket:  ") + 
										HobString(newClientSocket->toString()));
		return newClientSocket;
	}

}
