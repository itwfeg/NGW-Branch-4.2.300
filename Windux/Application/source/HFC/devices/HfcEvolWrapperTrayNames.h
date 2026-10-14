/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERTRAYNAMES_H
#define HFCEVOLWRAPPERTRAYNAMES_H

#include <string>

#include "HfcObject.h"


class EvolWrapperPackageTimings;


class HfcEvolWrapperTrayNames : public HfcObject
{
	public:

		HfcEvolWrapperTrayNames(EvolWrapperPackageTimings *timings);

		~HfcEvolWrapperTrayNames();

		void getTrayName(unsigned int index, std::string& result);

		void show(const std::string& title) const;

	private:
		std::vector<std::string> trayNames_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCEVOLWRAPPERTRAYNAMES_H
