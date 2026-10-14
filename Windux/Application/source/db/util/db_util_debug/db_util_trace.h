#ifndef DB_UTIL_TRACE_H
#define DB_UTIL_TRACE_H



#include <string>
#include <set>


#ifdef VXWORKS
	#ifdef NO_NAMESPACES
		#define util_env
		#define util_string
	#endif
	#include "../properties/properties.h"
	#include "../thread/mutex.h"
	#include "../thread/lock.h"
#else
	#include "../properties/properties.h"
	#include "../thread/mutex.h"
	#include "../thread/lock.h"

	using util_thread::Lock;
	using util_thread::Mutex;
#endif


class Trace
{
	public:
		// Types, subtypes, and friends

		// --- Constructors and destructor ---
		static Trace* getInstance();      // Singleton pattern's getInstance
		static Trace* getInstance(util_env::Properties& prop, bool useLock = false);

		// Accessor methods
		bool isEnabled(void) { Lock< Mutex<> > lock(_mutex_); return enabled; }
		bool enable(const std::string& filename = "");
		bool disable();
		bool isTraceAll(void) { Lock< Mutex<> > lock(_mutex_); return traceAll; }
		bool setTraceAll(bool ta) { Lock< Mutex<> > lock(_mutex_); bool temp = traceAll; traceAll = ta; return temp; }
		int getTraceLevel(void) { Lock< Mutex<> > lock(_mutex_); return traceLevel; }
		int setTraceLevel(int level) { Lock< Mutex<> > lock(_mutex_); int temp = traceLevel; traceLevel = level; return temp; }
		const std::string& getDebugFilename() { Lock< Mutex<> > lock(_mutex_); return debugFilename; }

		// Processing methods
		bool isTracingCategory(const std::string& category);
		void addTracingCategory(const std::string& category);
		void removeTracingCategory(const std::string& category);

//			void trace(const std::string& category, int level, const std::string& message);
		void trace(const std::string& category, int level, const std::string& filename,
				int lineNumber, const std::string& message);


	protected:
		// --- Constructors and destructor ---
		// Constructor is protected because of the Singleton pattern
		Trace();
		virtual ~Trace();


	private:
		bool enabled, traceAll;
		int traceLevel;
		std::string debugFilename;
		std::set<std::string> traceCategories;
		std::ostream *out;
		bool useLock_;
		static util_thread::Mutex<> _mutex_;

		void closeDebugFile();

		// --- Not implemented methods (linker will complain if called) ---
		// Do not allow the default copy constructor.
		Trace(const Trace& db);
		// Do not allow the default assignment operator.
		Trace operator=(const Trace& rhs);
};


// Macro to add filename and line number to all traces
#ifndef TRACE_NOTRACEMACRO
#define trace(a, b, c)  trace(a, b, __FILE__, __LINE__, c)
#endif



#endif // DB_UTIL_TRACE_H

