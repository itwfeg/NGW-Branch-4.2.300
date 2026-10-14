#include "sqlitefield.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

using namespace std;


const int SqliteField::SqliteTRUE  = 1;
const int SqliteField::SqliteFALSE = 0;
const string SqliteField::SqliteBOOLEAN("INTBOOLEAN");
const string SqliteField::SqliteDATETIME("TEXTDATETIME");
const string SqliteField::SqliteINTEGER("INTEGER");
const string SqliteField::SqliteTEXT("TEXT");


#ifdef USE_ACCOUNTANT
int SqliteField::_dataType_ = 0;
string SqliteField::_dataTypeName_("SqliteField");
#endif



SqliteField::SqliteField(const SqliteField& obj)
		: name_(obj.name_), valueType_(obj.valueType_), valueString_(obj.valueString_), valueInt_(obj.valueInt_), isNull_(obj.isNull_), modified_(obj.modified_), keyComponent_(obj.keyComponent_)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteField::SqliteField(const std::string fieldName, Type colType)
		: name_(fieldName), valueType_(colType), valueString_(), valueInt_(0), isNull_(true), modified_(false), keyComponent_(false)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}


SqliteField::SqliteField(const std::string fieldName, Type colType, const std::string& strValue)
		: name_(fieldName), valueType_(colType), valueString_(), valueInt_(0), isNull_(false), modified_(false), keyComponent_(false)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch ( colType )
	{
		case INVALID:
			break;
			
		case INTEGER:
			valueInt_ = util_string::stringtoint(strValue);
			break;

		case INTBOOLEAN:
			valueInt_ = util_string::stringtoint(strValue);
			if (valueInt_ != SqliteFALSE)
				valueInt_ = SqliteTRUE;
			break;

		default:
			valueString_ = strValue;
	}
}


SqliteField::SqliteField(const std::string fieldName, Type colType, int intValue)
		: name_(fieldName), valueType_(colType), valueString_(), valueInt_(0), isNull_(false), modified_(false), keyComponent_(false)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch ( colType )
	{
		case INVALID:
			isNull_ = true;
			break;
			
		case STRING:
		case DATETIME:
			valueString_ = util_string::inttostring(intValue);
			break;

		case INTBOOLEAN:
			valueInt_ = intValue;
			if (valueInt_ != SqliteFALSE)
				valueInt_ = SqliteTRUE;
			break;

		default:
			valueInt_ = intValue;
	}
}


SqliteField::~SqliteField()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void SqliteField::setValue(int value)
{
	switch ( valueType_ )
	{
		case INVALID:
			isNull_ = true;
			break;
		
		case STRING:
		case DATETIME:
			valueString_ = util_string::inttostring(value);
			isNull_ = false;
			break;
		
		default:
			valueInt_ = value;
			isNull_ = false;
			if (valueType_ == INTBOOLEAN && valueInt_ != SqliteFALSE)
				valueInt_ = SqliteTRUE;
	}
}


void SqliteField::setValue(const std::string& value)
{
	switch ( valueType_ )
	{
		case INVALID:
			isNull_ = true;
			break;
		
		case INTEGER:
			valueInt_ = util_string::stringtoint(value);
			isNull_ = false;
			break;

		case INTBOOLEAN:
			valueInt_ = util_string::stringtoint(value);
			if (valueInt_ != SqliteFALSE)
				valueInt_ = SqliteTRUE;
			isNull_ = false;
			break;

		default:
         valueString_ = value;
         isNull_ = false;
	}
}


SqliteField::operator int() const
{
	switch ( valueType_ )
	{
		case STRING:
		case DATETIME:
			return util_string::stringtoint(valueString_);

		default:
			return valueInt_;
	}
}


SqliteField::operator std::string() const
{
	switch ( valueType_ )
	{
		case INTEGER:
		case INTBOOLEAN:
			return util_string::inttostring(valueInt_);

		default:
			return valueString_;
	}
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
