//
/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "RFIDManager.h"
#include "RFIDEvent.h"
#include "TIRISReadOnlyId.h"
#include "RFIDStatus.h"

#include "Debug.h"
#include "Configuration.h"

#include "util/thread/lock.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RFIDManager::dataType = 0;
std::string RFIDManager::dataTypeName("RFIDManager"); 
#endif



RFIDManager *RFIDManager::_instance_ = 0;
util_thread::Mutex<> RFIDManager::_singletonMutex_;

const UINT8 RFIDManager::NULLID_[SIZE_ID_BYTES] = {0};


RFIDManager::RFIDManager()
	: Hobart::CEvolMsgReceiver("RFIDManager"),
	hComPort_(PAL_INVALID_COM_HANDLE_VALUE), RFIDManagerThread_(0), noIdTimerStarted_(false),
	noIdDebounceOutCntr_(0), noIdDebounceInCntr_(0), debounceTime_(0), debounceTimerComplete_(false), online_(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	tracing_        = Debug::isTracing("RFIDManager");
	tracingRun_     = Debug::isTracing("RFIDManager.Run");
	tracingEvent_   = Debug::isTracing("RFIDManager.processEvent");
	tracingError_   = Debug::isTracing("RFIDManager.Error");

	if ( tracing_ )
		Debug::trace("RFIDManager", "RFIDManager constructor");
	
	debounceTime_ = Configuration::getInstance()->getRFIDDebounceTime() * 1000;

	if ( tracing_ )
		Debug::trace("RFIDManager", "debounce time = " + util_string::ulongtostring(debounceTime_));

	memset(currentId_, 0, SIZE_ID_BYTES);
	memset(&commProperties_, 0, sizeof (commProperties_));

	RequestReadRO * pReadSinglePacket = (RequestReadRO *)readSingleBuffer;
	pReadSinglePacket->bp.startMark = START_MARK;
	pReadSinglePacket->bp.length = CMD_LENGTH_2;
	pReadSinglePacket->bp.command1 = CMD1_MODE_SINGLE | CMD1_POW_BURST1;//single read
	pReadSinglePacket->dataField = BURST_MODE_50MS; //power burst of 50ms
	pReadSinglePacket->BCC = CalculateBCC((UINT8 *)&pReadSinglePacket->bp.length , sizeof(RequestReadRO) - 2);

	memset(&readContinuousNormalBuffer, 0, sizeof(readContinuousNormalBuffer));
	RequestReadRO * pReadContinuousNormalPacket = (RequestReadRO *)readContinuousNormalBuffer;
	pReadContinuousNormalPacket->bp.startMark = START_MARK;
	pReadContinuousNormalPacket->bp.length = CMD_LENGTH_2;
	pReadContinuousNormalPacket->bp.command1 = CMD1_MODE_CONTINUOUS_NORMAL | CMD1_POW_BURST1;//continuous read
	pReadContinuousNormalPacket->dataField = BURST_MODE_50MS; //power burst of 50ms
	pReadContinuousNormalPacket->BCC = CalculateBCC((UINT8 *)&pReadContinuousNormalPacket->bp.length, sizeof(RequestReadRO) - 2);

	memset(&readContinuousLineBuffer, 0, sizeof(readContinuousLineBuffer));
	RequestReadRO * pReadContinuousLinePacket = (RequestReadRO *)readContinuousLineBuffer;
	pReadContinuousLinePacket->bp.startMark = START_MARK;
	pReadContinuousLinePacket->bp.length = CMD_LENGTH_2;
	pReadContinuousLinePacket->bp.command1 = CMD1_MODE_CONTINUOUS_LINE | CMD1_POW_BURST1;//continuous read
	pReadContinuousLinePacket->dataField = BURST_MODE_50MS; //power burst of 50ms
	pReadContinuousLinePacket->BCC = CalculateBCC((UINT8 *)&pReadContinuousLinePacket->bp.length, sizeof(RequestReadRO) - 2);

	RequestReadRO * pRequestVersionPacket = (RequestReadRO *)requestVersionBuffer;
	pRequestVersionPacket->bp.startMark = START_MARK;
	pRequestVersionPacket->bp.length = CMD_LENGTH_2;
	pRequestVersionPacket->bp.command1 = CMD1_MODE_REQUEST_VERSION | CMD1_POW_BURST1;//continuous read
	pRequestVersionPacket->dataField = BURST_MODE_50MS; //power burst of 50ms
	pRequestVersionPacket->BCC = CalculateBCC((UINT8 *)&pRequestVersionPacket->bp.length, sizeof(RequestReadRO) - 2);

	Configuration::getInstance()->setRFIDHWPresent(online_);
	
	TIRIS_SoftwareVersion_ = 0x00;  // initialize to NULL

	init();
}


