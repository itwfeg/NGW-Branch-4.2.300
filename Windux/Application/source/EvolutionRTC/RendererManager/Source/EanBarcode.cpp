#include <iostream>
#include "EanBarcode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EanBarcode::dataType = 0;
std::string EanBarcode::dataTypeName("EanBarcode"); 
#endif

EanBarcode::EanBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent) : GenericBarcode()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->bounds = bounds;
	this->charHeight = charHeight;
	this->leading = leading;
	this->faceMax = faceMax;
	this->ascent = ascent;	

	barcodeBad = false;

	symbolTableA = new UINT8[10];

	symbolTableA[0] = 0x0D;
	symbolTableA[1] = 0x19;
	symbolTableA[2] = 0x13;
	symbolTableA[3] = 0x3D;
	symbolTableA[4] = 0x23;
	symbolTableA[5] = 0x31;	
	symbolTableA[6] = 0x2F;
	symbolTableA[7] = 0x3B;	
	symbolTableA[8] = 0x37;	
	symbolTableA[9] = 0x0B;	

	symbolTableB = new UINT8[10];

	symbolTableB[0] = 0x72;
	symbolTableB[1] = 0x66;
	symbolTableB[2] = 0x6C;
	symbolTableB[3] = 0x42;
	symbolTableB[4] = 0x5C;
	symbolTableB[5] = 0x4E;
	symbolTableB[6] = 0x50;
	symbolTableB[7] = 0x44;
	symbolTableB[8] = 0x48;
	symbolTableB[9] = 0x74;

	symbolTableC = new UINT8[10];

	symbolTableC[0] = 0x27;
	symbolTableC[1] = 0x33;
	symbolTableC[2] = 0x1b;
	symbolTableC[3] = 0x21;
	symbolTableC[4] = 0x1d;
	symbolTableC[5] = 0x39;
	symbolTableC[6] = 0x05;
	symbolTableC[7] = 0x11;
	symbolTableC[8] = 0x09;
	symbolTableC[9] = 0x17;

	setCharacterPen();
	setBarcodePen();
	/* *** This method needs to be updated when true type fonts are implemented. *** */


	UINT16 fieldWidth = 0;
	UINT16 marginOffset = 0;
	UINT16 barcodeWidth = 0;

	//calculate the symbol height based on the rotation of the barcode
	switch (rotation)
	{
		case 0:
		{
			symbolHeight = bounds->getRectangleBottom() - bounds->getRectangleTop() - charHeight - leading - 1;
			break;
		}
		case 270:
		{
			symbolHeight = bounds->getRectangleRight() - bounds->getRectangleLeft() - charHeight - leading - 1; 	
			break;
		}
		default:
		{
			//cout << "symbolHeight: " << symbolHeight << endl;
		}
	}

	/* font implementation needed to complete these statements*/
	guardHeight = symbolHeight + (charHeight / 2);
	charOffset = (((7 * modSize) - faceMax) / 2);

	if (modSize == 0)
	{
		if(rotation == 0)
		{
//			modSize = (bounds->getRectangleRight() - bounds->getRectangleLeft()) / ModsEAN;
			modSize = (bounds->getRectangleRight() - bounds->getRectangleLeft()) / ModsUPCA;
		}
		else if(rotation == 270)
		{
//			modSize = (bounds->getRectangleBottom() - bounds->getRectangleTop()) / ModsEAN;
			modSize = (bounds->getRectangleBottom() - bounds->getRectangleTop()) / ModsUPCA;
		}
	}

	moduleSize = modSize;

	/* could not find EAN_USE_MARGIN defined in the Ultima code so....ats 06172002*/
//	marginOffset = 0;
   	marginOffset = moduleSize * LeftMargModsUPC;

	//calc field width
	if(rotation == 270)
	{
		fieldWidth = (bounds->getRectangleBottom() - bounds->getRectangleTop());
	}
	else
	{
		fieldWidth = (bounds->getRectangleRight() - bounds->getRectangleLeft());
	}	

//	barcodeWidth = marginOffset + (moduleSize *  ModsEAN);
	barcodeWidth = moduleSize * ( LeftMargModsUPC + ModsUPCA + RightMargModsUPC );
	// If the barcode does not fit in the form because of the margin used, reduce the
	// size of the module (moduleSize) in one pixel and recalculate (so it will now fit)
	while ( barcodeWidth > fieldWidth && moduleSize > 1 )
	{
		--moduleSize;
	   	marginOffset = moduleSize * LeftMargModsUPC;
		barcodeWidth = moduleSize * ( LeftMargModsUPC + ModsUPCA + RightMargModsUPC );
	}

	if((barcodeWidth == 0) || (barcodeWidth > fieldWidth))
	{
		barcodeBad = true;
		// cout<<"Barcode -- barcode width is greater than rendering area"<<endl; 
	}
	else
	{
		if(rotation == 0)
	  	{
			barcodePen.setPenLocation((bounds->getRectangleLeft() + marginOffset), bounds->getRectangleTop());
		}
		else	//270
		{
			barcodePen.setPenLocation(bounds->getRectangleLeft(), (bounds->getRectangleBottom() - marginOffset));
		}	 
	}
}

EanBarcode::~EanBarcode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	delete [] symbolTableA;
	delete [] symbolTableB;
	delete [] symbolTableC;
}

bool EanBarcode::getValueFromEANTable(INT16 row, INT16 column)
{

 UINT8 symbolTableEAN[10][6] = {{true, true, true, true, true, true,},
                         {true, true, false, true, false, false},
                         {true, true, false, false, true, false},
                         {true, true, false, false, false, true},
                         {true, false, true, true, false, false},
                         {true, false, false, true, true, false},
                         {true, false, false, false, true, true},
                         {true, false, true, false, true, false},
                         {true, false, true, false, false, true},
                         {true, false, false, true, false, true}
                         };

	if (symbolTableEAN[row][column])
		return true;
	else 
		return false;
}
