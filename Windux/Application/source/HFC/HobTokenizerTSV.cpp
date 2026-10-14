/* HT Scale Project
 * COPYRIGHT (c) 2026.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerTSV.h"
#include "HobString.h"

#include "Configuration.h"

#include "Integer.h"
#include "Bool.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobToken::dataType = 0;
std::string HobToken::dataTypeName("HobTokenizerTSV"); 
#endif


// Implementation of the toUpper function
std::string HobTokenizerTSV::toUpper(std::string str)
{
	// Test method seeing if other classes can call this one
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

std::string HobTokenizerTSV::getExclusionString()
{
	return Configuration::getInstance()->readProductFieldExclusions();
}

std::vector<std::string> HobTokenizerTSV::getRTsMappedToTags(std::string excludeStr, std::string RTTag)
{
	// 1. Split the main string by the space delimiter (vertical bar | ) to get individual pairs RT FUNCTION CODES and RS tags and place in vector
	std::vector<std::string> pairs = util_string::split(excludeStr, '|');
		
	// 2. Iterate through each pair, split itstrings  into key/value pairs and insert into them into a map
	std::map<std::string, std::string> strippedTags;
	for (const std::string& pair : pairs)
	{
		size_t delimiterPos = pair.find(':');						// Find the position of the colon
		if (delimiterPos != std::string::npos)
		{	
			std::string key = pair.substr(0, delimiterPos);			// Extract the key
			std::string value = pair.substr(delimiterPos + 1);		// Extract the value

			strippedTags[key] = value;								// Insert into the map
		}
	}

	// 3. GET map of results for each RT tag where the TAG=key and VALUE=tags_to_exclude(RT## Tag as key, csv str of tags to exclude for that RT Tag)	                  
	std::string tagsToDump = strippedTags[RTTag]; // = l2,l3		// RETURNS: l2,l3 
	std::vector<std::string> splittags = util_string::split(tagsToDump, ',');

	return splittags;
}

int HobTokenizerTSV::performTagDrop(std::map<std::string, int> fieldOffsetsToTags, HobString tagToDump)
{
	// fieldOffsetsToTags: maps a tag to its offset
	// tagsToDump: single tag we are trying to find in list - key to offset

	try
	{
		return fieldOffsetsToTags.at(tagToDump);
	}
	catch (const std::out_of_range& oor)
	{
		Debug::trace("HobTokenizerTSV.updateRecord", string("performTagDrop() - unknown or unmatched tag to offset: ") + tagToDump);
		return -1;
	}
}


