#include <string>
#include <iostream>
#include <cstdio>          // for getchar
#include <fstream>


#include "edb.h"

#include "driver2.h"

#include DBPRODUCTSRM_INC
#include DBPRODUCT_INC
#include DBQUERY_INC

/* #include LOCKMANAGER_INC (SJJC port 6/19/06) */
#include UTILSTRING_INC
#include THREAD_INC
#include PLATFORM_INC



#ifndef NO_NAMESPACES
using namespace edb;
using namespace util_platform;
using util_env::CLOptions;
using util_env::Properties;
#endif


using namespace std;



Driver::Driver(int argc, char* argv[])
		: options_('-'), properties_("defaults.ini"), debug_(0), recFunctions_(), dbFunctions_()
{
	// Deal with the command line options and parameters
	options_.processCommandLine(argc,argv);

	// Get the application properties if any
	CLOptions::option_iterator configFilename = options_.findOption("cfg");
	if ( configFilename != options_.endOption() )
		properties_.load(*configFilename);

	// Make properties available to DBManager
	DBManager::setConfiguration(&properties_);

	// Get the debugging mechanism and make it available to DBManager
	debug_ = ::Trace::getInstance(properties_);  /* (SJJC port 6/19/06) */
	DBManager::setDebug(debug_);


	// Build the map of functions
	recFunctions_.insert(make_pair(string("WriteOneItem"), &DriverThread::processWriteOneItem));
	recFunctions_.insert(make_pair(string("ReadAllItem"), &DriverThread::processReadAllItem));
	recFunctions_.insert(make_pair(string("ReadOneItem"), &DriverThread::processReadOneItem));
	dbFunctions_.insert(make_pair(string("ExecuteOneBackup"), &DriverThread::processExecuteOneBackup));
}


void Driver::testOne()
{
/*	tdata_[0].commandsFilename = "driver0.dat";
	tdata_[0].functions = functions_;

	// Here we spawn multiple threads and we start them. Each thread will execute the
	// testOneThread method with a different parameter.
	testOneThread(&tdata_[0]);
  */
	// Get the filename from the application properties if any
	string filename("driver");
	CLOptions::option_iterator dataFilename = options_.findOption("f");
	if ( dataFilename != options_.endOption() )
		filename = *dataFilename;

	DriverThread dt("driver", filename + "0" + ".std", recFunctions_, dbFunctions_);

	dt.start();

	Platform::getInstance().delay(1800000);
}


void Driver::testTwo()
{
	// Get the filename from the application properties if any
	string filename("driver");
	CLOptions::option_iterator dataFilename = options_.findOption("f");
	if ( dataFilename != options_.endOption() )
		filename = *dataFilename;

	DriverThread dt0("driver0", filename + "0" + ".std", recFunctions_, dbFunctions_);
	DriverThread dt1("driver1", filename + "1" + ".std", recFunctions_, dbFunctions_);

#ifdef VXWORKS
	dt1.setPriority(89);
#endif

	dt0.start();
	dt1.start();

	Platform::getInstance().delay(60000);

	Debug::trace("driver.driver", 24, "Ending driver execution");

}


void Driver::testError(const std::string& method, const std::string& table, const std::string& reason)
{
	cout << endl << endl << "ERROR: found in " << method << " when processing " << table << " TABLE." << endl << endl;
	if ( !reason.empty() )
		cout << "Reason: " << reason << endl << endl;
	cout << "\x07\x07\x07\x07\x07PRESS ENTER";
	getchar();
}


int Driver::DriverThread::doRun()
{
	Debug::trace("driver.processFile", 20, "Beginning thread test for command file " + commandsFilename_);

	db_ = DBManager::getInstance();
	db_->openDatabase();

	DBProductsRM* prods = db_->createProducts();


	string line;
	ifstream file(commandsFilename_.c_str());
	if ( !file )
	{
		Debug::trace("driver.processFile", 3, "Unable to open commands file " + commandsFilename_);

		return 0;
	}

	while ( getline(file, line) )
	{
		if ( line.size() )
		{
#ifdef VXWORKS
			string lineAux = line.substr(0, line.size() - 1);
#else
			string lineAux = line;
#endif
			if ( lineAux.size() )
			{
				STDRecord rec(lineAux, 1);

				recordFunctionContainer_type::iterator iter = recFunctions_.find(rec.getType());
				if ( iter != recFunctions_.end() )
					(this->*(iter->second))(prods, &rec);
				else
				{
					dbFunctionContainer_type::iterator iter = dbFunctions_.find(rec.getType());
					if ( iter != dbFunctions_.end() )
						(this->*(iter->second))(db_, &rec);
					else
						Debug::trace("driver.processFile", 3, "Unable to find function for record " +
								rec.getType());
				}

#ifdef VXWORKS
				if ( getPriority() == 89 )
					getPlatform().delay(10);
				else
					getPlatform().delay(0);
#endif
			}
		}

		Platform::getInstance().delay(Platform::getInstance().getCurrentTime().getMillisecond());
	}



	delete prods;

/*	LockManager::getInstance().debugState(); (SJJC port 6/19/06) */

//	db_->closeDatabase();          if one thread closes the db, what about the others?

	DBManager::destroyInstance(db_);
	db_ = 0;

/*	LockManager::getInstance().debugState(); (SJJC port 6/19/06) */

	Debug::trace("driver.processFile", 24, "Ending thread test");

	return 0;
}