RFIDManager::~RFIDManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if ( tracing_ )
		Debug::trace("RFIDManager", "~RFIDManager");

	deregisterAllEvents();

	if ( hComPort_ != PAL_INVALID_COM_HANDLE_VALUE )
	{

    #if defined(PAL_WIN32_RUN)
		if ( !::CloseHandle(hComPort_) )
    #elif   defined( __linux__ )
        if ( close(hComPort_) != 0 )
    #else
        #error "Detect close() error"
    #endif
        {
            PAL_showError("Failed to CloseHandle");
        }

	}

	//	This came from GCD, but do we need it? We get a compiler error:
	//	marqueemanager.cpp(108) : error C3861: 'shutdownTimerThreads': identifier not found
	if (RFIDManagerThread_)
	{
		RFIDManagerThread_->StopThread();
		RFIDManagerThread_->WaitForCompletion();
	}
//	From GCD. Causes error.		shutdownTimerThreads();
	ShutdownNicely();
}


void RFIDManager::init()
{
	registerEvent(RFIDReader, RFIDRequestCurrentID);
}


void RFIDManager::processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg)
{
	Util::CSmartPtr<EvolMessage> evolMsg = processEventExternal(msg);

	if ( evolMsg.notNull() )
		dispatchEvent(evolMsg.get());
}


Util::CSmartPtr<EvolMessage> RFIDManager::processEventExternal(Util::CSmartPtr< EvolMessageWrapper> & msg)
{
	EvolMessage evolMessage( msg->Data() );

	switch ( evolMessage.getDeviceType() )
	{
		case RFIDReader:
		{
			switch ( evolMessage.getEventType() )
			{
				case RFIDRequestCurrentID:
				{
					// Addded the RFID software version to this message, FHA 3/13
					//TIRISReadOnlyId * pidMsg = new TIRISReadOnlyId (this, currentId_, SIZE_ID_BYTES);
					TIRISReadOnlyId * pidMsg = new TIRISReadOnlyId (this, currentId_, SIZE_ID_BYTES, TIRIS_SoftwareVersion_);

					Util::CSmartPtr<EvolMessage> evolMsg ( pidMsg );

					if ( tracingEvent_ )
					{
						Debug::trace("RFIDManager.processEvent", "requesting current RFID");
						pidMsg->show();
					}

					return evolMsg;
				}
				case RFIDRequestCurrentStatus:
				{
					RFIDStatus * statMsg = new RFIDStatus (this, 0);

					statMsg->setComStatus(online_);

					Util::CSmartPtr<EvolMessage> evolMsg ( statMsg );

					if ( tracingEvent_ )
					{
						Debug::trace("RFIDManager.processEvent", "requesting current status");
						statMsg->show();
					}

					return evolMsg;
				}
				default:
				{
					if ( tracingEvent_ )
						Debug::trace("RFIDManager.processEvent", "received unknown eventType message.");

					break;
				}
			}

			break;
		}
		default:
		{
			if ( tracingEvent_ )
				Debug::trace("RFIDManager.processEvent", "received unknown DevType message.");

			break;
		}
	}

	return Util::CSmartPtr<EvolMessage>(0);
}


