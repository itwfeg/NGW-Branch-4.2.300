#ifndef DRIVER_H
#define DRIVER_H



#include "DBManager.h"

#include PROPERTIES_INC
#include DEBUG_INC
#include THREAD_INC
#include DBPRODUCTSRM_INC

#ifdef WIN32_OS
#include "util\cloptions\cloptions.h"
#include "std\access\stdrecord.h"
#else
#include "util/cloptions/cloptions.h"
#include "std/access/stdrecord.h"
#endif


#ifndef NO_NAMESPACES
using namespace edb;
using namespace util_thread;
#endif


#include <string>


class Driver
{
	public:
		// ---  Types and Subtypes  ---
		class DriverThread;
//		typedef bool (* function_type)(STDRecord* rec, ThreadData* data);
		typedef bool (DriverThread::* recordFunction_type)(DBProductsRM* prods, const STDRecord* rec);
		typedef std::map<std::string, recordFunction_type> recordFunctionContainer_type;
		typedef bool (DriverThread::* dbFunction_type)(DBManager* db, const STDRecord* rec);
		typedef std::map<std::string, dbFunction_type> dbFunctionContainer_type;
		class DriverThread : public util_thread::Thread<>
		{
			public:
				DriverThread(const std::string& threadName, const std::string& filename,
						const Driver::recordFunctionContainer_type& recordFunctions,
						const Driver::dbFunctionContainer_type& dbFunctions)
						: Thread<>(threadName), commandsFilename_(filename),
						recFunctions_(recordFunctions), dbFunctions_(dbFunctions), db_(0)
						{ }
				~DriverThread() { if ( db_ ) DBManager::destroyInstance(db_); }

				// --- Processing methods ---
				int doRun();
				bool processWriteOneItem(DBProductsRM* prods, const STDRecord* rec);
				bool processReadOneItem(DBProductsRM* prods, const STDRecord* rec);
				bool processReadAllItem(DBProductsRM* prods, const STDRecord* rec);
				bool processExecuteOneBackup(DBManager* db, const STDRecord* rec);

			private:
				std::string commandsFilename_;
				recordFunctionContainer_type recFunctions_;
				dbFunctionContainer_type dbFunctions_;
				edb::DBManager* db_;
		};

		// Constructors and destructor
		Driver(int argc, char * argv[]);

		// Processing methods
		void testOne();
		void testTwo();
		void testError(const std::string& method, const std::string& table, const std::string& reason = "");


	private:
		util_env::CLOptions options_;
		util_env::Properties properties_;
		::Trace* debug_;  /* (SJJC port 6/19/06) */
		recordFunctionContainer_type recFunctions_;
		dbFunctionContainer_type dbFunctions_;
		static const int _threads_ = 1;
};



#endif // DRIVER_H

