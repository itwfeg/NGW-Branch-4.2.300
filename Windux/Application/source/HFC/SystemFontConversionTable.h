/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SYSTEMFONTCONVERSIONTABLE_H
#define SYSTEMFONTCONVERSIONTABLE_H

#include <string>
#include <map>


/*This class is used to convert Evolution font change sequence to Ultima and 
Ultima to Evolution. This table should use only those fonts located in the system font directory
and will need to be changed if any font family is removed from the system*/ 

class SystemFontConversionTable
{
	public:
		SystemFontConversionTable();
		~SystemFontConversionTable();
		
		const std::string findUltimaMatch(const std::string evolKey);
		const std::string findEvolMatch(const std::string ultimaKey);

		const std::string defaultEvolutionFont() { return evolHobart9; }


		//escape sequences for ultima fonts
		static const std::string ultimaHobart5;
		static const std::string ultimaHobart9;
		static const std::string ultimaHobart10;
		static const std::string ultimaHobart12;
		static const std::string ultimaHobart14;
		static const std::string ultimaHobart18;
		static const std::string ultimaHobart20;
		static const std::string ultimaHobart24;
		static const std::string ultimaLondon18;
		static const std::string ultimaLondon24;
		static const std::string ultimaLondon36; 
		static const std::string ultimaSP6;
		static const std::string ultimaSP7;
		static const std::string ultimaSP8;
		static const std::string ultimaSP10;
		static const std::string ultimaSP14;
		static const std::string ultimaSP16;
		static const std::string ultimaSP18;
		static const std::string ultimaUltima16;
		static const std::string ultimaUltima18;
		static const std::string ultimaUltima24;
		static const std::string ultimaUltima36;

		//escape sequences for evolution fonts
		static const std::string evolHobart5;
		static const std::string evolHobart9;
		static const std::string evolHobart10;
		static const std::string evolHobart12;
		static const std::string evolHobart14;
		static const std::string evolHobart18;
		static const std::string evolHobart20;
		static const std::string evolHobart24;
		static const std::string evolLondon18;
		static const std::string evolLondon24;
		static const std::string evolLondon36;
		static const std::string evolSP6; 
		static const std::string evolSP7;
		static const std::string evolSP8;
		static const std::string evolSP10;
		static const std::string evolSP14;
		static const std::string evolSP16;
		static const std::string evolSP18;
		static const std::string evolUltima16;
		static const std::string evolUltima18;
		static const std::string evolUltima24;
		static const std::string evolUltima36;


	private:
		static std::map<const std::string, const std::string> ultimaFontMap;
		static std::map<const std::string, const std::string> evolutionFontMap;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//SYSTEMFONTCONVERSIONTABLE_H