DWORD RFIDManager::Run()
{
	bool stopThread = false;
	DWORD commErrCntr = 0;
	DWORD badBCCCntr = 0;
	UINT8 readComTimeoutCntr = 0;
    PAL_OVERLAPPED  ovl = {0};
	DWORD l_result = 0;
	UINT8 buff[MAX_TIRIS_PACKET];
	DWORD bytesWrote = 0;

    ovl.hEvent = PAL_absclass::PAL_createEvent(NULL, FALSE, FALSE, NULL);

	PAL_absclass::PAL_sleep(1);

	if ( ovl.hEvent != NULL )
	{
		if ( tracingRun_ )
			Debug::trace("RFIDManager.Run", "Starting Thread. Id = " + util_string::uinttohexstring(RFIDManagerThread_->GetThreadID()));
		try
		{
			l_result = WriteBytesBlocking(sizeof(requestVersionBuffer), (UINT8 *)requestVersionBuffer, &ovl, &bytesWrote);
			if ( l_result == COM_SUCCESS )
			{
				DWORD totalBytesRead = 0;
				ReceiveVersion * rv = 0;

				//read all the packets until a version info packet is found.
				//if no packets are available. comm will timeout in 5s.
				//when this happens l_result will be 0 and will drop out of the loop.
				do
				{
					l_result = ReadPacket(buff, MAX_TIRIS_PACKET, &ovl, &totalBytesRead);
					rv = (ReceiveVersion *)buff;
					if (l_result == COM_SUCCESS)
					{
						if ( (rv->bp.status & STATUS_VERSION) > 0 ) 
							break;
					}
					else
					{
						// Added test for valid software version packet but BCC failure if sw version is 0x20
						if(l_result == COM_BAD_BCC && (rv->bp.status & STATUS_VERSION) > 0 && rv->version == 0x20)
						{
							l_result = COM_SUCCESS;
							break;	// software version 0x20 always sends a bad BCC, ignore it
						}
						else
							if ( tracingError_ )
								Debug::trace("RFIDManager.Error", "**** ReadPacket failed to read the software version (result = " +
									util_string::uinttohexstring(l_result) + ")");
					}
				}
				while (l_result == COM_SUCCESS);


				if (l_result == COM_SUCCESS)
				{
					if ( tracingRun_ )
					{
						Debug::trace("RFIDManager.Run", "Received SW version = " + util_string::uinttohexstring((const unsigned char)rv->version));
						Debug::trace("RFIDManager.Run", "Received SW version BCC = " + util_string::uinttohexstring((const unsigned char)rv->BCC));
						Debug::trace("RFIDManager.Run", "Received SW version Packet Length = " + util_string::uinttohexstring((const unsigned char)rv->bp.length));
						Debug::trace("RFIDManager.Run", "Received SW version Status = " + util_string::uinttohexstring((const unsigned char)rv->bp.status));
					}

					// Eliminated test for specific software version number. If we received a valid software version
					// packet we don't care what the version number is.
					//if (rv->version == 0x15)
					//if(rv->version ==0x20) // Added for testing of new RFID module
					//{//we know we have the device since we received the software version packet.
						//now we send out the command to place it in continuous line read.

	//					l_result = WriteBytesBlocking(sizeof(readContinuousLineBuffer), (UINT8 *)&readContinuousLineBuffer, &ovl, &bytesWrote);

						online_ = true;
						Configuration::getInstance()->setRFIDHWPresent(online_);
						TIRIS_SoftwareVersion_ = rv->version;  // set to read in version number

						RFIDStatus msg (this, 0);
						msg.setComStatus(online_);
						dispatchEvent(&msg);


						while ( !m_ExitEvent.IsEventSet() && (!stopThread) )
						{
							l_result = WriteBytesBlocking(sizeof(readSingleBuffer), (UINT8 *)&readSingleBuffer, &ovl, &bytesWrote);
							if ( l_result != COM_SUCCESS )
								Debug::trace("RFIDManager.Run", "***** WriteBytesBlocking did not return success when writing request");

                            PAL_absclass::PAL_waitForSingleObj(m_ExitEvent, 200);
							l_result = ReadPacket(buff, MAX_TIRIS_PACKET, &ovl, &totalBytesRead);
							switch ( l_result )
							{
								case COM_BAD_BCC:
								{
									badBCCCntr++;

									if ( tracingError_ )
										Debug::trace("RFIDManager.Error", "Bad bcc counter = " + util_string::ulongtostring(badBCCCntr));

									break;
								}
								case COM_ERROR:
								{//shouldn't normally happen
									commErrCntr++;
									if ( commErrCntr >= 5 )
										stopThread = true;	

									if ( tracingError_ )
										Debug::trace("RFIDManager.Error", "COM_ERROR stopThread = " + util_string::booltostring(stopThread));

									break;
								}
								case TERMINATE_THREAD:
								{
									if ( tracingError_ )
										Debug::trace("RFIDManager.Error", "Terminating run thread ");

									break;
								}
								case COM_TIMED_OUT:
								{//shouldn't happen since we are in continuous line mode. we should always be getting
								 //reads.
									readComTimeoutCntr++;
									if ( readComTimeoutCntr >= 12 )
										stopThread = true;	

									if ( tracingError_ )
										Debug::trace("RFIDManager.Error", "COM_TIMED_OUT stopThread = " + util_string::booltostring(stopThread));

									break;
								}
								case COM_SUCCESS:
								{
									readComTimeoutCntr = 0;
									commErrCntr = 0;
									ProcessReadPacket(buff, totalBytesRead);

									break;
								}
								default:
								{
									if ( tracingError_ )
										Debug::trace("RFIDManager.Error", "Unknown result from read packet (result = " +
												util_string::uinttohexstring(l_result) + ") stopThread = " +
												util_string::booltostring(stopThread));

									break;
								}
							}

							if ( debounceTimerComplete_ )
							{
								noIdTimerStarted_ = false;

								debounceTimerComplete_ = false;

								DWORD percentOut = 0;

								if ((noIdDebounceOutCntr_ + noIdDebounceInCntr_) != 0)
									percentOut = (noIdDebounceOutCntr_ * 100) / (noIdDebounceOutCntr_ + noIdDebounceInCntr_);

								if ( tracingRun_ )
									Debug::trace("RFIDManager.Run", "timed out::percentOut = " + util_string::uinttostring(percentOut));

								if ( percentOut > 90 )
								{
									if ( tracingRun_ )
										Debug::trace("RFIDManager.Run", "dispatching null id");

									memcpy(currentId_, NULLID_, SIZE_ID_BYTES);

									//we now have a situation where no id was present for at least 10s.
									//so dispatch the no id message to the application.
									// Addded the RFID software version to this message, FHA 3/19
									//TIRISReadOnlyId idMsg(this, currentId_, SIZE_ID_BYTES);
									TIRISReadOnlyId idMsg(this, currentId_, SIZE_ID_BYTES, TIRIS_SoftwareVersion_);

									dispatchEvent(&idMsg);
								}
							}
						}
					//}
					//else
					//{
					//	if ( tracingError_ )
					//		Debug::trace ("RFIDManager.Error", "Software version received was not 0x15!!!! (received " +
					//				util_string::uinttohexstring(rv->version));
					//}
				}
			}
			else
			{
				if ( tracingError_ )
					Debug::trace("RFIDManager.Error", "WriteBytesBlocking failed to request software version");
			}
		}  // end of try
		catch(...)
		{
			if ( tracingError_ )
				Debug::trace("RFIDManager.Error", "***************** Got exception in RFID Manager Thread ****************");

			LogCatch;
		}
	}
	else
    {
        PAL_showError("Failed to create event for overlay structure");
    }

	online_ = false;
	Configuration::getInstance()->setRFIDHWPresent(online_);

	RFIDStatus msg (this, 0);
	dispatchEvent(&msg);

    if ( ! PAL_absclass::PAL_closeHandle(ovl.hEvent) )
    {
        PAL_showError("Failed to CloseHandle for overlay event");
    }

#if defined(PAL_WIN32_RUN)
	if ( !::CancelIo(hComPort_) )
    {
        PAL_showError("Failed to Cancel IO operations");
    }

#elif   defined( __linux__ )
    {
        FILE *fp = fdopen(hComPort_, "r+");     // open with O_RDWR flag (RFIDManagerLinux.cpp::RFIDManager::connect)
        if (fp != NULL)
        {
            __fpurge(fp);
        }
        else
        {
            PAL_showError("Failed to Cancel IO operations");
        }
        // do not need to fclose(fp)
    }

#else
    #error "Clear I/O buffer"
#endif

	return 0;
}


