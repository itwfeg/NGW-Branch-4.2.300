/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgItem.h"
//#include "HfcObject.h"
#include "Bool.h"
#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgItem::dataType = 0;
std::string CfgItem::dataTypeName("CfgItem"); 
#endif


#ifndef NO_NAMESPACES
using namespace util_string;
using namespace std;
#endif



CfgItem::CfgItem(const string &name, int fieldOffsetCfgNumber, Type type, const string &value, const string &commTag)
	: myCS_(), name_(name), fieldOffsetCfgNumber_(fieldOffsetCfgNumber), type_(type), value_(value), valueInt_(), valueBool_(), commTag_(commTag), defaulted_(true), modified_(true), persistent_(true)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	syncValues();
}


CfgItem::~CfgItem()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void CfgItem::syncValues()
{
	valueInt_ = (type_ == INT) ? stringtoint(value_) : 0;
	valueBool_ = (type_ == BOOL) ? stringtobool(value_) : false;
}

void CfgItem::setValue(const string &value)
{
	Util::CLockCriticalSection lock( myCS_ );
	if (value_ != value)
	{
		if (value.size() == 0 || value[0] == NULL) 
			value_ = "";
		else
			value_ = value;
		modified_ = true;
		defaulted_ = false;
		syncValues();
	}
}

void CfgItem::setValue(int value)
{
	setValue(inttostring(value));
}

void CfgItem::setValue(bool value)
{
	// Used Bool class to convert the boolean to string because it converts to "1" and "0"
	// booltostring converts it to "Y" and "N".
	setValue(string(Bool::toString(value)));
}

