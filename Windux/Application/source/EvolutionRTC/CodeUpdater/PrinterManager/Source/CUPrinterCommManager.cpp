#include "PAL_platformheader.h"
#include "CUPrinterCommManager.h"

#include "UsbManager.h"
#include "CUPrinterEvent.h"
#include "VmStatusMsg.h"
#include "PeripheralRequestCodeUpdate.h"
#include "UsbAddComponent.h"
#include "UsbRemoveComponent.h"

#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CUPrinterCommManager::dataType = 0;
std::string CUPrinterCommManager::dataTypeName("CUPrinterCommManager");
#endif

CUPrinterCommManager::CUPrinterCommManager(USBD_NODE_ID usbNodeId,SUsbDescriptors * descs) :
    IPeripheralUpdater(usbNodeId, UsbManager::getInstance()->getProductId(usbNodeId)), CUsbCommBase( descs, usbNodeId )
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    Debug::trace("CUPrinterCommManager", "CUPrinterCommManager constructor");

    pBulkData_ = NULL;
    sendingBulkData_ = false;
    bulkDataLength_ = 0;

    xmitting_ = false;
    recvdCUMessage_ = false;

    flashImgBuffer_ = 0;

    //TEST JRB
    lastTick_ = 0;
    testWakeup_ = false;

    Debug::trace("CUPrinterCommManager", "Create Pipes:");
    createPipes();
    startPipes();
#if defined ( __linux__ )
	Debug::trace("CUPrinterCommManager", HobString("PipeHandle =") + Hex::toString(getMyPipeHandle()));
#endif
}

CUPrinterCommManager::~CUPrinterCommManager()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
    Debug::trace("CUPrinterCommManager", "CUPrinterCommManager destructor");

	try
	{
	    UsbRemoveComponent removeMe(usbNodeId, this);
	    dispatchEvent(&removeMe);

	    deregisterAllEvents();

	    //shutdown the msg thread
	    ShutdownNicely();
	    
		if (flashImgBuffer_ != 0)
	        delete[] flashImgBuffer_;
	}
	catch (...)
	{
		Debug::trace("Exception", std::string("CUPrinterCommManager::~CUPrinterCommManager()"));
		LogCatch;
	}
	Debug::trace("CUPrinterCommManager", "CUPrinterCommManager destructor.  Goodbye");
}


void CUPrinterCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & UNUSED(msg) )
{
    UNREFERENCED_PARAMETER(msg);

    usbEvolHdr_.setSourcePhysicalAddress(0x00);
    usbEvolHdr_.setSourceLogicalAddress(USBEvolHeader::USER_INTERFACE);
}

void CUPrinterCommManager::sendBulkMessageCallBack(
            CUsbPipe::EvolPipeType UNUSED(pipe_type),
            DWORD UNUSED(result),
            bool UNUSED(user_defined_error) )
{
    UNREFERENCED_PARAMETER(user_defined_error);
    UNREFERENCED_PARAMETER(pipe_type);
    UNREFERENCED_PARAMETER(result);

    Debug::trace("CUPrinterCommManager.UsbIrp", "sendBulkDataCallback");

    sendingBulkData_ = false;
#if 0 //remove temp ats 032712
    if (result == CUsbPipe::TimeOut)
    {
        Debug::trace("CUPrinterCommManager.BulkData", "Bulk Transfer ERROR: S_usbHcdLib_TIMEOUT");
    }

    if (result == OK)
    {
        pBulkData_ = NULL;
        bulkDataLength_ = 0;

        VmStatusMsg status(NULL, VersionManagerUsbPrinterDownloading, HfcResponse::RESPONSESTYLE_NOINPUT, true);

        dispatchEvent(&status);
    }
    else
    {
        Debug::trace("CUPrinterCommManager.Error", "sendBulkDataCallback Printer Irp Not Successful");
        pBulkData_ = NULL;
        bulkDataLength_ = 0;

        VmStatusMsg status(NULL, VersionManagerUsbPrinterDownloadFailed, HfcResponse::RESPONSESTYLE_OK, false);

        dispatchEvent(&status);
    }
#endif
}

void CUPrinterCommManager::receiveErrorCallBack(
            CUsbPipe::EvolPipeType UNUSED(pipe_type),
            DWORD error,
            bool user_defined_error )
{
    UNREFERENCED_PARAMETER(pipe_type);
    printUSBError( error, user_defined_error );
}

