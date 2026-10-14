#include "sqliteconfiguration.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteConfiguration::NAME_FIELD   ("CoName");
const string SqliteConfiguration::VALUE_FIELD  ("CoValue");


#ifdef USE_ACCOUNTANT
int SqliteConfiguration::_dataType_ = 0;
string SqliteConfiguration::_dataTypeName_("SqliteConfiguration");
#endif



SqliteConfiguration::SqliteConfiguration(SqliteRecord* r, DBConfigurationI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteConfiguration::~SqliteConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
