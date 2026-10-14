/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "HobSocket.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "bootParameters.h"
#include "Configuration.h"
#include <time.h>

#define MAX_RECV_TIME 60	// in seconds
#define MAX_SEND_TIME 60	// in seconds
#define SEND_RETRY_DELAY 100		// in milliseconds
#define INVALID_SOCKET_HANDLE -1

#ifdef _WIN32
#pragma warning(disable : 4127)
#endif //_WIN32

#ifdef LINUX_FIX
#include <signal.h>
#if defined( __linux__)
#include <poll.h>
#endif
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Socket::_dataType_ = 0;
std::string Socket::_dataTypeName_("Socket"); 
#endif

    Socket::Socket()
: socketHandle((PAL_SOCKET)INVALID_SOCKET_HANDLE)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

#if defined ( __linux__ )
    sendFlags = MSG_NOSIGNAL;
#else
	sendFlags = 0;
#endif

	recvFlags = 0;

    socketAddressSize = sizeof (struct sockaddr_in);
    memset((char *)&address, 0, socketAddressSize); 
    address.sin_family = AF_INET;

    if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == ERROR_CODE)
        socketHandle = (PAL_SOCKET)INVALID_SOCKET_HANDLE;

    retryingEnabled = true;

    // In CommMechanism
    setMyReaderWriter((CMReaderWriter *)NULL);
    setMode(CommMechanism::MODE_INPUT | CommMechanism::MODE_OUTPUT);
    setSendEmptyMessage(false); // FT-634

    memset(socketAddress, '\0', INET_ADDR_LEN);
    nonBlockingIO = false;
}

Socket::~Socket()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

    if (myReaderWriter)
        delete myReaderWriter;
}


Socket::RetStatus Socket::handleError(int error)
{
    RetStatus retVal = STATUS_DONT;

    if ( error == 0 )
        error = PAL_absclass::PAL_wsaGetLastError();

    HobString debugMsg = "Socket::handleError() - ";
    switch ( error )
    {
        case WSAEWOULDBLOCK:
            debugMsg = "";
            retVal = STATUS_IO_PENDING;
            break;
        case WSAEADDRINUSE:
            debugMsg.append("The specified address is already in use.");
            break;
        case WSAEADDRNOTAVAIL:
            debugMsg.append("The specified address is not available from the local machine.");
            break;
        case WSAECONNREFUSED:
            debugMsg.append("The attempt to connect was forcefully rejected.");
            break;
        case WSAEDESTADDRREQ:
            debugMsg.append("A destination address is required.");
            break;
        case WSAEFAULT:
            debugMsg.append("Unable to allocate resources or parameters are not part of user address space.");
            break;
        case WSAEINPROGRESS:
            debugMsg.append("A blocking Windows Sockets operation is in progress.");
            break;
        case WSAEINTR:
            debugMsg.append("The blocking call was cancelled.");
            break;
        case WSAEINVAL:
            debugMsg.append("An invalid parameter was passed to a Windows Sockets operation.");
            break;
        case WSAENETDOWN:
            debugMsg.append("The Windows Sockets implementation has detected that the network subsystem has failed.");
            break;
        case WSAENETRESET:
            debugMsg.append("The connection must be reset because the Windows Sockets implementation dropped it.");
            break;
        case WSAENETUNREACH:
            debugMsg.append("The network can't be reached from this host at this time.");
            break;
        case WSAENOBUFS:
            debugMsg.append("No buffer space is available or buffer deadlock.");
            break;
        case WSAENOTCONN:
            debugMsg.append("The socket is not connected.");
            break;
        case WSAENOTSOCK:
            debugMsg.append("The descriptor passed is not a socket.");
            break;
        case WSAESHUTDOWN:
            debugMsg.append("The socket has been shutdown.");
            break;
        case WSAETIMEDOUT:
            debugMsg.append("Attempt to connect timed out without establishing a connection.");
            break;
        case WSANOTINITIALISED:
            debugMsg.append("The Windows Sockets DLL has not been initialized successfully.");
            break;
        case WSASYSNOTREADY:
            debugMsg.append("The underlying network subsystem is not ready for network communication.");
            break;
        case WSAVERNOTSUPPORTED:
            debugMsg.append("The version of Windows Sockets API support requested is not provided by this particular Windows Sockets implementation.");
            break;
        case WSAEHOSTUNREACH:
            debugMsg.append("A socket operation was attempted to an unreachable host (no route to host).");
            break;
        default:
            debugMsg.append("An unidentified Windows Sockets error (").append(Long::toString((long)error)).append(") has happened.");
            break;
    }

    if (debugMsg.size() > 0 && Debug::isTracing("Socket.error"))
        Debug::trace("Socket.error", debugMsg);
    return retVal;
}


