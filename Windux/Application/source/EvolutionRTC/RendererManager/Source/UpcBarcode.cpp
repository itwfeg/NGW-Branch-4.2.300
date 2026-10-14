#include <iostream>
#include "UpcBarcode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int UpcBarcode::dataType = 0;
std::string UpcBarcode::dataTypeName("UpcBarcode"); 
#endif

UpcBarcode::UpcBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent) : GenericBarcode()
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

	//initialize the symbol tables 
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

	symbolTableC = 0;
 //	symbolTableEAN = 0;

	UINT16 fieldWidth, marginOffset, barcodeWidth;

	setCharacterPen();
	setBarcodePen();
	/* *** This method needs to be updated when true type fonts are implemented. *** */
	/* for test we will assume the hobart 4 font ats 06202002*/
//	INT16 characterHeight = 9, leading = 1, ascent = 7;		/*these hardcoded values are for test only ats 06202002 */

	//calculate the symbol height based on the rotation of the barcode
	switch(rotation)
	{

		case 0:
		{
			symbolHeight = bounds->getRectangleBottom() - bounds->getRectangleTop() - charHeight - leading - 1;	//subtract off the height of the font
			break;
		}
		case 270:
		{
			symbolHeight = bounds->getRectangleRight() - bounds->getRectangleLeft() - charHeight - leading - 1;	//subtract off the height of the font
			break;
		}
	}
	/* font implementation needed to complete these statements*/
	guardHeight = symbolHeight + (charHeight / 2); /*fudge factor for now */ 
//		guardHeight = symbolHeight + ( //add the font character height / 2);
		charOffset = (((7 * modSize) - faceMax) / 2);
	  
	//calc field width
   	if(rotation == 270)
	{
	   	fieldWidth = (bounds->getRectangleBottom() - bounds->getRectangleTop());
	}
	else
	{
		fieldWidth = (bounds->getRectangleRight() - bounds->getRectangleLeft());
	}	

	if(modSize == 0)
	{
		if(rotation == 0)
		{
			modSize = fieldWidth / ModsUPCA;
		}
		else if(rotation == 270)
		{
			modSize = fieldWidth / ModsUPCA;
		}
	}		
	moduleSize = modSize;

   	marginOffset = moduleSize * LeftMargModsUPC;

	//calculate the over all barcode width    
	barcodeWidth = moduleSize * ( LeftMargModsUPC + ModsUPCA + RightMargModsUPC );
	// If the barcode does not fit in the form because of the margin used, reduce the
	// size of the module (moduleSize) in one pixel and recalculate (so it will now fit)
	while ( barcodeWidth > fieldWidth && moduleSize > 1 )
	{
		--moduleSize;
	   	marginOffset = moduleSize * LeftMargModsUPC;
		barcodeWidth = moduleSize * ( LeftMargModsUPC + ModsUPCA + RightMargModsUPC );
	}
	
	//will the barcode fit within the rectangle boundry 
	if((barcodeWidth == 0) || (barcodeWidth > fieldWidth))
	{
		barcodeBad = true;
		//cout<<"Barcode -- barcode width is greater than rendering area"<<endl;
	}
	else
	{
		if(rotation == 0)
		{

			//move the pen to the top left of the rendering rectangle
			barcodePen.setPenLocation((bounds->getRectangleLeft() + marginOffset), bounds->getRectangleTop());
		}
		else	//rotation of 270
		{
			//move the pen to the top left of the rendering rectangle
			barcodePen.setPenLocation(bounds->getRectangleLeft(), (bounds->getRectangleBottom() - marginOffset));
		}
							 
 	}
}

UpcBarcode::~UpcBarcode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	delete [] symbolTableA;
	delete [] symbolTableB;

}
