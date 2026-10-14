#ifndef DB_UTIL_DEBUG_H
#define DB_UTIL_DEBUG_H



#include <string>
#include <set>


#define TRACE_NOTRACEMACRO
#include "db_util_trace.h"
#undef TRACE_NOTRACEMACRO


class Debug
{
	public:
//			void Debug(const std::string& category, int level, const std::string& message);
		static void trace(const std::string& category, int level, const std::string& filename,
				int lineNumber, const std::string& message)
		{ Trace::getInstance()->trace(category, level, filename, lineNumber, message); }


	private:
		// --- Not implemented methods (linker will complain if called) ---
		// Do not allow the default constructor.
		Debug();
		// Do not allow the default copy constructor.
		Debug(const Debug& db);
		// Do not allow the default assignment operator.
		Debug operator=(const Debug& rhs);
};


// Macro to add filename and line number to all traces
#ifndef DEBUG_NOTRACEMACRO
#define trace(a, b, c)  trace(a, b, __FILE__, __LINE__, c)
#endif



#endif // DB_UTIL_DEBUG_H

