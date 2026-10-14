//#include "vxWorks.h"
#include "PrinterStockMetrics.h"
#include "Configuration.h"

#include "Debug.h"
#include "Integer.h"
#include "UInteger.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterStockMetrics::dataType = 0;
std::string PrinterStockMetrics::dataTypeName("PrinterStockMetrics"); 
#endif

UINT16 PrinterStockMetrics::MaxLength1_000 = 0;
UINT16 PrinterStockMetrics::MaxLength1_500 = 0;
UINT16 PrinterStockMetrics::MaxLength1_750 = 0;
UINT16 PrinterStockMetrics::MaxLength2_000 = 0;
UINT16 PrinterStockMetrics::MaxLength2_375 = 0;
UINT16 PrinterStockMetrics::MaxLength2_500 = 0;
UINT16 PrinterStockMetrics::MaxLength3_000 = 0;
UINT16 PrinterStockMetrics::MaxLength3_500 = 0;
UINT16 PrinterStockMetrics::MaxLength4_000 = 0;
UINT16 PrinterStockMetrics::MaxLength4_500 = 0;
UINT16 PrinterStockMetrics::MaxLength5_000 = 0;
UINT16 PrinterStockMetrics::MaxLength5_500 = 0;
UINT16 PrinterStockMetrics::MaxLength6_000 = 0;
UINT16 PrinterStockMetrics::MaxLength6_500 = 0;
UINT16 PrinterStockMetrics::MaxLength7_000 = 0;
UINT16 PrinterStockMetrics::MaxLength7_500 = 0;
UINT16 PrinterStockMetrics::MaxLength8_000 = 0;
UINT16 PrinterStockMetrics::MaxLength8_500 = 0;
UINT16 PrinterStockMetrics::MaxLength9_000 = 0;
UINT16 PrinterStockMetrics::MaxLength9_500 = 0;
UINT16 PrinterStockMetrics::MaxLength10_000 = 0;

UINT16 PrinterStockMetrics::MinLength1_000 = 0;
UINT16 PrinterStockMetrics::MinLength1_500 = 0;
UINT16 PrinterStockMetrics::MinLength1_750 = 0;
UINT16 PrinterStockMetrics::MinLength2_000 = 0;
UINT16 PrinterStockMetrics::MinLength2_375 = 0;
UINT16 PrinterStockMetrics::MinLength2_500 = 0;
UINT16 PrinterStockMetrics::MinLength3_000 = 0;
UINT16 PrinterStockMetrics::MinLength3_500 = 0;
UINT16 PrinterStockMetrics::MinLength4_000 = 0;
UINT16 PrinterStockMetrics::MinLength4_500 = 0;
UINT16 PrinterStockMetrics::MinLength5_000 = 0;
UINT16 PrinterStockMetrics::MinLength5_500 = 0;
UINT16 PrinterStockMetrics::MinLength6_000 = 0;
UINT16 PrinterStockMetrics::MinLength6_500 = 0;
UINT16 PrinterStockMetrics::MinLength7_000 = 0;
UINT16 PrinterStockMetrics::MinLength7_500 = 0;
UINT16 PrinterStockMetrics::MinLength8_000 = 0;
UINT16 PrinterStockMetrics::MinLength8_500 = 0;
UINT16 PrinterStockMetrics::MinLength9_000 = 0;
UINT16 PrinterStockMetrics::MinLength9_500 = 0;
UINT16 PrinterStockMetrics::MinLength10_000 = 0;

//KPC
UINT16 PrinterStockMetrics::MaxLength_KPC_1_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_1_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_1_750 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_2_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_2_375 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_2_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_3_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_3_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_4_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_4_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_5_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_5_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_6_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_6_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_7_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_7_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_8_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_8_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_9_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_9_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_KPC_10_000 = 0;

UINT16 PrinterStockMetrics::MinLength_KPC_1_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_1_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_1_750 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_2_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_2_375 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_2_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_3_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_3_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_4_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_4_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_5_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_5_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_6_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_6_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_7_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_7_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_8_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_8_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_9_000 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_9_500 = 0;
UINT16 PrinterStockMetrics::MinLength_KPC_10_000 = 0;

//FSSK Free Standing Self Service Printer (Kinetis)
UINT16 PrinterStockMetrics::MaxLength_FSSK_1_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_1_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_1_750 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_2_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_2_375 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_2_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_3_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_3_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_4_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_4_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_5_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_5_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_6_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_6_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_7_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_7_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_8_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_8_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_9_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_9_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_FSSK_10_000 = 0;

UINT16 PrinterStockMetrics::MinLength_FSSK_1_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_1_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_1_750 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_2_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_2_375 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_2_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_3_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_3_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_4_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_4_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_5_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_5_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_6_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_6_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_7_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_7_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_8_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_8_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_9_000 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_9_500 = 0;
UINT16 PrinterStockMetrics::MinLength_FSSK_10_000 = 0;

// HT-171: Changes for EPP
UINT16 PrinterStockMetrics::MaxLength_EPP_1_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_1_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_1_750 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_2_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_2_375 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_2_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_3_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_3_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_4_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_4_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_5_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_5_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_6_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_6_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_7_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_7_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_8_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_8_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_9_000 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_9_500 = 0;
UINT16 PrinterStockMetrics::MaxLength_EPP_10_000 = 0;

UINT16 PrinterStockMetrics::MinLength_EPP_1_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_1_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_1_750 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_2_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_2_375 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_2_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_3_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_3_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_4_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_4_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_5_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_5_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_6_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_6_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_7_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_7_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_8_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_8_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_9_000 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_9_500 = 0;
UINT16 PrinterStockMetrics::MinLength_EPP_10_000 = 0;



PrinterStockMetrics::PrinterStockMetrics(Hobart::CEvolMsgReceiver *pHobThread,
										 PrinterType printerType,
										 LabelWidth labelWidth,
										 UINT16 labelWidthInDots,
										 UINT16 labelLengthInLines) :	PrinterGeneric(PrStockMetrics, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->labelWidth = labelWidth;
	this->labelWidthInDots = labelWidthInDots;
	this->labelLengthInLines = labelLengthInLines;
}