int Socket::cmSend(const char *msg, int length)
{
    bool tracingSend = Debug::isTracing("Socket.send");
    bool tracingError = Debug::isTracing("Socket.error");

    if (msg && length > 0 && isValid())
    {
        /* buff is the buffer we will send.  If this socket has a reader/writer, that reader/writer will
         * have the chance to modify (tack on a header and/or footer) before we send the message.
         */
        char * buff = 0;
        int totalLength = length;

        if (myReaderWriter)
        {
            if (tracingSend)
                Debug::trace("Socket.send", HobString("cmSend - myReaderWriter is set to ") + Integer::toString(myReaderWriter->getReaderWriterId()));

            const int bufferLength( length + myReaderWriter->getRecordHeaderLength() + myReaderWriter->getRecordFooterLength() + 1);  // 2 for possible 'RT', 1 for possible RS, 1 for null
            
			buff = new char[bufferLength];
            totalLength = myReaderWriter->finalizeRecordForSending(buff, msg, length);

            if ( bufferLength < totalLength )
                Debug::trace("Socket.error", HobString("cmSend - buffer overrun ") + Integer::toString(bufferLength) + " versus " + Integer::toString(totalLength) );
        }
        else
        {
            // todo - maybe write error log message indicating I don't have a reader for determining the format
            //			for the data to be written to this socket.
            if (tracingSend)
                Debug::trace("Socket.send", "cmSend - myReaderWriter is unknown");
        }

        int bytesSent = -1;
        const char * wholeBuff = buff ? buff : msg;
        const char * pBuff = wholeBuff;
        int lengthToSend = totalLength;
        int totalBytesSent = 0;
        time_t startTime = time((time_t *)NULL);

        bool trueVal = true;

		while (trueVal)
        {
            bytesSent = send(socketHandle, pBuff, lengthToSend, sendFlags);

            if (bytesSent >= 0)  // everything sent OK
            {
                totalBytesSent += bytesSent;

                if (totalBytesSent < totalLength)
                {
                    // If we make the I/O non-blocking, then the queue may be full when trying to send a message.
                    // Or, the socket's send buffer size is less than the message's length, so only part of the buffer was sent.
                    // So, offset the buffer to send past the data that did get sent, and try sending the rest.
                    if (tracingError)
                        Debug::trace("Socket.error", HobString("cmSend - Didn't send entire buffer;  bytesSent=").append(Integer::toString(bytesSent)).append("  Will retry to send the rest."));

                    pBuff += bytesSent;
                    lengthToSend -= bytesSent;

                    // Don't update the retry count; this isn't considered a retry; still sending the buffer.
                }
                else
                {
                    // Entire message sent; break out of the loop.
                    if (tracingSend)
                        Debug::trace("Socket.send", HobString("cmSend - entire message sent - totalBytesSent=").append(Integer::toString(totalBytesSent)).
                                append("  message=").append(Hex::toString(wholeBuff, totalLength)));
                    break;
                }
            }

            else  // (bytesSent == ERROR_CODE)
            {
                RetStatus ret = handleError();
                if (ret == STATUS_IO_PENDING)
                {
                    // Output buffer is currently full, so just wait and retry; don't consider this a fatal error.
                    if (tracingError)
                        Debug::trace("Socket.error", "cmSend - WOULDBLOCK, will retry ...");
                }
                else
                {
                    //todo - any other errors that aren't fatal??? check target/h/errno.h

                    if (tracingError)
                    {
                        Debug::trace("Socket.error", "*******************************************************************************");
                        Debug::trace("Socket.error", HobString("cmsend - FATAL error - errno=").append(Integer::toString(errno)).
                                append("  message=").append(Hex::toString(wholeBuff, totalLength)));
                        Debug::trace("Socket.error", "*******************************************************************************");
                    }

                    /* fatal comm error with UI (Browser,DG); return -1 to the Session
                     * who called this send.  It is then up to the Session to "handle" the fatal
                     * error, by terminating the Session.  The negative return value indicates
                     * a fatal error, while 0 or positive indicates the number of bytes sent.
                     */

                    bytesSent = -1;
                    break;  // out of while loop, don't need to retry the message
                }
            }

            if ((time((time_t *)NULL) - startTime) > ((time_t)MAX_SEND_TIME))
            {
                // Took too long to send the message, quit trying to send it.
                // Don't return -1, since some (but not all) were sent.

                /* todo - write error log message
                 * message had to be dropped on the floor.  Couldn't send it.
                 */

                if (tracingError)
                    Debug::trace("Socket.error", HobString("cmSend - *** Entire record not sent within the MAX_SEND_TIME of ").
                            append(Integer::toString((int)MAX_SEND_TIME)).append(" seconds.").
                            append("  message=").append(Hex::toString(wholeBuff, totalLength)));
                break;
            }
            else
            {
                // Pause before retrying the message.

                PAL_absclass::PAL_sleep(SEND_RETRY_DELAY); 
            }
        }

        if ( myReaderWriter && buff )
            delete[] buff;

        return totalBytesSent;
    }

    return ERROR_CODE;
}


