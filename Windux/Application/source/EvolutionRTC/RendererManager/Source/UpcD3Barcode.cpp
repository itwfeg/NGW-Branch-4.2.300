#include <iostream>
#include "UpcD3Barcode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int UpcD3Barcode::dataType = 0;
std::string UpcD3Barcode::dataTypeName("UpcD3Barcode"); 
#endif

UpcD3Barcode::UpcD3Barcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 /* rotation */) : GenericBarcode()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->bounds = bounds;
	barcodeBad = false;

	//initialize symbol tables
	symbolTableA = new UINT8[10];

	symbolTableA[0] = 0x0D;
	symbolTableA[1] = 0x19;
	symbolTableA[2] = 0x13;
	symbolTableA[3] = 0x3D;
	symbolTableA[4] = 0x23;
	symbolTableA[5] = 0x31;	
	symbolTableA[6] = 0x2F;
	symbolTableA[7] = 0x3B;	
	symbolTableA[3] = 0x37;	
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

 //	symbolTableEAN = 0;

	//intialize the drawing pens 
	setCharacterPen();
	setBarcodePen();

	//determine the modulesize
	if(modSize <= 0)
	{
		moduleSize = (bounds->getRectangleRight() - bounds->getRectangleLeft())	/ ModsUPCD3LO;
	}
	else
	{
		moduleSize = modSize;
	}
	
	//determine the block height
	INT16 blockHeight = (bounds->getRectangleBottom() - bounds->getRectangleTop() 
														- (ModsUPCVmarg * moduleSize)) / 2;
	
	//create new image rectangle for hi side of barcode
	hiRect = new Hobart::Rectangle(bounds->getRectangleTop(), bounds->getRectangleTop() + blockHeight, 
													bounds->getRectangleRight(), bounds->getRectangleLeft());

	//create new image rectangle for low side of barcode
	lowRect = new Hobart::Rectangle(bounds->getRectangleBottom() - blockHeight, bounds->getRectangleBottom(),
															bounds->getRectangleRight(), bounds->getRectangleLeft());

}
UpcD3Barcode::~UpcD3Barcode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	delete hiRect;
	delete lowRect;
	delete [] symbolTableA;
	delete [] symbolTableB;
	delete [] symbolTableC;
}

bool UpcD3Barcode::isBarcodeBad()
{
	return GenericBarcode::isBarcodeBad();
}

void UpcD3Barcode::createHiSide(INT16 modSize, INT16 rotation)
{
	setupHiSide(modSize, rotation);
}
void UpcD3Barcode::createLowSide(INT16 modSize, INT16 rotation)
{
	setupLowSide(modSize, rotation);
}
void UpcD3Barcode::setupHiSide(INT16 modSize, INT16 rotation)
{

UINT16 fieldWidth, marginOffset, barcodeWidth;

	/* *** This method needs to be updated when true type fonts are implemented. *** */
	/* for test we will assume the hobart 4 font ats 06202002*/
#if defined(LINUX_FIX)
	INT16 characterHeight = 9, leading = 1;		/*these hardcoded values are for test only ats 06202002 */
#else
	INT16 characterHeight = 9, leading = 1, ascent = 7;		/*these hardcoded values are for test only ats 06202002 */
#endif

	//calculate the height of the symbols for the hi side of the barcode 
	symbolHeight = hiRect->getRectangleBottom() - hiRect->getRectangleTop() - characterHeight - leading - 1;

	//calculate the height of the guards for the barcode
	guardHeight = symbolHeight + (characterHeight / 2);  

	if(modSize <= 0)
	{
		moduleSize = (hiRect->getRectangleRight() - hiRect->getRectangleLeft()) / ModsUPCD3HI;
	}
	else
	{
		moduleSize = modSize;
	}

	marginOffset = moduleSize * ModsUPCHmarg;

	if(rotation == 270)
	{
		fieldWidth = hiRect->getRectangleBottom() - hiRect->getRectangleTop();
	}
	else
	{
		fieldWidth = hiRect->getRectangleRight() - hiRect->getRectangleLeft();
	}

	barcodeWidth = marginOffset + (moduleSize * ModsUPCHmarg);

	//make sure the barcode will fit into the rendering area
	if((barcodeWidth == 0) || (barcodeWidth > fieldWidth))
	{
		barcodeBad = true;
	}
	else
	{
		//move the pen to the top left of the rendering rectangle
		barcodePen.setPenLocation((hiRect->getRectangleLeft() + marginOffset), hiRect->getRectangleTop());
	}

}
void UpcD3Barcode::setupLowSide(INT16 modSize, INT16 rotation)
{

UINT16 fieldWidth, marginOffset, barcodeWidth;

	/* *** This method needs to be updated when true type fonts are implemented. *** */
	/* for test we will assume the hobart 4 font ats 06202002*/
#if defined(LINUX_FIX)
	INT16 characterHeight = 9, leading = 1;		/*these hardcoded values are for test only ats 06202002 */
#else
	INT16 characterHeight = 9, leading = 1, ascent = 7;		/*these hardcoded values are for test only ats 06202002 */
#endif

	//calculate the height of the symbols for the low side of the barcode 
	symbolHeight = lowRect->getRectangleBottom() - lowRect->getRectangleTop() - characterHeight - leading - 1;

	//calculate the height of the guards for the barcode
	guardHeight = symbolHeight + (characterHeight / 2);  



    if (modSize <= 0)
    {
        moduleSize = (lowRect->getRectangleRight() - lowRect->getRectangleLeft()) / ModsUPCD3LO;
    }
	else
	{
    	moduleSize = modSize;
	}

    marginOffset = moduleSize * ModsUPCHmarg;

    if (rotation == 0)
	{
        fieldWidth = (lowRect->getRectangleRight() - lowRect->getRectangleLeft());
	}
    else
	{
        fieldWidth = (lowRect->getRectangleBottom() - lowRect->getRectangleTop());
	}

	barcodeWidth = (marginOffset + (moduleSize * ModsUPCHmarg));
	if((barcodeWidth == 0) || (barcodeWidth > fieldWidth))
	{
		barcodeBad = true;
	}
	else
	{
		barcodePen.setPenLocation((lowRect->getRectangleLeft() + marginOffset), lowRect->getRectangleTop());
	}		
}