PrinterStockMetrics::PrinterStockMetrics(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelWidthInDots;
	for (unsigned int i = 0; i < sizeof(labelWidthInDots); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelLengthInLines;
	for (unsigned int i = 0; i < sizeof(labelLengthInLines); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterStockMetrics::~PrinterStockMetrics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

// static
void PrinterStockMetrics::initStatics()
{
	Configuration* config = Configuration::getInstance();
	
	PrinterStockMetrics::MaxLength1_000 = 0;
	PrinterStockMetrics::MaxLength1_500 = 0;
	PrinterStockMetrics::MaxLength1_750 = 0;
	PrinterStockMetrics::MaxLength2_000 = 0;
	PrinterStockMetrics::MaxLength2_375 = 0;
	PrinterStockMetrics::MaxLength2_500 = 0;
	PrinterStockMetrics::MaxLength3_000 = 0;
	PrinterStockMetrics::MaxLength3_500 = 0;
	PrinterStockMetrics::MaxLength4_000 = 0;
	PrinterStockMetrics::MaxLength4_500 = 0;
	PrinterStockMetrics::MaxLength5_000 = 0;
	PrinterStockMetrics::MaxLength5_500 = 0;
	PrinterStockMetrics::MaxLength6_000 = 0;
	PrinterStockMetrics::MaxLength6_500 = 0;
	PrinterStockMetrics::MaxLength7_000 = 0;
	PrinterStockMetrics::MaxLength7_500 = 0;
	PrinterStockMetrics::MaxLength8_000 = 0;
	PrinterStockMetrics::MaxLength8_500 = 0;
	PrinterStockMetrics::MaxLength9_000 = 0;
	PrinterStockMetrics::MaxLength9_500 = 0;
	PrinterStockMetrics::MaxLength10_000 = 0;
	
	PrinterStockMetrics::MinLength1_000 = 0;
	PrinterStockMetrics::MinLength1_500 = 0;
	PrinterStockMetrics::MinLength1_750 = 0;
	PrinterStockMetrics::MinLength2_000 = 0;
	PrinterStockMetrics::MinLength2_375 = 0;
	PrinterStockMetrics::MinLength2_500 = 0;
	PrinterStockMetrics::MinLength3_000 = 0;
	PrinterStockMetrics::MinLength3_500 = 0;
	PrinterStockMetrics::MinLength4_000 = 0;
	PrinterStockMetrics::MinLength4_500 = 0;
	PrinterStockMetrics::MinLength5_000 = 0;
	PrinterStockMetrics::MinLength5_500 = 0;
	PrinterStockMetrics::MinLength6_000 = 0;
	PrinterStockMetrics::MinLength6_500 = 0;
	PrinterStockMetrics::MinLength7_000 = 0;
	PrinterStockMetrics::MinLength7_500 = 0;
	PrinterStockMetrics::MinLength8_000 = 0;
	PrinterStockMetrics::MinLength8_500 = 0;
	PrinterStockMetrics::MinLength9_000 = 0;
	PrinterStockMetrics::MinLength9_500 = 0;
	PrinterStockMetrics::MinLength10_000 = 0;

	// HT-171: Changes for EPP
	PrinterStockMetrics::MaxLength_EPP_1_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_1_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_1_750 = 0;
	PrinterStockMetrics::MaxLength_EPP_2_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_2_375 = 0;
	PrinterStockMetrics::MaxLength_EPP_2_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_3_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_3_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_4_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_4_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_5_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_5_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_6_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_6_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_7_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_7_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_8_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_8_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_9_000 = 0;
	PrinterStockMetrics::MaxLength_EPP_9_500 = 0;
	PrinterStockMetrics::MaxLength_EPP_10_000 = 0;
	
	PrinterStockMetrics::MinLength_EPP_1_000 = 0;
	PrinterStockMetrics::MinLength_EPP_1_500 = 0;
	PrinterStockMetrics::MinLength_EPP_1_750 = 0;
	PrinterStockMetrics::MinLength_EPP_2_000 = 0;
	PrinterStockMetrics::MinLength_EPP_2_375 = 0;
	PrinterStockMetrics::MinLength_EPP_2_500 = 0;
	PrinterStockMetrics::MinLength_EPP_3_000 = 0;
	PrinterStockMetrics::MinLength_EPP_3_500 = 0;
	PrinterStockMetrics::MinLength_EPP_4_000 = 0;
	PrinterStockMetrics::MinLength_EPP_4_500 = 0;
	PrinterStockMetrics::MinLength_EPP_5_000 = 0;
	PrinterStockMetrics::MinLength_EPP_5_500 = 0;
	PrinterStockMetrics::MinLength_EPP_6_000 = 0;
	PrinterStockMetrics::MinLength_EPP_6_500 = 0;
	PrinterStockMetrics::MinLength_EPP_7_000 = 0;
	PrinterStockMetrics::MinLength_EPP_7_500 = 0;
	PrinterStockMetrics::MinLength_EPP_8_000 = 0;
	PrinterStockMetrics::MinLength_EPP_8_500 = 0;
	PrinterStockMetrics::MinLength_EPP_9_000 = 0;
	PrinterStockMetrics::MinLength_EPP_9_500 = 0;
	PrinterStockMetrics::MinLength_EPP_10_000 = 0;
	
	//KPC
	PrinterStockMetrics::MaxLength_KPC_1_000 = OneInchLength_ + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_1_500 = OneInchLength_ + HalfInchLength_ + QuarterInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_1_750 = (OneInchLength_ * 2 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_2_000 = (OneInchLength_ * 2 ) + ( 3 * EighthInchLength_ ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_2_375 = (OneInchLength_ * 2 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_2_500 = (OneInchLength_ * 3 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_3_000 = (OneInchLength_ * 3 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_3_500 = (OneInchLength_ * 4 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_4_000 = (OneInchLength_ * 4 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_4_500 = (OneInchLength_ * 5 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_5_000 = (OneInchLength_ * 5 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_5_500 = (OneInchLength_ * 6 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_6_000 = (OneInchLength_ * 6 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_6_500 = (OneInchLength_ * 7 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_7_000 = (OneInchLength_ * 7 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_7_500 = (OneInchLength_ * 8 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_8_000 = (OneInchLength_ * 8 ) + HalfInchLength_ - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_8_500 = (OneInchLength_ * 9 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_9_000 = (OneInchLength_ * 9 ) + HalfInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_9_500 = (OneInchLength_ * 10 ) - SixteenthInchLength_ - 1;
	PrinterStockMetrics::MaxLength_KPC_10_000 =(OneInchLength_ * 11 ) - SixteenthInchLength_;

	PrinterStockMetrics::MinLength_KPC_1_000 = OneInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_1_500 = OneInchLength_ + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_1_750 = OneInchLength_ + HalfInchLength_ + QuarterInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_2_000 = (OneInchLength_ * 2 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_2_375 = (OneInchLength_ * 2 ) + ( 3 * EighthInchLength_ ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_2_500 = (OneInchLength_ * 2 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_3_000 = (OneInchLength_ * 3 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_3_500 = (OneInchLength_ * 3 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_4_000 = (OneInchLength_ * 4 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_4_500 = (OneInchLength_ * 4 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_5_000 = (OneInchLength_ * 5 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_5_500 = (OneInchLength_ * 5 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_6_000 = (OneInchLength_ * 6 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_6_500 = (OneInchLength_ * 6 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_7_000 = (OneInchLength_ * 7 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_7_500 = (OneInchLength_ * 7 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_8_000 = (OneInchLength_ * 8 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_8_500 = (OneInchLength_ * 8 ) + HalfInchLength_ - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_9_000 = (OneInchLength_ * 9 ) - SixteenthInchLength_;
	PrinterStockMetrics::MinLength_KPC_9_500 = (OneInchLength_ * 9 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_KPC_10_000 =(OneInchLength_ * 10 ) - SixteenthInchLength_;

	//FSSK
	PrinterStockMetrics::MaxLength_FSSK_1_000 = (OneInchLength_ + HalfInchLength_) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_1_500 = (OneInchLength_ + HalfInchLength_ + QuarterInchLength_) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_1_750 = (OneInchLength_ * 2 ) - SixteenthInch_x3_len + 10;
	PrinterStockMetrics::MaxLength_FSSK_2_000 = (OneInchLength_ * 2 ) + ( 3 * EighthInchLength_ ) - SixteenthInch_x3_len - 1;
	//SOF-5119
	//PrinterStockMetrics::MaxLength_FSSK_2_375 = (OneInchLength_ * 2 ) + HalfInchLength_ - ( SixteenthInch_x3_len - 20 );
	PrinterStockMetrics::MaxLength_FSSK_2_375 = (OneInchLength_ * 2 ) + HalfInchLength_ + 25;
	PrinterStockMetrics::MaxLength_FSSK_2_500 = (OneInchLength_ * 3 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_3_000 = (OneInchLength_ * 3 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_3_500 = (OneInchLength_ * 4 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_4_000 = (OneInchLength_ * 4 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_4_500 = (OneInchLength_ * 5 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_5_000 = (OneInchLength_ * 5 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_5_500 = (OneInchLength_ * 6 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_6_000 = (OneInchLength_ * 6 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_6_500 = (OneInchLength_ * 7 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_7_000 = (OneInchLength_ * 7 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_7_500 = (OneInchLength_ * 8 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_8_000 = (OneInchLength_ * 8 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_8_500 = (OneInchLength_ * 9 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_9_000 = (OneInchLength_ * 9 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_9_500 = (OneInchLength_ * 10 ) - SixteenthInch_x3_len - 1;
	PrinterStockMetrics::MaxLength_FSSK_10_000 =(OneInchLength_ * 10 ) + HalfInchLength_ - SixteenthInch_x3_len - 1;

	PrinterStockMetrics::MinLength_FSSK_1_000 = OneInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_1_500 = OneInchLength_ + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_1_750 = OneInchLength_ + HalfInchLength_ + QuarterInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_2_000 = (OneInchLength_ * 2 ) - ( SixteenthInch_x3_len - 11 );
	PrinterStockMetrics::MinLength_FSSK_2_375 = (OneInchLength_ * 2 ) + ( 3 * EighthInchLength_ ) - SixteenthInch_x3_len;
	//SOF-5119
	PrinterStockMetrics::MinLength_FSSK_2_500 = (OneInchLength_ * 2 ) + HalfInchLength_ + 26;
	//PrinterStockMetrics::MinLength_FSSK_2_500 = (OneInchLength_ * 2 ) + HalfInchLength_ - ( SixteenthInch_x3_len - 19 );
	PrinterStockMetrics::MinLength_FSSK_3_000 = (OneInchLength_ * 3 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_3_500 = (OneInchLength_ * 3 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_4_000 = (OneInchLength_ * 4 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_4_500 = (OneInchLength_ * 4 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_5_000 = (OneInchLength_ * 5 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_5_500 = (OneInchLength_ * 5 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_6_000 = (OneInchLength_ * 6 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_6_500 = (OneInchLength_ * 6 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_7_000 = (OneInchLength_ * 7 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_7_500 = (OneInchLength_ * 7 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_8_000 = (OneInchLength_ * 8 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_8_500 = (OneInchLength_ * 8 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_9_000 = (OneInchLength_ * 9 ) - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_9_500 = (OneInchLength_ * 9 ) + HalfInchLength_ - SixteenthInch_x3_len;
	PrinterStockMetrics::MinLength_FSSK_10_000 =(OneInchLength_ * 10 ) - SixteenthInch_x3_len;

	bool enableLabelLength1_000 = true;
	bool enableLabelLength1_500 = true;
	bool enableLabelLength1_750 = true;
	bool enableLabelLength2_000 = true;
	bool enableLabelLength2_375 = true;
	bool enableLabelLength2_500 = true;
	bool enableLabelLength3_000 = true;
	bool enableLabelLength3_500 = true;
	bool enableLabelLength4_000 = true;
	bool enableLabelLength4_500 = true;
	bool enableLabelLength5_000 = true;
	bool enableLabelLength5_500 = true;
	bool enableLabelLength6_000 = true;
	bool enableLabelLength6_500 = true;
	bool enableLabelLength7_000 = true;
	bool enableLabelLength7_500 = true;
	bool enableLabelLength8_000 = true;
	bool enableLabelLength8_500 = true;
	bool enableLabelLength9_000 = true;
	bool enableLabelLength9_500 = true;
	bool enableLabelLength10_00 = true;
	bool enableLabelLengthContinuous = true;

	UINT16 dimBLength1_000 = 0;
	UINT16 dimBLength1_500 = 0;
	UINT16 dimBLength1_750 = 0;
	UINT16 dimBLength2_000 = 0;
	UINT16 dimBLength2_375 = 0;
	UINT16 dimBLength2_500 = 0;
	UINT16 dimBLength3_000 = 0;
	UINT16 dimBLength3_500 = 0;
	UINT16 dimBLength4_000 = 0;
	UINT16 dimBLength4_500 = 0;
	UINT16 dimBLength5_000 = 0;
	UINT16 dimBLength5_500 = 0;
	UINT16 dimBLength6_000 = 0;
	UINT16 dimBLength6_500 = 0;
	UINT16 dimBLength7_000 = 0;
	UINT16 dimBLength7_500 = 0;
	UINT16 dimBLength8_000 = 0;
	UINT16 dimBLength8_500 = 0;
	UINT16 dimBLength9_000 = 0;
	UINT16 dimBLength9_500 = 0;
	UINT16 dimBLength10_00 = 0;
	UINT16 dimBLengthContinuous = 0;
	
	try	{	 dimBLength1_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH1_000);	}
	catch (std::exception) {}
	try	{	 dimBLength1_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH1_500);	}
	catch (std::exception) {}
	try	{	 dimBLength1_750 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH1_750);	}
	catch (std::exception) {}
	try	{	 dimBLength2_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH2_000);	}
	catch (std::exception) {}
	try	{	 dimBLength2_375 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH2_375);	}
	catch (std::exception) {}
	try	{	 dimBLength2_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH2_500);	}
	catch (std::exception) {}
	try	{	 dimBLength3_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH3_000);	}
	catch (std::exception) {}
	try	{	 dimBLength3_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH3_500);	}
	catch (std::exception) {}
	try	{	 dimBLength4_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH4_000);	}
	catch (std::exception) {}
	try	{	 dimBLength4_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH4_500);	}
	catch (std::exception) {}
	try	{	 dimBLength5_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH5_000);	}
	catch (std::exception) {}
	try	{	 dimBLength5_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH5_500);	}
	catch (std::exception) {}
	try	{	 dimBLength6_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH6_000);	}
	catch (std::exception) {}
	try	{	 dimBLength6_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH6_500);	}
	catch (std::exception) {}
	try	{	 dimBLength7_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH7_000);	}
	catch (std::exception) {}
	try	{	 dimBLength7_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH7_500);	}
	catch (std::exception) {}
	try	{	 dimBLength8_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH8_000);	}
	catch (std::exception) {}
	try	{	dimBLength8_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH8_500);	}
	catch (std::exception) {}
	try	{	 dimBLength9_000 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH9_000);	}
	catch (std::exception) {}
	try	{	 dimBLength9_500 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH9_500);	}
	catch (std::exception) {}
	try	{	 dimBLength10_00 = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTH10_00);	}
	catch (std::exception) {}
	try	{	 dimBLengthContinuous = (UINT16) config->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);	}
	catch (std::exception) {}

	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_1_000) || dimBLength1_000 == 0 ) 
		enableLabelLength1_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_1_500) || dimBLength1_500 == 0 ) 
		enableLabelLength1_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_1_750) || dimBLength1_750 == 0 ) 
		enableLabelLength1_750 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_2_000) || dimBLength2_000 == 0 ) 
		enableLabelLength2_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_2_375) || dimBLength2_375 == 0 ) 
		enableLabelLength2_375 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_2_500) || dimBLength2_500 == 0 ) 
		enableLabelLength2_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_3_000) || dimBLength3_000 == 0 ) 
		enableLabelLength3_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_3_500) || dimBLength3_500 == 0 ) 
		enableLabelLength3_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_4_000) || dimBLength4_000 == 0 ) 
		enableLabelLength4_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_4_500) || dimBLength4_500 == 0 ) 
		enableLabelLength4_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_5_000) || dimBLength5_000 == 0 ) 
		enableLabelLength5_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_5_500) || dimBLength5_500 == 0 ) 
		enableLabelLength5_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_6_000) || dimBLength6_000 == 0 ) 
		enableLabelLength6_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_6_500) || dimBLength6_500 == 0 ) 
		enableLabelLength6_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_7_000) || dimBLength7_000 == 0 ) 
		enableLabelLength7_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_7_500) || dimBLength7_500 == 0 ) 
		enableLabelLength7_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_8_000) || dimBLength8_000 == 0 ) 
		enableLabelLength8_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_8_500) || dimBLength8_500 == 0 ) 
		enableLabelLength8_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_9_000) || dimBLength9_000 == 0 ) 
		enableLabelLength9_000 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_9_500) || dimBLength9_500 == 0 ) 
		enableLabelLength9_500 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_10_00) || dimBLength10_00 == 0 ) 
		enableLabelLength10_00 = false;
	if ( !config->toBoolCfg(fieldOffsetCfgEnableLabelLength_Continuous) || dimBLengthContinuous == 0 ) 
		enableLabelLengthContinuous = false;

	// Initialize the default for how much longer can a label size be, over its normal size (dimBLengthxxxx).
	// The definition of 'longer' is what percentage of the number of steps between this label and the next bigger-sized label that is enabled.
	int maxLabelLengthPct = 0;

	try	{ maxLabelLengthPct = (UINT16) config->toIntCfgI(CfgInternal::MAXLABELLENGTHPCT);	}
	catch (std::exception) {}

	// If invalid range, set it to the way we always used to do it, half way between each enabled label length
	if ( maxLabelLengthPct > 99 || maxLabelLengthPct < 1 )
		maxLabelLengthPct = 50;

	double maxPct = (double)maxLabelLengthPct / 100;

	UINT16 linesPerHalfInch = 102;
	UINT16 linesPerInch = 203;
	UINT16 linesOffset = 0;
	//KPC does a much better job sizing labels than BCM and ARA. I do not see the need for applying
	//the BCM/ARA algorithm since the ARK does not slip like the BCM/ARA printers. 
	//Simply check surrounding stock sizes if disabled then increase the window of the enabled stock.
	if( enableLabelLength1_000 ) {
		if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MaxLength_KPC_1_000 = PrinterStockMetrics::MaxLength_KPC_1_500;
		}
		if( ( !enableLabelLength1_750 ) && ( !enableLabelLength1_500 ) ){
			PrinterStockMetrics::MaxLength_KPC_1_000 =  PrinterStockMetrics::MaxLength_KPC_1_750 - 1;
		}
	}
		
	if( enableLabelLength1_500 ) {
		if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_KPC_1_500 = PrinterStockMetrics::MinLength_KPC_1_000;
		}
		if( !enableLabelLength1_750 ){
			PrinterStockMetrics::MaxLength_KPC_1_500 =  PrinterStockMetrics::MaxLength_KPC_1_750 - 1;
		}
		if( ( !enableLabelLength1_750 ) && ( !enableLabelLength2_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_1_750 = PrinterStockMetrics::MinLength_KPC_2_000;
		}		 
	} else if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_KPC_1_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_000 = 0;
	}


	if( enableLabelLength1_750 ) {
		if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_KPC_1_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_000 = 0;
		}
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MaxLength_KPC_1_750 = ( OneInchLength_ * 2 );
		}
		if( ( !enableLabelLength2_000 ) && ( !enableLabelLength2_375 ) ) {
			PrinterStockMetrics::MaxLength_KPC_1_750 = PrinterStockMetrics::MinLength_KPC_2_375;
		}	
	} else if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_KPC_1_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_000 = 0;
	}

	if( enableLabelLength2_000 ) {
		if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_KPC_2_000 = PrinterStockMetrics::MinLength_KPC_1_750;
		}
		if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MinLength_KPC_1_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_500 = 0;
		}
		if( !enableLabelLength2_375 ){
			PrinterStockMetrics::MaxLength_KPC_2_000 =  PrinterStockMetrics::MaxLength_KPC_2_375 - 1;
		}
		if( ( !enableLabelLength2_375 ) && ( !enableLabelLength2_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_2_000 = PrinterStockMetrics::MinLength_KPC_2_500;
		}		 
	} else if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MinLength_KPC_1_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_500 = 0;
	}
