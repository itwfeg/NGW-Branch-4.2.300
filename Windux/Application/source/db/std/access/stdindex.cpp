#include "stdindex.h"

// MSVC2010
#include <iostream>
#include <iterator>

using namespace std;


STDIndex::STDIndex(const std::string& fname, const std::string& k) :
		initialized_(false), filename(fname), key(k)
{
	readFile();

//	cout << "Constructed STDIndex (standard)" << endl;
}


STDIndex::~STDIndex()
{
//	cout << "Destructed STDIndex" << endl;
}


bool STDIndex::initialized()
{
	return initialized_;
}


STDIndex::position_type STDIndex::find(const string& key) const
{
	index_set::const_iterator it = indices.find(key);

	if ( it != indices.end() )
		return (*it).getPosition();
	else
		return INDEX_NOTFOUND;
}


STDIndex::position_type STDIndex::first() const
{
	index_set::const_iterator it = indices.begin();

	if ( it != indices.end() )
		return (*it).getPosition();
	else
		return INDEX_NOTFOUND;
}


STDIndex::position_type STDIndex::findNext(const string& key) const
{
	index_set::const_iterator it = indices.find(key);

	if ( it != indices.end() )
	{
		if ( ++it != indices.end() )
			return (*it).getPosition();
		else
			return INDEX_NOTFOUND;
	}
	else
		return INDEX_NOTFOUND;
}


STDIndex::position_type STDIndex::findPrevious(const string& key) const
{
	index_set::const_iterator it = indices.find(key);

	if ( it != indices.end() && it != indices.begin() )
		return (*--it).getPosition();
	else
		return INDEX_NOTFOUND;
}


STDIndex::position_type STDIndex::last() const
{
	index_set::const_iterator it = indices.end();

	if ( !indices.empty() )
		return (*--it).getPosition();
	else
		return INDEX_NOTFOUND;
}


int STDIndex::deleteIndex(const std::string& key)
{
	int count = (int)indices.erase(key);
	writeFile();

	return count;
}


int STDIndex::deleteAll()
{
	int count = (int)indices.size();

	indices.clear();
	writeFile();

	return count;
}


void STDIndex::addIndex(const string& key, position_type position, bool save)
{
	index_set::iterator it = indices.find(key);

	// If the index already existed remove it, since on sets it is illegal to
	// update elements (it could affect their automatic ordering)
	if ( indices.find(key) != indices.end() )
		indices.erase(key);

	indices.insert(Element(key, position));

	if ( save )
		writeFile();

	return;
}


void STDIndex::save()
{
	writeFile();
}


unsigned long int STDIndex::count()
{
	return (unsigned long)indices.size();
}


void STDIndex::readFile()
{
	ifstream file(filename.c_str());

	if ( file )
	{
		Element e;

		while ( file >> e )
			indices.insert(e);

		initialized_ = true;
	}

	return;
}


ostream& operator<<(ostream& os, const STDIndex::Element& e)
{
	return os << e.indexValue << " " << e.position;
}


void STDIndex::writeFile()
{
	ofstream file(filename.c_str());

	copy(indices.begin(), indices.end(), ostream_iterator<Element>(file, "\n"));

	return;
}



STDIndex::Element::Element(const std::string& key, position_type pos) :
		indexValue(key), position(pos)
{
//	cout << "Constructed STDIndex::Element (standard/default)" << endl;
}


STDIndex::Element::~Element()
{
//	cout << "Destructed STDIndex::Element" << endl;
}


STDIndex::position_type STDIndex::Element::getPosition() const
{
	return position;
}


STDIndex::position_type STDIndex::Element::setPosition(position_type pos)
{
	position_type old = position;

	position = pos;

	return old;
}


bool STDIndex::Element::compareElement(const STDIndex::Element& e1, const STDIndex::Element& e2)
{
	return e1.indexValue < e2.indexValue;
}


istream& operator>>(istream& is, STDIndex::Element& e)
{
	unsigned long int pos;

	is >> e.indexValue >> pos;
	e.position = pos;

	return is;
}


const STDIndex::position_type STDIndex::INDEX_NOTFOUND ((STDIndex::position_type)-1);




#ifdef STDINDEX_DRIVER


#include <iostream>


int main(int argc, char * argv[])
{
	STDIndex file("item.PLU#__.index", "PLU#__");

	file.saveRecord(r1);
	file.saveRecord(r2);
	file.saveRecord(r3);
	file.saveRecord(r4);

}


#endif // STDINDEX_DRIVER