int Socket::cmReceive(char *msgbuf)
{
    // Assumption:  The char *msgbuf passed in will point to a large enough buffer to receive the
    //					 largest message that could be received.

    bool tracingReceive = Debug::isTracing("Socket.receive");
    bool tracingError = Debug::isTracing("Socket.error");

    int bytesRead = -1;
    if (msgbuf && isValid())
    {
        if (!myReaderWriter)
            determineReaderWriter();

        if (!myReaderWriter)
        {
            if (tracingError)
                Debug::trace("Socket.error", HobString("cmReceive - HEY! I still couldn't determine the socket reader/writer for sockethandle=") +
                        HobString(Hex::toString((long long)socketHandle)) + HobString(" "));
            clearInputBuffer();
            // todo - write error log message indicating I couldn't determine a reader for the incoming
            //				data to this socket.
        }
        else
        {
            int ret = 0;
            char *pMsgbuf = msgbuf;
            bool readEntireRecord = false;
            bytesRead = 0;

            time_t startTime = time((time_t *)NULL);

            bool trueVal = true;

			while (trueVal)
            {
                if (bytesRead >= CMReaderWriter::MAX_BUFFER_LENGTH)
                {
                    /* todo - write warning log message that maximum number of bytes have been
                     * read without reading a complete message.
                     */
                    break;
                }

                ret = recv(socketHandle, pMsgbuf++, 1, recvFlags);

                if (ret == ERROR_CODE)
                {
                    if (!nonBlockingIO)
                    {
                        perror("read from socket");
                        clearInputBuffer();
                        *msgbuf = (char)NULL;
                    }
                    return 0;
                }
                else if (ret == 0)
                {
                    if ((time((time_t *)NULL) - startTime) > ((time_t)MAX_RECV_TIME))
                    {
                        if (tracingError)
                            Debug::trace("Socket.error", HobString("cmReceive - *** Entire record not read within the MAX_RECV_TIME of ") +
                                    HobString(Integer::toString((int)MAX_RECV_TIME)) + HobString(" seconds."));
                        break;
                    }
                }
                else
                {
                    bytesRead++;

                    if (myReaderWriter->finishedReadingRecord(msgbuf, bytesRead))
                    {
                        readEntireRecord = true;
                        break;
                    }
                }
            }

            *(msgbuf + bytesRead) = (char)NULL;

            if (tracingReceive)
                Debug::trace("Socket.receive", HobString("cmReceive - ") +
                        ((ret == ERROR_CODE) ? HobString("error=") : HobString("bytesRead=")) +
                        HobString(Integer::toString(bytesRead)) +
                        HobString("  message=") + HobString(Hex::toString(msgbuf, bytesRead)));

            // If there is a header on the record, allow the reader/writer to remove it.
            if (readEntireRecord)
                bytesRead = myReaderWriter->formatFromReceive(msgbuf, bytesRead, true);
        }
    }
    return bytesRead;
}

