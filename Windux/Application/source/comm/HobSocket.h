/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOCKET_H
#define SOCKET_H



#include "PAL_platformheader.h"

#include "CommMechanism.h"

#include "evolution.h"
//#include <winsock2.h> // replaced by PAL header


/**
 * This is an abstraction that maps to the native socket lib.
 *
 * All sockets we will be using in the Dominator will be connection-based sockets.
 * Even though they are faster, we will not use any connectionless (datagram)
 * sockets because the packets are not guaranteed to arrive in the appropriate
 * sequence, and they are less reliable.
 */
class Socket : public CommMechanism
{

    public:

        /* Macros for the return of functions (RetStatus). */
        typedef enum {
            STATUS_INVALID,

            STATUS_PARAM,						/* Parameter has wrong value */
            STATUS_NOMEMORY,					/* Not enough memory */
            STATUS_CANTOPENFILE,        	/* Unable to open a file */

            STATUS_TRUE,						/* The result is TRUE */
            STATUS_FALSE,						/* The result is FALSE */
            STATUS_OK,							/* Everything went OK */
            STATUS_DONT,						/* Something went wrong */
            STATUS_ERROR_CONTINUE,			/* Non-fatal error occurred. */
            STATUS_WARNING,					/* Unexpected but controlled situation */
            STATUS_NOTFOUND,					/* Could not find it */
            STATUS_NOTHINGTODO,				/* Nothing was done: nothing to do */
            STATUS_DONEPROCESSING,			/* Finished doing some task */
            STATUS_ENDOFLINE,					/* End of line reached */
            STATUS_ENDOFPARAGRAPH,			/* End of paragraph reached */
            STATUS_EOF,							/* End of file reached */

            STATUS_SHUTDOWN,					/* The result was to shutdown */
            STATUS_EVENTNOTPROCESSED,		/* An event was not processed */

            STATUS_IO_PENDING,				/* Used for overlapped Win32 comm status */
            STATUS_IO_INCOMPLETE,			/* Used for overlapped Win32 comm status */

            STATUS_WRONGRESOURCETYPE,   	/* The resource was not the type expected */

            STATUS_PNGLIBERROR,       	 	/* There was an error in the PNG library */
            STATUS_PNGINVALIDDATA,      	/* The png data is not valid */

            STATUS_STRINGMAXSIZETOOLONG,	/* String's max size is too long (>25000) */
            STATUS_STRINGSIZETOOLONG,		/* String's size is too long (>25000 or >max) */

            STATUS_UNKNOWN_MESSAGE,			/* The message type is unknown */
            STATUS_MESSAGE_SENT,				/* The message was sent */

            STATUS_POLLING,					/* The result was not achieved yet; starting
                                                                   to poll until result is achieved. */
            STATUS_SERVER_SUSPENDED,		/* The server (serial port) is suspended */

            STATUS_LAST
        } RetStatus;

        // Allows the message length indicator to contain 6 digits.
        static const int BUFFER_LENGTH_INDICATOR_LENGTH = 6;

        Socket();

        ~Socket();

        // From the CommMechanism API ...
        int cmSend(const char *msg, int length);
        //int cmSend(const HobString &msg) = 0;
        int cmReceive(char *msgbuf);
        int cmReceive(HobString &msgbuf);
        int cmNumMsgs();
        void cmClose();

        Socket::RetStatus handleError(int error = 0);

        bool isValid();

        void sConnect();
        void sConnect2();
        bool sConnect3();

		void ensureValid();

        /* Additional receive method to allow a Socket that doesn't have a reader/writer
         * to still receive data.
         * maxLength will be the maximum number of bytes read,
         * (less could be read if there aren't maxLength bytes in the socket).
         *
         * If maxLength == 0, I will keep reading until I get EOF (no more bytes in the socket),
         * in which case, the buffer provided must contain enough space to hold the maximum number of bytes
         * defined in CMReaderWriter::MAX_BUFFER_LENGTH.
         *
         * The actual number of bytes read is returned.
         */
        int receive(char *buff, int maxLength, bool peek = false);
        int rawSend(char *buff, int bytesToSend);

        void clearInputBuffer();

        std::string getAddress();
        void setAddress(const char *addr);
        STATUS setDnsName(const std::string& dnsOrIp);

        /**
         * This 'property' will be configurable, and so this class will have to access
         * the Configuration class to obtain the well known port.
         */
#if defined(LINUX_FIX)
        int getPort();
#else
        const int getPort();
#endif
        void setPort(const int port);

        int getSocketHandle();

        const char *toString();

        void clone(int newsocketHandle, struct sockaddr_in *newAddress);

        void setNonBlockingIO();

        bool isRetryingEnabled();
        void setRetryingEnabled(bool retryingEnabled);
        std::string getSocketErrorText(int error) const;

		int getSendFlags() const { return sendFlags; }
		int setSendFlags(int flags) { sendFlags |= flags; return sendFlags; }
		int unsetSendFlags(int flags) { sendFlags &= ~flags; return sendFlags; }

		int getRecvFlags() const { return recvFlags; }
		int setRecvFlags(int flags) { recvFlags |= flags; return recvFlags; }
		int unsetRecvFlags(int flags) { recvFlags &= ~flags; return recvFlags; }

    protected:

        struct sockaddr_in address;

        int socketAddressSize;

        PAL_SOCKET socketHandle;

    private:

        HobString toStringData;

        bool retryingEnabled;

		int sendFlags;
		int recvFlags;

        void determineReaderWriter();
        bool nonBlockingIO;
        char socketAddress[INET_ADDR_LEN];

#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
};

#endif //SOCKET_H
