/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcUpdateStatus.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Bool.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcUpdateStatus::dataType = 0;
std::string HfcUpdateStatus::dataTypeName("HfcUpdateStatus"); 
#endif

//std::list<HfcUpdateStatusFieldOffsets> HfcUpdateStatus::browserFieldList;


HfcUpdateStatus::HfcUpdateStatus()
	: updateSuccessful(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcUpdateStatus::HfcUpdateStatus(bool success)
	: updateSuccessful(success)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

/*
HfcUpdateStatus::HfcUpdateStatus(const char * / *data* /, int / *len* /, int / *formatType* /)
	: updateSuccessful(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}
*/

HfcUpdateStatus::~HfcUpdateStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/*
//	private
void HfcUpdateStatus::init()
{
	initFieldList();
}

// static
void HfcUpdateStatus::initFieldList()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetHfc...);
	}
}
*/

const char *HfcUpdateStatus::serialize(int format) const
{
	serializedData = "";

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();

	switch ( format )
	{
		case BrowserFormat:
		{
			serializedData.append(Bool::toString(updateSuccessful));
			break;
		}
		default:
			serializedData = HobString("HfcUpdateStatus::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.data();
}
