/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H


#include "evolution.h"
#include "HobSocket.h"


class ServerSocket : public Socket
{
	public:

		static const int MAX_NUMBER_OF_CONNECTIONS = 100;


		ServerSocket();
		ServerSocket(int port);

		void sBind();

		void sListen();

		void setNonBlocking();

		Socket * sAccept();


	private:

		bool bNonBlocking;
		void init();

};

#endif //SERVERSOCKET_H


