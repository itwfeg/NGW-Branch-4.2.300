#include "stdfile.h"



#include <string>
#include <fstream>

using namespace std;



STDFile::STDFile(const std::string& fname, const std::string& pkey) :
		filename(fname), primaryKey(pkey),
		primaryIndex(createIndexFilename(fname, pkey), pkey)
{
	// If no index file was present, create a new index file
	if ( !primaryIndex.initialized() )
		createIndexFile();

//	cout << "Constructed STDFile (standard)" << endl;
}


STDFile::~STDFile()
{
//	cout << "Destructed STDFile" << endl;
}


STDRecord* STDFile::findRecord(const string& key, const string& searchField)
{
	// Find the index corresponding to this record.
	if ( searchField.length() == 0 || searchField == primaryKey )
		return fetchRecord(primaryIndex.find(key));
	else
		return 0;
}


STDRecord* STDFile::first()
{
	return fetchRecord(primaryIndex.first());
}


STDRecord* STDFile::next(const STDRecord* current, const string& searchField)
{
	if ( searchField.length() == 0 || searchField == primaryKey )
		return fetchRecord(primaryIndex.findNext(current->getField(primaryKey)));
	else
		return 0;
}


STDRecord* STDFile::previous(const STDRecord* current, const string& searchField)
{
	if ( searchField.length() == 0 || searchField == primaryKey )
		return fetchRecord(primaryIndex.findPrevious(current->getField(primaryKey)));
	else
		return 0;
}


STDRecord* STDFile::last()
{
	return fetchRecord(primaryIndex.last());
}


void STDFile::saveRecord(const STDRecord* rec)
{
	string key = rec->getField(primaryKey);
#ifdef VXWORKS
	ofstream file(filename.c_str() , ios::ate);
#else
	ofstream file(filename.c_str() , ios_base::ate);
#endif
	STDIndex::position_type position = file.tellp();

	// Save the new record in the file
	file << *rec << endl;

	// Create a new index for this record
	primaryIndex.addIndex(key, position);
}


int STDFile::deleteRecord(const STDRecord* rec)
{
	return primaryIndex.deleteIndex(rec->getField(primaryKey));
}


int STDFile::deleteAll()
{
	return primaryIndex.deleteAll();
}


void STDFile::indexFile()
{
}


void STDFile::compressFile()
{
}


unsigned long int STDFile::count()
{
	return primaryIndex.count();
}


string STDFile::createIndexFilename(const string& fname, const string& key)
{
	string::size_type index = fname.rfind('.');
	string newname = fname.substr(0, index + 1);

	newname.append(key);
	newname.append(".index");

	return newname;
}


void STDFile::createIndexFile()
{
	string str;
	ifstream f(filename.c_str());
	STDIndex::position_type position = f.tellg();

	while ( getline(f, str) )
	{
		STDRecord rec = STDRecord(str, 0);   // '0' is to differenciate constructors
		primaryIndex.addIndex(rec.getField(primaryKey), position, false);

		position = f.tellg();
	}

	primaryIndex.save();
}


STDRecord* STDFile::fetchRecord(STDIndex::position_type p)
{
	if ( p != STDIndex::INDEX_NOTFOUND )
	{
		ifstream f(filename.c_str());
		f.seekg(p);

		string str;
		getline(f, str);

		return new STDRecord(str, 0);   // '0' is to differenciate constructors
	}
	else
		return 0;
}




#ifdef STDFILE_DRIVER


#include <iostream>


int main(int argc, char * argv[])
{
	unsigned long int tot;
	STDFile file("item.std", "PLU#__");

	tot = file.count();
	if ( tot > 0 )
		file.deleteAll();

	{
		STDRecord r1("OpTypeWriteOneItem\x1FPLU#__1001\x1FItemDsTest record 1001\x1FPrice_1.99", 0);
		STDRecord r2("OpTypeWriteOneItem\x1FPLU#__1002\x1FItemDsTest record 1002\x1FPrice_2.99", 0);
		STDRecord r3("OpTypeWriteOneItem\x1FPLU#__1003\x1FItemDsTest record 1003\x1FPrice_3.99", 0);
		STDRecord r4("OpTypeWriteOneItem\x1FPLU#__1004\x1FItemDsTest record 1004\x1FPrice_4.99", 0);

		file.saveRecord(r1);
		file.saveRecord(r2);
		file.saveRecord(r3);
		file.saveRecord(r4);
	}

	// Retrieve record 1002 and change its price
	STDRecord r5 = *file.findRecord("1002");

	cout << r5 << endl;

	r5.setField("Price_", "2.09");

	cout << r5 << endl;

	file.saveRecord(r5);

	// Verify that the changed record was changed
	STDFile::iterator it = file.findRecord("1002");

	cout << *it << endl;

	// Delete record 1003 and try to find it
	STDRecord r6 = *file.findRecord("1003");

	file.deleteRecord(r6);

	// Verify that deleted record does not exist

	STDRecord r7 = *file.findRecord("1003");

	cout << r7 << endl;

}


#endif // STDFILE_DRIVER

