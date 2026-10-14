#ifndef DRIVER_H
#define DRIVER_H



#include "DBManager.h"

#ifdef WIN32_OS
#include "util\cloptions\cloptions.h"
#include "util\properties\properties.h"
#include "util\debug\trace.h"
#else
#include "util/cloptions/cloptions.h"
#include "util/properties/properties.h"
#include "util/debug/trace.h"
#endif


#include <string>


class Driver
{
	public:
		// Constructors and destructor
		Driver(int argc, char * argv[]);

		// Processing methods
		void testAll();
		void testError(const std::string& method, const std::string& table, const std::string& reason = "");
		void testPause(const std::string& message);
		void testSpecial();
		void testSpecial2();

#ifndef NO_NAMESPACES
		template <class DBEntityRM> void testAllTestsSimple(edb::DBManager* db, const std::string& key,
				const std::string& field, const std::string& value1, const std::string& value2,
				const std::string& search, const std::string& start, const std::string& stop);
		template <class DBEntityRM> void testAllTests(edb::DBManager* db, const std::string& key,
				const std::string& field, const std::string& value1, const std::string& value2,
				const std::string& search, const std::string& start, const std::string& stop);
		template <class DBEntityRM> void testAllTestsCompound(edb::DBManager* db,
				const std::string& key1, const std::string& key2,
				const std::string& field, const std::string& value1, const std::string& value2,
				const std::string& start, const std::string& stop);
		template <class DBEntityRM> void testAllRecords(edb::DBManager* db);
		template <class DBEntityRM> void testAverageSpace(edb::DBManager* db);
		template <class DBEntityRM> void testCreate(edb::DBManager* db,
				const std::string& key, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testCreateCompound(edb::DBManager* db,
				const std::string& key1, const std::string& key2, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testChange(edb::DBManager* db,
				const std::string& key, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testChangeCompound(edb::DBManager* db,
				const std::string& key1, const std::string& key2, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testTextSearch(edb::DBManager* db, const std::string& search);
		template <class DBEntityRM> void testDelete(edb::DBManager* db, const std::string& key);
		template <class DBEntityRM> void testDeleteCompound(edb::DBManager* db,
				const std::string& key1, const std::string& key2);
		template <class DBEntityRM> void testDeleteAll(edb::DBManager* db);
		template <class DBEntityRM> void testIndependentQueries(edb::DBManager* db,
				const std::string& start2, const std::string& end2);
#else
		template <class DBEntityRM> void testAllTests(DBManager* db, const std::string& key,
				const std::string& field, const std::string& value1, const std::string& value2,
				const std::string& start, const std::string& stop);
		template <class DBEntityRM> void testAllRecords(DBManager* db);
		template <class DBEntityRM> void testCreate(DBManager* db,
				const std::string& key, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testChange(DBManager* db,
				const std::string& key, const std::string& desc = "", const std::string& rc = "");
		template <class DBEntityRM> void testDelete(DBManager* db, const std::string& key);
		template <class DBEntityRM> void testDeleteAll(DBManager* db);
		template <class DBEntityRM> void testIndependentQueries(DBManager* db,
				const std::string& start2, const std::string& end2);
#endif


	private:
		util_env::CLOptions options;
		util_env::Properties properties;
		Trace* debug;
};



#endif // DRIVER_H

