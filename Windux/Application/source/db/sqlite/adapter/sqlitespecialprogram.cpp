#include "sqlitespecialprogram.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSpecialProgram::NUMBER_FIELD         ("SPNum");
const string SqliteSpecialProgram::DEPTNUM_FIELD        ("DeptNum");
const string SqliteSpecialProgram::DESCRIPTION1_FIELD   ("SPName1");
const string SqliteSpecialProgram::DESCRIPTION2_FIELD   ("SPName2");
const string SqliteSpecialProgram::DEFAULTLENGTH_FIELD  ("SPDefL");
const string SqliteSpecialProgram::IMAGE1_FIELD         ("SPImag1");
const string SqliteSpecialProgram::IMAGE2_FIELD         ("SPImag2");


#ifdef USE_ACCOUNTANT
int SqliteSpecialProgram::_dataType_ = 0;
string SqliteSpecialProgram::_dataTypeName_("SqliteSpecialProgram");
#endif



SqliteSpecialProgram::SqliteSpecialProgram(SqliteRecord* r, DBSpecialProgramI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSpecialProgram::~SqliteSpecialProgram()
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