int Socket::cmReceive(HobString &msgbuf)
{
    int bytesRead = -1;
    msgbuf = "";

    if (isValid())
    {
        if (!myReaderWriter)
            determineReaderWriter();

        if (!myReaderWriter)
        {
            Debug::trace("Socket.error", HobString("cmReceive - HEY! I still couldn't determine the socket reader/writer for sockethandle=") +
                    HobString(Hex::toString((long long)socketHandle)) + HobString(" "));
            clearInputBuffer();
            // todo - write error log message indicating I couldn't determine a reader for the incoming
            //				data to this socket.
        }
        else
        {
            msgbuf.reserve(CMReaderWriter::MAX_BUFFER_LENGTH);

            int ret = 0;
            //char *pMsgbuf = msgbuf;
            bool readEntireRecord = false;
            bytesRead = 0;
            char byte = 0;

            time_t startTime = time((time_t *)NULL);

            bool trueVal = true;

			while (trueVal)
            {
                if ((bytesRead % 2048) == 0)
                    PAL_absclass::PAL_sleep(0); 

				ret = recv(socketHandle, &byte, 1, recvFlags);
				//Debug::trace("HandsFree", "cmReceive - after calling recv - ret=" + Integer::toString(ret) + "  byte=" + Hex::toString((unsigned char)byte));

                if (ret == ERROR_CODE)
                {
					int lastError = PAL_absclass::PAL_wsaGetLastError();
                    if (lastError != WSAEWOULDBLOCK)
                    {
                        Debug::trace("Socket.error", HobString("cmReceive - Got an error (") + Integer::toString(errno) + ") reading from the socket. Quiting. Currently in message buffer " + msgbuf);
						Debug::trace("Socket.error", HobString("lastError (") + Integer::toString(lastError) + ") ");
						if (!nonBlockingIO)
                        {
                            Debug::trace("Socket.error", HobString("Got inside Blocking if block"));
                            perror("read from socket");
                            clearInputBuffer();
                            msgbuf = "";
                            msgbuf.reserve(0);
                        }
                        return 0;
                    }
					else  // EWOULDBLOCK is not an error (simply there was no data to read)
					{
						Debug::trace("Socket.error", "cmReceive - not really an error: WSAEWOULDBLOCK");
						ret = 0;
					}
                }

                if (ret == 0)
                {
                    // This condition should never happen, but if it does we should return (as per Larry)
                    //					if ( msgbuf.size() == 0 )
                    //						break;

                    if ( (time((time_t *)NULL) - startTime) > ((time_t)MAX_RECV_TIME) )
                    {
                        Debug::trace("Socket.error", HobString("cmReceive - *** Entire record not read within the MAX_RECV_TIME of ") +
                                HobString(Integer::toString((int)MAX_RECV_TIME)) + HobString(" seconds."));
                        break;
                    }

                    // If there is no data to read, then release CPU
                    PAL_absclass::PAL_sleep( 60 ); 
                }
                else
                {
                    // if no more room to store data in the current msgbuf, then add some more.
                    // Reallocation using reserve() allows for fewer reallocations as the size increases.
                    if (!msgbuf.capacity())
                    {
                        Debug::trace("Socket.error", HobString("Before reserving more space, buffer size is ") + HobString(Integer::toString((int)msgbuf.size())) + HobString(", buffer is ") + msgbuf);
                        msgbuf.reserve(msgbuf.size() + CMReaderWriter::MAX_BUFFER_LENGTH);
                        Debug::trace("Socket.error", HobString("After reserving more space, buffer size is ") + HobString(Integer::toString((int)msgbuf.size())) + HobString(", buffer is ") + msgbuf);
                    }

                    bytesRead++;
                    msgbuf += byte;

                    if (myReaderWriter->finishedReadingRecord(msgbuf))
                    {
                        readEntireRecord = true;
                        break;
                    }
                }
            }

            Debug::trace("Socket.receive", HobString("cmReceive - ") +
                    ((ret == ERROR_CODE) ? HobString("error=") : HobString("bytesRead=")) +
                    HobString(Integer::toString(bytesRead)) +
                    HobString("  message=") + HobString(Hex::toString(msgbuf.data(), (int)msgbuf.size())));

            // If there is a header on the record, allow the reader/writer to remove it.
            if (readEntireRecord)
                bytesRead = myReaderWriter->formatFromReceive(msgbuf);
        }
    }

    // Free any excess allocated memory in the string.
    // This will cut the capacity of the string to the string's current size.
    msgbuf.reserve(0);
    return (int) msgbuf.size();
}


