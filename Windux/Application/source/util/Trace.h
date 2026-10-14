/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

// Based on the Debug class from TNP

#ifndef TRACE_H
#define TRACE_H

class CommMechanism;
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include "properties/properties.h"

#include "MsgTime.h"
#include "CEvolMsgReceiver.h"

#include "util/thread/mutex.h"

using namespace util_env;
class HobString;

/**
	Debug is used to help developers identify and localize problems in the code.

	Debug is preferred over Diagnostic since we can extend the functionality of Diagnostic
	yet only have to worry about excluding 1 class when we ship.

  Debug must be enabled via the Debug.setEnabled(bool) method for the trace()
  and pre/postCondition methods to work.  This allows one to turn debugging on and
  off quickly without having to exclude Debug during a recompile.
*/
class Trace : public Hobart::CEvolMsgReceiver
{
	public:
		~Trace();
	
		static Trace *getInstance();
		static Trace *getInstance(util_env::Properties& prop);

		static bool isRunning() {return (_instance_ ? true : false);}

		void load(Properties& prop);

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );
		
		bool isEnabled();
		void setEnabled(bool state, const char *filename = (char *)NULL);
		void setEnabled(bool state, const std::string &filename);

		bool isShowTiming() { return showTiming_; }
		void setShowTiming(bool timing) { showTiming_ = timing; }

		bool isAppendFile() { return appendFile; }
		void setAppendFile(bool append) { appendFile = append; }

		void setTraceAllCategories(bool state);
		bool isTaskInfoEnabled();
		void enableTaskInfo(bool enableTaskInfo);

		bool isConsoleOutput() const { return console; }
		void setConsoleOutput(bool value) { console = value; }

		int  getDebugStartupDelay() { return debugStartupDelay; }
		void setDebugStartupDelay(int delay) { debugStartupDelay = delay; }

		void closeDebugFile();


		/**
		 * A method that allows us to pass in an entry from a Properties instance
		 * in the form of "catagory1,catagory2,catagory3,..."
		 *
		 * Example cfg file entry:
		 * debugCatagories=HobFieldControl,HobTreeControl,HobListControl,HobDS,HobQueryDataSet
		 * 
		 * The above would then be parsed and the rhs would be passed in to this method.
		 */
		void addTraceCategories(HobString s);


		/**
		 * Returns true if the catagory is currently being traced.
		 */
		bool isTracing(const HobString& category);

		/**
		 * Set the minimum threshold for trace and warning output.
		 *         0 is highest level
		 *   +maxint is lowest level
		 *
		 * Setting this level to -1 will effectively turn off traces and warnings.
		 * NOTE: this does not effect direct access to 'out', nor does it effect output in the
		 *       checking functions. Use enableOutput() for that.
		 */
		int getTraceLevel();
		void setTraceLevel(int level);

		/**
		 * Category based tracing and warning.
		 * Pass in a unique String, Class or other object that supports a meaningful toString operation.
		 * Then when any call to trace or warning that takes a category
		 *   (eg. trace(Object category, String description))
		 * is called, the trace is only displayed if an addTraceCategory() call was made with the same
		 * category object.
		 */
		void addTraceCategory(const char *category);
		void addTraceCategory(const std::string &category);
		void addTraceCategory(const HobString &category);
		void removeTraceCategory(const char *category);
		void removeTraceCategory(const std::string &category);
		void removeTraceCategory(const HobString &category);

		void trace(const HobString& category, const HobString& description);
		void trace(int level, const HobString& description);
		void trace(const HobString& category, int level, const HobString& description);
		void trace(const HobString& category, int level, const HobString& fileName,
			const HobString& functionName, int lineNumber, const HobString& description);
		void trace(const HobString& category, int level, const HobString& fileName, 
			const HobString& className, const HobString& functionName, 
			int lineNumber, const HobString& description);
		void trace(const char * category, int level, const char * fileName, 
			const char *className, const char *functionName, int lineNumber, 
			const char *description);

		void setTraceAllCategoriesOf(const char *category);
		void setTraceAllCategoriesOf(const std::string &category);
		void setTraceAllCategoriesOf(const HobString &category);
		void clearTraceAllCategoriesOf(const char *category);
		void clearTraceAllCategoriesOf(const std::string &category);
		void clearTraceAllCategoriesOf(const HobString& category);
		bool matchesTraceAllCategoriesOf(const HobString &category);

		void show();

		void addTraceListener(CommMechanism *listener);
		void removeTraceListener(CommMechanism *listener);


	private:
	
		Trace();


		static Trace * _instance_;
		static util_thread::Mutex<> _instanceMutex_;

		int debugStartupDelay;
		bool traceAll;
		bool listTaskInfo;
		bool showTiming_;
		bool debugEnabled;
		std::ostream *debugFile;
		bool console;
		bool appendFile;
		int msgTooLargeCount;
		int overFlowCount;

		int outLevel;

		std::vector<std::string> traceCategories;
		std::vector<std::string> traceAllCategoriesOf;

		Util::CMSGTime basetime_;

		std::vector<CommMechanism *> traceListeners;

		Util::CSmartCriticalSection  traceCS;	

    #if defined(LINUX_FIX)
		void notifyTraceListeners(const char *msg, size_t msgLength);
    #else
		void notifyTraceListeners(const char *msg, unsigned int msgLength);
    #endif

		void addMessage(const HobString& msg);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#define traceIt(cat, lev, className, funcName, desc) trace(cat, lev, __FILE__, className, funcName, __LINE__, desc)

#endif // TRACE_H
