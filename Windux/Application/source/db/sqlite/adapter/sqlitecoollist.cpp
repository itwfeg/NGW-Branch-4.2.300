#include "sqlitecoollist.h"
#include "sqlitecoollisttext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolList::LISTKEY_FIELD          ("LLID");
const string SqliteCoolList::NUMBER_FIELD           ("LLNum");
const string SqliteCoolList::DEPTNUM_FIELD          ("DeptNum");
const string SqliteCoolList::FIRSTLINKNUMBER_FIELD  ("LLFLink");


#ifdef USE_ACCOUNTANT
int SqliteCoolList::_dataType_ = 0;
string SqliteCoolList::_dataTypeName_("SqliteCoolList");
#endif



SqliteCoolList::SqliteCoolList(SqliteRecord* r, DBCoolListI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolList::~SqliteCoolList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


bool SqliteCoolList::containsCoolListText(const DBCoolListTextI* t) const
{
	const SqliteCoolListText* rec = dynamic_cast<const SqliteCoolListText*>(t);

	if ( rec )
		return getListKey() == rec->getCoolListKey();

	return false;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
