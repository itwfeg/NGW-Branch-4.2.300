#include "sqliteflashfile.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteFlashFile::NAME_FIELD      ("FFName");
const string SqliteFlashFile::DATE_FIELD      ("FFDate");
const string SqliteFlashFile::COMPLETE_FIELD  ("FFCmplt");


#ifdef USE_ACCOUNTANT
int SqliteFlashFile::_dataType_ = 0;
string SqliteFlashFile::_dataTypeName_("SqliteFlashFile");
#endif



SqliteFlashFile::SqliteFlashFile(SqliteRecord* r, DBFlashFileI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteFlashFile::~SqliteFlashFile()
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
