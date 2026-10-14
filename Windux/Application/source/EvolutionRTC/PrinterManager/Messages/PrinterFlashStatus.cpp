//#include "vxWorks.h"
#include "PrinterFlashStatus.h"
#include "Printer.h"
#include "SystemEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "HobVersion.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterFlashStatus::dataType = 0;
std::string PrinterFlashStatus::dataTypeName("PrinterFlashStatus"); 
#endif

PrinterFlashStatus::PrinterFlashStatus(Hobart::CEvolMsgReceiver *pHobThread, PrinterKind prKind, SysFlashStatusMsg* pMsg): EvolMessage(PrinterSystem, SysFlashStatus, pHobThread),
BootStoredChecksum_(pMsg->BootStoredChecksum),
BootCalculatedChecksum_(pMsg->BootCalculatedChecksum),
FLUD1StoredChecksum_(pMsg->FLUD1StoredChecksum),
FLUD1CalculatedChecksum_(pMsg->FLUD1CalculatedChecksum),
FLUD2StoredChecksum_(pMsg->FLUD2StoredChecksum),
FLUD2CalculatedChecksum_(pMsg->FLUD2CalculatedChecksum),
PrinterStoredChecksum_(pMsg->PrinterStoredChecksum),
PrinterCalculatedChecksum_(pMsg->PrinterCalculatedChecksum),
FlashManufacturerID_(pMsg->FlashManufacturerID),
FlashDeviceID_(pMsg->FlashDeviceID),
SelectedFLUD_(pMsg->SelectedFLUD),
FLUD1Status_(pMsg->FLUD1Status),
FLUD2Status_(pMsg->FLUD2Status),
BPUpdateFailures_(pMsg->BPUpdateFailures),
F1F2UpdateFailures_(pMsg->F1F2UpdateFailures),
SysConfigStatus_(pMsg->SysConfigStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	BootCodeRevision_.setMajorRevision(pMsg->BootCodeMajorRevision);
	BootCodeRevision_.setMinorRevision(pMsg->BootCodeMinorRevision);

	FLUD1Revision_.setMajorRevision(pMsg->FLUD1MajorRevision);
	FLUD1Revision_.setMinorRevision(pMsg->FLUD1MinorRevision);

	FLUD2Revision_.setMajorRevision(pMsg->FLUD2MajorRevision);
	FLUD2Revision_.setMinorRevision(pMsg->FLUD2MinorRevision);

	PrinterRevision_.setMajorRevision(pMsg->PrinterMajorRevision);
	PrinterRevision_.setMinorRevision(pMsg->PrinterMinorRevision);

	if (prKind == EppPrinter)
	{
		BootCodeRevision_.setVersionType(HobVersion::EppPrnBootCodeSW);
		FLUD1Revision_.setVersionType(HobVersion::EppPrnFlashUpdateSW);
		FLUD2Revision_.setVersionType(HobVersion::EppPrnFlashUpdateSW);
		PrinterRevision_.setVersionType(HobVersion::EppPrinterSW);
	}
	/* Alex Zavala's code for XPE version */
	else if (prKind == Epp2Printer)
	{
		BootCodeRevision_.setVersionType(HobVersion::Epp2PrnBootCodeSW);	//These values do not exist
		FLUD1Revision_.setVersionType(HobVersion::Epp2PrnFlashUpdateSW);	//These values do not exist
		FLUD2Revision_.setVersionType(HobVersion::Epp2PrnFlashUpdateSW);	//These values do not exist
		PrinterRevision_.setVersionType(HobVersion::Epp2PrinterSW);
	}
	else if (prKind == EssPrinter)
	{
		BootCodeRevision_.setVersionType(HobVersion::EssPrnBootCodeSW);
		FLUD1Revision_.setVersionType(HobVersion::EssPrnFlashUpdateSW);
		FLUD2Revision_.setVersionType(HobVersion::EssPrnFlashUpdateSW);
		PrinterRevision_.setVersionType(HobVersion::EssPrinterSW);
	}
	else if (prKind == EscPrinter)
	{
		BootCodeRevision_.setVersionType(HobVersion::EscPrnBootCodeSW);
		FLUD1Revision_.setVersionType(HobVersion::EscPrnFlashUpdateSW);
		FLUD2Revision_.setVersionType(HobVersion::EscPrnFlashUpdateSW);
		PrinterRevision_.setVersionType(HobVersion::EscPrinterSW);

	}
	else if (prKind == ApcPrinter)
	{
		BootCodeRevision_.setVersionType(HobVersion::ApcPrnBootCodeSW);
		FLUD1Revision_.setVersionType(HobVersion::ApcPrnFlashUpdateSW);
		FLUD2Revision_.setVersionType(HobVersion::ApcPrnFlashUpdateSW);
		PrinterRevision_.setVersionType(HobVersion::ApcPrinterSW);

	}
	else
	{

		BootCodeRevision_.setVersionType(HobVersion::UnknownComponent);
		FLUD1Revision_.setVersionType(HobVersion::UnknownComponent);
		FLUD2Revision_.setVersionType(HobVersion::UnknownComponent);
		PrinterRevision_.setVersionType(HobVersion::UnknownComponent);

	}


}