void RFIDManager::timeoutProcess( __int32 timer_id )
{
	if ( timer_id == NoIdTimer )
	{
		debounceTimerComplete_ = true;
	}
}


bool RFIDManager::CheckBCC(BYTE* buffer, DWORD bytesRead)
{
	ValidIDPacket * pValid = (ValidIDPacket *)buffer;
	BYTE bcc = buffer[1];
	BYTE endIndx = pValid->bp.length + 1;

	for (int i = 2; i <= endIndx; i ++)
	{
		bcc = bcc ^ buffer [i];
	}

	if ( bcc == pValid->BCC )
		return true;
	else
	{
		Debug::trace("RFIDManager.Run", "CheckBCC: failed check. BytesRead = " + util_string::ulongtostring(bytesRead) +
			" buffer = '" + util_string::stringtohexstring(std::string((char*)buffer, sizeof(ValidIDPacket))) +
			"' and calculated BCC from index 1 to " + util_string::uinttostring(endIndx) + " both included");

		return false;
	}
}


void RFIDManager::ProcessReadPacket(BYTE* buffer, DWORD bytesRead)
{
	ReadBasePacket *pBase = (ReadBasePacket *)buffer;

	if ( (pBase->status & STATUS_DBCC_OK) == STATUS_DBCC_OK ) //a valid id
	{
		if ( CheckBCC(buffer, bytesRead) )
		{
			UINT8 tempId[SIZE_ID_BYTES];
			ValidIDPacket * pValid = (ValidIDPacket *)pBase;

			for (int i = 0; i < SIZE_ID_BYTES; i++)
				tempId[SIZE_ID_BYTES - i - 1] = pValid->id[i];

			if (memcmp(tempId, currentId_, SIZE_ID_BYTES) != 0)
			{//we have detected a new id
				if (debounceTimerComplete_)
				{
					debounceTimerComplete_ = false;
					noIdTimerStarted_ = false;
				}
				else if (noIdTimerStarted_)
				{
					noIdTimerStarted_ = false;
					cancelTimer(NoIdTimer);
				}
				memcpy(currentId_, tempId, SIZE_ID_BYTES);
				//we have a new id and it is different than the old id so
				//dispatch it immediately.
				// Added the software version to this message, FHA 3/13
				//TIRISReadOnlyId idMsg(this, currentId_, SIZE_ID_BYTES);
				TIRISReadOnlyId idMsg(this, currentId_, SIZE_ID_BYTES, TIRIS_SoftwareVersion_);

				dispatchEvent(&idMsg);

				if ( tracingRun_ )
				{
					Debug::trace("RFIDManager.Run", "Received good id");
					idMsg.show();
				}
			}
			else if (noIdTimerStarted_)
			{
				noIdDebounceInCntr_++;
			}
		}
		else
			Debug::trace("RFIDManager.Run", "Checksum validation failed");
	}
	else //no id present
	{
		if ( noIdTimerStarted_ )
		{
			noIdDebounceOutCntr_++;
		}
		else if ( memcmp(NULLID_, currentId_, SIZE_ID_BYTES) != 0 )
		{//currentId_ is not null. we need to debounce this
			noIdTimerStarted_ = true;
			noIdDebounceOutCntr_ = 0;
			noIdDebounceInCntr_ = 0;
			createTimer(debounceTime_, NoIdTimer);
			Debug::trace("RFIDManager.Run", "Creating a timer to log out the current id");
		}
	}
}

