/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AccessKeys.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AccessKeys::dataType = 0;
std::string AccessKeys::dataTypeName("AccessKeys"); 
#endif

AccessKeys::AccessKeys(int keys )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	keys_ = keys;
}

AccessKeys::AccessKeys(const char * /* data */, int /* len */, int /* formatType */)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

AccessKeys::~AccessKeys()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

int AccessKeys::getKeys() const
{
	return keys_;
}
void AccessKeys::setKeys(int keys)
{
	keys_ = keys;
}

bool AccessKeys::isSupervisorFunctionKeyPressed() const
{
	if (keys_ & AccessKeys::SupervisorImmediate)
		return true;
	else
		return false;
}

bool AccessKeys::isSupervisorKeyPresent() const
{
	if (keys_ & AccessKeys::SupervisorPermanent)
		return true;
	else
		return false;
}

bool AccessKeys::isServiceKeyPresent() const
{
	if (keys_ & AccessKeys::ServicePermanent)
		return true;
	else
		return false;
}

bool AccessKeys::isSelfServeKeyPresent() const
{
	if (keys_ & AccessKeys::SelfServePermanent)
		return true;
	else
		return false;
}

bool AccessKeys::isQATestKeyPresent() const
{
	if (keys_ & AccessKeys::QATest)
		return true;
	else
		return false;
}

bool AccessKeys::isMFGTestKeyPresent() const
{
	if (keys_ & AccessKeys::MFGTest)
		return true;
	else
		return false;
}

bool AccessKeys::isAnyAccessKeyPresent() const
{
	if (Debug::isTracing("AccessKeys"))
	{
		Debug::trace("AccessKeys", "isAnyAccessKeyPresent - keys_: " + Integer::toString(keys_));
		Debug::trace("AccessKeys", "isAnyAccessKeyPresent - SupervisorImmediate: " + (keys_ & AccessKeys::SupervisorImmediate) ? "true" : "false");
		Debug::trace("AccessKeys", "isAnyAccessKeyPresent - SupervisorPermanent: " + (keys_ & AccessKeys::SupervisorPermanent) ? "true" : "false");
		Debug::trace("AccessKeys", "isAnyAccessKeyPresent - ServicePermanent: " + (keys_ & AccessKeys::ServicePermanent) ? "true" : "false");
		Debug::trace("AccessKeys", "isAnyAccessKeyPresent - SelfServePermanent: " + (keys_ & AccessKeys::SelfServePermanent) ? "true" : "false");
	}
	if ((keys_ & AccessKeys::SupervisorImmediate) || (keys_ & AccessKeys::SupervisorPermanent) ||
		 (keys_ & AccessKeys::ServicePermanent) || (keys_ & AccessKeys::SelfServePermanent))
		return true;
	else
		return false;
}

void AccessKeys::setSupervisorFunctionKeyPressed(bool pressed)
{
	if (pressed)
		keys_ |= AccessKeys::SupervisorImmediate;
	else
		keys_ &= ~AccessKeys::SupervisorImmediate;
}

void AccessKeys::setSupervisorKeyPresent(bool present)
{
	if (present)
		keys_ |= AccessKeys::SupervisorPermanent;
	else
		keys_ &= ~AccessKeys::SupervisorPermanent;
}

void AccessKeys::setServiceKeyPresent(bool present)
{
	if (present)
		keys_ |= AccessKeys::ServicePermanent;
	else
		keys_ &= ~AccessKeys::ServicePermanent;
}

void AccessKeys::setQAServiceKeyPresent(bool present)
{
	if (present)
		keys_ |= AccessKeys::QATest;
	else
		keys_ &= ~AccessKeys::QATest;
}

void AccessKeys::setSelfServeKeyPresent(bool present)
{
	if (present)
		keys_ |= AccessKeys::SelfServePermanent;
	else
		keys_ &= ~AccessKeys::SelfServePermanent;
}

void AccessKeys::operator|=(const int newKeys)
{
	keys_ |= newKeys;
}

void AccessKeys::operator&=(const int newKeys)
{
	keys_ &= newKeys;
}

const char *AccessKeys::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			serializedData.append(Bool::toString(keys_ & SupervisorPermanent));
			serializedData.append(BrowserFormatSeparator);
			
			if ( (keys_ & SupervisorImmediate) > 0)
				serializedData.append(Bool::toString(true));
			else
				serializedData.append(Bool::toString(false));

			serializedData.append(BrowserFormatSeparator);
			
			if ( (keys_ & ServicePermanent) > 0)
				serializedData.append(Bool::toString(true));
			else
				serializedData.append(Bool::toString(false));

			serializedData.append(BrowserFormatSeparator);
			
			if ( (keys_ & SelfServePermanent) > 0)
				serializedData.append(Bool::toString(true));
			else
				serializedData.append(Bool::toString(false));

			serializedData.append(BrowserFormatSeparator);
			
			if ( (keys_ & QATest) > 0)
				serializedData.append(Bool::toString(true));
			else
				serializedData.append(Bool::toString(false));

			serializedData.append(BrowserFormatSeparator);

			if( (keys_ &  MFGTest) > 0)
				serializedData.append(Bool::toString(true));
			else
				serializedData.append(Bool::toString(false));

			break;

		default:
			serializedData = HobString("AccessKeys::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
