
#ifndef HOBART_Ping_h
#define HOBART_Ping_h
//
// Ping.h
//

/**************************************************************************
 ****  DADF MODULE HEADER   ***


TYPE:			Function

PURPOSE:		To ping without shelling out to MS-DOS/CMD.

FUNCTION:		Interrogates the existence of a selected PU.

INTERFACES:		see below

RESOURCES:		Winsock2

REFERENCES:		None.

PROCESSING:		Create a RAW_PAL_SOCKET, send ICMP data, wait for the result..time-out
if PU is not available.

DATA:			See 'DATA:' in the class header below.

 ****  END MODULE HEADER   ***
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#if defined( _WIN32 )
#include "winsock2.h"
#endif

#pragma pack(1)

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8


// IP Header -- RFC 791
typedef struct tagIPHDR
{
    u_char VIHL;			// Version and IHL
    u_char TOS;			// Type Of Service
    short TotLen;			// Total Length
    short ID;			// Identification
    short FlagOff;		// Flags and Fragment Offset
    u_char TTL;			// Time To Live
    u_char Protocol;		// Protocol
    u_short Checksum;		// Checksum
    struct in_addr iaSrc;		// Internet Address - Source
    struct in_addr iaDst;		// Internet Address - Destination
} IPHDR, *PIPHDR;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
    u_char Type;			// Type
    u_char Code;			// Code
    u_short Checksum;		// Checksum
    u_short ID;			// Identification
    u_short Seq;			// Sequence
    CHAR Data;			// Data
} ICMPHDR, *PICMPHDR;


#define REQ_DATASIZE 32		// Echo Request Data size

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
    ICMPHDR icmpHdr;
    DWORD dwTime;
    CHAR cData[REQ_DATASIZE];
} ECHOREQUEST, *PECHOREQUEST;


// ICMP Echo Reply
typedef struct tagECHOREPLY
{
    IPHDR ipHdr;
    ECHOREQUEST echoRequest;
    CHAR cFiller[256];
} ECHOREPLY, *PECHOREPLY;



#pragma pack()

namespace Hobart
{

    class CPing
    {
        private:

            // Description              Fill in echo request header and send to destination.
            // Arguments:
            //          s                       A socket.
            //          lpstToAddr:     Address of target.                      
            // Return:                  Returns count of bytes sent.
            static int SendEchoRequest (PAL_SOCKET s, LPSOCKADDR_IN lpstToAddr);

            // Description              Receive incoming data and parse out fields.
            // Arguments:
            //          s                       A socket.
            //          lpsaFrom:       Address of target.      
            //          pTTL:           Pointer to a TTL structure.
            // Return:                  Return time sent.
            static DWORD RecvEchoReply (PAL_SOCKET s, LPSOCKADDR_IN lpsaFrom,
                    u_char * pTTL);

            // Description              Waits for the result.
            // Arguments:
            //          s                       A socket.               
            // Return:                  Returns 0 if timed-out.
            static int WaitForEchoReply (PAL_SOCKET s);


            // Description              Calculate check-sum.
            // Arguments:
            //          addr            Where the data is..             
            //          len:            Length of data.
            //  Returns:                The checksum.
            static u_short in_cksum (u_short * addr, int len);


        public:
            // Description              Determine of the selected Host is available.
            // Arguments:
            //          i_Host          The host.
            //          i_PUName    set to true if the i_Host is the name of the machine
            //                                  otherwise the function assumes that i_Host specifies the ipaddress
            //      i_Attempts  number of times it will ping i_Host
            //  Returns:                true -> found it; false -> nowhere to be seen.
            static bool Ping (std::string & i_Host, bool i_PUName =
                    false, int i_Attempts = 4);
    };

};

#endif
