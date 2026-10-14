//#include "vxWorks.h"
#if defined( __linux__ )
    #include <string.h>
#endif

#include "VacuumFlourescentVirtualImage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int VacuumFlourescentVirtualImage::dataType = 0;
std::string VacuumFlourescentVirtualImage::dataTypeName("VacuumFlourescentVirtualImage"); 
#endif

VacuumFlourescentVirtualImage::VacuumFlourescentVirtualImage(): NumericVirtualImage(NumericVacuumFlourescentDisplay)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	memset (tareField_, 0, MAX_FIELD_SIZE);
	memset (tareField_, ' ', TARE_FIELD_WIDTH);
	memset (weightField_, 0, MAX_FIELD_SIZE);
	memset (weightField_, ' ', WEIGHT_FIELD_WIDTH);
	memset (unitPriceField_, 0, MAX_FIELD_SIZE);
	memset (unitPriceField_, ' ', UNIT_PRICE_FIELD_WIDTH);
	memset (totalPriceField_, 0, MAX_FIELD_SIZE);
	memset (totalPriceField_, ' ', TOTAL_PRICE_FIELD_WIDTH);
}

VacuumFlourescentVirtualImage::~VacuumFlourescentVirtualImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void VacuumFlourescentVirtualImage::setTare(char* tare)
{
	rightJustify(tare, tareField_, TARE_FIELD_WIDTH);
}

void VacuumFlourescentVirtualImage::setWeight(char* weight)
{
	rightJustify(weight, weightField_, WEIGHT_FIELD_WIDTH);
}

void VacuumFlourescentVirtualImage::setUnitPrice(char * price)
{
	rightJustify(price, &unitPriceField_[1], UNIT_PRICE_FIELD_WIDTH - 1);
}

void VacuumFlourescentVirtualImage::setTotalPrice(char * price)
{
	rightJustify(price, totalPriceField_, TOTAL_PRICE_FIELD_WIDTH);
}

void VacuumFlourescentVirtualImage::setPoundsPerPrice(char * lbs)
{
	writePPPField(lbs);
}

char* VacuumFlourescentVirtualImage::getJustifiedTare()
{
	return tareField_;
}

char* VacuumFlourescentVirtualImage::getJustifiedWeight()
{
	return weightField_;
}

char* VacuumFlourescentVirtualImage::getJustifiedUnitPrice()
{
	return unitPriceField_;
}

char* VacuumFlourescentVirtualImage::getJustifiedTotalPrice()
{
	return totalPriceField_;
}


char* VacuumFlourescentVirtualImage::setField(NmFieldType fieldType, char * newText)
{
	

	switch (fieldType)
	{
		case NmTare:
		{
			rightJustify(newText, tareField_, TARE_FIELD_WIDTH);
			return tareField_;
		}
		case NmWeight:
		{
			rightJustify(newText, weightField_, WEIGHT_FIELD_WIDTH);
			return weightField_;
		}
		case NmUnitPrice:
		{
			//both NmUnitPrice and NmPoundsPerPrice go into the variable unitPriceField_
			rightJustify(newText, &unitPriceField_[1], UNIT_PRICE_FIELD_WIDTH - 1);
			return unitPriceField_;
		}
		case NmTotalPrice:
		{
			rightJustify(newText, totalPriceField_, TOTAL_PRICE_FIELD_WIDTH);
			return totalPriceField_;
		}
		case NmPoundsPerPrice:
		{
			writePPPField(newText);

			return unitPriceField_;
		}
		default:
			return 0;
	}

}

bool VacuumFlourescentVirtualImage::rightJustify(char *text, char * justified, int sizeField)
{
	int sizeText = (int) strlen(text);
	int padSize = 0;
	int i = 0, j = 0;
	int actualWidth = 0;
	bool validInput = getActualWidth(text, sizeText, actualWidth);

	if (!validInput)
	{
		//invalid input data so just blank it out
		for (i = 0; i < sizeField; i++)
			justified[i] = ' ';
		justified[i] = 0;

		return false;
	}

	if (actualWidth < sizeField)
		padSize = sizeField - actualWidth;

	for (i = 0; i < padSize; i++)
		justified[i] = ' ';

	if (actualWidth > sizeField)
		actualWidth = sizeField;

	
	for (j = 0; (actualWidth > 0); i++, j++)
	{
		justified[i] = text[j];
		if ( (text[j] != ',') && (text[j] != '.'))
			actualWidth --;
	}

	justified[i] = 0;

	return true;

}

/*text length minus the number of '.' and ','  since .'s and ,'s dont take space in field.
  they are on different parts of the display. However a '-' will take field space.
  
  also, only numeric values or '.''s or ',''s or '-''s ' ''s are allowed.*/

bool VacuumFlourescentVirtualImage::getActualWidth(char *text, int sizeText, int& actualWidth)
{

	actualWidth = 0;
 
	if (sizeText == 0)
	{
		actualWidth = 0;
		return false;
	}

	for (int i = 0; i < sizeText; i++)
	{
		if ( (text[i] == '-') || (text[i] == ' ') || ((text[i] >= '0') && (text[i] <= '9')) )
			actualWidth++;
		else
			if ( (text[i] != ',') && (text[i] != '.'))
			{
				actualWidth	= 0;
				return false;
			}
	}

	return true;
}


void VacuumFlourescentVirtualImage::writePPPField(char* lbs)
{
	//this field is one character wide and goed into variable unitPriceField_
 	if ( (lbs[0] >= '0') && (lbs[0] <= '9') )
		unitPriceField_[0] = lbs[0];
	else
		unitPriceField_[0] = ' ';				
}
