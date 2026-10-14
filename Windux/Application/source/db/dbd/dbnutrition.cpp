#include "edb.h"
#include DBNUTRITION_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBNutrition::DBNutrition(edb_adapter::DBNutritionI* d)
		: decorated_(d)
{
}


DBNutrition::DBNutrition(const DBNutrition& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBNutrition::~DBNutrition()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBNutrition& rec)
{
	os << "DBNutrition Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isNutritionColumn1Null() )
		os << ", Nutrition Column 1: " << rec.getNutritionColumn1();
	if ( !rec.isNutritionColumn2Null() )
		os << ", Nutrition Column 2: " << rec.getNutritionColumn2();
	if ( !rec.isNutritionColumn3Null() )
		os << ", Nutrition Column 3: " << rec.getNutritionColumn3();
	if ( !rec.isNutritionColumn4Null() )
		os << ", Nutrition Column 4: " << rec.getNutritionColumn4();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
