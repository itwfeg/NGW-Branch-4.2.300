#include <string>


#include "edb.h"
#include DBSTATUSSTORAGE_INC
#include DBEXCEPTION_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBStatusStorage::DBStatusStorage()
		: name_(), value_(0)
{
}


DBStatusStorage::DBStatusStorage(const std::string& name, std::string* value)
		: name_(name), value_(value)
{
}


DBStatusStorage::DBStatusStorage(const DBStatusStorage& rec)
		: name_(rec.name_), value_(new string(*rec.value_))
{
}


DBStatusStorage::~DBStatusStorage()
{
	if ( value_ )
		delete value_;
}


std::string DBStatusStorage::getField(const std::string& field) const
{
	if ( !field.compare("Name") )
		return getName();
	else if ( !field.compare("Value") )
		return getValue();

	return "";
}


void DBStatusStorage::setField(const std::string& field, const std::string& value)
{
	if ( !field.compare("Name") )
		setName(value);
	else if ( !field.compare("Value") )
		setValue(value);
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