int Socket::cmNumMsgs()
{
    //struct stat data;
    //int ret = fstat(socketHandle, &data);

    //Debug::trace("Socket", HobString("cmNumMsgs - socketHandle=") + HobString(Integer::toString(socketHandle)) + HobString(" "));
    int data = -2;
    int ret = -2;
	char buff[2];
    //ret = ioctl(socketHandle, FIONMSGS, (int)&data);
    //Debug::trace("Socket", HobString("cmNumMsgs - FIONMSGS - ret=") + HobString(Integer::toString(ret)) + HobString(" "));
/*
    ret = PAL_absclass::PAL_ioctlsocket(socketHandle, PAL_FIONREAD, (u_long *)&data); 
    HobString hsdata(Integer::toString(data));
    Debug::trace("Socket.nummsgs", HobString("cmNumMsgs - FIONREAD - ret= ") + HobString(Integer::toString(ret)) +
            HobString(" data=") + hsdata);
*/
    //struct stat datas;
    //ret = ioctl(socketHandle, FIOFSTATGET, (int)&datas);
    //Debug::trace("Socket", HobString("cmNumMsgs - FIOFSTATGET - ret=") + HobString(Integer::toString(ret)) + HobString(" "));
    //return (data > 0) ? 1 : 0;

	bool tracingError = Debug::isTracing("Socket.error");

	// Last param is to just peek at the socket, don't actually read
	ret = receive(buff, 1, true);
	if (ret > 0)
	{
		ret = 1;
	}
	else if (ret == ERROR_CODE)
	{
        RetStatus err = handleError();
        if (err == STATUS_IO_PENDING)
        {
            Debug::trace("Socket.numMsgs", "cmNumMsgs - STATUS_IO_PENDING");
			ret = 0;
        }
        else
        {
			ret = -1;

            //todo - any other errors that aren't fatal??? check target/h/errno.h

            if (tracingError)
            {
                Debug::trace("Socket.error", "*******************************************************************************");
                Debug::trace("Socket.error", HobString("cmNumMsgs - FATAL error - errno=").append(Integer::toString(errno)));
                Debug::trace("Socket.error", "*******************************************************************************");
            }
		}
	}

	Debug::trace("Socket.numMsgs", "cmNumMsgs - returning " + Integer::toString(ret));
	return ret;
}


void Socket::cmClose()
{
    if (isValid())
    {
        Debug::trace("Socket.close", HobString("Socket.cmClose - Closing socket: socketHandle=") + Hex::toString((long long)socketHandle));

        PAL_absclass::PAL_closesocket(socketHandle);
        socketHandle = (PAL_SOCKET)INVALID_SOCKET_HANDLE;
    }
}


bool Socket::isValid()
{
    return ((socketHandle != INVALID_SOCKET_HANDLE) ? true : false);
}

void Socket::ensureValid()
{
	if ((PAL_SOCKET)INVALID_SOCKET_HANDLE == socketHandle)
	{
		if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == ERROR_CODE)
		{
			socketHandle = (PAL_SOCKET)INVALID_SOCKET_HANDLE;
		}
	}
}

void Socket::sConnect()
{
    if (isValid())
    {
        {
            HobString str("sConnect - Just before calling connect.  socketHandle=");
            str += Hex::toString((long long)socketHandle);
            str += ", socketAddressSize=";
            str += Integer::toString(socketAddressSize);
            Debug::trace("Socket", str);
        }

        if (connect(socketHandle, (struct sockaddr *) &address, socketAddressSize) == ERROR_CODE)
        {
            Debug::trace("Socket.error", "connect failed: error " + getSocketErrorText(PAL_absclass::PAL_wsaGetLastError()));
            perror("connect");
            cmClose();
        }
        else
            Debug::trace("Socket", HobString("sConnect - Connected to socket handle: ") + HobString(Hex::toString((long long)socketHandle)));
    }
    else
        Debug::trace("Socket", "sConnect - Can't attempt to connect; this isn't a valid socket!!");
}


