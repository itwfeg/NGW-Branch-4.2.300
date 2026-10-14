#include "edb.h"
#include DBCOOLCATEGORYPRODUCTIONSTEP_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryProductionStep::DBCoolCategoryProductionStep(edb_adapter::DBCoolCategoryProductionStepI* d)
		: decorated_(d)
{
}


DBCoolCategoryProductionStep::DBCoolCategoryProductionStep(const DBCoolCategoryProductionStep& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolCategoryProductionStep::~DBCoolCategoryProductionStep()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolCategoryProductionStep& rec)
{
	os << "DBCoolCategoryProductionStep CategoryNumber " << rec.getCategoryNumber() << ", ProductionStepKey " <<
			rec.getProductionStepKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
