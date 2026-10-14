#ifdef APPLICATION_DRIVER


#include <string>
#include <iostream>
#include <cstdio>          // for getchar


#include "edb.h"

#include "driver.h"

#include DBPRODUCTSRM_INC
#include DBOPERATORSRM_INC
#include DBOPERATORCHANGESRM_INC
#include DBCLASSESRM_INC
#include DBLABELSRM_INC
#include DBROMLABELSRM_INC
#include DBLABELTEXTSRM_INC
#include DBTEXTSRM_INC
#include DBNOTESRM_INC
#include DBMESSAGESRM_INC
#include DBNUTRITIONSRM_INC
#include DBOPERATORNOTESRM_INC
#include DBTOTALTYPESRM_INC
#include DBPRODUCTTOTALSRM_INC
#include DBPRODUCTDEFAULTSRM_INC
#include DBCONFIGURATIONSRM_INC
#include DBSOFTKEYSRM_INC
#include DBPRODUCTSTATISTICSRM_INC
#include DBFLASHFILESRM_INC
#include DBPRESENTATIONMETHODSRM_INC
#include DBCATEGORIESRM_INC
#include DBSTREAMINGGRAPHICSRM_INC
#include DBQUERY_INC

#include DBPRODUCT_INC
#include DBOPERATOR_INC
#include DBOPERATORCHANGE_INC
#include DBCLASS_INC
#include DBLABEL_INC
#include DBROMLABEL_INC
#include DBLABELTEXT_INC
#include DBTEXT_INC
#include DBNOTE_INC
#include DBMESSAGE_INC
#include DBNUTRITION_INC
#include DBOPERATORNOTE_INC
#include DBTOTALTYPE_INC
#include DBPRODUCTTOTAL_INC
#include DBPRODUCTDEFAULT_INC
#include DBCONFIGURATION_INC
#include DBSOFTKEY_INC
#include DBPRODUCTSTATISTIC_INC
#include DBFLASHFILE_INC
#include DBPRESENTATIONMETHOD_INC
#include DBCATEGORY_INC
#include DBSTREAMINGGRAPHIC_INC

#include LOCKMANAGER_INC
#include TYPES_INC

#include DIRECTORY_INC


#ifndef NO_NAMESPACES
using namespace edb;
using util_env::CLOptions;
using util_env::Properties;
#endif


using namespace std;




Driver::Driver(int argc, char* argv[])
		: options('-'), properties("defaults.ini"), debug(0)
{
	// Deal with the command line options and parameters
	options.processCommandLine(argc,argv);

	// Get the application properties if any
	CLOptions::option_iterator configFilename = options.findOption("cfg");
	if ( configFilename != options.endOption() )
		properties.load(*configFilename);

	// Make properties available to DBManager
	DBManager::setConfiguration(&properties);

	// Get the debugging mechanism and make it available to DBManager
	debug = Trace::getInstance(properties);
	DBManager::setDebug(debug);

//	testSpecial();
}


void Driver::testSpecial()
{
	for ( int i = 0; i < 100; ++i )
	{
		testPause("Before allocating");
		char* buffer = new char[12*1024];
		testPause("After allocating");
		delete[] buffer;
		testPause("After deallocating");
	}
}