void Socket::sConnect2()
{
	if ( !isValid() )
		Debug::trace("Socket", "sConnect - Can't attempt to connect; this isn't a valid socket!!");
	else
	{
		int errorValue = 0;

		Debug::trace( "Socket", "sConnect - Before calling connect, socketHandle=" + util_string::inttostring(socketHandle) +
			", socketAddressSize=" + util_string::inttostring(socketAddressSize) );

		if ( SOCKET_ERROR == connect( socketHandle, (struct sockaddr*) &address, socketAddressSize ) )
		{
			if (( nonBlockingIO ) && ( WSAEWOULDBLOCK == PAL_absclass::PAL_wsaGetLastError() ))
			{
				int retValue = 0;
				fd_set writefds, exceptfds;
				struct timeval timeout;

				for ( int i = 0; (( i < 20 ) && ( retValue == 0 )); ++i )
				{
#if defined ( __linux__ )
					// Linux needs to instantiate this every time
					timeout.tv_sec = 1; //1 second
					timeout.tv_usec = 0;
#endif
					Debug::trace( "Socket.error", "not connected yet ..." );

					FD_ZERO( &writefds );
					FD_SET( socketHandle, &writefds );
					FD_ZERO( &exceptfds );
					FD_SET( socketHandle, &exceptfds );
#if defined ( __linux__ )
					// Linux needs to actually pass a valid socket handle in (windows ignores the socket handle)
					retValue = select( socketHandle+1, NULL, &writefds, &exceptfds, &timeout );
#else
					retValue = select( 0, NULL, &writefds, &exceptfds, &timeout );
#endif 
                    
					switch ( retValue )
					{
					case SOCKET_ERROR:
						{
                                errorValue = PAL_absclass::PAL_wsaGetLastError();
							break;
						}

					case 1:
						{
							// socket is writable, we have connected!?!
							if ( FD_ISSET( socketHandle, &writefds ) )
							{
#if defined ( __linux__ )
								struct sockaddr peerAddr;
								socklen_t peerAddrlen = sizeof(peerAddr);

								// make sure we are really connected!!
								if ( 0 != getpeername( socketHandle, &peerAddr, &peerAddrlen ) )
									retValue = 0; // nope, false alarm...
#endif      // __linux__
								if ( retValue != 0 )
									Debug::trace("Socket.error", "connected");
							}

							// socket in error state, clear error
							else if ( FD_ISSET( socketHandle, &exceptfds ) )
							{
								int result = 0;
#if defined ( __linux__ )
								socklen_t resultLength = sizeof(result);
								if ( getsockopt(socketHandle, SOL_SOCKET, SO_ERROR, (void*)&result, &resultLength) )
#else
								int resultLength = sizeof(result);
								if ( getsockopt(socketHandle, SOL_SOCKET, SO_ERROR, (char*)&result, &resultLength) )
#endif      // __linux__
                                        errorValue = PAL_absclass::PAL_wsaGetLastError();
								else
									errorValue = result;
							}
							break;
						}
					} // switch
				} // for

				if ( !retValue )
				{
					/* Timed out */
					Debug::trace("Socket.error", "connect failed: timed out ");
					perror("connect");
					cmClose();

					return;
				}
			}
			else
			{
                errorValue = PAL_absclass::PAL_wsaGetLastError();
			}
		}

		if ( errorValue )
		{
			Debug::trace("Socket.error", "connect failed: error " + getSocketErrorText(errorValue));
			perror("connect");
			cmClose();
		}
		else
		{
			Debug::trace("Socket", HobString("sConnect - Connected to socket handle: ") + HobString(Hex::toString((long long)socketHandle)));
		}
	}
}

bool Socket::sConnect3()
{
	bool success = false;
    if (isValid())
    {
		int connectError = connect(socketHandle, (struct sockaddr *) &address, socketAddressSize);
		if (SOCKET_ERROR == connectError)
		{
			int lastError = PAL_absclass::PAL_wsaGetLastError();
			switch (lastError)
			{
			case WSAEISCONN:
				Debug::trace("Socket.error", "connected");
				success = true;
				break;
			case WSAEALREADY:
			case WSAEWOULDBLOCK:
			case WSAEINPROGRESS:
				Debug::trace("Socket.error", "connection in progress");
				success = false;
				break;
			default:
				Debug::trace("Socket.error", "connect failed: error " + getSocketErrorText(lastError));
				perror("connect");
				cmClose();
				success = false;
				break;
			}
		}
		else
		{
			Debug::trace("Socket.error", "connected");
			success = true;
		}
    }
    return success;
}