void CUPrinterCommManager::setupCodeFlash(CuPrnCmdEnum cmdType)
{
    char str[255];
    DWORD result = OK;
    bool is_custom_code = false;

    switch (cmdType)
    {
        case CuPrnResetErrCounter:
            {
                CuPrnGenericMsg msg;
                msg.cuEvent = CuPrnReset;
                msg.pad0 = 0;

                int sizeOfData = sizeof(msg);

                usbEvolHdr_.setDestinationPhysicalAddress(0x0);
                usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                usbEvolHdr_.setMessageLength((UINT16)sizeOfData);


                usbEvolHdr_.Serialize( str );

                memcpy( &str[10], (char *) &msg, sizeof(msg));
                sendUsbInterruptMessage( ( const char * ) str,  sizeof(msg) + 10, result, is_custom_code );

                Debug::trace("CUPrinterCommManager", "sending out command = reset failure counters");
            }
            break;
        case CuPrnRunNormal:
        case CuPrnBurnPrinter:
        case CuPrnBurnBootCode:
        case CuPrnBurnFlud:
            {
                prnCmd_.cuEvent = (UINT8)CuPrnCommand;
                prnCmd_.pad0 = 0;
                prnCmd_.command = (UINT8)cmdType;
                prnCmd_.pad1 = 0;
                prnCmd_.StartAddress = 0;
                prnCmd_.EndAddress = 0;

                switch(prnCmd_.command)
                {
                    case CuPrnRunNormal:
                        Debug::trace("CUPrinterCommManager", "sending out command = run normal");
                        break;
                    case CuPrnBurnPrinter:
                        Debug::trace("CUPrinterCommManager", "sending out command = burn printer code");
                        break;
                    case CuPrnBurnBootCode:
                        Debug::trace("CUPrinterCommManager", "sending out command = burn boot code");
                        break;
                    case CuPrnBurnFlud:
                        Debug::trace("CUPrinterCommManager", "sending out command = burn FLUD");
                        break;
                }

                /*If we have a code update, then we cannot send this out until we know the size of the
                  usb peripheral code update. we actually send this out in the downloadCode method. this
                  gets transmitted out just before the bulk transfers sending the actual code down.*/
                if (cmdType == CuPrnRunNormal)
                {
                    prnCmd_.codeSize = 0;

                    int sizeOfData = sizeof(prnCmd_);

                    usbEvolHdr_.setDestinationPhysicalAddress(0x0);
                    usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
                    usbEvolHdr_.setMessageLength((UINT16)sizeOfData);

                    usbEvolHdr_.Serialize( str );

                    memcpy( &str[10], (char *) &prnCmd_, sizeof(prnCmd_));
                    sendUsbInterruptMessage( ( const char * ) str,  sizeof(prnCmd_) + 10, result, is_custom_code );
                }
            }
            break;
        default:
            {
            }
            break;
    }

    if ( result != OK ) //error
    {
        printUSBError( result, is_custom_code );
    }
}

bool CUPrinterCommManager::createCommLink()
{
    return true;
}

bool CUPrinterCommManager::initializePeripheral()
{
    /*setup the int out transfer to initiate the flash update*/

    return true;
}

