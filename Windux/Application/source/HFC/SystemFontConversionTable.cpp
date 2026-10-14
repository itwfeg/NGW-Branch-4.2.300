/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemFontConversionTable.h"
#include "HobString.h"
#include "Debug.h"
#include "Hex.h"


/* Font equivalencies between Ultima and Evolution fonts.
Observations:

Ultima scales use 4 different fonts at various sizes. Those fonts are:
	SP, with sizes 6, 7, 8, 10, 14, and 16
	Ultima, with sizes 16, 18, 24, and 36
	Hobart, with sizes 5, 9, 10, 12, 14, 18, 20, and 24
	London, with sizes 18, 24, and 36

- Ultima's SP font. The size matches are as follows:
Ultima Font    |   Evolution Font
SP 6           |   Sp 8
SP 7           |   Sp 10
SP 8           |   Sp 12
SP 10          |   Sp 13
SP 14          |   Sp 16
SP 16          |   SpLarge 18
SP 18          |   SpLarge 20

- Ultima's Ultima font.	The size matches are as follows:
Ultima Font    |   Evolution Font
Ultima 16      |   Ultima 7
Ultima 18      |   Ultima 8
Ultima 24      |   Ultima 10
Ultima 36      |   Ultima 15

- Ultima's Hobart font.	The size matches are as follows:
Ultima Font    |   Evolution Font
Hobart 5       |   HobartBold 3
Hobart 9       |   HobartBold 3
Hobart 10      |   HobartBold 4
Hobart 12      |   HobartBold 5
Hobart 14      |   HobartPlain 6
Hobart 18      |   HobartPlain 7
Hobart 20      |   HobartPlain 8
Hobart 24      |   HobartPlain 10

- Ultima's London font. The size matches are as follows:
Ultima Font    |   Evolution Font
London 18      |   London 11
London 24      |   London 15
London 36      |   London 22
*/

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SystemFontConversionTable::dataType = 0;
std::string SystemFontConversionTable::dataTypeName("SystemFontConversionTable"); 
#endif
																			 //"<esc>fontID"													
const std::string SystemFontConversionTable::ultimaHobart5		= std::string("\x1B""\x20");	// \x20 = space
const std::string SystemFontConversionTable::ultimaHobart9		= std::string("\x1B""\x21");	// \x21 = !
const std::string SystemFontConversionTable::ultimaHobart10		= std::string("\x1B""\x22");	// \x22 = "
const std::string SystemFontConversionTable::ultimaHobart12		= std::string("\x1B""\x23");	// \x23 = # 
const std::string SystemFontConversionTable::ultimaHobart14		= std::string("\x1B""\x24");	// \x24 = $
const std::string SystemFontConversionTable::ultimaHobart18		= std::string("\x1B""\x25");	// \x25 = %
const std::string SystemFontConversionTable::ultimaHobart20		= std::string("\x1B""\x26");	// \x26 = &
const std::string SystemFontConversionTable::ultimaHobart24		= std::string("\x1B""\x27");	// \x27 = ' 
const std::string SystemFontConversionTable::ultimaLondon18		= std::string("\x1B""\x29");	// \x29 = )
const std::string SystemFontConversionTable::ultimaLondon24		= std::string("\x1B""\x2A");	// \x2A = *
const std::string SystemFontConversionTable::ultimaLondon36		= std::string("\x1B""\x2B");	// \x2B = +
const std::string SystemFontConversionTable::ultimaSP6			= std::string("\x1B""\x2E");	// \x2E = . 
const std::string SystemFontConversionTable::ultimaSP7			= std::string("\x1B""\x2F");	// \x2F = /
const std::string SystemFontConversionTable::ultimaSP8			= std::string("\x1B""\x30");	// \x30 = 0 
const std::string SystemFontConversionTable::ultimaSP10			= std::string("\x1B""\x2C");	// \x2C = ,
const std::string SystemFontConversionTable::ultimaSP14			= std::string("\x1B""\x2D");	// \x2D = -
const std::string SystemFontConversionTable::ultimaSP16         = std::string("\x1B""\x81");	// \x81 = 
const std::string SystemFontConversionTable::ultimaSP18         = std::string("\x1B""\x82");	// \x82 = 
const std::string SystemFontConversionTable::ultimaUltima16		= std::string("\x1B""\x34");	// \x34 = 4
const std::string SystemFontConversionTable::ultimaUltima18		= std::string("\x1B""\x35");	// \x35 = 5
const std::string SystemFontConversionTable::ultimaUltima24		= std::string("\x1B""\x36");	// \x36 = 6
const std::string SystemFontConversionTable::ultimaUltima36		= std::string("\x1B""\x37");	// \x37 = 7

