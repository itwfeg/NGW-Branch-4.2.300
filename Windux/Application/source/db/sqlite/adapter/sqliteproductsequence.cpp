#include "sqliteproductsequence.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProductSequence::PRODUCTKEY_FIELD                 ("PrID");
const string SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD  ("PCLang");
const string SqliteProductSequence::CHARACTERSEQUENCE_FIELD          ("PCSeq");
const string SqliteProductSequence::PRODUCTNAME_FIELD                ("PCName");
const string SqliteProductSequence::PRODUCTPRICE_FIELD               ("PrPrice");


#ifdef USE_ACCOUNTANT
int SqliteProductSequence::_dataType_ = 0;
string SqliteProductSequence::_dataTypeName_("SqliteProductSequence");
#endif



SqliteProductSequence::SqliteProductSequence(SqliteRecord* r, DBProductSequenceI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductSequence::~SqliteProductSequence()
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