bool Driver::DriverThread::processWriteOneItem(DBProductsRM* prods, const STDRecord* rec)
{
	bool result = true;

	Debug::trace("driverThread.writeOneItem", 20, "Started function");

	if ( prods )
	{
		const string* data = &rec->getField("PLU#__");
		Debug::trace("driverThread.writeOneItem", 21, "Trying to find record with PLU " + *data);
		DBProduct* p = prods->findPLU(*data, "0");
		if ( !p )
		{
			Debug::trace("driverThread.writeOneItem", 21, "Record not found. Attempting to create.");
			p = prods->createRecord();
			if ( p )
			{
				Debug::trace("driverThread.writeOneItem", 21, "Record created.");
				p->setPLU(*data);
			}
		}

		if ( p )
		{
			if ( !rec->isFieldNull("RptCls") )
				p->setReportingClass(rec->getField("RptCls"));

			if ( !rec->isFieldNull("ByCont") )
				p->setByCount(util_string::stringtoint(rec->getField("ByCont")));

			if ( !rec->isFieldNull("BCForc") )
				p->setForceByCount(util_string::stringtobool(rec->getField("BCForc")));

			if ( !rec->isFieldNull("Price_") )
				p->setPrice(util_string::stringtoimpint(rec->getField("Price_"), 2));

			if ( !rec->isFieldNull("PrcFac") )
				p->setPriceFactor(util_string::stringtobool(rec->getField("PrcFac")));

			if ( !rec->isFieldNull("PrcFrc") )
				p->setForcePrice(util_string::stringtobool(rec->getField("PrcFrc")));

			if ( !rec->isFieldNull("MerVal") )
				p->setDiscountPrice1(util_string::stringtoint(rec->getField("MerVal")));

			if ( !rec->isFieldNull("MerV_2") )
				p->setDiscountPrice2(util_string::stringtoint(rec->getField("MerV_2")));

			if ( !rec->isFieldNull("MerV_3") )
				p->setDiscountPrice3(util_string::stringtoint(rec->getField("MerV_3")));

			if ( !rec->isFieldNull("MerchM") )
				p->setDiscountMethod(util_string::stringtoint(rec->getField("MerchM")));

			if ( !rec->isFieldNull("ExcPrc") )
				p->setExceptionPrice(util_string::stringtoint(rec->getField("ExcPrc")));

			if ( !rec->isFieldNull("ItemDs") )
				p->setDescription(rec->getField("ItemDs"));

			if ( !rec->isFieldNull("BarCNS") )
				p->setBarCodePrefix(util_string::stringtoint(rec->getField("BarCNS")));

			if ( !rec->isFieldNull("BarCod") )
			{
				const string* data = &rec->getField("BarCod");
				if ( data->size() > 5 )
				{
					p->setBarCodeVendorNumber(util_string::stringtoint(data->substr(0, data->size() - 5)));
					p->setBarCodeProductNumber(util_string::stringtoint(data->substr(data->size() - 5, 5)));
				}
				else
				{
					p->setBarCodeVendorNumberNull();
					if ( data->size() )
						p->setBarCodeProductNumber(util_string::stringtoint(*data));
					else
						p->setBarCodeProductNumberNull();
				}
			}

			if ( !rec->isFieldNull("NetWt_") )
				p->setNetWeight(util_string::stringtoint(rec->getField("NetWt_")));

			if ( !rec->isFieldNull("ShlfLf") )
				p->setShelfLifeDays(util_string::stringtoint(rec->getField("ShlfLf")));

			if ( !rec->isFieldNull("ShLfHo") )
				p->setShelfLifeHours(util_string::stringtoint(rec->getField("ShLfHo")));

			if ( !rec->isFieldNull("ShLfDP") )
				p->setDontPrintShelfLife(util_string::stringtobool(rec->getField("ShLfDP")));

			if ( !rec->isFieldNull("PrdLif") )
				p->setProductLifeDays(util_string::stringtoint(rec->getField("PrdLif")));

			if ( !rec->isFieldNull("PrLfHo") )
				p->setProductLifeHours(util_string::stringtoint(rec->getField("PrLfHo")));

			if ( !rec->isFieldNull("PdLfDP") )
				p->setDontPrintProductLife(util_string::stringtobool(rec->getField("PdLfDP")));

			if ( !rec->isFieldNull("PdPODP") )
				p->setDontPrintPackedOn(util_string::stringtobool(rec->getField("PdPODP")));

			if ( !rec->isFieldNull("SpMsg#") )
				p->setSpecialMessage(rec->getField("SpMsg#"));

			if ( !rec->isFieldNull("ExTxt#") )
				p->setText(rec->getField("ExTxt#"));

			if ( !rec->isFieldNull("Nutr#_") )
				p->setNutrition(rec->getField("Nutr#_"));

			if ( !rec->isFieldNull("Note#_") )
				p->setNote(rec->getField("Note#_"));

			if ( !rec->isFieldNull("Grphc#") )
				p->setGraphic1(rec->getField("Grphc#"));

			if ( !rec->isFieldNull("Grph02") )
				p->setGraphic2(rec->getField("Grph02"));

			if ( !rec->isFieldNull("Grph03") )
				p->setGraphic3(rec->getField("Grph03"));

			if ( !rec->isFieldNull("Grph04") )
				p->setGraphic4(rec->getField("Grph04"));

			if ( !rec->isFieldNull("Lbl_01") )
				p->setLabel1(rec->getField("Lbl_01"));

			if ( !rec->isFieldNull("LblR01") )
				p->setLabel1Rotation(util_string::stringtoint(rec->getField("LblR01")));

			if ( !rec->isFieldNull("LblP01") )
				p->setLabel1Placement(util_string::stringtoint(rec->getField("LblP01")));

			if ( !rec->isFieldNull("Lbl_02") )
				p->setLabel2(rec->getField("Lbl_02"));

			if ( !rec->isFieldNull("LblR02") )
				p->setLabel2Rotation(util_string::stringtoint(rec->getField("LblR02")));

			if ( !rec->isFieldNull("LblP02") )
				p->setLabel2Placement(util_string::stringtoint(rec->getField("LblP02")));

			if ( !rec->isFieldNull("Lbl_03") )
				p->setLabel3(rec->getField("Lbl_03"));

			if ( !rec->isFieldNull("LblR03") )
				p->setLabel3Rotation(util_string::stringtoint(rec->getField("LblR03")));

			if ( !rec->isFieldNull("LblP03") )
				p->setLabel3Placement(util_string::stringtoint(rec->getField("LblP03")));

			if ( !rec->isFieldNull("PrTare") )
				p->setProportionalTare(util_string::stringtoint(rec->getField("PrTare")));

			if ( !rec->isFieldNull("Tare01") )
				p->setTare(util_string::stringtoint(rec->getField("Tare01")));

			if ( !rec->isFieldNull("TarF01") )
				p->setForceTare(util_string::stringtobool(rec->getField("TarF01")));

			if ( !rec->isFieldNull("BnsPts") )
				p->setBonusPoints(util_string::stringtoint(rec->getField("BnsPts")));

			if ( !rec->isFieldNull("IntkMd") )
				p->setIntakeMode(util_string::stringtoint(rec->getField("IntkMd")));

			if ( !rec->isFieldNull("AplySL") )
				p->setApplySecurityLabel(util_string::stringtobool(rec->getField("AplySL")));

			Debug::trace("driverThread.writeOneItem", 21, "Saving record.");
			prods->saveRecord(p);

			delete p;
		}
	}
	else
		result = false;

	Debug::trace("driverThread.writeOneItem", 24, "Ended function");

	return result;
}