void Driver::testSpecial2()
{
	DBManager* db = DBManager::getInstance();
	DBProductsRM* prods = db->createProducts();
	DBQuery<DBProduct>* query = prods->retrieveRangePLU("2100", "4650");
	DBQuery<DBProduct>::iterator iter1 = query->end(), iter2 = query->end();
	DateTime before, after;
	int iterations = 0;

	before.getCurrentDateTime();
	for ( int i = 0; i < 1000; ++i )
	{
		iter1 = query->begin();
		while ( iter1 != iter2 )
		{
			++iter1;
			++iterations;
		}

	}
	after.getCurrentDateTime();
	cout << "Start time: " << before << " End time: " << after << " Iterations: " << iterations << endl;



/*
	cout << "-------------------------------------------------------------------" << endl;
	cout << "Moving with the first iterator" << endl;

	iter1 = query->begin();
	cout << "1st product is " << *iter1 << endl;

	++++++iter1;
	cout << "4th product is " << *iter1;

	cout << "-------------------------------------------------------------------" << endl;
	cout << "Moving with the second iterator" << endl;

	iter2 = query->begin();
	cout << "1st product is " << *iter2 << endl;

	++iter2;
	cout << "2nd product is " << *iter2 << endl;

	cout << "-------------------------------------------------------------------" << endl;
	cout << "Back to the first iterator" << endl;

	cout << "4th product again is " << *iter1 << endl;

	++iter1;
	cout << "5th product is " << *iter1 << endl;

	++iter1;
	cout << "6th product is " << *iter1 << endl;

	--iter1;
	cout << "5th product is " << *iter1 << endl;

	cout << "-------------------------------------------------------------------" << endl;
	cout << "Back to the second iterator" << endl;

	cout << "2nd product again is " << *iter2 << endl;

	++++iter2;
	cout << "4th product is " << *iter2 << endl;

	--iter2;
	cout << "3rd product is " << *iter2 << endl;
*/
}