DWORD RFIDManager::ReadPacket(UINT8 * buffer, DWORD bufferLen, PAL_POVERLAPPED pOVL, DWORD *pTotalBytesRead)
{
	//mark + length + status + data + bcc
	DWORD result = 0;
	DWORD bytesRead = 0;

	*pTotalBytesRead = 0;

	memset (buffer, 0, bufferLen);

	//read start mark, pick off junk if it exists.
	do
	{
		result = ReadBytesBlocking(1, buffer, pOVL, &bytesRead);
	}
	while ((result == COM_SUCCESS) && (buffer[0] != 1));

	if ( result == COM_SUCCESS )
	{
		*pTotalBytesRead += bytesRead;

		//read length
		result = ReadBytesBlocking(1, &buffer[1], pOVL, &bytesRead);
		if ( result == COM_SUCCESS )
		{
			*pTotalBytesRead += bytesRead;

			if ( buffer[1] < MAX_TIRIS_BODY )
			{
				// read status + data + bcc
				UINT8 bytesRemaining = buffer[1] + 1; 
				result = ReadBytesBlocking(bytesRemaining, &buffer[2], pOVL, &bytesRead);
				if (result == COM_SUCCESS)
				{
					*pTotalBytesRead += bytesRead;
					
					UINT8 calcBCC = CalculateBCC(&buffer[1], *pTotalBytesRead - 2);
 					if ( calcBCC == buffer[*pTotalBytesRead - 1] )
						return COM_SUCCESS;
					else
					{
						if ( tracingError_ )
							Debug::trace("RFIDManager.Error", "Calculated checksum does not match received");

						return COM_BAD_BCC;
					}

				}
				else
				{
					if ( tracingError_ )
						Debug::trace("RFIDManager.Error", "Failed to read whole packet");

					return result;
				}
			}
			else
			{
				if ( tracingError_ )
					Debug::trace("RFIDManager.Error", "Length received (" + util_string::uinttostring(buffer[1]) +
							") is greater than maximum");

				return COM_ERROR;
			}

		}
		else
			return result;

	}
	else
		return result;

}


UINT8 RFIDManager::CalculateBCC(UINT8 * buffer, DWORD len)
{
	UINT8 bcc = buffer[0];

	for (DWORD i = 1; i < len; i++)
		bcc = bcc ^ buffer[i];

	return bcc;
}

void RFIDManager::PAL_showError(const std::string& text, PAL_ERRNO_TYPE error_no)
{
	if ( tracingError_ )
	{
		std::string message;

    #if defined(PAL_WIN32_RUN)
		message.assign(text).append(". Error ").append(util_string::uinttostring(error_no)).append(": ").append(getLastErrorMessage(error_no));

    #elif   defined( __linux__ )
		message.assign(text).append(". Error ").append(util_string::uinttostring(error_no)).append(": ").append(strerror(error_no));

    #else
        #error "Implement error message function for the system"
    #endif

		Debug::trace("RFIDManager.Error", message);
	}
}


RFIDManager *RFIDManager::getInstance()
{
	if ( _instance_ == 0 )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);

		if ( _instance_ == 0 )
			_instance_ = new RFIDManager();
	}

	return _instance_;
}


void RFIDManager::deleteInstance()
{
	if ( _instance_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);

		if ( _instance_ )
		{
			delete _instance_;
			_instance_ = 0;
		}
	}
}