/* Additional receive method to allow a Socket that doesn't have a reader/writer
 * to still receive data.
 *
 * maxLength will be the maximum number of bytes read,
 * (less could be read if there aren't maxLength bytes in the socket).
 *
 * If maxLength == 0, I will keep reading until I get EOF (no more bytes in the socket),
 * in which case, the buffer provided must contain enough space to hold the maximum number of bytes
 * defined in CMReaderWriter::MAX_BUFFER_LENGTH.
 *
 * The actual number of bytes read is returned.
 *
 * NOTE: This is not part of the CommMechanism API!!!
 *
 */
int Socket::receive( char *buff, int maxLength, bool peek )
{
    int bytesRead = ERROR_CODE;

    if ( !buff )
        Debug::trace( "Socket.error", "no buffer for recv" );
    else if ( !isValid() )
        Debug::trace( "Socket.error", "invalid socket" );
    else
    {
#ifdef WIN32
        bytesRead = recv( socketHandle, buff, maxLength, (peek ? (MSG_PEEK | recvFlags) : recvFlags) );
#else
        // detect hangup!
        struct pollfd fds;
        fds.fd = socketHandle;
        fds.events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;

        const int pollrc = poll( &fds, 1, 0 );

        if ( pollrc < 0 )
            Debug::trace( "Socket.error", "poll error " + util_string::inttostring( errno ) );
        else if ( 0 == pollrc )
            bytesRead = 0;
        // when peeking, allow data to be read off socket even if its closed/error
        else if (( !peek ) && ( (POLLERR | POLLHUP | POLLRDHUP) & fds.revents ))
            Debug::trace( "Socket.error", "socket in error state (no peek)" + util_string::inttostring( fds.revents ) );
        else if ( POLLIN & fds.revents )
        {
            bytesRead = recv( socketHandle, buff, maxLength, (peek ? (MSG_PEEK | recvFlags) : recvFlags) );

            // no data available and socket in error state, terminate connection
            // when data is available return good code (even when socket bad) to allow data to be read before we close it
            if (( !bytesRead ) && ( (POLLERR | POLLHUP | POLLRDHUP) & fds.revents ))
            {
                Debug::trace( "Socket.error", "socket in error state (recv)" + util_string::inttostring( fds.revents ) );
                bytesRead = ERROR_CODE;
            }
        }
        else if (( peek ) && ( (POLLERR | POLLHUP | POLLRDHUP) & fds.revents ))
            Debug::trace( "Socket.error", "socket in error state (peek)" + util_string::inttostring( fds.revents ) );
#endif

        if ( bytesRead == SOCKET_ERROR )
        {
            if (!nonBlockingIO)
            {
                perror("read from socket");
                Debug::trace("Socket.error", "receive - ERROR=" + util_string::inttostring(PAL_absclass::PAL_wsaGetLastError()) + "  clearing input buffer");
                clearInputBuffer();
            }

            return ERROR_CODE;
        }
        else
        {
            Debug::trace("Socket.receive", "receive - bytesRead=" + util_string::inttostring(bytesRead) +
                    "  buff=" + std::string(Hex::toString(buff, bytesRead)) + 
				    "  socket handle: " + HobString(Hex::toString((long long)socketHandle)) + "  peek?:" + ((peek) ? "yes" : "no"));
        }
    }

    return bytesRead;
}

int Socket::rawSend(char *buff, int bytesToSend)
{
    int bytesSent;

    bytesSent = send(socketHandle, buff, bytesToSend, sendFlags);

    return (bytesSent);
}

void Socket::clearInputBuffer()
{
    HobString stars("*******************************************************************************");
    HobString clearingMsg("Clearing Socket input buffer ");

    Debug::trace("Socket", stars);
    Debug::trace("Socket", clearingMsg);
    Debug::trace("Socket", stars);

    int msgLen = 0;
    char msgbuf[17];
    do
    {
        memset(msgbuf, 0, 17);
        msgLen = recv(socketHandle, msgbuf, 16, recvFlags);
        if (msgLen == ERROR_CODE)
        {
            Debug::trace("Socket", clearingMsg + HobString("- error:") + HobString(Integer::toString(msgLen)));
            perror("read from socket");
        }
        else
        {
            Debug::trace("Socket", clearingMsg + HobString("- bytesIgnored: ") + HobString(Hex::toString(msgbuf, msgLen)));
        }
    } while (msgLen > 0);

    Debug::trace("Socket", stars);

}

