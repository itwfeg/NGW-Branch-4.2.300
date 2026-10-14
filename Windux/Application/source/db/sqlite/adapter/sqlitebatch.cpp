#include "sqlitebatch.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteBatch::NUMBER_FIELD                  ("BaNum");
const string SqliteBatch::SCALEID_FIELD                 ("ScID");
const string SqliteBatch::VERSION_FIELD                 ("BaVer");
const string SqliteBatch::REMOTEVERSION_FIELD           ("BaRVer");
const string SqliteBatch::ORIGIN_FIELD                  ("BaOrigin");
const string SqliteBatch::TYPE_FIELD                    ("BaType");
const string SqliteBatch::CREATEDDATETIME_FIELD         ("BaCreaDT");
const string SqliteBatch::COMPLETED_FIELD               ("BaComp");
const string SqliteBatch::COMPLETEDDATETIME_FIELD       ("BaCompDT");
const string SqliteBatch::SIGNATURE_FIELD               ("BaSig");
const string SqliteBatch::PENDINGAPPLICATION_FIELD      ("BaPAppl");
const string SqliteBatch::APPLIEDDATETIME_FIELD         ("BaApplDT");
const string SqliteBatch::PENDINGSYNCHRONIZATION_FIELD  ("BaPSync");
const string SqliteBatch::SYNCHRONIZEDDATETIME_FIELD    ("BaSyncDT");
const string SqliteBatch::PENDINGDELETION_FIELD         ("BaPDelt");
const string SqliteBatch::DELETEDDATETIME_FIELD         ("BaDeltDT");


#ifdef USE_ACCOUNTANT
int SqliteBatch::_dataType_ = 0;
string SqliteBatch::_dataTypeName_("SqliteBatch");
#endif



SqliteBatch::SqliteBatch(SqliteRecord* r, DBBatchI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteBatch::~SqliteBatch()
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
