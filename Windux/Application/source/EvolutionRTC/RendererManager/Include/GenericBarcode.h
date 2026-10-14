#ifndef GENERICBARCODE_H
#define GENERICBARCODE_H
//#include "vxWorks.h"
#include "Barcode.h"
#include "Rectangle.h"
#include "Pen.h"

class GenericBarcode
{
	public:
	
 		GenericBarcode();
		virtual ~GenericBarcode();

		INT16 getModuleSize();
		void setModuleSize(INT16 size){moduleSize = size;}
		INT16 getGuardHeight();
		void setGuardHeight(INT16 height){guardHeight = height;}
		INT16 getSymbolHeight();
		void setSymbolHeight(INT16 height){ symbolHeight = height;}
		INT16 getCharOffset();
		void setCharOffset(INT16 offset){charOffset = offset;}
		INT16 getCharHeight();
		void setCharHeight(INT16 height){charHeight = height;}
		INT16 getLeading();
		void setLeading(INT16 lead){leading = lead;}
		INT16 getMaxFace();
		void setMaxFace(INT16 face){faceMax = face;}
		INT16 getAscent();	
		void setAscent(INT16 Ascent){ascent = Ascent;}

		void setPens(){setCharacterPen(); setBarcodePen();}
		bool isBarcodeBad();
		
		INT16 getBarcodePenVertical();
		INT16 getBarcodePenHorizontal();
 		INT16 getBarcodePenColor();
		void moveBarcodePen(INT16 horizontal, INT16 vertical);
		void resetPen(INT16 horizontal, INT16 vertical);

		SymbolTables getCurrentTable();
		
		//user has to set the table type before blitting the symbols
		void setCurrentTableUPCA();
		void setCurrentTableUPCB();
		void setCurrentTableUPCC();									
		void setCurrentTableEAN();

		UINT8 getSymbol(SymbolTables table, UINT8 index);

	protected:
		void setCharacterPen();
		void setBarcodePen();

	/*---------------------------------------------------*
	 *   BitMaped Values for UPC A,B,C Module Characters *
	 *   Seven Modules are used - lower 7 bits of byte   *
	 *   bit = 1 -> Draw Dark Module                     *
	 *   bit = 0 -> Draw Light Module                    *
	 *---------------------------------------------------*/

		 UINT8 *symbolTableA;
	/*     0    1    2    3    4    5    6    7    8    9 */
	 //   0x0D,0x19,0x13,0x3d,0x23,0x31,0x2f,0x3b,0x37,0x0b


		UINT8 *symbolTableB;
	/*     0    1    2    3    4    5    6    7    8    9 */
	//    0x72,0x66,0x6c,0x42,0x5c,0x4e,0x50,0x44,0x48,0x74


		UINT8 *symbolTableC;
	/*     0    1    2    3    4    5    6    7    8    9 */
	//    0x27,0x33,0x1b,0x21,0x1d,0x39,0x05,0x11,0x09,0x17

	//  	UINT8 **symbolTableEAN;

		SymbolTables currentTable;	//current symbol table in use
		Hobart::Rectangle *bounds;
	//	Font *font;
		UINT16 *data;
		INT16 moduleSize;	//width of a single module
		INT16 guardHeight;	//height of the guard bars	
		INT16 symbolHeight;	//height of the symbol bars
		INT16 charOffset;	//hoizontal offset to position characters under symbol
		INT16 charHeight;
		INT16 leading;
		INT16 faceMax;
		INT16 ascent;
		Pen charPen;		//pen used to draw the characters
		Pen barcodePen;		//Pen used to draw the barcode
		bool barcodeBad;	//is the current barcode bad or not

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//GENERICBARCODE_H