bool CUPrinterCommManager::downloadCode(const char * pCode)
{
    char str[512];
    DWORD result = OK;
    bool is_custom_code = false;
    int blockSize = 0;
    int fileSize = 0;

#if defined(LINUX_FIX)
    // Rewirte with C++ standard to retrieve file size

	if (PAL_absclass::PAL_getFileSize(pCode, &fileSize))
    {
        Debug::trace("EvolWrapperCommManager", "have file size!");
        blockSize = 1024;
    }
	else
	{
        Debug::trace("CUPrinterCommManager", "failed to get file size.");
        return false;
	}
/*
	ifstream::pos_type  file_size = 0;
    ifstream    myFile(pCode, ios::in|ios::ate);
    if ( !(myFile.is_open()) ) {
        Debug::trace("CUPrinterCommManager", "failed to get file size.");
        return false;
    }
    file_size = myFile.tellg();
    myFile.close();

    if ( (file_size < 0) || (file_size > 0xFFFF) )
    {
        Debug::trace("CUPrinterCommManager", "file is too large(>65535 bytes).");
        return false;
    }

    // Assign the acceptable file size
    blockSize = 1024;
    fileSize = file_size;
*/
#else
    WIN32_FILE_ATTRIBUTE_DATA attr_data;
    if ( GetFileAttributesExA(  pCode, GetFileExInfoStandard, &attr_data ) > 0 )
    {
        if ( attr_data.nFileSizeHigh > 0 )
        {
            Debug::trace("CUPrinterCommManager", "file is too large(>65535 bytes).");
            return false;
        }
        else
        {
            blockSize = 1024;
            fileSize = attr_data.nFileSizeLow;
        }
    }
    else
    {
        Debug::trace("CUPrinterCommManager", "failed to get file size.");
        return false;
    }
#endif//LINUX_FIX

    int bytesRead = 0;
    UINT8* tmpPtr;
    //    int byteCount = 0;

    Debug::trace("CUPrinterCommManager", HobString("file size = ") + Integer::toString(fileSize));

    if ((fileSize > 0) && (blockSize > 0))
    {
        //want to make sure that the usb task does not start handling these irps until they
        //are both setup. so we use a critical section to ensure this.

        Util::CLockCriticalSection lock( cuPrinterCommManagerCS );

        prnCmd_.codeSize = fileSize;

        usbEvolHdr_.setDestinationPhysicalAddress(0x0);
        usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
        usbEvolHdr_.setMessageLength(sizeof(CuPrnCommandMsg));
        usbEvolHdr_.Serialize(str);

        memcpy(&str[10], (char *) &prnCmd_, sizeof(CuPrnCommandMsg));
        sendUsbInterruptMessage( ( const char * ) str,  sizeof(CuPrnCommandMsg) + 10, result, is_custom_code );

#if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep( 1 );
#else
        Sleep( 1 );
#endif

        if (flashImgBuffer_ != 0)
        {
            delete[] flashImgBuffer_;
            flashImgBuffer_ = 0;
        }

        tmpPtr = flashImgBuffer_ =(UINT8 *) new char[fileSize];
        memset(flashImgBuffer_, 0, fileSize);

        if(tmpPtr != 0)
        {
            ifstream myFile(pCode, ios::in | ios::binary);
            myFile.seekg(0, ios::beg);

            do
            {
                myFile.read(( char * )tmpPtr, blockSize);
                bytesRead = myFile.gcount();

                tmpPtr+= bytesRead;
            }
            while (bytesRead == blockSize);

            myFile.close();

            pBulkData_ = (UINT16 *)flashImgBuffer_;
            bulkDataLength_ = fileSize;

            sendUsbBulkMessage( ( const char * )pBulkData_, bulkDataLength_ );
        }
    }

    return true;
}

bool CUPrinterCommManager::finalizePeripheral()
{
    return true;
}

bool CUPrinterCommManager::rebootPeripheral()
{
    return true;
}

bool CUPrinterCommManager::deleteCommLink()
{
    return true;
}

void CUPrinterCommManager::processUsbMessage(
        std::string & msg,
        CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
    UNREFERENCED_PARAMETER(pipe_type);

    const char * ptr = msg.c_str();
    USBEvolHeader usbEvolHeader((char *) ptr, true);

    UINT16 physicalAddress = usbEvolHeader.getSourcePhysicalAddress();
// removed FHA 5/31/11, not used    UINT16 logicalAddress = usbEvolHeader.getSourceLogicalAddress();

    ptr+= 10;
	receivedCodeUpdaterMessage((CuPrnGenericMsg *) ptr, physicalAddress);
}

