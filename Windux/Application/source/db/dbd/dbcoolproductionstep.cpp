#include "edb.h"
#include DBCOOLPRODUCTIONSTEP_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolProductionStep::DBCoolProductionStep(edb_adapter::DBCoolProductionStepI* d)
		: decorated_(d)
{
}


DBCoolProductionStep::DBCoolProductionStep(const DBCoolProductionStep& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolProductionStep::~DBCoolProductionStep()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolProductionStep& rec)
{
	os << "DBCoolProductionStep (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isAllCategoriesNull() )
		os << ", AllCategories: " << rec.getAllCategories();
	if ( !rec.isProductionStepNumberNull() )
		os << ", ProductionStepNumber: " << rec.getProductionStepNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
