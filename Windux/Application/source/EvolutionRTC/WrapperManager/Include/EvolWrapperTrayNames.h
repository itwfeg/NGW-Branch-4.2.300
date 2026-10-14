/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EVOLWRAPPERTRAYNAMES_H
#define EVOLWRAPPERTRAYNAMES_H

#include <string>

#include "HfcObject.h"


class EvolWrapperPackageTimings;


class EvolWrapperTrayNames : public HfcObject
{
	public:

		EvolWrapperTrayNames(EvolWrapperPackageTimings *timings);

		~EvolWrapperTrayNames();

		void getTrayName(unsigned int index, std::string& result);

		void show(const std::string& title) const;

	private:
		std::vector<std::string> trayNames_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//EVOLWRAPPERTRAYNAMES_H
