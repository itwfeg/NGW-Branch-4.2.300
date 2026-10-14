/**
 * @file PrinterTestCassette.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting results of print roller validation test.
 * Created on: Aug 28, 2020
 */
#include"PrinterTestCassette.h"
#include "Debug.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTestCassette::dataType = 0;
std::string PrinterTestCassette::dataTypeName("PrinterTestCassette"); 
#endif

PrinterTestCassette::PrinterTestCassette( Hobart::CEvolMsgReceiver *pHobThread,
            							  UINT16 measured,
			            				  UINT16 actual ) : PrinterGeneric( PrTestCassette, pHobThread, FirstPrinter )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	measuredSize_ = measured;
	actualSize_ = actual;

	//print roller validation test assumes the media sensor has been calibrated and 
	//that 9.5" stock is loaded into the cassette. 
	//print rollers that are in spec will measure a 9.5" label to within 60 steps
	if( measuredSize_ >= actualSize_ ) {
		if( ( measuredSize_ - actualSize_ ) <= MAX_DELTA ) {
			result_ = true;
		} else {
			result_ = false;
		}
	} else {
		if( ( actualSize_ - measuredSize_ ) <= MAX_DELTA ) {
			result_ = true;
		} else {
			result_ = false;
		}
	}
}

PrinterTestCassette::PrinterTestCassette(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &measuredSize_;
	for (unsigned int i = 0; i < sizeof(measuredSize_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &actualSize_;
	for (unsigned int i = 0; i < sizeof(actualSize_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PrinterTestCassette::~PrinterTestCassette()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *PrinterTestCassette::serialize() const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &measuredSize_;
	for (unsigned int i = 0; i < sizeof(measuredSize_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &actualSize_;
	for (unsigned int i = 0; i < sizeof(actualSize_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterTestCassette::show()
{
	Debug::trace("PrinterTestCassette", "measuredSize_: " + Integer::toString(measuredSize_));
	Debug::trace("PrinterTestCassette", "actualSize_: " + Integer::toString(actualSize_));
	
	int delta = 0;
	if( measuredSize_ >= actualSize_ ) {
		delta = measuredSize_ - actualSize_;
	} else {
		delta = actualSize_ - measuredSize_;
	}
	Debug::trace("PrinterTestCassette", "delta: " + Integer::toString(delta));
	if( delta <= MAX_DELTA ) {
		Debug::trace("PrinterTestCassette", "Print Roller within spec.");
	} else {
		Debug::trace("PrinterTestCassette", "Print Roller not within spec!!!");
	}
}