#if 0
	if( enableLabelLength2_375 ) {		
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_KPC_2_375 = ( OneInchLength_ * 2 ) + 1;
		}
		if( !enableLabelLength1_750 ) {			
			PrinterStockMetrics::MinLength_KPC_1_750 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_750 = 0;	
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MaxLength_KPC_2_375 = PrinterStockMetrics::MaxLength_KPC_2_500;
		}
		if( ( !enableLabelLength2_500 ) && ( !enableLabelLength3_000 ) ) {		
			PrinterStockMetrics::MaxLength_KPC_2_375 = PrinterStockMetrics::MinLength_KPC_3_000;
		}
	} else if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_KPC_1_750 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_750 = 0;	
	}
#endif

	if( enableLabelLength2_375 ) {
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_KPC_2_375 = (OneInchLength_ * 2 );
		}
		if( !enableLabelLength1_750 ) {			
			PrinterStockMetrics::MinLength_KPC_1_750 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_750 = 0;		
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MaxLength_KPC_2_375 = PrinterStockMetrics::MaxLength_KPC_2_500 - 1; 
		}
		if( ( !enableLabelLength2_500 ) && ( !enableLabelLength3_000 ) ) {	
			PrinterStockMetrics::MaxLength_KPC_2_500 = PrinterStockMetrics::MinLength_KPC_3_000;
		}
	} else if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_KPC_1_750 = 0;
			PrinterStockMetrics::MaxLength_KPC_1_750 = 0;		
	}

	if( enableLabelLength2_500 ) {
		if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_KPC_2_500 = (OneInchLength_ * 2) + (3 * EighthInchLength_) + 1;
		}
		if( !enableLabelLength2_000 ) {			
			PrinterStockMetrics::MinLength_KPC_2_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_000 = 0;		
		}
		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MaxLength_KPC_2_500 = PrinterStockMetrics::MaxLength_KPC_3_000 - 1; 
		}
		if( ( !enableLabelLength3_000 ) && ( !enableLabelLength3_500 ) ) {	
			PrinterStockMetrics::MaxLength_KPC_2_500 = PrinterStockMetrics::MinLength_KPC_3_500;
		}
	} else if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_KPC_2_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_000 = 0;		
	}

	if( enableLabelLength3_000 ) {

		if( !enableLabelLength2_500 ) {		
			PrinterStockMetrics::MinLength_KPC_3_000 = (OneInchLength_ * 2 ) + HalfInchLength_ +1;
		}

		if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_KPC_2_375 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_375 = 0;
		}
		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MaxLength_KPC_3_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength3_500 ) && ( !enableLabelLength4_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_3_000 = PrinterStockMetrics::MinLength_KPC_4_000;			
		}
	} else if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_KPC_2_375 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_375 = 0;
	}

	if( enableLabelLength3_500 ) {

		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_KPC_3_500 = (OneInchLength_ * 3 ) + 1;
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MinLength_KPC_2_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_500 = 0;	
		}
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MaxLength_KPC_3_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength4_000 ) && ( !enableLabelLength4_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_3_500 = PrinterStockMetrics::MinLength_KPC_4_500;		
		}
	} else if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MinLength_KPC_2_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_2_500 = 0;	
	}

	if( enableLabelLength4_000 ) {

		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_KPC_4_000 = (OneInchLength_ * 3 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_KPC_3_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_3_000 = 0;		
		}
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MaxLength_KPC_4_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength4_500 ) && ( !enableLabelLength5_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_4_000 = PrinterStockMetrics::MinLength_KPC_5_000;		
		}
	} else if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_KPC_3_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_3_000 = 0;		
	}
	
	if( enableLabelLength4_500 ) {
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_KPC_4_500 = (OneInchLength_ * 4 ) + 1;
		}
		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_KPC_3_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_3_500 = 0;		
		}
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MaxLength_KPC_4_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength5_000 ) && ( !enableLabelLength5_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_4_500 = PrinterStockMetrics::MinLength_KPC_5_500;			
		}
	} else if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_KPC_3_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_3_500 = 0;		
	}
	
	if( enableLabelLength5_000 ) {
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_KPC_5_000 = (OneInchLength_ * 4 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_KPC_4_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_4_000 = 0;		
		}
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MaxLength_KPC_5_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength5_500 ) && ( !enableLabelLength6_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_5_000 = PrinterStockMetrics::MinLength_KPC_6_000;				
		}
	} else if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_KPC_4_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_4_000 = 0;		
	}

	if( enableLabelLength5_500 ) {
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_KPC_5_500 = (OneInchLength_ * 5 ) + 1;
		}
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_KPC_4_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_4_500 = 0;		
		}
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MaxLength_KPC_5_500 += HalfInchLength_; 
		}
		if( ( !enableLabelLength6_000 ) && ( !enableLabelLength6_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_5_500 = PrinterStockMetrics::MinLength_KPC_6_500;			
		}
	} else if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_KPC_4_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_4_500 = 0;		
	}

	if( enableLabelLength6_000 ) {
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_KPC_6_000 = (OneInchLength_ * 4 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_KPC_5_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_5_000 = 0;			
		}
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MaxLength_KPC_6_000 += HalfInchLength_; 
		}
		if( ( !enableLabelLength6_500 ) && ( !enableLabelLength7_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_6_000 = PrinterStockMetrics::MinLength_KPC_7_000;			
		}
	} else if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_KPC_5_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_5_000 = 0;			
	}

	if( enableLabelLength6_500 ) {
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_KPC_6_500 = (OneInchLength_ * 6 ) + 1;
		}
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_KPC_5_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_5_500 = 0;			
		}
		if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MaxLength_KPC_6_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength7_000 ) && ( !enableLabelLength7_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_6_500 = PrinterStockMetrics::MinLength_KPC_7_500;			
		}
	} else if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_KPC_5_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_5_500 = 0;			
	}

	if( enableLabelLength7_000 ) {
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_KPC_7_000 = (OneInchLength_ * 6 ) + HalfInchLength_ + 1;;
		}
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_KPC_6_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_6_000 = 0;			
		}
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MaxLength_KPC_7_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength7_500 ) && ( !enableLabelLength8_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_7_000 = PrinterStockMetrics::MinLength_KPC_8_000;			
		}
	} else if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_KPC_6_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_6_000 = 0;			
	}

	if( enableLabelLength7_500 ) {
		if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MinLength_KPC_7_500 = PrinterStockMetrics::MinLength_KPC_7_000;
		}
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_KPC_6_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_6_500 = 0;			
		}
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MaxLength_KPC_7_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength8_000 ) && ( !enableLabelLength8_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_7_500 = PrinterStockMetrics::MinLength_KPC_8_500;			
		}
	} else if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_KPC_6_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_6_500 = 0;			
	}

	if( enableLabelLength8_000 ) {
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_KPC_8_000 = PrinterStockMetrics::MinLength_KPC_7_500;
		}
		if( !enableLabelLength7_000 ) {		
			PrinterStockMetrics::MinLength_KPC_7_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_7_000 = 0;			
		}
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MaxLength_KPC_8_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength8_500 ) && ( !enableLabelLength9_000 ) ) {
			PrinterStockMetrics::MaxLength_KPC_8_000 = PrinterStockMetrics::MinLength_KPC_9_000;			
		}
	} else if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MinLength_KPC_7_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_7_000 = 0;			
	}

	if( enableLabelLength8_500 ) {
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_KPC_8_500 = PrinterStockMetrics::MinLength_KPC_8_000;
		}
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_KPC_7_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_7_500 = 0;			
		}
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MaxLength_KPC_8_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength9_000 ) && ( !enableLabelLength9_500 ) ) {
			PrinterStockMetrics::MaxLength_KPC_8_500 = PrinterStockMetrics::MinLength_KPC_9_500;			
		}
	} else if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_KPC_7_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_7_500 = 0;			
	}

	if( enableLabelLength9_000 ) {
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_KPC_9_000 = PrinterStockMetrics::MinLength_KPC_8_500; 
		}
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_KPC_8_000 = 0;
			PrinterStockMetrics::MinLength_KPC_8_000 = 0;		
		}
		if( !enableLabelLength9_500 ) {
			PrinterStockMetrics::MaxLength_KPC_9_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength9_500 ) && ( !enableLabelLength10_00 ) ) {
			PrinterStockMetrics::MaxLength_KPC_9_000 = PrinterStockMetrics::MinLength_KPC_10_000;			
		}
	} else if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_KPC_8_000 = 0;
			PrinterStockMetrics::MinLength_KPC_8_000 = 0;		
	}

	if( enableLabelLength9_500 ) {
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MinLength_KPC_9_500 = PrinterStockMetrics::MinLength_KPC_9_000;
		}
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_KPC_8_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_8_500 = 0;			
		}
		if( !enableLabelLength10_00 ) {
			PrinterStockMetrics::MaxLength_KPC_9_500 += HalfInchLength_;
		}
	} else if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_KPC_8_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_8_500 = 0;			
	}

	if( enableLabelLength10_00 ) {
		if( !enableLabelLength9_500 ) {
			PrinterStockMetrics::MinLength_KPC_10_000 = PrinterStockMetrics::MinLength_KPC_9_500;
			PrinterStockMetrics::MinLength_KPC_9_500 = 0;
			PrinterStockMetrics::MaxLength_KPC_9_500 = 0;
		}
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MinLength_KPC_9_000 = 0;
			PrinterStockMetrics::MaxLength_KPC_9_000 = 0;			
		}
		PrinterStockMetrics::MaxLength_KPC_10_000 += HalfInchLength_;
	}


	//FSSK
	if( enableLabelLength1_000 ) {
		if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_1_000 = PrinterStockMetrics::MaxLength_FSSK_1_500;
		}
		if( ( !enableLabelLength1_750 ) && ( !enableLabelLength1_500 ) ){
			PrinterStockMetrics::MaxLength_FSSK_1_000 =  PrinterStockMetrics::MaxLength_FSSK_1_750 - 1;
		}
	}
		
	if( enableLabelLength1_500 ) {
		if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_FSSK_1_500 = PrinterStockMetrics::MinLength_FSSK_1_000;
		}
		if( !enableLabelLength1_750 ){
			PrinterStockMetrics::MaxLength_FSSK_1_500 =  PrinterStockMetrics::MaxLength_FSSK_1_750 - 1;
		}
		if( ( !enableLabelLength1_750 ) && ( !enableLabelLength2_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_1_750 = PrinterStockMetrics::MinLength_FSSK_2_000;
		}		 
	} else if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_FSSK_1_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_000 = 0;
	}


	if( enableLabelLength1_750 ) {
		if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_FSSK_1_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_000 = 0;
		}
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_1_750 = ( OneInchLength_ * 2 );
		}
		if( ( !enableLabelLength2_000 ) && ( !enableLabelLength2_375 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_1_750 = PrinterStockMetrics::MinLength_FSSK_2_375;
		}	
	} else if( !enableLabelLength1_000 ) {
			PrinterStockMetrics::MinLength_FSSK_1_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_000 = 0;
	}

	if( enableLabelLength2_000 ) {
		if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_FSSK_2_000 = PrinterStockMetrics::MinLength_FSSK_1_750;
		}
		if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MinLength_FSSK_1_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_500 = 0;
		}
		if( !enableLabelLength2_375 ){
			PrinterStockMetrics::MaxLength_FSSK_2_000 =  PrinterStockMetrics::MaxLength_FSSK_2_375 - 1;
		}
		if( ( !enableLabelLength2_375 ) && ( !enableLabelLength2_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_2_000 = PrinterStockMetrics::MinLength_FSSK_2_500;
		}		 
	} else if( !enableLabelLength1_500 ) {
			PrinterStockMetrics::MinLength_FSSK_1_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_500 = 0;
	}
