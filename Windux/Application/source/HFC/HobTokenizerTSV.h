/* HT Scale Project
 * COPYRIGHT (c) 2026.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKENIZERTSV_H
#define HOBTOKENIZERTSV_H

#include "Product.h"
#include "evolution.h"
#include "HfcObject.h"
#include "HobString.h"
#include "ProductDevEvent.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <map>
#include <list>
#include <vector>

 /**
  * @brief A collection of static helper utilities.
  */

class HobTokenizerTSV
{
	public:

		// TEST access
		static std::string toUpper(std::string str);
		
		static std::string getExclusionString();
		static std::vector<std::string> getRTsMappedToTags(std::string excludeStr, std::string rtTag);
		static int performTagDrop(std::map< std::string, int> fieldOffsetsToTags, HobString tagsToDump);

	private:
	

};

#endif //HOBTOKENIZERTSV_H
