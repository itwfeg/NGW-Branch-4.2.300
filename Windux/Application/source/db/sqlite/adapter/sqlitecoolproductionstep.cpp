#include "sqlitecoolproductionstep.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolProductionStep::PRODUCTIONSTEPID_FIELD   ("LSID");
const string SqliteCoolProductionStep::DEPTNUM_FIELD            ("DeptNum");
const string SqliteCoolProductionStep::NAME1_FIELD              ("LSName1");
const string SqliteCoolProductionStep::NAME2_FIELD              ("LSName2");
const string SqliteCoolProductionStep::ALLCATEGORIES_FIELD      ("LSAllCa");
const string SqliteCoolProductionStep::PRODUCTIONSTEPNUM_FIELD  ("LSKey");


#ifdef USE_ACCOUNTANT
int SqliteCoolProductionStep::_dataType_ = 0;
string SqliteCoolProductionStep::_dataTypeName_("SqliteCoolProductionStep");
#endif



SqliteCoolProductionStep::SqliteCoolProductionStep(SqliteRecord* r, DBCoolProductionStepI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolProductionStep::~SqliteCoolProductionStep()
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