#if 0
	if( enableLabelLength2_375 ) {		
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_FSSK_2_375 = ( OneInchLength_ * 2 ) + 1;
		}
		if( !enableLabelLength1_750 ) {			
			PrinterStockMetrics::MinLength_FSSK_1_750 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_750 = 0;	
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_2_375 = PrinterStockMetrics::MaxLength_FSSK_2_500;
		}
		if( ( !enableLabelLength2_500 ) && ( !enableLabelLength3_000 ) ) {		
			PrinterStockMetrics::MaxLength_FSSK_2_375 = PrinterStockMetrics::MinLength_FSSK_3_000;
		}
	} else if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_FSSK_1_750 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_750 = 0;	
	}
#endif

	if( enableLabelLength2_375 ) {
		if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_FSSK_2_375 = (OneInchLength_ * 2 );
		}
		if( !enableLabelLength1_750 ) {			
			PrinterStockMetrics::MinLength_FSSK_1_750 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_750 = 0;		
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_2_375 = PrinterStockMetrics::MaxLength_FSSK_2_500 - 1; 
		}
		if( ( !enableLabelLength2_500 ) && ( !enableLabelLength3_000 ) ) {	
			PrinterStockMetrics::MaxLength_FSSK_2_500 = PrinterStockMetrics::MinLength_FSSK_3_000;
		}
	} else if( !enableLabelLength1_750 ) {
			PrinterStockMetrics::MinLength_FSSK_1_750 = 0;
			PrinterStockMetrics::MaxLength_FSSK_1_750 = 0;		
	}

	if( enableLabelLength2_500 ) {
		if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_FSSK_2_500 = (OneInchLength_ * 2) + (3 * EighthInchLength_) + 1;
		}
		if( !enableLabelLength2_000 ) {			
			PrinterStockMetrics::MinLength_FSSK_2_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_000 = 0;		
		}
		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_2_500 = PrinterStockMetrics::MaxLength_FSSK_3_000 - 1; 
		}
		if( ( !enableLabelLength3_000 ) && ( !enableLabelLength3_500 ) ) {	
			PrinterStockMetrics::MaxLength_FSSK_2_500 = PrinterStockMetrics::MinLength_FSSK_3_500;
		}
	} else if( !enableLabelLength2_000 ) {
			PrinterStockMetrics::MinLength_FSSK_2_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_000 = 0;		
	}

	if( enableLabelLength3_000 ) {

		if( !enableLabelLength2_500 ) {		
			PrinterStockMetrics::MinLength_FSSK_3_000 = (OneInchLength_ * 2 ) + HalfInchLength_ +1;
		}

		if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_FSSK_2_375 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_375 = 0;
		}
		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_3_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength3_500 ) && ( !enableLabelLength4_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_3_000 = PrinterStockMetrics::MinLength_FSSK_4_000;			
		}
	} else if( !enableLabelLength2_375 ) {
			PrinterStockMetrics::MinLength_FSSK_2_375 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_375 = 0;
	}

	if( enableLabelLength3_500 ) {

		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_FSSK_3_500 = (OneInchLength_ * 3 ) + 1;
		}
		if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MinLength_FSSK_2_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_500 = 0;	
		}
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_3_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength4_000 ) && ( !enableLabelLength4_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_3_500 = PrinterStockMetrics::MinLength_FSSK_4_500;		
		}
	} else if( !enableLabelLength2_500 ) {
			PrinterStockMetrics::MinLength_FSSK_2_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_2_500 = 0;	
	}

	if( enableLabelLength4_000 ) {

		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_FSSK_4_000 = (OneInchLength_ * 3 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_FSSK_3_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_3_000 = 0;		
		}
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_4_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength4_500 ) && ( !enableLabelLength5_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_4_000 = PrinterStockMetrics::MinLength_FSSK_5_000;		
		}
	} else if( !enableLabelLength3_000 ) {
			PrinterStockMetrics::MinLength_FSSK_3_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_3_000 = 0;		
	}
	
	if( enableLabelLength4_500 ) {
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_FSSK_4_500 = (OneInchLength_ * 4 ) + 1;
		}
		if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_FSSK_3_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_3_500 = 0;		
		}
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_4_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength5_000 ) && ( !enableLabelLength5_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_4_500 = PrinterStockMetrics::MinLength_FSSK_5_500;			
		}
	} else if( !enableLabelLength3_500 ) {
			PrinterStockMetrics::MinLength_FSSK_3_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_3_500 = 0;		
	}
	
	if( enableLabelLength5_000 ) {
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_FSSK_5_000 = (OneInchLength_ * 4 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_FSSK_4_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_4_000 = 0;		
		}
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_5_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength5_500 ) && ( !enableLabelLength6_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_5_000 = PrinterStockMetrics::MinLength_FSSK_6_000;				
		}
	} else if( !enableLabelLength4_000 ) {
			PrinterStockMetrics::MinLength_FSSK_4_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_4_000 = 0;		
	}

	if( enableLabelLength5_500 ) {
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_FSSK_5_500 = (OneInchLength_ * 5 ) + 1;
		}
		if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_FSSK_4_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_4_500 = 0;		
		}
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_5_500 += HalfInchLength_; 
		}
		if( ( !enableLabelLength6_000 ) && ( !enableLabelLength6_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_5_500 = PrinterStockMetrics::MinLength_FSSK_6_500;			
		}
	} else if( !enableLabelLength4_500 ) {
			PrinterStockMetrics::MinLength_FSSK_4_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_4_500 = 0;		
	}

	if( enableLabelLength6_000 ) {
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_FSSK_6_000 = (OneInchLength_ * 4 ) + HalfInchLength_ + 1;
		}
		if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_FSSK_5_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_5_000 = 0;			
		}
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_6_000 += HalfInchLength_; 
		}
		if( ( !enableLabelLength6_500 ) && ( !enableLabelLength7_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_6_000 = PrinterStockMetrics::MinLength_FSSK_7_000;			
		}
	} else if( !enableLabelLength5_000 ) {
			PrinterStockMetrics::MinLength_FSSK_5_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_5_000 = 0;			
	}

	if( enableLabelLength6_500 ) {
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_FSSK_6_500 = (OneInchLength_ * 6 ) + 1;
		}
		if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_FSSK_5_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_5_500 = 0;			
		}
		if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_6_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength7_000 ) && ( !enableLabelLength7_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_6_500 = PrinterStockMetrics::MinLength_FSSK_7_500;			
		}
	} else if( !enableLabelLength5_500 ) {
			PrinterStockMetrics::MinLength_FSSK_5_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_5_500 = 0;			
	}

	if( enableLabelLength7_000 ) {
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_FSSK_7_000 = (OneInchLength_ * 6 ) + HalfInchLength_ + 1;;
		}
		if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_FSSK_6_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_6_000 = 0;			
		}
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_7_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength7_500 ) && ( !enableLabelLength8_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_7_000 = PrinterStockMetrics::MinLength_FSSK_8_000;			
		}
	} else if( !enableLabelLength6_000 ) {
			PrinterStockMetrics::MinLength_FSSK_6_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_6_000 = 0;			
	}

	if( enableLabelLength7_500 ) {
		if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MinLength_FSSK_7_500 = PrinterStockMetrics::MinLength_FSSK_7_000;
		}
		if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_FSSK_6_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_6_500 = 0;			
		}
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_7_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength8_000 ) && ( !enableLabelLength8_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_7_500 = PrinterStockMetrics::MinLength_FSSK_8_500;			
		}
	} else if( !enableLabelLength6_500 ) {
			PrinterStockMetrics::MinLength_FSSK_6_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_6_500 = 0;			
	}

	if( enableLabelLength8_000 ) {
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_FSSK_8_000 = PrinterStockMetrics::MinLength_FSSK_7_500;
		}
		if( !enableLabelLength7_000 ) {		
			PrinterStockMetrics::MinLength_FSSK_7_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_7_000 = 0;			
		}
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_8_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength8_500 ) && ( !enableLabelLength9_000 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_8_000 = PrinterStockMetrics::MinLength_FSSK_9_000;			
		}
	} else if( !enableLabelLength7_000 ) {
			PrinterStockMetrics::MinLength_FSSK_7_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_7_000 = 0;			
	}

	if( enableLabelLength8_500 ) {
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_FSSK_8_500 = PrinterStockMetrics::MinLength_FSSK_8_000;
		}
		if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_FSSK_7_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_7_500 = 0;			
		}
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MaxLength_FSSK_8_500 += HalfInchLength_;
		}
		if( ( !enableLabelLength9_000 ) && ( !enableLabelLength9_500 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_8_500 = PrinterStockMetrics::MinLength_FSSK_9_500;			
		}
	} else if( !enableLabelLength7_500 ) {
			PrinterStockMetrics::MinLength_FSSK_7_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_7_500 = 0;			
	}

	if( enableLabelLength9_000 ) {
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_FSSK_9_000 = PrinterStockMetrics::MinLength_FSSK_8_500; 
		}
		if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_FSSK_8_000 = 0;
			PrinterStockMetrics::MinLength_FSSK_8_000 = 0;		
		}
		if( !enableLabelLength9_500 ) {
			PrinterStockMetrics::MaxLength_FSSK_9_000 += HalfInchLength_;
		}
		if( ( !enableLabelLength9_500 ) && ( !enableLabelLength10_00 ) ) {
			PrinterStockMetrics::MaxLength_FSSK_9_000 = PrinterStockMetrics::MinLength_FSSK_10_000;			
		}
	} else if( !enableLabelLength8_000 ) {
			PrinterStockMetrics::MinLength_FSSK_8_000 = 0;
			PrinterStockMetrics::MinLength_FSSK_8_000 = 0;		
	}

	if( enableLabelLength9_500 ) {
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MinLength_FSSK_9_500 = PrinterStockMetrics::MinLength_FSSK_9_000;
		}
		if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_FSSK_8_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_8_500 = 0;			
		}
		if( !enableLabelLength10_00 ) {
			PrinterStockMetrics::MaxLength_FSSK_9_500 += HalfInchLength_;
		}
	} else if( !enableLabelLength8_500 ) {
			PrinterStockMetrics::MinLength_FSSK_8_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_8_500 = 0;			
	}

	if( enableLabelLength10_00 ) {
		if( !enableLabelLength9_500 ) {
			PrinterStockMetrics::MinLength_FSSK_10_000 = PrinterStockMetrics::MinLength_FSSK_9_500;
			PrinterStockMetrics::MinLength_FSSK_9_500 = 0;
			PrinterStockMetrics::MaxLength_FSSK_9_500 = 0;
		}
		if( !enableLabelLength9_000 ) {
			PrinterStockMetrics::MinLength_FSSK_9_000 = 0;
			PrinterStockMetrics::MaxLength_FSSK_9_000 = 0;			
		}
		PrinterStockMetrics::MaxLength_FSSK_10_000 += HalfInchLength_;
	}

	if ( enableLabelLength1_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_500 )
			linesOffset = dimBLength1_500;
		else if ( enableLabelLength1_750 )
			linesOffset = dimBLength1_750;
		else if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength1_000 = dimBLength1_000 + linesOffset;
	}

	if ( enableLabelLength1_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_750 )
			linesOffset = dimBLength1_750;
		else if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength1_500 = dimBLength1_500 + linesOffset;
	}

	if ( enableLabelLength1_750 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_750) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength1_750 = dimBLength1_750 + linesOffset;
	}

	if ( enableLabelLength2_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength2_000 = dimBLength2_000 + linesOffset;
	}

	if ( enableLabelLength2_375 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_375) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength2_375 = dimBLength2_375 + linesOffset;
	}

	if ( enableLabelLength2_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength2_500 = dimBLength2_500 + linesOffset;
	}

	if ( enableLabelLength3_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength3_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength3_000 = dimBLength3_000 + linesOffset;
	}

	if ( enableLabelLength3_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength3_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength3_500 = dimBLength3_500 + linesOffset;
	}

	if ( enableLabelLength4_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength4_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength4_000 = dimBLength4_000 + linesOffset;
	}

	if ( enableLabelLength4_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength4_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength4_500 = dimBLength4_500 + linesOffset;
	}

	if ( enableLabelLength5_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength5_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength5_000 = dimBLength5_000 + linesOffset;
	}

	if ( enableLabelLength5_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength5_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength5_500 = dimBLength5_500 + linesOffset;
	}

	if ( enableLabelLength6_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength6_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength6_000 = dimBLength6_000 + linesOffset;
	}

	if ( enableLabelLength6_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength6_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength6_500 = dimBLength6_500 + linesOffset;
	}

	if ( enableLabelLength7_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength7_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength7_000 = dimBLength7_000 + linesOffset;
	}

	if ( enableLabelLength7_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength7_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength7_500 = dimBLength7_500 + linesOffset;
	}

	if ( enableLabelLength8_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength8_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength8_000 = dimBLength8_000 + linesOffset;
	}

	if ( enableLabelLength8_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength8_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength8_500 = dimBLength8_500 + linesOffset;
	}

	if ( enableLabelLength9_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength9_000) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength9_000 = dimBLength9_000 + linesOffset;
	}

	if ( enableLabelLength9_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength9_500) * maxPct + 0.5 );

		PrinterStockMetrics::MaxLength9_500 = dimBLength9_500 + linesOffset;
	}

	if ( enableLabelLength10_00 )
		PrinterStockMetrics::MaxLength10_000 = dimBLength10_00 + linesPerInch;

	if ( enableLabelLength1_000 )
		PrinterStockMetrics::MinLength1_000 = dimBLength1_000 - linesPerHalfInch;

	if ( enableLabelLength1_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength1_500 - linesPerInch;

		PrinterStockMetrics::MinLength1_500 = linesOffset + 1;
	}

	if ( enableLabelLength1_750 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength1_750 - linesPerInch;

		PrinterStockMetrics::MinLength1_750 = linesOffset + 1;
	}

	if ( enableLabelLength2_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_000 - linesPerInch;

		PrinterStockMetrics::MinLength2_000 = linesOffset + 1;
	}

	if ( enableLabelLength2_375 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_375 - linesPerInch;

		PrinterStockMetrics::MinLength2_375 = linesOffset + 1;
	}

	if ( enableLabelLength2_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_500 - linesPerInch;

		PrinterStockMetrics::MinLength2_500 = linesOffset + 1;
	}

	if ( enableLabelLength3_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength3_000 - linesPerInch;

		PrinterStockMetrics::MinLength3_000 = linesOffset + 1;
	}


	if ( enableLabelLength3_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength1_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength3_500 - linesPerInch;

		PrinterStockMetrics::MinLength3_500 = linesOffset + 1;
	}

	if ( enableLabelLength4_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength2_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength4_000 - linesPerInch;

		PrinterStockMetrics::MinLength4_000 = linesOffset + 1;
	}

	if ( enableLabelLength4_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength2_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength4_500 - linesPerInch;

		PrinterStockMetrics::MinLength4_500 = linesOffset + 1;
	}

	if ( enableLabelLength5_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength3_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength5_000 - linesPerInch;

		PrinterStockMetrics::MinLength5_000 = linesOffset + 1;
	}

	if ( enableLabelLength5_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength3_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength5_500 - linesPerInch;

		PrinterStockMetrics::MinLength5_500 = linesOffset + 1;
	}

	if ( enableLabelLength6_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength4_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength6_000 - linesPerInch;

		PrinterStockMetrics::MinLength6_000 = linesOffset + 1;
	}

	if ( enableLabelLength6_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength4_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength6_500 - linesPerInch;

		PrinterStockMetrics::MinLength6_500 = linesOffset + 1;
	}

	if ( enableLabelLength7_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength5_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength7_000 - linesPerInch;

		PrinterStockMetrics::MinLength7_000 = linesOffset + 1;
	}

	if ( enableLabelLength7_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength5_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength7_500 - linesPerInch;

		PrinterStockMetrics::MinLength7_500 = linesOffset + 1;
	}

	if ( enableLabelLength8_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength6_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength8_000 - linesPerInch;

		PrinterStockMetrics::MinLength8_000 = linesOffset + 1;
	}

	if ( enableLabelLength8_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength6_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength8_500 - linesPerInch;

		PrinterStockMetrics::MinLength8_500 = linesOffset + 1;
	}

	if ( enableLabelLength9_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength7_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength9_000 - linesPerInch;

		PrinterStockMetrics::MinLength9_000 = linesOffset + 1;
	}

	if ( enableLabelLength9_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_000 )
			linesOffset = PrinterStockMetrics::MaxLength9_000;
		else if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength7_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength9_500 - linesPerInch;

		PrinterStockMetrics::MinLength9_500 = linesOffset + 1;
	}

	if ( enableLabelLength10_00 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_500 )
			linesOffset = PrinterStockMetrics::MaxLength9_500;
		else if ( enableLabelLength9_000 )
			linesOffset = PrinterStockMetrics::MaxLength9_000;
		else if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength8_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength10_00 - linesPerInch;

		PrinterStockMetrics::MinLength10_000 = linesOffset + 1;
	}

	if (Debug::isTracing("PrinterStockMetrics.initStatics"))
	{
		Debug::trace("PrinterStockMetrics.initStatics", HobString(" ... KPC PRINTER ..."));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("Length   Min     normal     Max "));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.0   ") + uinttostring(MinLength_KPC_1_000) + " " + inttostring(OneInchLength_) + " " + uinttostring(MaxLength_KPC_1_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.5   ") + uinttostring(MinLength_KPC_1_500) + " " + inttostring(OneInchLength_ + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_1_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.75  ") + uinttostring(MinLength_KPC_1_750) + " " + inttostring(OneInchLength_ + HalfInchLength_ + QuarterInchLength_) + " " + uinttostring(MaxLength_KPC_1_750));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.0   ") + uinttostring(MinLength_KPC_2_000) + " " + inttostring(OneInchLength_ * 2) + " " + uinttostring(MaxLength_KPC_2_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.375   ") + uinttostring(MinLength_KPC_2_375) + " " + inttostring((OneInchLength_ * 2) + (3 * EighthInchLength_)) + " " + uinttostring(MaxLength_KPC_2_375));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.5   ") + uinttostring(MinLength_KPC_2_500) + " " + inttostring((OneInchLength_ * 2 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_2_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.0   ") + uinttostring(MinLength_KPC_3_000) + " " + inttostring(OneInchLength_ * 3) + " " + uinttostring(MaxLength_KPC_3_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.5   ") + uinttostring(MinLength_KPC_3_500) + " " + inttostring((OneInchLength_ * 3 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_3_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.0   ") + uinttostring(MinLength_KPC_4_000) + " " + inttostring(OneInchLength_ * 4) + " " + uinttostring(MaxLength_KPC_4_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.5   ") + uinttostring(MinLength_KPC_4_500) + " " + inttostring((OneInchLength_ * 4 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_4_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.0   ") + uinttostring(MinLength_KPC_5_000) + " " + inttostring(OneInchLength_ * 5) + " " + uinttostring(MaxLength_KPC_5_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.5   ") + uinttostring(MinLength_KPC_5_500) + " " + inttostring((OneInchLength_ * 5 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_5_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.0   ") + uinttostring(MinLength_KPC_6_000) + " " + inttostring(OneInchLength_ * 6) + " " + uinttostring(MaxLength_KPC_6_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.5   ") + uinttostring(MinLength_KPC_6_500) + " " + inttostring((OneInchLength_ * 6 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_6_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.0   ") + uinttostring(MinLength_KPC_7_000) + " " + inttostring(OneInchLength_ * 7) + " " + uinttostring(MaxLength_KPC_7_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.5   ") + uinttostring(MinLength_KPC_7_500) + " " + inttostring((OneInchLength_ * 7 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_7_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.0   ") + uinttostring(MinLength_KPC_8_000) + " " + inttostring(OneInchLength_ * 8) + " " + uinttostring(MaxLength_KPC_8_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.5   ") + uinttostring(MinLength_KPC_8_500) + " " + inttostring((OneInchLength_ * 8 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_8_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.0   ") + uinttostring(MinLength_KPC_9_000) + " " + inttostring(OneInchLength_ * 9) + " " + uinttostring(MaxLength_KPC_9_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.5   ") + uinttostring(MinLength_KPC_9_500) + " " + inttostring((OneInchLength_ * 9 ) + HalfInchLength_) + " " + uinttostring(MaxLength_KPC_9_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("10.0  ") + uinttostring(MinLength_KPC_10_000) + " " + inttostring(OneInchLength_ * 10) + " " + uinttostring(MaxLength_KPC_10_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("continuous  ") + inttostring(dimBLengthContinuous));

		Debug::trace("PrinterStockMetrics.initStatics", HobString(" ... FSSK PRINTER ..."));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("Length   Min     normal     Max "));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.0   ") + uinttostring(MinLength_FSSK_1_000) + " " + inttostring(OneInchLength_) + " " + uinttostring(MaxLength_FSSK_1_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.5   ") + uinttostring(MinLength_FSSK_1_500) + " " + inttostring(OneInchLength_ + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_1_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.75  ") + uinttostring(MinLength_FSSK_1_750) + " " + inttostring(OneInchLength_ + HalfInchLength_ + QuarterInchLength_) + " " + uinttostring(MaxLength_FSSK_1_750));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.0   ") + uinttostring(MinLength_FSSK_2_000) + " " + inttostring(OneInchLength_ * 2) + " " + uinttostring(MaxLength_FSSK_2_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.375   ") + uinttostring(MinLength_FSSK_2_375) + " " + inttostring((OneInchLength_ * 2) + (3 * EighthInchLength_)) + " " + uinttostring(MaxLength_FSSK_2_375));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.5   ") + uinttostring(MinLength_FSSK_2_500) + " " + inttostring((OneInchLength_ * 2 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_2_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.0   ") + uinttostring(MinLength_FSSK_3_000) + " " + inttostring(OneInchLength_ * 3) + " " + uinttostring(MaxLength_FSSK_3_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.5   ") + uinttostring(MinLength_FSSK_3_500) + " " + inttostring((OneInchLength_ * 3 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_3_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.0   ") + uinttostring(MinLength_FSSK_4_000) + " " + inttostring(OneInchLength_ * 4) + " " + uinttostring(MaxLength_FSSK_4_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.5   ") + uinttostring(MinLength_FSSK_4_500) + " " + inttostring((OneInchLength_ * 4 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_4_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.0   ") + uinttostring(MinLength_FSSK_5_000) + " " + inttostring(OneInchLength_ * 5) + " " + uinttostring(MaxLength_FSSK_5_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.5   ") + uinttostring(MinLength_FSSK_5_500) + " " + inttostring((OneInchLength_ * 5 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_5_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.0   ") + uinttostring(MinLength_FSSK_6_000) + " " + inttostring(OneInchLength_ * 6) + " " + uinttostring(MaxLength_FSSK_6_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.5   ") + uinttostring(MinLength_FSSK_6_500) + " " + inttostring((OneInchLength_ * 6 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_6_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.0   ") + uinttostring(MinLength_FSSK_7_000) + " " + inttostring(OneInchLength_ * 7) + " " + uinttostring(MaxLength_FSSK_7_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.5   ") + uinttostring(MinLength_FSSK_7_500) + " " + inttostring((OneInchLength_ * 7 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_7_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.0   ") + uinttostring(MinLength_FSSK_8_000) + " " + inttostring(OneInchLength_ * 8) + " " + uinttostring(MaxLength_FSSK_8_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.5   ") + uinttostring(MinLength_FSSK_8_500) + " " + inttostring((OneInchLength_ * 8 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_8_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.0   ") + uinttostring(MinLength_FSSK_9_000) + " " + inttostring(OneInchLength_ * 9) + " " + uinttostring(MaxLength_FSSK_9_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.5   ") + uinttostring(MinLength_FSSK_9_500) + " " + inttostring((OneInchLength_ * 9 ) + HalfInchLength_) + " " + uinttostring(MaxLength_FSSK_9_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("10.0  ") + uinttostring(MinLength_FSSK_10_000) + " " + inttostring(OneInchLength_ * 10) + " " + uinttostring(MaxLength_FSSK_10_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("continuous  ") + inttostring(dimBLengthContinuous));

		Debug::trace("PrinterStockMetrics.initStatics", HobString(" ... NON-EPP PRINTER ..."));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("maxPct = ") + doubletostring( maxPct ) );
		Debug::trace("PrinterStockMetrics.initStatics", HobString("Length   Min     normal     Max "));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.0   ") + uinttostring(MinLength1_000) + " " + inttostring(dimBLength1_000) + " " + uinttostring(MaxLength1_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.5   ") + uinttostring(MinLength1_500) + " " + inttostring(dimBLength1_500) + " " + uinttostring(MaxLength1_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.75  ") + uinttostring(MinLength1_750) + " " + inttostring(dimBLength1_750) + " " + uinttostring(MaxLength1_750));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.0   ") + uinttostring(MinLength2_000) + " " + inttostring(dimBLength2_000) + " " + uinttostring(MaxLength2_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.375 ") + uinttostring(MinLength2_375) + " " + inttostring(dimBLength2_375) + " " + uinttostring(MaxLength2_375));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.5   ") + uinttostring(MinLength2_500) + " " + inttostring(dimBLength2_500) + " " + uinttostring(MaxLength2_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.0   ") + uinttostring(MinLength3_000) + " " + inttostring(dimBLength3_000) + " " + uinttostring(MaxLength3_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.5   ") + uinttostring(MinLength3_500) + " " + inttostring(dimBLength3_500) + " " + uinttostring(MaxLength3_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.0   ") + uinttostring(MinLength4_000) + " " + inttostring(dimBLength4_000) + " " + uinttostring(MaxLength4_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.5   ") + uinttostring(MinLength4_500) + " " + inttostring(dimBLength4_500) + " " + uinttostring(MaxLength4_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.0   ") + uinttostring(MinLength5_000) + " " + inttostring(dimBLength5_000) + " " + uinttostring(MaxLength5_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.5   ") + uinttostring(MinLength5_500) + " " + inttostring(dimBLength5_500) + " " + uinttostring(MaxLength5_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.0   ") + uinttostring(MinLength6_000) + " " + inttostring(dimBLength6_000) + " " + uinttostring(MaxLength6_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.5   ") + uinttostring(MinLength6_500) + " " + inttostring(dimBLength6_500) + " " + uinttostring(MaxLength6_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.0   ") + uinttostring(MinLength7_000) + " " + inttostring(dimBLength7_000) + " " + uinttostring(MaxLength7_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.5   ") + uinttostring(MinLength7_500) + " " + inttostring(dimBLength7_500) + " " + uinttostring(MaxLength7_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.0   ") + uinttostring(MinLength8_000) + " " + inttostring(dimBLength8_000) + " " + uinttostring(MaxLength8_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.5   ") + uinttostring(MinLength8_500) + " " + inttostring(dimBLength8_500) + " " + uinttostring(MaxLength8_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.0   ") + uinttostring(MinLength9_000) + " " + inttostring(dimBLength9_000) + " " + uinttostring(MaxLength9_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.5   ") + uinttostring(MinLength9_500) + " " + inttostring(dimBLength9_500) + " " + uinttostring(MaxLength9_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("10.0  ") + uinttostring(MinLength10_000) + " " + inttostring(dimBLength10_00) + " " + uinttostring(MaxLength10_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("continuous  ") + inttostring(dimBLengthContinuous));
	}

	//------------------------
	// HT-171: Changes for EPP
	//------------------------

	// Initialize the default for how much longer can a label size be, over its normal size (dimBLengthxxxx).
	// The definition of 'longer' is what percentage of the number of steps between this label and the next bigger-sized label that is enabled.
	int maxLabelLengthPct_EPP = 0;

	try	{ maxLabelLengthPct_EPP = (UINT16) config->toIntCfgI(CfgInternal::MAXLABELLENGTHPCTEPP);	}
	catch (std::exception) {}

	// If invalid range, set it to the way we always used to do it, half way between each enabled label length
	if ( maxLabelLengthPct_EPP > 99 || maxLabelLengthPct_EPP < 1 )
		maxLabelLengthPct_EPP = 50;

	double maxPct_EPP = (double)maxLabelLengthPct_EPP / 100;

	//UINT16 linesPerHalfInch = 102;
	//UINT16 linesPerInch = 203;
	//UINT16 linesOffset = 0;

	if ( enableLabelLength1_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_500 )
			linesOffset = dimBLength1_500;
		else if ( enableLabelLength1_750 )
			linesOffset = dimBLength1_750;
		else if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_1_000 = dimBLength1_000 + linesOffset;
	}

	if ( enableLabelLength1_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_750 )
			linesOffset = dimBLength1_750;
		else if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_1_500 = dimBLength1_500 + linesOffset;
	}

	if ( enableLabelLength1_750 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_000 )
			linesOffset = dimBLength2_000;
		else if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength1_750) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_1_750 = dimBLength1_750 + linesOffset;
	}

	if ( enableLabelLength2_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_375 )
			linesOffset = dimBLength2_375;
		else if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_2_000 = dimBLength2_000 + linesOffset;
	}

	if ( enableLabelLength2_375 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_500 )
			linesOffset = dimBLength2_500;
		else if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_375) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_2_375 = dimBLength2_375 + linesOffset;
	}

	if ( enableLabelLength2_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_000 )
			linesOffset = dimBLength3_000;
		else if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength2_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_2_500 = dimBLength2_500 + linesOffset;
	}

	if ( enableLabelLength3_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_500 )
			linesOffset = dimBLength3_500;
		else if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength3_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_3_000 = dimBLength3_000 + linesOffset;
	}

	if ( enableLabelLength3_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_000 )
			linesOffset = dimBLength4_000;
		else if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength3_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_3_500 = dimBLength3_500 + linesOffset;
	}

	if ( enableLabelLength4_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_500 )
			linesOffset = dimBLength4_500;
		else if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength4_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_4_000 = dimBLength4_000 + linesOffset;
	}

	if ( enableLabelLength4_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_000 )
			linesOffset = dimBLength5_000;
		else if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength4_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_4_500 = dimBLength4_500 + linesOffset;
	}

	if ( enableLabelLength5_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_500 )
			linesOffset = dimBLength5_500;
		else if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength5_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_5_000 = dimBLength5_000 + linesOffset;
	}

	if ( enableLabelLength5_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_000 )
			linesOffset = dimBLength6_000;
		else if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength5_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_5_500 = dimBLength5_500 + linesOffset;
	}

	if ( enableLabelLength6_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_500 )
			linesOffset = dimBLength6_500;
		else if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength6_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_6_000 = dimBLength6_000 + linesOffset;
	}

	if ( enableLabelLength6_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_000 )
			linesOffset = dimBLength7_000;
		else if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength6_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_6_500 = dimBLength6_500 + linesOffset;
	}

	if ( enableLabelLength7_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_500 )
			linesOffset = dimBLength7_500;
		else if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength7_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_7_000 = dimBLength7_000 + linesOffset;
	}

	if ( enableLabelLength7_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_000 )
			linesOffset = dimBLength8_000;
		else if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength7_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_7_500 = dimBLength7_500 + linesOffset;
	}

	if ( enableLabelLength8_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_500 )
			linesOffset = dimBLength8_500;
		else if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength8_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_8_000 = dimBLength8_000 + linesOffset;
	}

	if ( enableLabelLength8_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_000 )
			linesOffset = dimBLength9_000;
		else if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength8_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_8_500 = dimBLength8_500 + linesOffset;
	}

	if ( enableLabelLength9_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_500 )
			linesOffset = dimBLength9_500;
		else if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength9_000) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_9_000 = dimBLength9_000 + linesOffset;
	}

	if ( enableLabelLength9_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength10_00 )
			linesOffset = dimBLength10_00;

		if ( linesOffset == 0 )
			linesOffset = linesPerInch;
		else 
			linesOffset = (UINT16)( (linesOffset - dimBLength9_500) * maxPct_EPP + 0.5 );

		PrinterStockMetrics::MaxLength_EPP_9_500 = dimBLength9_500 + linesOffset;
	}

	if ( enableLabelLength10_00 )
		PrinterStockMetrics::MaxLength_EPP_10_000 = dimBLength10_00 + linesPerInch;

	if ( enableLabelLength1_000 )
		PrinterStockMetrics::MinLength_EPP_1_000 = dimBLength1_000 - linesPerHalfInch;

	if ( enableLabelLength1_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength1_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_1_500 = linesOffset + 1;
	}

	if ( enableLabelLength1_750 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength1_750 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_1_750 = linesOffset + 1;
	}

	if ( enableLabelLength2_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_2_000 = linesOffset + 1;
	}

	if ( enableLabelLength2_375 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_375 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_2_375 = linesOffset + 1;
	}

	if ( enableLabelLength2_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength2_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_2_500 = linesOffset + 1;
	}

	if ( enableLabelLength3_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;
		else if ( enableLabelLength1_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength3_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_3_000 = linesOffset + 1;
	}

	if ( enableLabelLength3_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_000;
		else if ( enableLabelLength1_750 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_750;
		else if ( enableLabelLength1_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_1_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength3_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_3_500 = linesOffset + 1;
	}

	if ( enableLabelLength4_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_500;
		else if ( enableLabelLength2_375 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_375;
		else if ( enableLabelLength2_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength4_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_4_000 = linesOffset + 1;
	}

	if ( enableLabelLength4_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_000;
		else if ( enableLabelLength2_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_2_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength4_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_4_500 = linesOffset + 1;
	}

	if ( enableLabelLength5_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_500;
		else if ( enableLabelLength3_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength5_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_5_000 = linesOffset + 1;
	}

	if ( enableLabelLength5_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_000;
		else if ( enableLabelLength3_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_3_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength5_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_5_500 = linesOffset + 1;
	}

	if ( enableLabelLength6_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_500;
		else if ( enableLabelLength4_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength6_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_6_000 = linesOffset + 1;
	}

	if ( enableLabelLength6_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_000;
		else if ( enableLabelLength4_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_4_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength6_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_6_500 = linesOffset + 1;
	}

	if ( enableLabelLength7_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_500;
		else if ( enableLabelLength5_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength7_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_7_000 = linesOffset + 1;
	}

	if ( enableLabelLength7_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_000;
		else if ( enableLabelLength5_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_5_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength7_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_7_500 = linesOffset + 1;
	}

	if ( enableLabelLength8_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_500;
		else if ( enableLabelLength6_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength8_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_8_000 = linesOffset + 1;
	}

	if ( enableLabelLength8_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_000;
		else if ( enableLabelLength6_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_6_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength8_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_8_500 = linesOffset + 1;
	}

	if ( enableLabelLength9_000 )
	{
		linesOffset = 0;
		if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_500;
		else if ( enableLabelLength7_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength9_000 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_9_000 = linesOffset + 1;
	}

	if ( enableLabelLength9_500 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_9_000;
		else if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_000;
		else if ( enableLabelLength7_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_7_500;

		if ( linesOffset == 0 )
			linesOffset = dimBLength9_500 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_9_500 = linesOffset + 1;
	}

	if ( enableLabelLength10_00 )
	{
		linesOffset = 0;
		if ( enableLabelLength9_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_9_500;
		else if ( enableLabelLength9_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_9_000;
		else if ( enableLabelLength8_500 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_500;
		else if ( enableLabelLength8_000 )
			linesOffset = PrinterStockMetrics::MaxLength_EPP_8_000;

		if ( linesOffset == 0 )
			linesOffset = dimBLength10_00 - linesPerInch;

		PrinterStockMetrics::MinLength_EPP_10_000 = linesOffset + 1;
	}

	if (Debug::isTracing("PrinterStockMetrics.initStatics"))
	{
		Debug::trace("PrinterStockMetrics.initStatics", HobString(" ... EPP PRINTER ..."));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("maxPct_EPP = ") + doubletostring( maxPct_EPP ) );
		Debug::trace("PrinterStockMetrics.initStatics", HobString("Length   Min     normal     Max "));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.0   ") + uinttostring(MinLength_EPP_1_000) + " " + inttostring(dimBLength1_000) + " " + uinttostring(MaxLength_EPP_1_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.5   ") + uinttostring(MinLength_EPP_1_500) + " " + inttostring(dimBLength1_500) + " " + uinttostring(MaxLength_EPP_1_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("1.75  ") + uinttostring(MinLength_EPP_1_750) + " " + inttostring(dimBLength1_750) + " " + uinttostring(MaxLength_EPP_1_750));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.0   ") + uinttostring(MinLength_EPP_2_000) + " " + inttostring(dimBLength2_000) + " " + uinttostring(MaxLength_EPP_2_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.375 ") + uinttostring(MinLength_EPP_2_375) + " " + inttostring(dimBLength2_375) + " " + uinttostring(MaxLength_EPP_2_375));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("2.5   ") + uinttostring(MinLength_EPP_2_500) + " " + inttostring(dimBLength2_500) + " " + uinttostring(MaxLength_EPP_2_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.0   ") + uinttostring(MinLength_EPP_3_000) + " " + inttostring(dimBLength3_000) + " " + uinttostring(MaxLength_EPP_3_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("3.5   ") + uinttostring(MinLength_EPP_3_500) + " " + inttostring(dimBLength3_500) + " " + uinttostring(MaxLength_EPP_3_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.0   ") + uinttostring(MinLength_EPP_4_000) + " " + inttostring(dimBLength4_000) + " " + uinttostring(MaxLength_EPP_4_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("4.5   ") + uinttostring(MinLength_EPP_4_500) + " " + inttostring(dimBLength4_500) + " " + uinttostring(MaxLength_EPP_4_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.0   ") + uinttostring(MinLength_EPP_5_000) + " " + inttostring(dimBLength5_000) + " " + uinttostring(MaxLength_EPP_5_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("5.5   ") + uinttostring(MinLength_EPP_5_500) + " " + inttostring(dimBLength5_500) + " " + uinttostring(MaxLength_EPP_5_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.0   ") + uinttostring(MinLength_EPP_6_000) + " " + inttostring(dimBLength6_000) + " " + uinttostring(MaxLength_EPP_6_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("6.5   ") + uinttostring(MinLength_EPP_6_500) + " " + inttostring(dimBLength6_500) + " " + uinttostring(MaxLength_EPP_6_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.0   ") + uinttostring(MinLength_EPP_7_000) + " " + inttostring(dimBLength7_000) + " " + uinttostring(MaxLength_EPP_7_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("7.5   ") + uinttostring(MinLength_EPP_7_500) + " " + inttostring(dimBLength7_500) + " " + uinttostring(MaxLength_EPP_7_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.0   ") + uinttostring(MinLength_EPP_8_000) + " " + inttostring(dimBLength8_000) + " " + uinttostring(MaxLength_EPP_8_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("8.5   ") + uinttostring(MinLength_EPP_8_500) + " " + inttostring(dimBLength8_500) + " " + uinttostring(MaxLength_EPP_8_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.0   ") + uinttostring(MinLength_EPP_9_000) + " " + inttostring(dimBLength9_000) + " " + uinttostring(MaxLength_EPP_9_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("9.5   ") + uinttostring(MinLength_EPP_9_500) + " " + inttostring(dimBLength9_500) + " " + uinttostring(MaxLength_EPP_9_500));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("10.0  ") + uinttostring(MinLength_EPP_10_000) + " " + inttostring(dimBLength10_00) + " " + uinttostring(MaxLength_EPP_10_000));
		Debug::trace("PrinterStockMetrics.initStatics", HobString("continuous  ") + inttostring(dimBLengthContinuous));
	}
}

const char *PrinterStockMetrics::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelWidthInDots;
	for (unsigned int i = 0; i < sizeof(labelWidthInDots); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelLengthInLines;
	for (unsigned int i = 0; i < sizeof(labelLengthInLines); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterStockMetrics::show()
{
	if (!Debug::isTracing( std::string("PrinterStockMetrics") ))
		return;
	
	Debug::trace("PrinterStockMetrics", "PrinterStockMetrics: ");

	PrinterGeneric::show();

	if (labelWidth == WideLabel)
	{
		Debug::trace("PrinterStockMetrics", HobString("labelWidth: WideLabel"));
	}
	else if (labelWidth == NarrowLabel)
	{
		Debug::trace("PrinterStockMetrics", HobString("labelWidth: NarrowLabel"));
	}
	else if (labelWidth == UFWLabel)
	{
		Debug::trace("PrinterStockMetrics", HobString("labelWidth: UFWLabel"));
	}
	else
	{
		Debug::trace("PrinterStockMetrics", HobString("labelWidth: Unknown ") +
											HobString(UInteger::toString((UINT16) labelWidth)));
	}
	
	Debug::trace("PrinterStockMetrics", HobString("labelWidthInDots: ") +
										HobString(UInteger::toString(labelWidthInDots)));
	Debug::trace("PrinterStockMetrics", HobString("labelLengthInLines: ") +
										HobString(UInteger::toString(labelLengthInLines)));
}

LabelWidth PrinterStockMetrics::getLabelWidth() const
{
	return labelWidth;
}

void PrinterStockMetrics::setLabelWidth(LabelWidth labelWidth)
{
	if (labelWidth == WideLabel)
	{
		this->labelWidth = WideLabel;
		labelWidthInDots = WideLabelWidth;
	}
	else if (labelWidth == NarrowLabel)
	{
		this->labelWidth = NarrowLabel;
		labelWidthInDots = NarrowLabelWidth;
	}
	else if (labelWidth == UFWLabel)
	{
		this->labelWidth = UFWLabel;
		labelWidthInDots = UFWLabelWidth;
	}
	else
	{
		this->labelWidth = UnknownLabel;
		labelWidthInDots = UnknownLabelWidth;
	}
}

UINT16 PrinterStockMetrics::getLabelWidthInDots() const
{
	return labelWidthInDots;
}

void PrinterStockMetrics::setLabelWidthInDots(UINT16 labelWidthInDots)
{
	this->labelWidthInDots = labelWidthInDots;
}

UINT16 PrinterStockMetrics::getLabelLengthInLines() const
{
	return labelLengthInLines;
}

void PrinterStockMetrics::setLabelLengthInLines(UINT16 labelLengthInLines)
{
	this->labelLengthInLines = labelLengthInLines;
	Debug::trace( "PrinterStockMetrics", string("setLabelLengthInLines() - labelLengthInLines set to:") + inttostring( labelLengthInLines ) );
}

// these are general purpose routines		
//Small fix by Alex Zavala on the else if conditions
LabelWidth PrinterStockMetrics::getLabelWidth(UINT16 numDots)
{
	LabelWidth rv = UnknownLabel;

	if ((0 < numDots) && (numDots <= NarrowLabelWidth))
	{
		rv = NarrowLabel;
	}
	else if ((NarrowLabelWidth < numDots) && (numDots <= WideLabelWidth))
	{
		rv = WideLabel;
	}
	else if ((WideLabelWidth < numDots) && (numDots <= UFWLabelWidth))
	{
		rv = UFWLabel;
	}

	return rv;
}

LabelWidthDots PrinterStockMetrics::getLabelWidthDots(LabelWidth labelWidth)
{
	LabelWidthDots rv = UnknownLabelWidth;

	if (labelWidth == NarrowLabel)
	{
		rv = NarrowLabelWidth;
	}
	else if (labelWidth == WideLabel)
	{
		rv = WideLabelWidth;
	}
	else if (labelWidth == UFWLabel)
	{
		rv = UFWLabelWidth;
	}

	return rv;
}

LabelLength PrinterStockMetrics::getLabelLength(UINT16 numLines)
{
	LabelLength rv = UnknownLabelLength;

	if ((0 < numLines) && (numLines <= Length1_000_Lines))
	{
		rv = Length1_000;
	}
	else if ((Length1_000_Lines < numLines) && (numLines <= Length1_500_Lines))
	{
		rv = Length1_500;
	}
	else if ((Length1_500_Lines < numLines) && (numLines <= Length1_750_Lines))
	{
		rv = Length1_750;
	}
	else if ((Length1_750_Lines < numLines) && (numLines <= Length2_000_Lines))
	{
		rv = Length2_000;
	}
	else if ((Length2_000_Lines < numLines) && (numLines <= Length2_375_Lines))
	{
		rv = Length2_375;
	}
	else if ((Length2_375_Lines < numLines) && (numLines <= Length2_500_Lines))
	{
		rv = Length2_500;
	}
	else if ((Length2_500_Lines < numLines) && (numLines <= Length3_000_Lines))
	{
		rv = Length3_000;
	}
	else if ((Length3_000_Lines < numLines) && (numLines <= Length3_500_Lines))
	{
		rv = Length3_500;
	}
	else if ((Length3_500_Lines < numLines) && (numLines <= Length4_000_Lines))
	{
		rv = Length4_000;
	}
	else if ((Length4_000_Lines < numLines) && (numLines <= Length4_500_Lines))
	{
		rv = Length4_500;
	}
	else if ((Length4_500_Lines < numLines) && (numLines <= Length5_000_Lines))
	{
		rv = Length5_000;
	}
	else if ((Length5_000_Lines < numLines) && (numLines <= Length5_500_Lines))
	{
		rv = Length5_500;
	}
	else if ((Length5_500_Lines < numLines) && (numLines <= Length6_000_Lines))
	{
		rv = Length6_000;
	}
	else if ((Length6_000_Lines < numLines) && (numLines <= Length6_500_Lines))
	{
		rv = Length6_500;
	}
	else if ((Length6_500_Lines < numLines) && (numLines <= Length7_000_Lines))
	{
		rv = Length7_000;
	}
	else if ((Length7_000_Lines < numLines) && (numLines <= Length7_500_Lines))
	{
		rv = Length7_500;
	}
	else if ((Length7_500_Lines < numLines) && (numLines <= Length8_000_Lines))
	{
		rv = Length8_000;
	}
	else if ((Length8_000_Lines < numLines) && (numLines <= Length8_500_Lines))
	{
		rv = Length8_500;
	}
	else if ((Length8_500_Lines < numLines) && (numLines <= Length9_000_Lines))
	{
		rv = Length9_000;
	}
	else if ((Length9_000_Lines < numLines) && (numLines <= Length9_500_Lines))
	{
		rv = Length9_500;
	}
	else if ((Length9_500_Lines < numLines) && (numLines <= Length10_000_Lines))
	{
		rv = Length10_000;
	}

	return rv;
}

LabelLengthLines PrinterStockMetrics::getLabelLengthLines(LabelLength labelLength)
{
	LabelLengthLines rv = UnknownLabelLengthLines;

	if (labelLength == Length1_000)
	{
		rv = Length1_000_Lines;
	}
	else if (labelLength == Length1_500)
	{
		rv = Length1_500_Lines;
	}
	else if (labelLength == Length1_750)
	{
		rv = Length1_750_Lines;
	}
	else if (labelLength == Length2_000)
	{
		rv = Length2_000_Lines;
	}
	else if (labelLength == Length2_375)
	{
		rv = Length2_375_Lines;
	}
	else if (labelLength == Length2_500)
	{
		rv = Length2_500_Lines;
	}
	else if (labelLength == Length3_000)
	{
		rv = Length3_000_Lines;
	}
	else if (labelLength == Length3_500)
	{
		rv = Length3_500_Lines;
	}
	else if (labelLength == Length4_000)
	{
		rv = Length4_000_Lines;
	}
	else if (labelLength == Length4_500)
	{
		rv = Length4_500_Lines;
	}
	else if (labelLength == Length5_000)
	{
		rv = Length5_000_Lines;
	}
	else if (labelLength == Length5_500)
	{
		rv = Length5_500_Lines;
	}
	else if (labelLength == Length6_000)
	{
		rv = Length6_000_Lines;
	}
	else if (labelLength == Length6_500)
	{
		rv = Length6_500_Lines;
	}
	else if (labelLength == Length7_000)
	{
		rv = Length7_000_Lines;
	}
	else if (labelLength == Length7_500)
	{
		rv = Length7_500_Lines;
	}
	else if (labelLength == Length8_000)
	{
		rv = Length8_000_Lines;
	}
	else if (labelLength == Length8_500)
	{
		rv = Length8_500_Lines;
	}
	else if (labelLength == Length9_000)
	{
		rv = Length9_000_Lines;
	}
	else if (labelLength == Length9_500)
	{
		rv = Length9_500_Lines;
	}
	else if (labelLength == Length10_000)
	{
		rv = Length10_000_Lines;
	}

	return rv;
}

LabelLengthLines PrinterStockMetrics::getActualLabelLengthLines(UINT16 numLinesMeasured, PrinterKind prKind)
{
	Debug::trace("PrinterStockMetrics.getActualLabelLengthLines", HobString("PrinterKind = ") + inttostring( prKind ) );

	LabelLengthLines rv = UnknownLabelLengthLines;

	if (numLinesMeasured == 0)
		return rv;

	if( prKind == EppPrinter || prKind == Epp2Printer || prKind == Epp3Printer )
	{
		//------------------------
		// HT-171: Changes for EPP
		//------------------------

		if ((MinLength_EPP_1_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_1_000))
		{
			rv = Length1_000_Lines;
		}
		else if ((MinLength_EPP_1_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_1_500))
		{
			rv = Length1_500_Lines;
		}
		else if ((MinLength_EPP_1_750 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_1_750))
		{
			rv = Length1_750_Lines;
		}
		else if ((MinLength_EPP_2_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_2_000))
		{
			rv = Length2_000_Lines;
		}
		else if ((MinLength_EPP_2_375 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_2_375))
		{
			rv = Length2_375_Lines;
		}
		else if ((MinLength_EPP_2_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_2_500))
		{
			rv = Length2_500_Lines;
		}
		else if ((MinLength_EPP_3_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_3_000))
		{
			rv = Length3_000_Lines;
		}
		else if ((MinLength_EPP_3_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_3_500))
		{
			rv = Length3_500_Lines;
		}
		else if ((MinLength_EPP_4_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_4_000))
		{
			rv = Length4_000_Lines;
		}
		else if ((MinLength_EPP_4_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_4_500))
		{
			rv = Length4_500_Lines;
		}
		else if ((MinLength_EPP_5_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_5_000))
		{
			rv = Length5_000_Lines;
		}
		else if ((MinLength_EPP_5_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_5_500))
		{
			rv = Length5_500_Lines;
		}
		else if ((MinLength_EPP_6_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_6_000))
		{
			rv = Length6_000_Lines;
		}
		else if ((MinLength_EPP_6_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_6_500))
		{
			rv = Length6_500_Lines;
		}
		else if ((MinLength_EPP_7_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_7_000))
		{
			rv = Length7_000_Lines;
		}
		else if ((MinLength_EPP_7_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_7_500))
		{
			rv = Length7_500_Lines;
		}
		else if ((MinLength_EPP_8_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_8_000))
		{
			rv = Length8_000_Lines;
		}
		else if ((MinLength_EPP_8_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_8_500))
		{
			rv = Length8_500_Lines;
		}
		else if ((MinLength_EPP_9_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_9_000))
		{
			rv = Length9_000_Lines;
		}
		else if ((MinLength_EPP_9_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_9_500))
		{
			rv = Length9_500_Lines;
		}
		else if ((MinLength_EPP_10_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_EPP_10_000))
		{
			rv = Length10_000_Lines;
		}
		else
		{
			Debug::trace("PrinterManager.Error", HobString("PrinterManager: ") +
												 HobString(" Label Sizing Error, numLinesMeasured: ") +
												 HobString(Integer::toString(numLinesMeasured)));
		}
	}
	else if( prKind == KPCPrinter )
	{
		if ((MinLength_KPC_1_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_1_000))
		{
			rv = Length1_000_Lines;
		}
		else if ((MinLength_KPC_1_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_1_500))
		{
			rv = Length1_500_Lines;
		}
		else if ((MinLength_KPC_1_750 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_1_750))
		{
			rv = Length1_750_Lines;
		}
		else if ((MinLength_KPC_2_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_2_000))
		{
			rv = Length2_000_Lines;
		}
		else if ((MinLength_KPC_2_375 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_2_375))
		{
			rv = Length2_375_Lines;
		}
		else if ((MinLength_KPC_2_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_2_500))
		{
			rv = Length2_500_Lines;
		}
		else if ((MinLength_KPC_3_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_3_000))
		{
			rv = Length3_000_Lines;
		}
		else if ((MinLength_KPC_3_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_3_500))
		{
			rv = Length3_500_Lines;
		}
		else if ((MinLength_KPC_4_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_4_000))
		{
			rv = Length4_000_Lines;
		}
		else if ((MinLength_KPC_4_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_4_500))
		{
			rv = Length4_500_Lines;
		}
		else if ((MinLength_KPC_5_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_5_000))
		{
			rv = Length5_000_Lines;
		}
		else if ((MinLength_KPC_5_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_5_500))
		{
			rv = Length5_500_Lines;
		}
		else if ((MinLength_KPC_6_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_6_000))
		{
			rv = Length6_000_Lines;
		}
		else if ((MinLength_KPC_6_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_6_500))
		{
			rv = Length6_500_Lines;
		}
		else if ((MinLength_KPC_7_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_7_000))
		{
			rv = Length7_000_Lines;
		}
		else if ((MinLength_KPC_7_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_7_500))
		{
			rv = Length7_500_Lines;
		}
		else if ((MinLength_KPC_8_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_8_000))
		{
			rv = Length8_000_Lines;
		}
		else if ((MinLength_KPC_8_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_8_500))
		{
			rv = Length8_500_Lines;
		}
		else if ((MinLength_KPC_9_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_9_000))
		{
			rv = Length9_000_Lines;
		}
		else if ((MinLength_KPC_9_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_9_500))
		{
			rv = Length9_500_Lines;
		}
		else if ((MinLength_KPC_10_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_KPC_10_000))
		{
			rv = Length10_000_Lines;
		}
		else
		{
			Debug::trace("PrinterManager.Error", HobString("PrinterManager: ") +
												 HobString(" Label Sizing Error, numLinesMeasured: ") +
												 HobString(Integer::toString(numLinesMeasured)));
		}
	} else if( prKind == SSKPCPrinter ) {
		if ((MinLength_FSSK_1_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_1_000))
		{
			rv = Length1_000_Lines;
		}
		else if ((MinLength_FSSK_1_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_1_500))
		{
			rv = Length1_500_Lines;
		}
		else if ((MinLength_FSSK_1_750 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_1_750))
		{
			rv = Length1_750_Lines;
		}
		else if ((MinLength_FSSK_2_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_2_000))
		{
			rv = Length2_000_Lines;
		}
		else if ((MinLength_FSSK_2_375 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_2_375))
		{
			rv = Length2_375_Lines;
		}
		else if ((MinLength_FSSK_2_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_2_500))
		{
			rv = Length2_500_Lines;
		}
		else if ((MinLength_FSSK_3_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_3_000))
		{
			rv = Length3_000_Lines;
		}
		else if ((MinLength_FSSK_3_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_3_500))
		{
			rv = Length3_500_Lines;
		}
		else if ((MinLength_FSSK_4_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_4_000))
		{
			rv = Length4_000_Lines;
		}
		else if ((MinLength_FSSK_4_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_4_500))
		{
			rv = Length4_500_Lines;
		}
		else if ((MinLength_FSSK_5_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_5_000))
		{
			rv = Length5_000_Lines;
		}
		else if ((MinLength_FSSK_5_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_5_500))
		{
			rv = Length5_500_Lines;
		}
		else if ((MinLength_FSSK_6_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_6_000))
		{
			rv = Length6_000_Lines;
		}
		else if ((MinLength_FSSK_6_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_6_500))
		{
			rv = Length6_500_Lines;
		}
		else if ((MinLength_FSSK_7_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_7_000))
		{
			rv = Length7_000_Lines;
		}
		else if ((MinLength_FSSK_7_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_7_500))
		{
			rv = Length7_500_Lines;
		}
		else if ((MinLength_FSSK_8_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_8_000))
		{
			rv = Length8_000_Lines;
		}
		else if ((MinLength_FSSK_8_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_8_500))
		{
			rv = Length8_500_Lines;
		}
		else if ((MinLength_FSSK_9_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_9_000))
		{
			rv = Length9_000_Lines;
		}
		else if ((MinLength_FSSK_9_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_9_500))
		{
			rv = Length9_500_Lines;
		}
		else if ((MinLength_FSSK_10_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength_FSSK_10_000))
		{
			rv = Length10_000_Lines;
	    }
	    else
	    {
			Debug::trace("PrinterManager.Error", HobString("PrinterManager: ") +
												 HobString(" Label Sizing Error, numLinesMeasured: ") +
												 HobString(Integer::toString(numLinesMeasured)));
		}
	} else {
		if ((MinLength1_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength1_000))
		{
			rv = Length1_000_Lines;
		}
		else if ((MinLength1_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength1_500))
		{
			rv = Length1_500_Lines;
		}
		else if ((MinLength1_750 <= numLinesMeasured) && (numLinesMeasured <= MaxLength1_750))
		{
			rv = Length1_750_Lines;
		}
		else if ((MinLength2_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength2_000))
		{
			rv = Length2_000_Lines;
		}
		else if ((MinLength2_375 <= numLinesMeasured) && (numLinesMeasured <= MaxLength2_375))
		{
			rv = Length2_375_Lines;
		}
		else if ((MinLength2_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength2_500))
		{
			rv = Length2_500_Lines;
		}
		else if ((MinLength3_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength3_000))
		{
			rv = Length3_000_Lines;
		}
		else if ((MinLength3_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength3_500))
		{
			rv = Length3_500_Lines;
		}
		else if ((MinLength4_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength4_000))
		{
			rv = Length4_000_Lines;
		}
		else if ((MinLength4_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength4_500))
		{
			rv = Length4_500_Lines;
		}
		else if ((MinLength5_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength5_000))
		{
			rv = Length5_000_Lines;
		}
		else if ((MinLength5_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength5_500))
		{
			rv = Length5_500_Lines;
		}
		else if ((MinLength6_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength6_000))
		{
			rv = Length6_000_Lines;
		}
		else if ((MinLength6_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength6_500))
		{
			rv = Length6_500_Lines;
		}
		else if ((MinLength7_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength7_000))
		{
			rv = Length7_000_Lines;
		}
		else if ((MinLength7_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength7_500))
		{
			rv = Length7_500_Lines;
		}
		else if ((MinLength8_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength8_000))
		{
			rv = Length8_000_Lines;
		}
		else if ((MinLength8_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength8_500))
		{
			rv = Length8_500_Lines;
		}
		else if ((MinLength9_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength9_000))
		{
			rv = Length9_000_Lines;
		}
		else if ((MinLength9_500 <= numLinesMeasured) && (numLinesMeasured <= MaxLength9_500))
		{
			rv = Length9_500_Lines;
		}
		else if ((MinLength10_000 <= numLinesMeasured) && (numLinesMeasured <= MaxLength10_000))
		{
			rv = Length10_000_Lines;
		}
		else
		{
			Debug::trace("PrinterManager.Error", HobString("PrinterManager: ") +
												 HobString(" Label Sizing Error, numLinesMeasured: ") +
												 HobString(Integer::toString(numLinesMeasured)));
		}
	}
	Debug::trace("PrinterStockMetrics.getActualLabelLengthLines", "Length = " + Integer::toString( rv ) );
	return rv;
}

bool PrinterStockMetrics::isContinuousStock() const
{
	UINT16 dimBLengthContinuous = 0;
	try
	{
		dimBLengthContinuous = (UINT16) Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);
	}
	catch (std::exception)
	{
	}
	bool result = (dimBLengthContinuous > 0 && labelLengthInLines == dimBLengthContinuous) ? true : false;
	Debug::trace( "PrinterStockMetrics", string("isContinuousStock() - labelLengthInLines:") + inttostring( labelLengthInLines ) + string("  returning:") + booltostring(result) );
	return result;
}

void PrinterStockMetrics::setContinuousStock()
{
	UINT16 dimBLengthContinuous = 0;
	try
	{
		dimBLengthContinuous =(UINT16) Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);
	}
	catch (std::exception)
	{
	}
	labelLengthInLines = dimBLengthContinuous;
	Debug::trace( "PrinterStockMetrics", string("setContinuousStock() - labelLengthInLines set to:") + inttostring( labelLengthInLines ) );
}

