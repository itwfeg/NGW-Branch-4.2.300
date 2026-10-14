#include "PAL_platformheader.h"

#if defined ( __linux__ )
#include <netdb.h>
#endif

#include "utilunicode.h"
#include "ErrorHandling.h"

#include "Ping.h"


bool Hobart::CPing::Ping (std::string & i_Host, bool i_PUName, int i_Attempts)
{
    LPHOSTENT lpHost;
    struct    sockaddr_in saDest;
    struct    sockaddr_in saSrc;
    u_char    cTTL;
    int       nLoop;
    int       nRet;

    memset( &saDest, 0, sizeof(struct sockaddr_in) );
    memset( &saSrc, 0, sizeof(struct sockaddr_in) );

    try
    {
#if !defined ( __linux__ )
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 0 );

        err = WSAStartup( wVersionRequested, &wsaData );
        Assert(err == 0, Util::CNamedException( L"Socket Initialize error" ) );

        Assert(( LOBYTE( wsaData.wVersion ) == 2 ||HIBYTE( wsaData.wVersion ) == 0 ), Util::CNamedException( L"Socket Version Error" ) );
#endif      //!__linux__

        // Create a Raw socket
        PAL_SOCKET rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        Assert (rawSocket != SOCKET_ERROR, Util::CNamedException (L"Socket Error"));

        if(i_PUName)
        {
            // Lookup host
            lpHost = gethostbyname(i_Host.c_str());
            if (lpHost == NULL)
            {
                fprintf( stderr, "\nHost not found: %s\n", i_Host.c_str() );
                nRet=PAL_absclass::PAL_closesocket(rawSocket);
                Assert (nRet == 0, Util::CNamedException (L"Socket Error"));
                return false;
            }

            saDest.sin_addr.s_addr = *((u_long FAR *) (lpHost->h_addr));
        }
        else
        {
            saDest.sin_addr.s_addr = inet_addr(i_Host.c_str());
        }

        // Setup destination socket address
        saDest.sin_family = AF_INET;
        saDest.sin_port = 0;

        // Ping multiple times
        for (nLoop = 0; nLoop < i_Attempts; nLoop++)
        {
            // Send ICMP echo request
            nRet = SendEchoRequest(rawSocket, &saDest);
			
			if (nRet)
			{
		        // Use select() to wait for data to be received
		        nRet = WaitForEchoReply(rawSocket);
				
				if (nRet == SOCKET_ERROR)
					nRet = 0;							        
			}
            if (!nRet)
            {
                nRet=PAL_absclass::PAL_closesocket(rawSocket);
                //Assert (nRet == 0, Util::CNamedException (L"Socket Error"));
                return false;
            }

            // Receive reply
            RecvEchoReply(rawSocket, &saSrc, &cTTL);

        }
        nRet=PAL_absclass::PAL_closesocket(rawSocket);
        Assert (nRet == 0, Util::CNamedException (L"Socket Error"));

        return true;
    }
    catch (...)
    {
        LogCatchAndThrow;
    }
}


int Hobart::CPing::SendEchoRequest (PAL_SOCKET s,LPSOCKADDR_IN lpstToAddr) 
{
    try
    {
        ECHOREQUEST echoReq;
        memset( &echoReq, 0, sizeof(ECHOREQUEST) );

        int	nId = 1;
        int	nSeq = 1;
        int nRet;

        // Fill in echo request
        echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
        echoReq.icmpHdr.Code		= 0;
        echoReq.icmpHdr.Checksum	= 0;
        echoReq.icmpHdr.ID			= (u_short)nId++;
        echoReq.icmpHdr.Seq			= (u_short)nSeq++;

        // Fill in some data to send
        for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
            echoReq.cData[nRet] = ' '+ (char)nRet;

        // Save tick count when sent
        echoReq.dwTime				= PAL_absclass::PAL_getTickCount();

        // Put data in packet and compute checksum
        echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

        // Send the echo request  								  
        nRet = sendto(s,						/* socket */
                (PCHAR)&echoReq,			/* buffer */
                sizeof(ECHOREQUEST),
                0,							/* flags */
                (LPSOCKADDR)lpstToAddr,	/* destination */
                sizeof(SOCKADDR_IN));		/* address length */

        if (nRet ==SOCKET_ERROR)
		{
			nRet = 0;
		}		

        return (nRet);
    }
    catch (...)
    {
        LogCatchAndThrow;
    }
}


// RecvEchoReply()
// 
DWORD Hobart::CPing::RecvEchoReply(PAL_SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL) 
{
    try
    {
        ECHOREPLY echoReply;
        int nRet;

#if defined ( __linux__ )
        socklen_t nAddrLen = sizeof(struct sockaddr_in);
#else
        int nAddrLen = sizeof(struct sockaddr_in);
#endif      // __linux__

        // Receive the echo reply	
        nRet = recvfrom(s,					// socket
                (PCHAR)&echoReply,	// buffer
                sizeof(ECHOREPLY),	// size of buffer
                0,					// flags
                (LPSOCKADDR)lpsaFrom,	// From address
                &nAddrLen);			// pointer to address len

        Assert (nRet != SOCKET_ERROR, Util::CNamedException (L"Socket Error"));

        // return time sent and IP TTL
        *pTTL = echoReply.ipHdr.TTL;
        return(echoReply.echoRequest.dwTime);   
    }
    catch (...)
    {
        LogCatchAndThrow;
    }
}


int Hobart::CPing::WaitForEchoReply (PAL_SOCKET s)
{
    struct timeval Timeout;
    fd_set readfds;

#if defined ( __linux__ )
    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
#else
    readfds.fd_count = 1;
    readfds.fd_array[0] = s;
#endif      // __linux__

    Timeout.tv_sec = 1;
    Timeout.tv_usec = 0;

#if defined ( __linux__ )
	return(select(s+1, &readfds, NULL, NULL, &Timeout));
#else
    return(select(1, &readfds, NULL, NULL, &Timeout));
#endif
}


//
// Mike Muuss' in_cksum() function
// and his comments from the original
// ping program
//
// * Author -
// *	Mike Muuss
// *	U. S. Army Ballistic Research Laboratory
// *	December, 1983

/*
 *			I N _ C K S U M
 *
 * Checksum routine for Internet Protocol family headers (C Version)
 *
 */
u_short Hobart::CPing::in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = (u_short)~sum;				/* truncate to 16 bits */
	return (answer);
}