void CUPrinterCommManager::receivedCodeUpdaterMessage(CuPrnGenericMsg *genMsg, UINT16 physicalAddress) // , HobThread *pHobThread)
{
    switch (genMsg->cuEvent)
    {
        case CuPrnWakeup:
            {
                //added ats

                if(physicalAddress != 0)
                {
                    usbEvolHdr_.setDestinationPhysicalAddress(physicalAddress);
                }
                else
                {
                    usbEvolHdr_.setDestinationPhysicalAddress(0x46);
                }
                CuPrnWakeupMsg *cuWakeupMsg = (CuPrnWakeupMsg *) genMsg;


                Debug::trace("CUPrinterCommManager", "received wakeup msg");
                Debug::trace("CUPrinterCommManager", HobString("flud major rev = ") + Integer::toString(cuWakeupMsg->FlashUpdateMajorRev));
                Debug::trace("CUPrinterCommManager", HobString("flud minor rev = ") + Integer::toString(cuWakeupMsg->FlashUpdateMinorRev));
                Debug::trace("CUPrinterCommManager", HobString("Dormant flud major rev = ") + Integer::toString(cuWakeupMsg->FlashUpdateDormantMajorRev));
                Debug::trace("CUPrinterCommManager", HobString("Dormant flud minor rev = ") + Integer::toString(cuWakeupMsg->FlashUpdateDormantMinorRev));
                Debug::trace("CUPrinterCommManager", HobString("Printer major rev = ") + Integer::toString(cuWakeupMsg->PrinterMajorRev));
                Debug::trace("CUPrinterCommManager", HobString("Printer minor rev = ") + Integer::toString(cuWakeupMsg->PrinterMinorRev));
                Debug::trace("CUPrinterCommManager", HobString("Boot code major rev = ") + Integer::toString(cuWakeupMsg->BootCodeMajorRev));
                Debug::trace("CUPrinterCommManager", HobString("Boot code minor rev = ") + Integer::toString(cuWakeupMsg->BootCodeMinorRev));
                Debug::trace("CUPrinterCommManager", HobString("Hardware major rev = ") + Integer::toString(cuWakeupMsg->HardwareMajorRev));
                Debug::trace("CUPrinterCommManager", HobString("Hardware minor rev = ") + Integer::toString(cuWakeupMsg->HardwareMinorRev));
                Debug::trace("CUPrinterCommManager", HobString("Manufacturer ID = ") + Integer::toString(cuWakeupMsg->ManufacturerID));
                Debug::trace("CUPrinterCommManager", HobString("Device ID = ") + Integer::toString(cuWakeupMsg->DeviceID));
                Debug::trace("CUPrinterCommManager", HobString("Bit Error = ") + Hex::toString(cuWakeupMsg->BitError));
                Debug::trace("CUPrinterCommManager", HobString("Address = ") + Hex::toString(cuWakeupMsg->AddressError));
                Debug::trace("CUPrinterCommManager", HobString("Selected FLUD = ") + Integer::toString(cuWakeupMsg->SelectedFLUD));
                Debug::trace("CUPrinterCommManager", HobString("FLUD1 Status = ") + Integer::toString(cuWakeupMsg->FLUD1Status));
                Debug::trace("CUPrinterCommManager", HobString("FLUD2 Status = ") + Integer::toString(cuWakeupMsg->FLUD2Status));
                Debug::trace("CUPrinterCommManager", HobString("Configuration = ") + Integer::toString(cuWakeupMsg->Configuration));
                Debug::trace("CUPrinterCommManager", HobString("BootPrnFailureCnt = ") + Integer::toString(cuWakeupMsg->BootPrnFailureCnt));
                Debug::trace("CUPrinterCommManager", HobString("FludFailureCnt = ") + Integer::toString(cuWakeupMsg->FludFailureCnt));

                //TEST JRB Alex Zavala's code for XPE version
                if (cuWakeupMsg->BitError == 0x11111111)
                {
                    Debug::trace("CUPrinterCommManager", HobString("force printer update"));
                    HobVersion hardwareVersion;
                    HobVersion printerSoftwareVersion;

					/*************** BEGINNING OF CODE CHANGE **************************/
					switch (usbDeviceType)
                    {
						case HOBART_ESS_PRINTER_ID_CU:
						case HOBART_EVO_PRINTER_ID_CU:
						case HOBART_ESC_PRINTER_ID_CU:
							hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
							printerSoftwareVersion.setVersionType(HobVersion::EppPrinterSW);
							break;
						case HOBART_XPE_PRINTER_ID_CU:	//Alex Zavala 8/1
							hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
							printerSoftwareVersion.setVersionType(HobVersion::Epp2PrinterSW);
							break;
					}
					/*************** END OF CODE CHANGE **************************/

					//hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
					//printerSoftwareVersion.setVersionType(HobVersion::EppPrinterSW);

                    hardwareVersion.setMajorRevision(1);
                    hardwareVersion.setMinorRevision(0);

                    printerSoftwareVersion.setMajorRevision(0);
                    printerSoftwareVersion.setMinorRevision(0);

                    //check on printer code
                    PeripheralRequestCodeUpdate reqPrnCodeUpdate(usbNodeId,
                            hardwareVersion,
                            printerSoftwareVersion);
                    dispatchEvent(&reqPrnCodeUpdate);
                }


                else if (cuWakeupMsg->BitError == 0x33333333)
                {
                    Debug::trace("CUPrinterCommManager", HobString("update normally"));

                    HobVersion hardwareVersion;
                    HobVersion bootCodeSoftwareVersion;
                    HobVersion flashUpdateSoftwareVersion;
                    HobVersion printerSoftwareVersion;

                    switch (usbDeviceType)
                    {
                        case HOBART_ESS_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EssPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EssPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EssPrinterSW);
                            break;
                        case HOBART_EVO_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EppPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EppPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EppPrinterSW);
                            break;
							/* Alex Zavala's code for XPE version*/
						case HOBART_XPE_PRINTER_ID_CU:	//Alex Zavala 8/1
							hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
							bootCodeSoftwareVersion.setVersionType(HobVersion::Epp2PrnBootCodeSW);			//these values do not exist
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::Epp2PrnFlashUpdateSW);	//these values do not exist
							printerSoftwareVersion.setVersionType(HobVersion::Epp2PrinterSW);
							break;
                        case HOBART_ESC_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EscPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EscPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EscPrinterSW);
                            break;
                        default:
                            break;
                    }

                    hardwareVersion.setMajorRevision(cuWakeupMsg->HardwareMajorRev);
                    hardwareVersion.setMinorRevision(cuWakeupMsg->HardwareMinorRev);

                    bootCodeSoftwareVersion.setMajorRevision(cuWakeupMsg->BootCodeMajorRev);
                    bootCodeSoftwareVersion.setMinorRevision(cuWakeupMsg->BootCodeMinorRev);

                    flashUpdateSoftwareVersion.setMajorRevision(cuWakeupMsg->FlashUpdateMajorRev);
                    flashUpdateSoftwareVersion.setMinorRevision(cuWakeupMsg->FlashUpdateMinorRev);

                    printerSoftwareVersion.setMajorRevision(cuWakeupMsg->PrinterMajorRev);
                    printerSoftwareVersion.setMinorRevision(cuWakeupMsg->PrinterMinorRev);

                    Debug::trace("CUPrinterCommManager", HobString("hardwareVersion: ") + HobString(hardwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("printer software version: ") + HobString(printerSoftwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("Boot Code software version: ") + HobString(bootCodeSoftwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("FLUD software version: ") + HobString(flashUpdateSoftwareVersion.getRevisionString()));

                    //the printer code has three distinct components (printer, flash updater, and boot)

                    int bootFailureCnt = (cuWakeupMsg->BootPrnFailureCnt & 0xf0) >> 4; //high order nibble
                    int prnFailureCnt = cuWakeupMsg->BootPrnFailureCnt & 0xf; //low order nibble
                    int flud1FailureCnt = (cuWakeupMsg->FludFailureCnt & 0xf0) >> 4; //high order nibble
                    int flud2FailureCnt = cuWakeupMsg->FludFailureCnt & 0xf; //low order nibble
                    int failureCnt = 0;

                    //we simply use the largest of any failure count. If any of the failure counts are above
                    //3 then we leave the printer running the flud. We dont flash or run the printer code.
                    if (flud1FailureCnt > failureCnt)
                        failureCnt = flud1FailureCnt;
                    if (flud2FailureCnt > failureCnt)
                        failureCnt = flud2FailureCnt;
                    if (prnFailureCnt > failureCnt)
                        failureCnt = prnFailureCnt;
                    if (bootFailureCnt > failureCnt)
                        failureCnt = bootFailureCnt;

                    UsbAddComponent addComponent(usbNodeId, hardwareVersion);
                    dispatchEvent(&addComponent);

                    PeripheralRequestCodeUpdate reqBootCodeUpdate(usbNodeId,
                            hardwareVersion,
                            bootCodeSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqBootCodeUpdate);

                    //check on Flash updater code
                    PeripheralRequestCodeUpdate reqFlashCodeUpdate(usbNodeId,
                            hardwareVersion,
                            flashUpdateSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqFlashCodeUpdate);

                    //check on Printer code
                    PeripheralRequestCodeUpdate reqPrnCodeUpdate(usbNodeId,
                            hardwareVersion,
                            printerSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqPrnCodeUpdate);

                }
                else if (cuWakeupMsg->BitError == 0x44444444)
                {
                    Debug::trace("CUPrinterCommManager", HobString("repeat wakeup mode"));
                    testWakeup_ = true;
                }
                else if (cuWakeupMsg->BitError == 0x55555555)
                {
                    Debug::trace("CUPrinterCommManager", HobString("force flud update"));

                    HobVersion hardwareVersion;
                    HobVersion printerSoftwareVersion;
					
					/* Alex Zavala's code for XPE version for commented code below*/
					switch (usbDeviceType)
                    {
                        case HOBART_ESS_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EssPrnFlashUpdateSW);
                            break;
                        case HOBART_EVO_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EppPrnFlashUpdateSW);
                            break;
							/* Alex Zavala's code for XPE version*/
						case HOBART_XPE_PRINTER_ID_CU:	//Alex Zavala 8/1
							hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::Epp2PrnFlashUpdateSW);	//these values do not exist
							break;
                        case HOBART_ESC_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EscPrnFlashUpdateSW);
                            break;
                        default:
                            break;
                    }
                    //hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                    //printerSoftwareVersion.setVersionType(HobVersion::EppPrnFlashUpdateSW);

                    hardwareVersion.setMajorRevision(1);
                    hardwareVersion.setMinorRevision(0);

                    printerSoftwareVersion.setMajorRevision(0);
                    printerSoftwareVersion.setMinorRevision(0);

                    //check on printer code
                    PeripheralRequestCodeUpdate reqPrnCodeUpdate(usbNodeId,
                            hardwareVersion,
                            printerSoftwareVersion);
                    dispatchEvent(&reqPrnCodeUpdate);
                }
                else if (cuWakeupMsg->BitError == 0x66666666)
                {
                    Debug::trace("CUPrinterCommManager", HobString("force boot code update"));
                    HobVersion hardwareVersion;
                    HobVersion printerSoftwareVersion;

					/* Alex Zavala's code for XPE version for commented code below*/
					switch (usbDeviceType)
                    {
                        case HOBART_ESS_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EssPrnBootCodeSW);
                            break;
                        case HOBART_EVO_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EppPrnBootCodeSW);
                            break;
							/* Alex Zavala's code for XPE version*/
						case HOBART_XPE_PRINTER_ID_CU:	//Alex Zavala 8/1
							hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::Epp2PrnBootCodeSW);	//these values do not exist
							break;
                        case HOBART_ESC_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
                            printerSoftwareVersion.setVersionType(HobVersion::EscPrnBootCodeSW);
                            break;
                        default:
                            break;
                    }

                    //hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                    //printerSoftwareVersion.setVersionType(HobVersion::EppPrnBootCodeSW);

                    hardwareVersion.setMajorRevision(1);
                    hardwareVersion.setMinorRevision(0);

                    printerSoftwareVersion.setMajorRevision(0);
                    printerSoftwareVersion.setMinorRevision(0);

                    //check on printer code
                    PeripheralRequestCodeUpdate reqPrnCodeUpdate(usbNodeId,
                            hardwareVersion,
                            printerSoftwareVersion);
                    dispatchEvent(&reqPrnCodeUpdate);
                }
                else if (cuWakeupMsg->BitError == 0x77777777)
                {
                    Debug::trace("CUPrinterCommManager", HobString("force run normal"));
                    setupCodeFlash(CuPrnRunNormal);
                }
                else
                {
                    Debug::trace("CUPrinterCommManager", HobString("normal run"));

                    HobVersion hardwareVersion;
                    HobVersion bootCodeSoftwareVersion;
                    HobVersion flashUpdateSoftwareVersion;
                    HobVersion printerSoftwareVersion;

                    switch (usbDeviceType)
                    {
                        case HOBART_ESS_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EssPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EssPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EssPrinterSW);
                            break;
                        case HOBART_EVO_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EppPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EppPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EppPrinterSW);
                            break;
							/* Alex Zavala's code for XPE version*/
						case HOBART_XPE_PRINTER_ID_CU:// Alex Zavala 8/1
							hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
							bootCodeSoftwareVersion.setVersionType(HobVersion::Epp2PrnBootCodeSW);			//these values do not exist
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::Epp2PrnFlashUpdateSW);	//these values do not exist
							printerSoftwareVersion.setVersionType(HobVersion::Epp2PrinterSW);
							break;
                        case HOBART_ESC_PRINTER_ID_CU:
                            hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
                            bootCodeSoftwareVersion.setVersionType(HobVersion::EscPrnBootCodeSW);
                            flashUpdateSoftwareVersion.setVersionType(HobVersion::EscPrnFlashUpdateSW);
                            printerSoftwareVersion.setVersionType(HobVersion::EscPrinterSW);
                            break;
                        default:
                            break;
                    }

                    hardwareVersion.setMajorRevision(cuWakeupMsg->HardwareMajorRev);
                    hardwareVersion.setMinorRevision(cuWakeupMsg->HardwareMinorRev);

                    bootCodeSoftwareVersion.setMajorRevision(cuWakeupMsg->BootCodeMajorRev);
                    bootCodeSoftwareVersion.setMinorRevision(cuWakeupMsg->BootCodeMinorRev);

                    flashUpdateSoftwareVersion.setMajorRevision(cuWakeupMsg->FlashUpdateMajorRev);
                    flashUpdateSoftwareVersion.setMinorRevision(cuWakeupMsg->FlashUpdateMinorRev);

                    printerSoftwareVersion.setMajorRevision(cuWakeupMsg->PrinterMajorRev);
                    printerSoftwareVersion.setMinorRevision(cuWakeupMsg->PrinterMinorRev);

                    Debug::trace("CUPrinterCommManager", HobString("hardwareVersion: ") + HobString(hardwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("printer software version: ") + HobString(printerSoftwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("Boot Code software version: ") + HobString(bootCodeSoftwareVersion.getRevisionString()));
                    Debug::trace("CUPrinterCommManager", HobString("FLUD software version: ") + HobString(flashUpdateSoftwareVersion.getRevisionString()));

                    //the printer code has three distinct components (printer, flash updater, and boot)

                    int bootFailureCnt = (cuWakeupMsg->BootPrnFailureCnt & 0xf0) >> 4; //high order nibble
                    int prnFailureCnt = cuWakeupMsg->BootPrnFailureCnt & 0xf; //low order nibble
                    int flud1FailureCnt = (cuWakeupMsg->FludFailureCnt & 0xf0) >> 4; //high order nibble
                    int flud2FailureCnt = cuWakeupMsg->FludFailureCnt & 0xf; //low order nibble
                    int failureCnt = 0;

                    //we simply use the largest of any failure count. If any of the failure counts are above
                    //3 then we leave the printer running the flud. We dont flash or run the printer code.
                    if (flud1FailureCnt > failureCnt)
                        failureCnt = flud1FailureCnt;
                    if (flud2FailureCnt > failureCnt)
                        failureCnt = flud2FailureCnt;
                    if (prnFailureCnt > failureCnt)
                        failureCnt = prnFailureCnt;
                    if (bootFailureCnt > failureCnt)
                        failureCnt = bootFailureCnt;

                    UsbAddComponent addComponent(usbNodeId, hardwareVersion);
                    dispatchEvent(&addComponent);

                    PeripheralRequestCodeUpdate reqBootCodeUpdate(usbNodeId,
                            hardwareVersion,
                            bootCodeSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqBootCodeUpdate);

                    //check on Flash updater code
                    PeripheralRequestCodeUpdate reqFlashCodeUpdate(usbNodeId,
                            hardwareVersion,
                            flashUpdateSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqFlashCodeUpdate);

                    //check on Printer code
                    PeripheralRequestCodeUpdate reqPrnCodeUpdate(usbNodeId,
                            hardwareVersion,
                            printerSoftwareVersion,
                            failureCnt);
                    dispatchEvent(&reqPrnCodeUpdate);
                }
                break;
            }

        default:
            Debug::trace("CUPrinterCommManager.Error", HobString("Received Unknown CUPrinter message type: 0x") +
                    HobString(Hex::toString(genMsg->cuEvent)));
            break;
    }
}

bool CUPrinterCommManager::areTransactionsComplete()
{
    if ((pBulkData_ == NULL) && (xmitting_ == false) )
        return true;
    else
        return false;
}

void CUPrinterCommManager::sendReqWakeup()
{
    char str[512];
    DWORD result = OK;
    bool is_custom_code = false;

    CuPrnGenericMsg prnWkup;

    prnWkup.cuEvent = CuPrnWakeup;
    prnWkup.pad0 = 0;

    int sizeOfData = sizeof(prnWkup);

    usbEvolHdr_.setDestinationPhysicalAddress(0x0);
    usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
    usbEvolHdr_.setMessageLength((UINT16)sizeOfData);
    usbEvolHdr_.Serialize(str);

    memcpy(&str[10], (char *) &prnWkup, sizeOfData);
    sendUsbInterruptMessage( ( const char * ) str,  sizeof(prnWkup) + 10, result, is_custom_code );
}
