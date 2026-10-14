#include <iostream>
using namespace std;

#include "GenericBarcode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericBarcode::dataType = 0;
std::string GenericBarcode::dataTypeName("GenericBarcode"); 
#endif

GenericBarcode::GenericBarcode()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	currentTable = None;
}
GenericBarcode::~GenericBarcode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
INT16 GenericBarcode::getModuleSize()
{
	return moduleSize;
}
INT16 GenericBarcode::getGuardHeight()
{
	return guardHeight;
}
INT16 GenericBarcode::getSymbolHeight()
{
	return symbolHeight;
}
INT16 GenericBarcode::getCharOffset()
{
	return charOffset;
}
INT16 GenericBarcode::getCharHeight()
{
	return charHeight;
}
INT16 GenericBarcode::getLeading()
{
	return leading;	
}
INT16 GenericBarcode::getMaxFace()
{
	return faceMax;
}
INT16 GenericBarcode::getAscent()
{
	return ascent;
}
bool GenericBarcode::isBarcodeBad()
{
	return barcodeBad;
}

void GenericBarcode::setCharacterPen()
{
	charPen.setPenState(Down);
	charPen.setPenColor(Black);
}
void GenericBarcode::setBarcodePen()
{
	barcodePen.setPenState(Down);
	barcodePen.setPenColor(Black);	/* white for test only ats 06172002 */
}
INT16 GenericBarcode::getBarcodePenVertical()
{
	return barcodePen.getPenLocationVertical();
}

INT16 GenericBarcode::getBarcodePenHorizontal()
{
	return barcodePen.getPenLocationHorizontal();
}
INT16 GenericBarcode::getBarcodePenColor()
{
	return barcodePen.getSetVal();
}
void GenericBarcode::moveBarcodePen(INT16 horizontal, INT16 vertical)
{
	INT16 newHorizontal = barcodePen.getPenLocationHorizontal() + horizontal;
	INT16 newVertical = barcodePen.getPenLocationVertical() + vertical;

	barcodePen.movePenTo(newHorizontal, newVertical);
}
void GenericBarcode::resetPen(INT16 horizontal, INT16 vertical)
{
	barcodePen.setPenLocation(horizontal, vertical);
}

SymbolTables GenericBarcode::getCurrentTable()
{
	return currentTable;
}
void GenericBarcode::setCurrentTableUPCA()
{
	currentTable = UPCAcodes;
}
void GenericBarcode::setCurrentTableUPCB()
{
	currentTable = UPCBcodes;
}
void GenericBarcode::setCurrentTableUPCC()
{
	currentTable = UPCCcodes;
}
void GenericBarcode::setCurrentTableEAN()
{
	currentTable = EANcodes;
}
UINT8 GenericBarcode::getSymbol(SymbolTables table, UINT8 index)
{

UINT8 symbol = 0;
	
	currentTable = table;

	switch(table)
	{
		case UPCAcodes:
		{
			symbol = symbolTableA[index];
			break;
		}
		case UPCBcodes:
		{
			symbol = symbolTableB[index];
			break;
		}
		case UPCCcodes:
		{
			symbol = symbolTableC[index];
			break;
		}
		case EANcodes:
		{
//	   		symbol = symbolTable
			break;
		}
		default:
		{

			cout<<"GenericBarcode -- bad table type or no table type selected"<<endl;
		}	
	}
	return symbol;
}