//escape sequences for evolution fonts						   	"<esc><esc>fn=family name;ps=pointSize<esc>"
const std::string SystemFontConversionTable::evolHobart5		= "\x1B""\x1B""fn=HobartBold;ps=3\x1B";
const std::string SystemFontConversionTable::evolHobart9		= "\x1B""\x1B""fn=HobartBold;ps=3\x1B";
const std::string SystemFontConversionTable::evolHobart10		= "\x1B""\x1B""fn=HobartBold;ps=4\x1B";
const std::string SystemFontConversionTable::evolHobart12		= "\x1B""\x1B""fn=HobartBold;ps=5\x1B";
const std::string SystemFontConversionTable::evolHobart14		= "\x1B""\x1B""fn=HobartPlain;ps=6\x1B";
const std::string SystemFontConversionTable::evolHobart18		= "\x1B""\x1B""fn=HobartPlain;ps=7\x1B";
const std::string SystemFontConversionTable::evolHobart20		= "\x1B""\x1B""fn=HobartPlain;ps=8\x1B";
const std::string SystemFontConversionTable::evolHobart24		= "\x1B""\x1B""fn=HobartPlain;ps=10\x1B";
const std::string SystemFontConversionTable::evolLondon18		= "\x1B""\x1B""fn=London;ps=11\x1B";
const std::string SystemFontConversionTable::evolLondon24		= "\x1B""\x1B""fn=London;ps=15\x1B";
const std::string SystemFontConversionTable::evolLondon36		= "\x1B""\x1B""fn=London;ps=22\x1B";
const std::string SystemFontConversionTable::evolSP6            = "\x1B""\x1B""fn=Sp;ps=8\x1B"; 
const std::string SystemFontConversionTable::evolSP7            = "\x1B""\x1B""fn=Sp;ps=10\x1B";
const std::string SystemFontConversionTable::evolSP8            = "\x1B""\x1B""fn=Sp;ps=12\x1B";
const std::string SystemFontConversionTable::evolSP10           = "\x1B""\x1B""fn=Sp;ps=13\x1B";
const std::string SystemFontConversionTable::evolSP14           = "\x1B""\x1B""fn=Sp;ps=16\x1B";
const std::string SystemFontConversionTable::evolSP16           = "\x1B""\x1B""fn=SpLarge;ps=16\x1B";
const std::string SystemFontConversionTable::evolSP18           = "\x1B""\x1B""fn=SpLarge;ps=20\x1B";
const std::string SystemFontConversionTable::evolUltima16       = "\x1B""\x1B""fn=Ultima;ps=7\x1B";
const std::string SystemFontConversionTable::evolUltima18		= "\x1B""\x1B""fn=Ultima;ps=8\x1B";
const std::string SystemFontConversionTable::evolUltima24		= "\x1B""\x1B""fn=Ultima;ps=10\x1B";
const std::string SystemFontConversionTable::evolUltima36		= "\x1B""\x1B""fn=Ultima;ps=15\x1B";

std::map<const std::string, const std::string> SystemFontConversionTable::ultimaFontMap;
std::map<const std::string, const std::string> SystemFontConversionTable::evolutionFontMap;


SystemFontConversionTable::SystemFontConversionTable()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//construct both Evolution and Ultima conversion tables
	if (ultimaFontMap.size() == 0)
	{
		//Ultima table
		ultimaFontMap.insert(make_pair(ultimaHobart5,  evolHobart5));	//table is used when Evolution equivalent font is needed 
		ultimaFontMap.insert(make_pair(ultimaHobart9,  evolHobart9));
		ultimaFontMap.insert(make_pair(ultimaHobart10, evolHobart10)); 
		ultimaFontMap.insert(make_pair(ultimaHobart12, evolHobart12));
		ultimaFontMap.insert(make_pair(ultimaHobart14, evolHobart14));
		ultimaFontMap.insert(make_pair(ultimaHobart18, evolHobart18));
		ultimaFontMap.insert(make_pair(ultimaHobart20, evolHobart20));
		ultimaFontMap.insert(make_pair(ultimaHobart24, evolHobart24));
		ultimaFontMap.insert(make_pair(ultimaLondon18, evolLondon18));
		ultimaFontMap.insert(make_pair(ultimaLondon24, evolLondon24));
		ultimaFontMap.insert(make_pair(ultimaLondon36, evolLondon36));
		ultimaFontMap.insert(make_pair(ultimaSP6,      evolSP6));
		ultimaFontMap.insert(make_pair(ultimaSP7,      evolSP7));
		ultimaFontMap.insert(make_pair(ultimaSP8,      evolSP8));
		ultimaFontMap.insert(make_pair(ultimaSP10,     evolSP10));
		ultimaFontMap.insert(make_pair(ultimaSP14,     evolSP14));
		ultimaFontMap.insert(make_pair(ultimaSP16,     evolSP16));
		ultimaFontMap.insert(make_pair(ultimaSP18,     evolSP18));
		ultimaFontMap.insert(make_pair(ultimaUltima16, evolUltima16));
		ultimaFontMap.insert(make_pair(ultimaUltima18, evolUltima18));
		ultimaFontMap.insert(make_pair(ultimaUltima24, evolUltima24));
		ultimaFontMap.insert(make_pair(ultimaUltima36, evolUltima36));
	}


	if (evolutionFontMap.size() == 0)
	{
		//Evolution table
		evolutionFontMap.insert(make_pair(evolHobart5 , ultimaHobart5));	//table is used when Ultima equivalent font is needed 
		evolutionFontMap.insert(make_pair(evolHobart9 , ultimaHobart9));
		evolutionFontMap.insert(make_pair(evolHobart10, ultimaHobart10)); 
		evolutionFontMap.insert(make_pair(evolHobart12, ultimaHobart12));
		evolutionFontMap.insert(make_pair(evolHobart14, ultimaHobart14));
		evolutionFontMap.insert(make_pair(evolHobart18, ultimaHobart18));
		evolutionFontMap.insert(make_pair(evolHobart20, ultimaHobart20));
		evolutionFontMap.insert(make_pair(evolHobart24, ultimaHobart24));
		evolutionFontMap.insert(make_pair(evolLondon18, ultimaLondon18));
		evolutionFontMap.insert(make_pair(evolLondon24, ultimaLondon24));
		evolutionFontMap.insert(make_pair(evolLondon36, ultimaLondon36));
		evolutionFontMap.insert(make_pair(evolSP6     , ultimaSP6));
		evolutionFontMap.insert(make_pair(evolSP7     , ultimaSP7));
		evolutionFontMap.insert(make_pair(evolSP8     , ultimaSP8));
		evolutionFontMap.insert(make_pair(evolSP10    , ultimaSP10));
		evolutionFontMap.insert(make_pair(evolSP14    , ultimaSP14));
		evolutionFontMap.insert(make_pair(evolSP16    , ultimaSP16));
		evolutionFontMap.insert(make_pair(evolSP18    , ultimaSP18));
		evolutionFontMap.insert(make_pair(evolUltima16, ultimaUltima16));
		evolutionFontMap.insert(make_pair(evolUltima18, ultimaUltima18));
		evolutionFontMap.insert(make_pair(evolUltima24, ultimaUltima24));
		evolutionFontMap.insert(make_pair(evolUltima36, ultimaUltima36));
	}
}

