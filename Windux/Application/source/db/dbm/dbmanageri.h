#ifndef DBMANAGERI_H
#define DBMANAGERI_H



#include "edb.h"

#include PROPERTIES_INC
#include DEBUG_INC

//#include "trace.h"
#include TRACE_INC

#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

	class DBManagerI
	{
		public:
			virtual const util_env::Properties& getProperties() const = 0;
			virtual ::Trace& getDebug() const = 0;  /* (SJJC port 6/19/06) */
			virtual void reportErrorContext() const = 0;
	};

#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif



#endif // DBMANAGERI_H

