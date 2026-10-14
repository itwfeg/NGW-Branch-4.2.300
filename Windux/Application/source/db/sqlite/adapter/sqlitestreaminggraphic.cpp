#include "sqlitestreaminggraphic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteStreamingGraphic::NUMBER_FIELD           ("SGNum");
const string SqliteStreamingGraphic::NEXT_FIELD             ("SGr_SGNum");
const string SqliteStreamingGraphic::TIME_FIELD             ("SGTime");
const string SqliteStreamingGraphic::ACTIONTYPE_FIELD       ("SGActT");
const string SqliteStreamingGraphic::ACTIONPARAMETER_FIELD  ("SGActP");
const string SqliteStreamingGraphic::IMAGE_FIELD            ("SGImage");


#ifdef USE_ACCOUNTANT
int SqliteStreamingGraphic::_dataType_ = 0;
string SqliteStreamingGraphic::_dataTypeName_("SqliteStreamingGraphic");
#endif



SqliteStreamingGraphic::SqliteStreamingGraphic(SqliteRecord* r, DBStreamingGraphicI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteStreamingGraphic::~SqliteStreamingGraphic()
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