SystemFontConversionTable::~SystemFontConversionTable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const std::string SystemFontConversionTable::findUltimaMatch(const std::string evolKey)
{
	if (!evolutionFontMap.empty())
	{
		if (Debug::isTracing("SystemFontConversionTable"))
		{
			std::map<const std::string, const std::string>::iterator mi = evolutionFontMap.begin();
			while (mi != evolutionFontMap.end())
			{
				HobString efm1(Hex::toString( ((*mi).first).c_str() ));
				HobString efm2(Hex::toString( ((*mi).second).c_str() ));
				Debug::trace("SystemFontConversionTable", HobString("evolutionFontMapa: ") + efm1 + HobString("  ") + efm2);
				Debug::trace("SystemFontConversionTable", HobString("evolutionFontMapb: ") + HobString((*mi).first) + HobString("  ") + HobString((*mi).second));
				++mi;
			}
			Debug::trace("SystemFontConversionTable", HobString("Looking for: ") + HobString(Hex::toString(evolKey.c_str())));
			Debug::trace("SystemFontConversionTable", HobString("Looking for: ") + HobString(evolKey));
		}

		if (evolutionFontMap.find(evolKey) != evolutionFontMap.end())
			return evolutionFontMap[evolKey];
		else
	  		Debug::trace("SystemFontConversionTable", "no ultima match found");
	}
	else
		Debug::trace("SystemFontConversionTable", "Empty Evolution table");

	return std::string("");
}

const std::string SystemFontConversionTable::findEvolMatch(const std::string ultimaKey)
{
	if (!ultimaFontMap.empty())
	{
		if (Debug::isTracing("SystemFontConversionTable"))
		{
			std::map<const std::string , const std::string >::iterator mi = ultimaFontMap.begin();
			while (mi != ultimaFontMap.end())
			{
				HobString efm1(Hex::toString( ((*mi).first).c_str() ));
				HobString efm2(Hex::toString( ((*mi).second).c_str() ));
				Debug::trace("SystemFontConversionTable", HobString("ultimaFontMap: ") + efm1 + HobString("  ") + efm2);
				Debug::trace("SystemFontConversionTable", HobString("evolutionFontMapb: ") + HobString((*mi).first) + HobString("  ") + HobString((*mi).second));
				++mi;
			}
			Debug::trace("SystemFontConversionTable", HobString("Looking for: ") + HobString(Hex::toString(ultimaKey.c_str())));
			Debug::trace("SystemFontConversionTable", HobString("Looking for: ") + HobString(ultimaKey));
		}

		if (ultimaFontMap.find(ultimaKey) != ultimaFontMap.end())
			return ultimaFontMap[ultimaKey];
		else
	  		Debug::trace("SystemFontConversionTable", "no Evol match found");
	}
	else
		Debug::trace("SystemFontConversionTable", "Empty Ultima table");

	return std::string("");
}