std::string Socket::getAddress()
{
    char *ip;
    ip = inet_ntoa(address.sin_addr);
    return std::string(ip); 
}

void Socket::setAddress(const char *addr)
{
    address.sin_addr.s_addr = inet_addr(addr);
}

STATUS Socket::setDnsName(const std::string& dnsOrIp)
{

    if (bootParameters::isValidIp(dnsOrIp))
    {
        Debug::trace("Socket", "we have a valid ip address");

        setAddress(dnsOrIp.c_str());

        return 0; 
    }
    else
    {
        /* Removed this query as per Jeff (1/9/07). */
        //		bool dnsEnabled = Configuration::getInstance()->isDnsEnabled();

        //		if (dnsEnabled)
        //		{
        std::string ipStr("");

        Debug::trace("Socket", "try to resolve host = " + dnsOrIp);

        if (bootParameters::resolveHostNameIntoIpAddress(dnsOrIp, ipStr) == OK)
        {
            Debug::trace("Socket", "resolved ip = " + ipStr);
            setAddress(ipStr.c_str());
            return OK;
        }
        else
            Debug::trace("Socket", "Failed to resolve ip = " + ipStr);
        //		}
    }

    return ERROR_CODE;
}

int Socket::getPort()
{
    return ntohs(address.sin_port);
}
void Socket::setPort(const int port)
{
    address.sin_port = htons((u_short)port);
}

int Socket::getSocketHandle()
{
    return (int)socketHandle;
}

bool Socket::isRetryingEnabled()
{
    return retryingEnabled;
}
void Socket::setRetryingEnabled(bool retryingEnabled)
{
    this->retryingEnabled = retryingEnabled;
}


void Socket::determineReaderWriter()
{
    int bytesRead = 0;
    char buff[CMReaderWriter::LONGEST_IDENTIFIER_LENGTH + 1];

    // Look at, but don't remove from the socket, the first few bytes in the stream.

    memset(buff, 0, CMReaderWriter::LONGEST_IDENTIFIER_LENGTH + 1);
    bytesRead = recv(socketHandle, buff, CMReaderWriter::LONGEST_IDENTIFIER_LENGTH, MSG_PEEK | recvFlags);
    if (bytesRead > 0)
        myReaderWriter = CMReaderWriter::createReaderWriter(buff);
}



std::string Socket::getSocketErrorText(int error) const
{
    switch ( error )
    {
        case WSAEADDRNOTAVAIL:         return "10049: Cannot assign requested address - The requested address is not valid in its context";
        case WSAECONNRESET:            return "10054: Connection reset by peer - An existing connection was forcibly closed by the remote host";
        case WSAECONNREFUSED:          return "10061: Connection refused - No connection could be made because the target machine actively refused it (remote server not listening)";
        case WSAEHOSTUNREACH:          return "10065: No route to host - A socket operation was attempted to an unreachable host (no software available to reach network)";
        default:                       return util_string::inttostring(error);
    }
}



const char *Socket::toString()
{
    toStringData = "Socket: handle=";
    toStringData += Integer::toString((int)socketHandle);
    toStringData += "  address=";
    toStringData += getAddress();
    toStringData += ":";
    toStringData += Integer::toString(getPort());

    return toStringData.c_str();
}


void Socket::clone(int newsocketHandle, struct sockaddr_in *newAddress)
{
    if (isValid())
        cmClose();

    socketHandle = newsocketHandle;
    memcpy(&address, newAddress, sizeof(struct sockaddr_in));
}


void Socket::setNonBlockingIO()
{
    // turn on non-blocking i/o
    int data = TRUE;
    nonBlockingIO = true;
    int ret = PAL_absclass::PAL_ioctlsocket(socketHandle, PAL_FIONBIO, (u_long*)&data);
    HobString hsdata(Integer::toString(data));
    Debug::trace("Socket", HobString("set blocking io: FIONBIO - ret=") + HobString(Integer::toString(ret)) +
            HobString(" data=") + hsdata);
}

