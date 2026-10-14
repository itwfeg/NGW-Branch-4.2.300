#include "sqliteignoreword.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteIgnoreWord::LANGUAGE_FIELD  ("IWLang");
const string SqliteIgnoreWord::NAME_FIELD      ("IWName");


#ifdef USE_ACCOUNTANT
int SqliteIgnoreWord::_dataType_ = 0;
string SqliteIgnoreWord::_dataTypeName_("SqliteIgnoreWord");
#endif



SqliteIgnoreWord::SqliteIgnoreWord(SqliteRecord* r, DBIgnoreWordI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}


SqliteIgnoreWord::~SqliteIgnoreWord()
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
