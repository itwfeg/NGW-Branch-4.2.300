#include <iostream>
#include "Ean128CBarcode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Ean128CBarcode::dataType = 0;
std::string Ean128CBarcode::dataTypeName("Ean128CBarcode"); 
#endif

Ean128CBarcode::Ean128CBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent) : GenericBarcode()
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

	EAN128CModCodes = new UINT16[108];

	EAN128CModCodes[0]=0x6cc; EAN128CModCodes[1]=0x66c; EAN128CModCodes[2]=0x666; EAN128CModCodes[3]=0x498; EAN128CModCodes[4]=0x48c;
	EAN128CModCodes[5]=0x44c; EAN128CModCodes[6]=0x4c8; EAN128CModCodes[7]=0x4c4; EAN128CModCodes[8]=0x464; EAN128CModCodes[9]=0x648; 
	EAN128CModCodes[10]=0x644; EAN128CModCodes[11]=0x624; EAN128CModCodes[12]=0x59c; EAN128CModCodes[13]=0x4dc; EAN128CModCodes[14]=0x4ce; 
	EAN128CModCodes[15]=0x5cc; EAN128CModCodes[16]=0x4ec; EAN128CModCodes[17]=0x4e6; EAN128CModCodes[18]=0x672; EAN128CModCodes[19]=0x65c; 
	EAN128CModCodes[20]=0x64e; EAN128CModCodes[21]=0x6e4; EAN128CModCodes[22]=0x674; EAN128CModCodes[23]=0x76e; EAN128CModCodes[24]=0x74c; 
	EAN128CModCodes[25]=0x72c; EAN128CModCodes[26]=0x726; EAN128CModCodes[27]=0x764; EAN128CModCodes[28]=0x734; EAN128CModCodes[29]=0x732; 
	EAN128CModCodes[30]=0x6d8; EAN128CModCodes[31]=0x6c6; EAN128CModCodes[32]=0x636; EAN128CModCodes[33]=0x518; EAN128CModCodes[34]=0x458; 
	EAN128CModCodes[35]=0x446; EAN128CModCodes[36]=0x588; EAN128CModCodes[37]=0x468; EAN128CModCodes[38]=0x462; EAN128CModCodes[39]=0x688; 
	EAN128CModCodes[40]=0x628; EAN128CModCodes[41]=0x622; EAN128CModCodes[42]=0x5b8; EAN128CModCodes[43]=0x58e; EAN128CModCodes[44]=0x46e; 
	EAN128CModCodes[45]=0x5d8; EAN128CModCodes[46]=0x5c6; EAN128CModCodes[47]=0x476; EAN128CModCodes[48]=0x776; EAN128CModCodes[49]=0x68e; 
	EAN128CModCodes[50]=0x62e; EAN128CModCodes[51]=0x6e8; EAN128CModCodes[52]=0x6e2; EAN128CModCodes[53]=0x6ee; EAN128CModCodes[54]=0x758; 
	EAN128CModCodes[55]=0x746; EAN128CModCodes[56]=0x716; EAN128CModCodes[57]=0x768; EAN128CModCodes[58]=0x762; EAN128CModCodes[59]=0x71a; 
	EAN128CModCodes[60]=0x77a; EAN128CModCodes[61]=0x642; EAN128CModCodes[62]=0x78a; EAN128CModCodes[63]=0x530; EAN128CModCodes[64]=0x50c; 
	EAN128CModCodes[65]=0x4b0; EAN128CModCodes[66]=0x486; EAN128CModCodes[67]=0x42c; EAN128CModCodes[68]=0x426; EAN128CModCodes[69]=0x590; 
	EAN128CModCodes[70]=0x584; EAN128CModCodes[71]=0x4d0; EAN128CModCodes[72]=0x4c2; EAN128CModCodes[73]=0x434; EAN128CModCodes[74]=0x432; 
	EAN128CModCodes[75]=0x612; EAN128CModCodes[76]=0x650; EAN128CModCodes[77]=0x7ba; EAN128CModCodes[78]=0x614; EAN128CModCodes[79]=0x47a; 
	EAN128CModCodes[80]=0x53c; EAN128CModCodes[81]=0x4bc; EAN128CModCodes[82]=0x49e; EAN128CModCodes[83]=0x5e4; EAN128CModCodes[84]=0x4f4; 
	EAN128CModCodes[85]=0x4f2; EAN128CModCodes[86]=0x7a4; EAN128CModCodes[87]=0x794; EAN128CModCodes[88]=0x792; EAN128CModCodes[89]=0x6de; 
	EAN128CModCodes[90]=0x6f6; EAN128CModCodes[91]=0x7b6; EAN128CModCodes[92]=0x578; EAN128CModCodes[93]=0x51e; EAN128CModCodes[94]=0x45e; 
	EAN128CModCodes[95]=0x5e8; EAN128CModCodes[96]=0x5e2; EAN128CModCodes[97]=0x7a8; EAN128CModCodes[98]=0x7a2; EAN128CModCodes[99]=0x5de; 
	EAN128CModCodes[100]=0x5ee; EAN128CModCodes[101]=0x75e; EAN128CModCodes[102]=0x7ae; EAN128CModCodes[103]=0x684; EAN128CModCodes[104]=0x690; 
	EAN128CModCodes[105]=0x69c; EAN128CModCodes[106]=0x63a; EAN128CModCodes[107]=0x600;


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
			modSize = (bounds->getRectangleRight() - bounds->getRectangleLeft()) / ModsEAN128C;
		}
		else if(rotation == 270)
		{
			modSize = (bounds->getRectangleBottom() - bounds->getRectangleTop()) / ModsEAN128C;
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

	barcodeWidth = marginOffset + (moduleSize * ModsEAN128C);
	// If the barcode does not fit in the form because of the margin used, reduce the
	// size of the module (moduleSize) in one pixel and recalculate (so it will now fit)
	while ( barcodeWidth > fieldWidth && moduleSize > 1 )
	{
		--moduleSize;
	   	marginOffset = moduleSize * LeftMargModsUPC;
		barcodeWidth = moduleSize * ModsEAN128C;
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
		else	// 270
		{
			barcodePen.setPenLocation(bounds->getRectangleLeft(), (bounds->getRectangleBottom() - marginOffset));
		}	 
	}
}

Ean128CBarcode::~Ean128CBarcode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

UINT16 Ean128CBarcode::getValueFromEANTable(INT16 index)
{
	return EAN128CModCodes[index];
}


