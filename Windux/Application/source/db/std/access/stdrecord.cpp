#include "stdrecord.h"



#include <ostream>


using namespace std;


STDRecord::STDRecord(const std::string& t) : type(t), fields()
{
//	cout << "Constructed STDRecord (basic)" << endl;
}


STDRecord::STDRecord(const std::string& line, int) : type(), fields()
{
	parseRecord(line);

//	cout << "Constructed STDRecord (standard)" << endl;
}


STDRecord::STDRecord(const STDRecord& rec) : type(rec.type), fields(rec.fields)
{
//	cout << "Constructed STDRecord (copy)" << endl;
}


STDRecord::~STDRecord()
{
//	cout << "Destructed STDRecord" << endl;
}


const string& STDRecord::getType() const
{
	return type;
}


const string& STDRecord::getField(const string& tag) const
{
	fields_type::const_iterator it = fields.find(tag);
	if ( it != fields.end() )
		return it->second;
	else
		return FIELD_NOT_FOUND;
}


string& STDRecord::setField(const string& tag, const string& value)
{
	fields_type::iterator it = fields.find(tag);

	if ( it != fields.end() )
		return it->second = value;
	else
		return fields[tag] = value;
}


bool STDRecord::isFieldNull(const string& tag) const
{
	fields_type::const_iterator it = fields.find(tag);

	return (it != fields.end()) ? false : true;
}


ostream& operator<<(ostream& os, const STDRecord& rec)
{
	os << rec.TYPE_TAG << rec.type;

	STDRecord::fields_type::const_iterator it = rec.fields.begin();
	STDRecord::fields_type::const_iterator end = rec.fields.end();

	while ( it != end )
	{
		os << rec.FIELD_SEPARATOR << it->first << it->second;

		it ++;
	}

	return os;
}

/*
ostream & operator<<(ostream& os, const STDRecord::iterator & it)
{
	return os << it->first << it->second;
}
*/

void STDRecord::parseRecord(const std::string& str)
{
	string::size_type beginIndex = 0, endIndex;

	// invariant: beginIndex points at the next field to parse
	while ( beginIndex < str.size() )
	{
		endIndex = str.find(FIELD_SEPARATOR, beginIndex);
		if ( endIndex == string::npos )
			endIndex = str.size();

		if ( beginIndex < endIndex )
			parseField(str, beginIndex, endIndex);

		beginIndex = endIndex + FIELD_SEPARATOR_SIZE;
	}
}


void STDRecord::parseField(const string& str, string::size_type begin,
		string::size_type end)
{
	string::size_type dataSize = end - (begin + TAG_SIZE);

	if ( end >= begin + TAG_SIZE )
	{
		string tag = str.substr(begin, TAG_SIZE);
		string data = str.substr(begin + TAG_SIZE, dataSize);

		if ( tag == TYPE_TAG )
			type = data;
		else
			fields.insert(make_pair(tag, data));
	}
}


const string STDRecord::TYPE_TAG = "OpType";
const string STDRecord::FIELD_NOT_FOUND = "*!* Field Not Found *!*";




#ifdef STDRECORD_DRIVER


#include <iostream>


int main(int argc, char * argv[])
{
	STDRecord record("OpTypeWriteOneItem\x1FPLU#__1000\x1FItmDscTest record\x1FPrice_1.99");
	STDRecord r2(record);

	cout << "OpTypeWriteOneItem\x1FPLU#__1000\x1FItmDscTest record\x1FPrice_1.99" << endl;

	cout << record << endl;
	cout << r2 << endl;

	cout << "Field PLU#__: " << r2.getField("PLU#__") << endl;
	cout << "Field PL#___: " << r2.getField("PL#___") << endl;

	r2.setField("PLU#__", "2000");
	cout << r2;
}


#endif // STDRECORD_DRIVER


