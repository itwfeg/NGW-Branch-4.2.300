#include "NutriDataColumn.h"


#include "Debug.h"
#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NutriDataColumn::dataType = 0;
std::string NutriDataColumn::dataTypeName("NutriDataColumn"); 
#endif

using namespace std;

NutriDataColumn::NutriDataColumn(int column, const std::string& data) 
				: columnNumber_(column) 
{ 
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	parseColumnData(data); 
}

NutriDataColumn::~NutriDataColumn() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


string NutriDataColumn::getData(const std::string& tag) const
{
	tagDataContainer_type::const_iterator iter = columnData_.find(tag);

	if ( iter != columnData_.end() )
		return iter->second;

	return "";
}



void NutriDataColumn::parseColumnData(const std::string& stream)
{
	static const char separator = '\x1A';
	static const int tagLength = 4;
	int index = 0, length = (int) stream.size();

	Debug::trace("NutriDataColumn", 20, "Nutrition Data Column " + 
		util_string::inttostring(columnNumber_) + ": Parsing data \"" + stream + "\"");

	while ( index + tagLength <= length )
	{
		Debug::trace("NutriDataColumn.parsing", 20, "Nutrition Data Column " +
			util_string::inttostring(columnNumber_) + ": Parsing at location " + util_string::inttostring(index));

		std::string tag(stream, index, tagLength);
		index += tagLength;
		std::string::size_type index2 = stream.find(separator, index);

		if ( index2 != string::npos )
		{
			string data(stream, index, index2 - index);

			columnData_[tag] = data;

			Debug::trace("NutriDataColumn.parsing", 20, "Nutrition Data Column " +
				util_string::inttostring(columnNumber_) + ": Parsing found separator at " +
				util_string::inttostring(index2) + "\nAdding tag \"" + tag + "\" with data \"" + data + "\"");

			index = (int)index2 + 1;
		}
		else
		{
			string data(stream, index);

			columnData_[tag] = data;

			Debug::trace("NutriDataColumn.parsing", 20, "Nutrition Data Column " +
				util_string::inttostring(columnNumber_) + ": Parsing did not find separator\nAdding tag \""
				+ tag + "\" with data \"" + data + "\"");

			index = length;
		}
	}
}

