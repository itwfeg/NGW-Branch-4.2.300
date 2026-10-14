#include "sqliteoperatornote.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteOperatorNote::TEXT_FIELD  ("ONText");


#ifdef USE_ACCOUNTANT
int SqliteOperatorNote::_dataType_ = 0;
string SqliteOperatorNote::_dataTypeName_("SqliteOperatorNote");
#endif



SqliteOperatorNote::SqliteOperatorNote(SqliteRecord* r, DBOperatorNoteI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteOperatorNote::~SqliteOperatorNote()
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