void Driver::testAll()
{
//	testPause("Before beginning testing");

	DBManager* db = DBManager::getInstance();

	debug = &db->getDebug();
	debug->trace("database.driver.init", 20, "Beginning test all");
//	if ( debug )
//	{
//		util_filesystem::Directory dir("c:\\users\\juancsj\\Evolution");
//		cout << dir;
//
//		return;
//	}

	db->checkDatabase();

	db->openDatabase();

	testSpecial2();

#if 0
	if ( db )
	{
		const DBManager::factoryMethodContainer_type& createMethods = db->getCreateMethods();
		DBManager::factoryMethodIterator iter = createMethods.begin();
		while ( iter != createMethods.end() )
		{
			DBRecordManager* dbrm = (**(iter++))();
			if ( dbrm->isEnabled() )
			{
				cout << "Record Manager enabled" << endl;
				cout << "Record Manager deleted " << dbrm->deleteAll() << " records." << endl;
			}
			else
				cout << "Record Manager disabled" << endl;
		}
	}
	else
#endif
	{
		for ( int i = 0; i < 1; ++i )
		{
			testAllTests<DBProductsRM>(db, string("26"), string("CLNUM"), string("2000"),
					string("1000"), string("Record created"), string("10000"), string("999999"));
	//		testPause("Completed all tests for Products");

			testAllTests<DBClassesRM>(db, string("1000"), string("CLA_CLNUM"), string("999996"),
					string("999999"), string("a"), string("10000"), string("999999"));
	//		testPause("Completed all tests for Classes");

			testAllTests<DBOperatorsRM>(db, string("25"), string("OPSAWNOTE"), string("Y"),
					string("N"), string("a"), string("3000"), string("6000"));
	//		testPause("Completed all tests for Operators");

			testAllTestsSimple<DBOperatorChangesRM>(db, string("3"), string("OCOLDSV"), string("Y"),
					string("N"), string("a"), string("3000"), string("6000"));
	//		testPause("Completed all tests for Operator Changes");

			testAllTests<DBLabelsRM>(db, string("25"), string("CLNUM"), string("2000"),
					string("1000"), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for Labels");

			testAllTests<DBROMLabelsRM>(db, string("25"), string("RLBBIN"), string("2000"),
					string("1000"), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for Labels");

			testAllTests<DBTextsRM>(db, string("25"), string("CLNUM"), string("2000"),
					string("1000"), string("sodium"), string("3"), string("600"));
	//		testPause("Completed all tests for Texts");

			testAllTests<DBNotesRM>(db, string("25"), string("CLNUM"), string("2000"),
					string("1000"), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for Notes");

			testAllTests<DBMessagesRM>(db, string("25"), string("CLNUM"), string("2000"),
					string("1000"), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for Messages");

			testAllTests<DBNutritionsRM>(db, string("25"), string("CLNUM"), string("2000"),
					string("1000"), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for Nutritions");

	/*		testAllTests<DBOperatorNotesRM>(db, string("25"), string("ONTEXT"), string("IMPORTANT: Do not turn off the scale unless for an emergency."),
					string("PARTY TONIGHT: Don't forget to bring your own beer."), string("a"), string("3"), string("600"));
	//		testPause("Completed all tests for OperatorNotes");
	  */
			testAllTests<DBProductDefaultsRM>(db, string("5"), string("PDCLNUM"), string("2000"),
					string("1000"), string("a"), string("1"), string("10"));
	//		testPause("Completed all tests for ProductDefaults");

//			testAllTestsCompound<DBProductTotalsRM>(db, string("10"), string("1"), string("TTRUNS"), string("2000"),
//					string("1000"), string("10000"), string("999999"));
	//		testPause("Completed all tests for ProductTotals");

			testAllTests<DBLabelTextsRM>(db, string("25"), string("LTTEXT"), string("Label text 20"),
					string("Label text 25"), string("a"), string("10"), string("99"));
	//		testPause("Completed all tests for LabelTexts");

			testAllTests<DBConfigurationsRM>(db, string("abcde"), string("COVALUE"), string("fghijk"),
					string("lmnopq"), string("a"), string("aaa"), string("zzz"));
	//		testPause("Completed all tests for Configurations");

			testAllTests<DBTotalTypesRM>(db, string("25"), string("TTNAME"), string("totals 23"),
					string("totals 25"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for TotalTypes");

			testAllTests<DBSoftKeysRM>(db, string("25"), string("SKDESC"), string("softkey 23"),
					string("softkey 25"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for SoftKeys");

//			testAllTestsCompound4<DBProductStatistcsRM>(db, string("25"), string("SKDESC"), string("softkey 23"),
//					string("softkey 25"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for ProductStatistics");

			testAllTests<DBFlashFilesRM>(db, string("flashfile1"), string("FFDATE"), string("09/02/03"),
					string("09/20/03"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for FlashFiles");

//			testAllTestsCompound<DBPresentationMethodsRM>(db, string("category1"), string("CANAME"), string("name1"),
//					string("name2"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for Categories");

			testAllTests<DBCategoriesRM>(db, string("category1"), string("CANAME"), string("name1"),
					string("name2"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for Categories");

			testAllTests<DBStreamingGraphicsRM>(db, string("graphic1"), string("SGNAME"), string("name1"),
					string("name2"), string("a"), string("1"), string("99"));
	//		testPause("Completed all tests for StreamingGraphics");
		}
	}

	LockManager::getInstance().debugState();
/*
	db->closeDatabase();

	LockManager::getInstance().debugState();

	db->compressDatabase();
	db->deleteDatabase();

	LockManager::getInstance().debugState();
*/
	debug->trace("database.driver.init", 21,
			"Before destroying the DBManager instance.");

	DBManager::destroyInstance(db);

	debug->trace("database.driver.init", 24, "Ending test all");
}


void Driver::testError(const std::string& method, const std::string& table, const std::string& reason)
{
	cout << endl << endl << "ERROR: found in " << method << " when processing " << table << " TABLE." << endl << endl;
	if ( !reason.empty() )
		cout << "Reason: " << reason << endl << endl;
	cout << "\x07\x07\x07\x07\x07PRESS ENTER";
	getchar();
}


void Driver::testPause(const string& message)
{
	cout << endl << message << endl << "PRESS ENTER";
	getchar();
}


template <class DBEntityRM>
void Driver::testAllTestsSimple(edb::DBManager* db, const std::string& key, const std::string& field,
		const std::string& /*value1*/, const std::string& value2, const std::string& /*searchText*/,
		const std::string& /*start*/, const std::string& /*stop*/)
{
	debug->trace("database.driver.alltests", 20, "Beginning all tests for entity ");

	testAllRecords<DBEntityRM>(db);
//	testPause("Completed All Records test");
	testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
//	testCreate<DBEntityRM>(db, key, field, value1);
//	testPause("Completed Create test");
	testChange<DBEntityRM>(db, key, field, value2);
//	testPause("Completed Change test");
//	testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
//	testTextSearch<DBEntityRM>(db, searchText);
//	testPause("Completed Text Search test");
//	testIndependentQueries<DBEntityRM>(db, start, stop);
//	testPause("Completed Independent Queries test");
	//testDelete<DBEntityRM>(db, key);
//	testPause("Completed Delete test");
	//testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
	testDeleteAll<DBEntityRM>(db);
//	testPause("Completed Delete All test");
	testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");

	debug->trace("database.driver.alltests", 24, "Ending all tests for entity ");
}


template <class DBEntityRM>
void Driver::testAllTests(edb::DBManager* db, const std::string& /*key*/, const std::string& /*field*/,
		const std::string& /*value1*/, const std::string& /*value2*/, const std::string& /*searchText*/,
		const std::string& /*start*/, const std::string& /*stop*/)
{
	debug->trace("database.driver.alltests", 20, "Beginning all tests for entity ");

	testAllRecords<DBEntityRM>(db);
//	testPause("Completed All Records test");
	testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
	//testCreate<DBEntityRM>(db, key, field, value1);
//	testPause("Completed Create test");
	//testChange<DBEntityRM>(db, key, field, value2);
//	testPause("Completed Change test");
	//testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
//	testTextSearch<DBEntityRM>(db, searchText);
//	testPause("Completed Text Search test");
//	testIndependentQueries<DBEntityRM>(db, start, stop);
//	testPause("Completed Independent Queries test");
	//testDelete<DBEntityRM>(db, key);
//	testPause("Completed Delete test");
	//testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");
//	testDeleteAll<DBEntityRM>(db);
//	testPause("Completed Delete All test");
//	testAverageSpace<DBEntityRM>(db);
//	testPause("Completed Average Space test");

	debug->trace("database.driver.alltests", 24, "Ending all tests for entity ");
}


template <class DBEntityRM>
void Driver::testAllTestsCompound(edb::DBManager* db, const std::string& key1,
		const std::string& key2, const std::string& field,
		const std::string& value1, const std::string& value2, const std::string& start, const std::string& stop)
{
	debug->trace("database.driver.alltestsCompound", 20, "Beginning all tests for entity ");

	testAllRecords<DBEntityRM>(db);
	testCreateCompound<DBEntityRM>(db, key1, key2, field, value1);
	testChangeCompound<DBEntityRM>(db, key1, key2, field, value2);
//	testIndependentQueries<DBEntityRM>(db, start, stop);
	testDeleteCompound<DBEntityRM>(db, key1, key2);
	testDeleteAll<DBEntityRM>(db);

	debug->trace("database.driver.alltestsCompound", 24, "Ending all tests for entity ");
}


template <class DBEntityRM>
void Driver::testAllRecords(edb::DBManager* db)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.allRecords", 20, "Beginning testAllRecords");

	int records = 0, valid = 0;
	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::query_type* query = rm->retrieveAll();
	typename DBEntityRM::query_type::size_type totalq = query->size();
	typename DBEntityRM::query_type::size_type totalrm = rm->countAll();

	typename DBEntityRM::query_type::const_iterator it = query->begin();
	typename DBEntityRM::query_type::const_iterator end = query->end();

	cout << endl << endl << "******* LIST OF ALL " << DBEntityRM::DESCRIPTOR
			<< " RECORDS *******" << endl << endl;
	while ( it != end )
	{
		if ( it->isRecordValid() )
			++valid;

		cout << *it;
//		cout << *it++;

		++records;
		if ( records == 2 )
		{
			--it;
			cout << "The previous record is ..." << endl << *it++;
			cout << "Now we go back to where we left off before" << endl;
		}

		++it;
	}

	cout << "Listed " << totalq << " records (" << valid << " valid) of " << totalrm <<
			" records reported to be stored in the table." << endl;

	if ( totalq != totalrm || records != totalq )
		testError("testAll", DBEntityRM::DESCRIPTOR,
				"The total number of records in record manager and in the query did not match.");

	if ( valid != records )
		testError("testAll", DBEntityRM::DESCRIPTOR,
				"Not all records stored in the database were valid.");

	delete query;
	delete rm;

	debug->trace("database.driver.tests.allRecords", 24, "Ending testAllRecords");
#endif
}


template <class DBEntityRM>
void Driver::testAverageSpace(edb::DBManager* db)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.averageSpace", 20, "Beginning testAverageSpace");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::query_type::size_type totalrm = rm->countAll();

	cout << endl << endl << "******* AVERAGE SPACE OCCUPIED BY " << DBEntityRM::DESCRIPTOR
			<< " RECORDS *******" << endl << endl;

	cout << "The average space occupied by the " << totalrm
			<< " records (in addition to any deleted records) stored is " <<
			rm->calculateAverageSpace() << " bytes." << endl;

	delete rm;

	debug->trace("database.driver.tests.averageSpace", 24, "Ending testAverageSpace");
#endif
}


template <class DBEntityRM>
void Driver::testCreate(edb::DBManager* db, const std::string& key, const std::string& field, const std::string& value)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.create", 20, "Beginning testCreate");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm->find(key);

	cout << endl << endl << "******* CREATING " << DBEntityRM::DESCRIPTOR <<
			" RECORD " << key << " *******" << endl << endl;
	if ( !rec )
	{
		rec = rm->createRecord();
		rec->setKey(key);
		if ( !field.empty() )
			rec->setField(field, value);

		cout << "New Record " << rec->getKey() << " is: " << endl << *rec << endl;

		// Save the created product
		cout << "Saving created Record " << rec->getKey() << endl;
		rm->saveRecord(rec);
		delete rec;


		// Verify saved record
		cout << "Verifying that the record was saved" << endl;
		rec = rm->find(key);
		if ( rec )
		{
			cout << "Found Record " << rec->getKey() << ": " << endl << *rec << endl;

			if ( key != rec->getKey() )
				testError("testCreate", DBEntityRM::DESCRIPTOR, "Key value not saved correctly.");

			if ( !field.empty() && value != rec->getField(field) )
				testError("testCreate", DBEntityRM::DESCRIPTOR, field + "'s value not saved correctly.");

			delete rec;
		}
		else
			testError("testCreate", DBEntityRM::DESCRIPTOR, "Unable to create record in database.");
	}
	else
	{
		testError("testCreate", DBEntityRM::DESCRIPTOR, "Record already existed.");

		delete rec;
	}

	delete rm;

	debug->trace("database.driver.tests.create", 24, "Ending testCreate");
#endif
}


template <class DBEntityRM>
void Driver::testCreateCompound(edb::DBManager* db, const std::string& key1,
		const std::string& key2, const std::string& field, const std::string& value)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.testsCompound.create", 20, "Beginning testCreateCompound");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm->findKey(key1, key2);

	cout << endl << endl << "******* CREATING " << DBEntityRM::DESCRIPTOR <<
			" RECORD " << key1 << "," << key2 << " *******" << endl << endl;
	if ( !rec )
	{
		rec = rm->createRecord();
		rec->setKey1(key1);
		rec->setKey2(key2);
		if ( !field.empty() )
			rec->setField(field, value);

		cout << "New Record " << rec->getKey1() << "," << rec->getKey2() << " is: " << endl << *rec << endl;

		// Save the created product
		cout << "Saving created Record " << rec->getKey1() << "," << rec->getKey2() << endl;
		rm->saveRecord(rec);
		delete rec;


		// Verify saved record
		cout << "Verifying that the record was saved" << endl;
		rec = rm->findKey(key1, key2);
		if ( rec )
		{
			cout << "Found Record " << rec->getKey1() << "," << rec->getKey2() << ": " << endl << *rec << endl;

			if ( key1 != rec->getKey1() && key2 != rec->getKey2() )
				testError("testCreate", DBEntityRM::DESCRIPTOR, "Key value not saved correctly.");

			if ( !field.empty() && value != rec->getField(field) )
				testError("testCreate", DBEntityRM::DESCRIPTOR, field + "'s value not saved correctly.");

			delete rec;
		}
		else
			testError("testCreate", DBEntityRM::DESCRIPTOR, "Unable to create record in database.");
	}
	else
	{
		testError("testCreate", DBEntityRM::DESCRIPTOR, "Record already existed.");

		delete rec;
	}

	delete rm;

	debug->trace("database.driver.testsCompound.create", 24, "Ending testCreateCompound");
#endif
}


template <class DBEntityRM>
void Driver::testChange(edb::DBManager* db, const std::string& key, const std::string& field, const std::string& value)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.change", 20, "Beginning testChange");

	DBEntityRM* rm1 = db->create<DBEntityRM>();
	DBEntityRM* rm2 = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm1->find(key);

	cout << endl << endl << "******* CHANGING " << DBEntityRM::DESCRIPTOR
			<< " RECORD " << key << " *******" << endl << endl;
	if ( rec )
	{
		bool modified = false;

		cout << "Found Record " << rec->getKey() << " through rm1: " << endl << *rec << endl;
		typename DBEntityRM::record_type* rec2 = rm2->find(key);
		cout << "Found record " << rec2->getKey() << " through rm2: " << endl << *rec2 << endl;
		delete rec2;
		if ( !field.empty() )
		{
			cout << "Changing Record " << rec->getKey() << "'s " << field << " from ";
			cout << rec->getField(field) << " to " << value << endl;

			rec->setField(field, value);
			modified = true;
		}

		if ( modified )
		{
			cout << "New Record " << rec->getKey() << " is: " << endl << *rec << endl;

			// Save the modified product
			cout << "Saving modified Record " << rec->getKey() << " through rm1" << endl;
			rm1->saveRecord(rec);
			delete rec;


			// Verify saved record
			cout << "Verifying that the record was saved" << endl;
			rec = rm1->find(key);
			if ( rec )
			{
				cout << "Found Record " << rec->getKey() << " through rm1: " << endl << *rec << endl;

				if ( modified )
				{
					if ( value != rec->getField(field) )
						testError("testChange", DBEntityRM::DESCRIPTOR, "New " + field + " not saved.");
				}

				delete rec;
			}
			else
				testError("testChange", DBEntityRM::DESCRIPTOR, "Record disappeared.");


			// Verify saved record through the other record manager
			rec = rm2->find(key);
			if ( rec )
			{
				cout << "Found Record " << rec->getKey() << " through rm2: " << endl << *rec << endl;

				if ( modified )
				{
					if ( value != rec->getField(field) )
						testError("testChange", DBEntityRM::DESCRIPTOR, "New " + field + " not saved.");
				}

				delete rec;
			}
			else
				testError("testChange", DBEntityRM::DESCRIPTOR, "Record disappeared.");
		}
		else
			testError("testChange", DBEntityRM::DESCRIPTOR, "Did not try to modify record.");
	}
	else
		testError("testChange", DBEntityRM::DESCRIPTOR, "Record did not exist.");

	delete rm1;
	delete rm2;

	debug->trace("database.driver.tests.change", 24, "Ending testChange");
#endif
}


template <class DBEntityRM>
void Driver::testChangeCompound(edb::DBManager* db, const std::string& key1,
		const std::string& key2, const std::string& field, const std::string& value)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.testsCompound.change", 20, "Beginning testChangeCompound");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm->findKey(key1, key2);

	cout << endl << endl << "******* CHANGING " << DBEntityRM::DESCRIPTOR
			<< " RECORD " << key1 << "," << key2 << " *******" << endl << endl;
	if ( rec )
	{
		bool modified = false;

		cout << "Found Record " << rec->getKey1() << "," << rec->getKey2() << ": " << endl << *rec << endl;
		if ( !field.empty() )
		{
			cout << "Changing Record " << rec->getKey1() << "," << rec->getKey2() << "'s " << field << " from ";
			cout << rec->getField(field) << " to " << value << endl;

			rec->setField(field, value);
			modified = true;
		}

		if ( modified )
		{
			cout << "New Record " << rec->getKey1() << "," << rec->getKey2() << " is: " << endl << *rec << endl;

			// Save the modified product
			cout << "Saving modified Record " << rec->getKey1() << "," << rec->getKey2() << endl;
			rm->saveRecord(rec);
			delete rec;


			// Verify saved record
			cout << "Verifying that the record was saved" << endl;
			rec = rm->findKey(key1, key2);
			if ( rec )
			{
				cout << "Found Record " << rec->getKey1() << "," << rec->getKey2() << ": " << endl << *rec << endl;

				if ( modified )
				{
					if ( value != rec->getField(field) )
						testError("testChange", DBEntityRM::DESCRIPTOR, "New " + field + " not saved.");
				}

				delete rec;
			}
			else
				testError("testChange", DBEntityRM::DESCRIPTOR, "Record disappeared.");
		}
		else
			testError("testChange", DBEntityRM::DESCRIPTOR, "Did not try to modify record.");
	}
	else
		testError("testChange", DBEntityRM::DESCRIPTOR, "Record did not exist.");

	delete rm;

	debug->trace("database.driver.testsCompound.change", 24, "Ending testChangeCompound");
#endif
}


template <class DBEntityRM>
void Driver::testTextSearch(edb::DBManager* db, const std::string& search)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.textSearch", 20, "Beginning testTextSearch");

	int records = 0, valid = 0;
	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::query_type* query = rm->retrieveKeywordInText(search);
	if ( query )
	{
		typename DBEntityRM::query_type::size_type totalq = query->size();
		typename DBEntityRM::query_type::size_type totalrm = rm->countAll();

		typename DBEntityRM::query_type::const_iterator it = query->begin();
		typename DBEntityRM::query_type::const_iterator end = query->end();

		cout << endl << endl << "******* LIST OF ALL " << DBEntityRM::DESCRIPTOR
				<< " RECORDS containing \"" << search << "\" *******" << endl << endl;
		while ( it != end )
		{
			if ( it->isRecordValid() )
				++valid;

			cout << *it++;

			++records;
		}

		cout << "Listed " << totalq << " records (" << valid << " valid) of " << totalrm <<
				" records reported to be stored in the table." << endl;

		if ( records != totalq )
			testError("testTextSearch", DBEntityRM::DESCRIPTOR,
					"The total number of records in the query did not match the records displayed.");

		if ( valid != records )
			testError("testTextSearch", DBEntityRM::DESCRIPTOR,
					"Not all records shown were valid.");

		delete query;
	}
	delete rm;

	debug->trace("database.driver.tests.textSearch", 24, "Ending testTextSearch");
#endif
}


template <class DBEntityRM>
void Driver::testDelete(edb::DBManager* db, const std::string& key)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.delete", 20, "Beginning testDelete");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm->find(key);

	cout << endl << endl << "******* DELETING " << DBEntityRM::DESCRIPTOR
			<< " RECORD " << key << " *******" << endl << endl;
	if ( rec )
	{
		cout << "Deleting Record " << rec->getKey() << " is: " << endl << *rec << endl;
		if ( !rm->deleteRecord(rec) )
		{
			cout << "Unable to delete Record " << rec->getKey() << ": " << endl << *rec << endl;
			testError("testDelete", DBEntityRM::DESCRIPTOR, "Did not delete record.");
		}

		delete rec;


		// Verify deleted record
		cout << "Verifying that the record was deleted" << endl;
		rec = rm->find(key);
		if ( rec )
		{
			cout << "Unable to delete Record " << rec->getKey() << ": " << endl << *rec << endl;

			testError("testDelete", DBEntityRM::DESCRIPTOR, "Did not delete record.");

			delete rec;
		}
		else
			cout << "Record deleted" << endl;
	}
	else
		testError("testDelete", DBEntityRM::DESCRIPTOR, "Record did not exist.");

	delete rm;

	debug->trace("database.driver.tests.delete", 24, "Ending testDelete");
#endif
}


template <class DBEntityRM>
void Driver::testDeleteCompound(edb::DBManager* db, const std::string& key1,
		const std::string& key2)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.testsCompound", 20, "Beginning testDeleteCompound");

	DBEntityRM* rm = db->create<DBEntityRM>();
	typename DBEntityRM::record_type* rec = rm->findKey(key1, key2);

	cout << endl << endl << "******* DELETING " << DBEntityRM::DESCRIPTOR
			<< " RECORD " << key1 << "," << key2 << " *******" << endl << endl;
	if ( rec )
	{
		cout << "Deleting Record " << rec->getKey1() << "," << rec->getKey2() << " is: " << endl << *rec << endl;
		if ( !rm->deleteRecord(rec) )
		{
			cout << "Unable to delete Record " << rec->getKey1() << "," << rec->getKey2() << ": " << endl << *rec << endl;
			testError("testDelete", DBEntityRM::DESCRIPTOR, "Did not delete record.");
		}

		delete rec;


		// Verify deleted record
		cout << "Verifying that the record was deleted" << endl;
		rec = rm->findKey(key1, key2);
		if ( rec )
		{
			cout << "Unable to delete Record " << rec->getKey1() << "," << rec->getKey2() << ": " << endl << *rec << endl;

			testError("testDelete", DBEntityRM::DESCRIPTOR, "Did not delete record.");

			delete rec;
		}
		else
			cout << "Record deleted" << endl;
	}
	else
		testError("testDelete", DBEntityRM::DESCRIPTOR, "Record did not exist.");

	delete rm;

	debug->trace("database.driver.testsCompound.delete", 24, "Ending testDeleteCompound");
#endif
}


template <class DBEntityRM>
void Driver::testDeleteAll(edb::DBManager* db)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.deleteAll", 20, "Beginning testDeleteAll");

	DBEntityRM* rm = db->create<DBEntityRM>();
	int count = rm->countAll();

	cout << endl << endl << "******* TESTING DELETING ALL " << DBEntityRM::DESCRIPTOR
			<< " RECORDS *******" << endl << endl;

	cout << "Current number of records is " << count << endl;

	cout << "Going to delete them all now ..." << endl;
	int deleted = rm->deleteAll();
	cout << "Deleted " << deleted << " records." << endl;
	if ( deleted != count )
		testError("testDeleteAll", DBEntityRM::DESCRIPTOR, "Did not delete all records.");

	int number = rm->countAll();
	cout << "Current number of records after attempting to delete them all is " << number << endl;

	if ( number )
		testError("testDeleteAll", DBEntityRM::DESCRIPTOR, "Did not delete all records.");

	delete rm;

	debug->trace("database.driver.tests.deleteAll", 24, "Ending testDeleteAll");
#endif
}


template <class DBEntityRM>
void Driver::testIndependentQueries(edb::DBManager* db, const std::string& start2, const std::string& stop2)
{
#ifndef NO_FUNCTIONTEMPLATES
	debug->trace("database.driver.tests.independentQueries", 20, "Beginning testIndependentQueries");

	DBEntityRM* rm = db->create<DBEntityRM>();
	DBEntityRM* rm2 = db->create<DBEntityRM>();

	typename DBEntityRM::query_type* query = rm->retrieveAll();
	typename DBEntityRM::query_type* query2 = rm2->retrieveRange(start2, stop2);

	typename DBEntityRM::query_type::size_type total = query->size();
	typename DBEntityRM::query_type::size_type total2 = query2->size();

	typename DBEntityRM::query_type::const_iterator it = query->begin();
	typename DBEntityRM::query_type::const_iterator end = query->end();
	typename DBEntityRM::query_type::const_iterator it2 = query2->begin();
	typename DBEntityRM::query_type::const_iterator end2 = query2->end();


	cout << endl << endl << "******* TESTING INDEPENDENT " << DBEntityRM::DESCRIPTOR
			<< " QUERIES *******" << endl << endl;

	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;


	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;


	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;


	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;


	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;


	if ( it != end )
		cout << "Query1: " << *it++;
	if ( it2 != end2 )
		cout << "Query2: " << *it2++;

	cout << "Listed " << total << " records from query1 and " << total2 << " records from query2." << endl;

	delete query;
	delete query2;

	delete rm;
	delete rm2;

	debug->trace("database.driver.tests.independentQueries", 24, "Ending testIndependentQueries");
#endif
}



#ifdef VXWORKS
int main(int argc, char* argv[]);

STATUS launchEDB(char *argument1=NULL, char *argument2=NULL)
{

	int argc = 1;
	char* argv[5];
	char* appName = "driver";

	argv[0] = appName;

	if (argument1 != NULL)
	{
		argc++;
		argv[1] = argument1;
	}

	if (argument2 != NULL)
	{
		argc++;
		argv[2] = argument2;
	}
	
	return main (argc, argv);
}

#endif

#include DIRECTORY_INC


int main(int argc, char * argv[])
{
	Driver driver(argc, argv);

	driver.testAll();
}



#endif // APPLICATION_DRIVER