bool Driver::DriverThread::processReadOneItem(DBProductsRM* prods, const STDRecord* rec)
{
	if ( prods )
	{
		const string* data = &rec->getField("PLU#__");
		DBProduct* p = prods->findPLU(*data, "0");
		if ( p )
		{
			cout << *p;

			delete p;
		}

		return true;
	}

	return false;
}


bool Driver::DriverThread::processReadAllItem(DBProductsRM* prods, const STDRecord* /*rec*/)
{
	if ( prods )
	{
		DBQuery<DBProduct>* query = prods->retrieveAll();
		if ( query )
		{
			DBQuery<DBProduct>::iterator iter = query->begin();

			while ( iter != query->end() )
			{
				cout << *iter;

#ifdef VXWORKS
				if ( getPriority() == 89 )
					getPlatform().delay(10);
				else
					getPlatform().delay(0);
#endif

				++ iter;
			}

			delete query;
		}

		return true;
	}

	return false;
}


bool Driver::DriverThread::processExecuteOneBackup(DBManager* db, const STDRecord* /*rec*/)
{
	bool result = true;

	Debug::trace("driverThread.executeOneBackup", 20, "Started function");

	if ( db )
	{
		Debug::trace("driverThread.executeOneBackup", 21, "Attempting the backup");

		db->backupDatabase("temp");
	}
	else
		result = false;

	Debug::trace("driverThread.executeOneBackup", 24, "Ended function");

	return result;
}



#ifdef VXWORKS
int main(int argc, char* argv[]);

STATUS launchEDB(char *argument1=NULL, char *argument2=NULL,char *argument3=NULL, char *argument4=NULL)
{

	int argc = 1;
	char* argv[5];
	char* appName = "driver2";

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

	if (argument3 != NULL)
	{
		argc++;
		argv[3] = argument3;
	}

	if (argument4 != NULL)
	{
		argc++;
		argv[4] = argument4;
	}

	return main (argc, argv);
}

#endif


#ifdef VXWORKS
int main(int argc, char * argv[])
{
	Driver driver(argc, argv);

//	driver.testOne();
	driver.testTwo();
}
#endif


