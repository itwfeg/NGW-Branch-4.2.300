#include "edb.h"
#include DBSTREAMINGGRAPHIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBStreamingGraphic::DBStreamingGraphic(edb_adapter::DBStreamingGraphicI* d)
		: decorated_(d)
{
}


DBStreamingGraphic::DBStreamingGraphic(const DBStreamingGraphic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBStreamingGraphic::~DBStreamingGraphic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBStreamingGraphic& rec)
{
	os << "DBStreamingGraphic Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isImageNull() )
		os << ", Image: " << rec.getImage();
	if ( !rec.isTimeNull() )
		os << ", Time: " << rec.getTime();
	if ( !rec.isActionTypeNull() )
		os << ", ActionType: " << rec.getActionType();
	if ( !rec.isActionParameterNull() )
		os << ", ActionParameter: " << rec.getActionParameter();
	if ( !rec.isNextNull() )
		os << "Next: " << rec.getNext();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
