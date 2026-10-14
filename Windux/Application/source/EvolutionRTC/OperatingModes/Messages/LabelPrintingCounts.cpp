#include "evolution.h"
#include "OpModeEvent.h"
#include "LabelPrintingCounts.h"
#include "../../db/util/e_string/utilstring.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelPrintingCounts::dataType = 0;
std::string LabelPrintingCounts::dataTypeName("LabelPrintingCounts");
#endif

LabelPrintingCounts::LabelPrintingCounts(Hobart::CEvolMsgReceiver *pHobThread, const int currentPrinted, const int totalLabels) : EvolMessage(OperationMode, OpModeLabelsPrintedCounts, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	currentPrintedCount = currentPrinted;
	totalLabelsCount = totalLabels;
}

LabelPrintingCounts::LabelPrintingCounts(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &currentPrintedCount;
	for (unsigned int i = 0; i < sizeof(currentPrintedCount); i++)
		*pChar++ = buffer[j++];

	pChar = (char *)&totalLabelsCount;
	for (unsigned int i = 0; i < sizeof(totalLabelsCount); i++)
		*pChar++ = buffer[j++];
}

LabelPrintingCounts::~LabelPrintingCounts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelPrintingCounts::serialize() const
{
	serializedData = "";

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *)&currentPrintedCount;
	for (unsigned int i = 0; i < sizeof(currentPrintedCount); i++)
		serializedData.append(1, pChar[i]);

	pChar = (char *)&totalLabelsCount;
	for (unsigned int i = 0; i < sizeof(totalLabelsCount); i++)
		serializedData.append(1, pChar[i]);
	return serializedData.data();
}
