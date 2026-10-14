//#include "vxWorks.h"
#include "TIRISReadOnlyId.h"
#include "RFIDEvent.h"
#include "Debug.h"

#include "Hex.h"

#include <string>
#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TIRISReadOnlyId::dataType = 0;
std::string TIRISReadOnlyId::dataTypeName("TIRISReadOnlyId"); 
#endif

// Added the RFID software version to this message, FHA 3/2013
// TIRISReadOnlyId::TIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread,
										   //UINT8 * currentId, UINT32 size) : EvolMessage(RFIDReader, RFIDCurrentID, pHobThread)
TIRISReadOnlyId::TIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, UINT8 *currentId, UINT32 size, UINT8 swVersionRFID)
	: EvolMessage(RFIDReader, RFIDCurrentID, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	currentId_.append((const char *)currentId, size);
	TIRIS_SoftwareVersion_ = swVersionRFID;
}

TIRISReadOnlyId::TIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, std::string currentId)
	: EvolMessage(RFIDReader, RFIDCurrentID, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	currentId_ = currentId;
	TIRIS_SoftwareVersion_ = 0;
}

TIRISReadOnlyId::TIRISReadOnlyId(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	std::string::size_type sz = 0;

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sz;
	for (unsigned int i = 0; i < sizeof(sz); i++)
	{
		*pChar++ = buffer[j++];
	}

	currentId_.clear();
	for (unsigned int i = 0; i < sz; i++)
	{
		currentId_.append(1, buffer[j++]);
	}

	// Added the RFID software version to this mesage, FHA 3/13
	//TIRIS_SoftwareVersion_.clear();
	//TIRIS_SoftwareVersion_.append(1, buffer[j++]);
	TIRIS_SoftwareVersion_ = buffer[j++];
}

TIRISReadOnlyId::~TIRISReadOnlyId()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *TIRISReadOnlyId::serialize() const
{
	EvolMessage::serialize();

	std::string::size_type sz = currentId_.size();
	// Now, for the derived class items
	char *pChar = (char *) &sz;
	for (unsigned int i = 0; i < sizeof(sz); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	serializedData.append(currentId_);
	serializedData.append(( const char*)&TIRIS_SoftwareVersion_);

	return serializedData.data();
}

void TIRISReadOnlyId::show()
{
	Debug::addTraceCategory("TIRISReadOnlyId");
	Debug::trace("TIRISReadOnlyId", "TIRISReadOnlyId: ");

	EvolMessage::show();

	std::string msg("id = ");

	Hex::nibblize(msg, currentId_.data(), (int)currentId_.size(), true);
	Debug::trace("TIRISReadOnlyId", msg + "Received SW version = " + util_string::uinttohexstring((const unsigned char)TIRIS_SoftwareVersion_));


	Debug::removeTraceCategory("TIRISReadOnlyId");
}