PrinterFlashStatus::PrinterFlashStatus(const char *buffer): EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();


	// Now, for the derived class items

	BootCodeRevision_ = HobVersion(&buffer[j]);
	BootCodeRevision_.serialize();
	int mLength = BootCodeRevision_.getSerializedLength();
	j += mLength;

	char *pChar = (char *) &BootStoredChecksum_;
	for (unsigned int i = 0; i < sizeof(BootStoredChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &BootCalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(BootCalculatedChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	FLUD1Revision_ = HobVersion(&buffer[j]);
	FLUD1Revision_.serialize();
	mLength = FLUD1Revision_.getSerializedLength();
	j += mLength;

	pChar = (char *) &FLUD1StoredChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD1StoredChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FLUD1CalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD1CalculatedChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	FLUD2Revision_ = HobVersion(&buffer[j]);
	FLUD2Revision_.serialize();
	mLength = FLUD2Revision_.getSerializedLength();
	j += mLength;

	pChar = (char *) &FLUD2StoredChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD2StoredChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FLUD2CalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD2CalculatedChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	PrinterRevision_ = HobVersion(&buffer[j]);
	PrinterRevision_.serialize();
	mLength = PrinterRevision_.getSerializedLength();
	j += mLength;

	pChar = (char *) &PrinterStoredChecksum_;
	for (unsigned int i = 0; i < sizeof(PrinterStoredChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &PrinterCalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(PrinterCalculatedChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FlashManufacturerID_;
	for (unsigned int i = 0; i < sizeof(FlashManufacturerID_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FlashDeviceID_;
	for (unsigned int i = 0; i < sizeof(FlashDeviceID_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &SelectedFLUD_;
	for (unsigned int i = 0; i < sizeof(SelectedFLUD_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FLUD1Status_;
	for (unsigned int i = 0; i < sizeof(FLUD1Status_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &FLUD2Status_;
	for (unsigned int i = 0; i < sizeof(FLUD2Status_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &BPUpdateFailures_;
	for (unsigned int i = 0; i < sizeof(BPUpdateFailures_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &F1F2UpdateFailures_;
	for (unsigned int i = 0; i < sizeof(F1F2UpdateFailures_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &SysConfigStatus_;
	for (unsigned int i = 0; i < sizeof(SysConfigStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PrinterFlashStatus::~PrinterFlashStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterFlashStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pCC = BootCodeRevision_.serialize();
	int mLength = BootCodeRevision_.getSerializedLength();

	serializedData.append(pCC, mLength);


	char *pChar = (char *) &BootStoredChecksum_;
	for (unsigned int i = 0; i < sizeof(BootStoredChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &BootCalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(BootCalculatedChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	pCC = FLUD1Revision_.serialize();
	mLength = FLUD1Revision_.getSerializedLength();

	serializedData.append(pCC, mLength);


	pChar = (char *) &FLUD1StoredChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD1StoredChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FLUD1CalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD1CalculatedChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pCC = FLUD2Revision_.serialize();
	mLength = FLUD2Revision_.getSerializedLength();

	serializedData.append(pCC, mLength);


	pChar = (char *) &FLUD2StoredChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD2StoredChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FLUD2CalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(FLUD2CalculatedChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pCC = PrinterRevision_.serialize();
	mLength = PrinterRevision_.getSerializedLength();

	serializedData.append(pCC, mLength);


	pChar = (char *) &PrinterStoredChecksum_;
	for (unsigned int i = 0; i < sizeof(PrinterStoredChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &PrinterCalculatedChecksum_;
	for (unsigned int i = 0; i < sizeof(PrinterCalculatedChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FlashManufacturerID_;
	for (unsigned int i = 0; i < sizeof(FlashManufacturerID_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FlashDeviceID_;
	for (unsigned int i = 0; i < sizeof(FlashDeviceID_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &SelectedFLUD_;
	for (unsigned int i = 0; i < sizeof(SelectedFLUD_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FLUD1Status_;
	for (unsigned int i = 0; i < sizeof(FLUD1Status_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &FLUD2Status_;
	for (unsigned int i = 0; i < sizeof(FLUD2Status_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &BPUpdateFailures_;
	for (unsigned int i = 0; i < sizeof(BPUpdateFailures_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &F1F2UpdateFailures_;
	for (unsigned int i = 0; i < sizeof(F1F2UpdateFailures_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &SysConfigStatus_;
	for (unsigned int i = 0; i < sizeof(SysConfigStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterFlashStatus::show()
{
	bool isTracing = Debug::isTracing("PrinterFlashStatus");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterFlashStatus") );
#else
		Debug::addTraceCategory("PrinterFlashStatus");
#endif
	}

	Debug::trace("PrinterFlashStatus", "PrinterFlashStatus: ");


	Debug::trace("PrinterFlashStatus", HobString("BootCodeRevision_: ") + HobString(BootCodeRevision_.toString()->c_str()));
	Debug::trace("PrinterFlashStatus", HobString("BootStoredChecksum_: ") + Integer::toString(BootStoredChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("BootCalculatedChecksum_: ") + Integer::toString(BootCalculatedChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD1Revision_: ") + HobString(FLUD1Revision_.toString()->c_str()));
	Debug::trace("PrinterFlashStatus", HobString("FLUD1StoredChecksum_: ") + Integer::toString(FLUD1StoredChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD1CalculatedChecksum_: ") + Integer::toString(FLUD1CalculatedChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD2Revision_: ") + HobString(FLUD2Revision_.toString()->c_str()));
	Debug::trace("PrinterFlashStatus", HobString("FLUD2StoredChecksum_: ") + Integer::toString(FLUD2StoredChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD2CalculatedChecksum_: ") + Integer::toString(FLUD2CalculatedChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("PrinterRevision_: ") + HobString(PrinterRevision_.toString()->c_str()));
	Debug::trace("PrinterFlashStatus", HobString("PrinterStoredChecksum_: ") + Integer::toString(PrinterStoredChecksum_));
	Debug::trace("PrinterFlashStatus", HobString("PrinterCalculatedChecksum_: ") + Integer::toString(PrinterCalculatedChecksum_));

	Debug::trace("PrinterFlashStatus", HobString("FlashManufacturerID_: ") + Integer::toString(FlashManufacturerID_));
	Debug::trace("PrinterFlashStatus", HobString("FlashDeviceID_: ") + Integer::toString(FlashDeviceID_));
	Debug::trace("PrinterFlashStatus", HobString("SelectedFLUD_: ") + Integer::toString(SelectedFLUD_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD1Status_: ") + Integer::toString(FLUD1Status_));
	Debug::trace("PrinterFlashStatus", HobString("FLUD2Status_: ") + Integer::toString(FLUD2Status_));
	Debug::trace("PrinterFlashStatus", HobString("BPUpdateFailures_: ") + Integer::toString(BPUpdateFailures_));
	Debug::trace("PrinterFlashStatus", HobString("F1F2UpdateFailures_: ") + Integer::toString(F1F2UpdateFailures_));
	Debug::trace("PrinterFlashStatus", HobString("SysConfigStatus_: ") + Integer::toString(SysConfigStatus_));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterFlashStatus") );
#else
		Debug::removeTraceCategory("PrinterFlashStatus");
#endif
	}
}


HobVersion& PrinterFlashStatus::getSelectedFLUDRevision()
{
	if (SelectedFLUD_ == 1)
		return FLUD1Revision_;
	else
		return FLUD2Revision_;
}