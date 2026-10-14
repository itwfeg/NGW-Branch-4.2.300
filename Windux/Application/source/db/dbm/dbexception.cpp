#include "edb.h"
#include DBEXCEPTION_INC



#ifndef NO_NAMESPACES
namespace edb
{
#endif


DBException::DBException(Type t, const std::string& what, const std::exception* e)
		: util_exception::Exception(what, e), type(t)
{
}


DBException::~DBException() throw()
{
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif

