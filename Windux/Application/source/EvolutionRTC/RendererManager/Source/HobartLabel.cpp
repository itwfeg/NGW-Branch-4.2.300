#include <iostream>
#include "stdio.h"


#include "HobartLabel.h"
#include "FormRecord.h"
#include "SystemFontConversionTable.h"
#include "Barcode.h"                    // For LeftMargModsUPC, ModsUPCA, and RightMargModsUPC

#include "Debug.h"
#include "Hex.h"
#include "utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobartLabel::dataType = 0;
std::string HobartLabel::dataTypeName("HobartLabel"); 
#endif

using namespace std;
const double HobartLabel::_ultimaConversionFactor_ = 1.3208L; // .0065 * 25.4 * 8

//	PTR 565: This does not work for Hilite type labels that only contain an embedded graphic.
//#define EVOLUTION_MIN_LABEL_SIZE 138
		
HobartLabel::HobartLabel(Blob *rawData, bool exitIfOldFormat)
		: header_(), lsd_(), numberOfForms_(0), formsList_(), iter_(), formatChanged_(false), binaryData_()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	iter_ = formsList_.begin();
	binaryValid_ = createLabelFromBlob(rawData, exitIfOldFormat);
}


HobartLabel::HobartLabel(const ResourceHeader& header, const LabelStockDescriptor& lsd,
		const FormsListType& formsList)
		: header_(header), lsd_(lsd), numberOfForms_((INT32)formsList.size()), formsList_(formsList),
		iter_(), formatChanged_(false), binaryData_()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	iter_ = formsList_.begin();
	binaryValid_ = true;
}


HobartLabel::~HobartLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	while (formsList_.empty() != true)
	{
		FormRecord *pForm = formsList_.front();
		formsList_.pop_front();

		delete pForm;
	}
}


LabelStockDescriptor *HobartLabel::getLabelStockDescriptor()
{
	return &lsd_;
}


INT32 HobartLabel::getNumberOfForms()
{
	return numberOfForms_;
}


HobartLabel::FormsListType* HobartLabel::getFormsList()
{
	return &formsList_;
}



void HobartLabel::startAtListTop()
{
	iter_ = formsList_.begin();
}


FormRecord *HobartLabel::getFormFromList()
{
	if(iter_ != formsList_.end())
		return *iter_++;
	else
	{
//		cout<<"Over pop of list"<<endl;
		iter_ = formsList_.begin();	//return the pointer back to the top of the list
		return 0;
	}
}



/*
One of the main aspects in the conversion from a 6 dot per millimeter coordinate system
to an 8 dot per millimeter one is to understand that in each sq. mm. one system has a 6x6
grid while in the same sq. mm. the other system has an 8x8 grid (see figures below). We
need to find a conversion algorithm that converts a given pixel from the first grid (in
figure 1 they are numbered between 0 and 35) to a pixel from the other grid (in figure 2
they have assigned the number of their counterpart pixel from the other coordinate system)
so that the conversion is always consistent and it conserves, at least, horizontal and
vertical lines (diagonal lines are less important in our case because we don't usually
have any in our labels).



 -----------------------------------------------
|       |       |       |       |       |       |
|   0   |   1   |   2   |   3   |   4   |   5   |
|       |       |       |       |       |       |
|-----------------------------------------------|
|       |       |       |       |       |       |
|   6   |   7   |   8   |   9   |  10   |  11   |
|       |       |       |       |       |       |
|-----------------------------------------------|
|       |       |       |       |       |       |
|  12   |  13   |  14   |  15   |  16   |  17   |
|       |       |       |       |       |       |
|-----------------------------------------------|
|       |       |       |       |       |       |
|  18   |  19   |  20   |  21   |  22   |  23   |
|       |       |       |       |       |       |
|-----------------------------------------------|
|       |       |       |       |       |       |
|  24   |  25   |  26   |  27   |  28   |  29   |
|       |       |       |       |       |       |
|-----------------------------------------------|
|       |       |       |       |       |       |
|  30   |  31   |  32   |  33   |  34   |  35   |
|       |       |       |       |       |       |
 -----------------------------------------------
Figure 1: 6x6 grid over a 1 sq. mm. of label.



 -----------------------------------------------
|  0  |  1  |     |  2  |  3  |     |  4  |  5  |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
|  6  |  7  |     |  8  |  9  |     | 10  | 11  |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
|     |     |     |     |     |     |     |     |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
| 12  | 13  |     | 14  | 15  |     | 16  | 17  |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
| 18  | 19  |     | 20  | 21  |     | 22  | 23  |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
|     |     |     |     |     |     |     |     |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
| 24  | 25  |     | 26  | 27  |     | 28  | 29  |
|     |     |     |     |     |     |     |     |
|-----------------------------------------------|
| 30  | 31  |     | 32  | 33  |     | 34  | 35  |
|     |     |     |     |     |     |     |     |
 -----------------------------------------------
Figure 2: 8x8 grid over same sq. mm. of label. The numbered pixels correspond to the
same numbered pixels in figure 1.



The formulas to convert a point of coordinates (x, y) from the 6x6 dot per millimeter
coordinate system to its corresponding (x', y') point in the 8x8 dot per millimeter system
are:

x' = (x/6)*8 + x%6 + (x%6)/2
and
y' = (y/6)*8 + y%6 + (y%6)/2

where all operations use integer arithmetic.

When converting a segment (point + length) only the point is converted using those
formulas, while the length needs different formulas. Let's assume that f() is the
function that performs the coordinate conversion defined above:

f(x) = (x/6)*8 + x%6 + (x%6)/2, where x and the arithmetic used are both integers

then if l is the length of a segment that starts in coordinate x, the new length l'
must be such that the new coordinate of the last point in the converted segment be
consistent with the old coordinate of the last point in the original segment through
the conversion function (f), which works only for coordinates:

f(x) + l' - 1 = f(x + l - 1)

or

l' = f(x + l - 1) - f(x) + 1
*/

bool HobartLabel::createLabelFromBlob(Blob *rawData, bool exitIfOldFormat)
{
	INT32 byteCounter = 0, formPosition = 0, tagSize = -1;
	INT32 savePlace0, savePlace;
	SystemFontConversionTable* fontConversion = 0;

	// Initially we will assume that this is an old format label. If this variable is
	// set to true it means that the label has coordinates for a 6 dot-per-milimeter
	// printer and the label forms use font specified as a number (instead of family
	// name and point size).
	bool oldFormat = true;

	Debug::trace("LabelType.binary", 20, "Starting to process label binary.");

	//extract the header_ from the Blob
	if ( rawData->getInt8(2) == 'L' )
	{
		byteCounter = header_.setMajorVersion(rawData, byteCounter);
		byteCounter = header_.setMinorVersion(rawData, byteCounter);
		// Verify version of this label is not newer than versions we can handle
		if ( !header_.verifyVersion() )
			return false;

		oldFormat = false;

		Debug::trace("LabelType.binary", 20, "Label format version 1.0.");

//	PTR 565: This does not work for Hilite type labels that only contain an embedded graphic.
//		//Bare minimum size of Label should be greater than 138 (by record definition) -> if not this record is crap, so toss it
//		//Header -> 44 bytes, Label Stock Descriptor -> 8 bytes, Forms List -> 16 bytes, Form Definitions -> 70 bytes for total of 138
//		if (rawData->getSize() < EVOLUTION_MIN_LABEL_SIZE)
//			return false;
	}
	else
	{
		// Only if the format of the label is version 1.0 (case of the if block) we
		// have the format current. In any other case (this else block) we will want
		// to upgrade it to version 1.0, which is the meaning of formatChanged_ flag. 
		formatChanged_ = true;

		Debug::trace("LabelType.binary", 20, "Old Label format.");
	}
	byteCounter = header_.setResourceType(rawData, byteCounter);
	byteCounter = header_.setResourceId(rawData, byteCounter);
	byteCounter = header_.setResourceLoaded(rawData, byteCounter);
	byteCounter = header_.setResourceLength(rawData, byteCounter);
	//	PTR 565:	Verify the resource length against the rawData size. They should be the same.
	//				In the case of PTR 375 the rawData size was less than the resource length
	if (header_.getResourceLength() != rawData->getSize())
		return false;

	byteCounter = header_.setResourceName(rawData, byteCounter);
	if ( !header_.verifyTypeLabel() )
		return false;
	header_.show();

	savePlace0 = byteCounter;
	byteCounter = setLabelStockDesriptor(rawData, byteCounter, oldFormat);
	lsd_.show();

	savePlace = byteCounter;
	while(rawData->getUint32(byteCounter))
	{
		byteCounter += sizeof(UINT32);
		numberOfForms_++;
	}
	byteCounter = savePlace;	//restore the counter which should be pointing to the form *'s
	Debug::trace("LabelType.binary", 20, "Label has " + util_string::inttostring(numberOfForms_) + " label forms.");


	// In the following two blocks (the for block and the if block) we perform some testing of
	// the label form length in order to help determine if the current label is an old format
	// label or a new format one. This code will not be necessary in production because then
	// all labels will be either old (Ultima and Quantum) or new (Evolution), without any
	// intermediate format
	for(int i = 0; i < numberOfForms_; i++)
	{
		UINT32 tempAddr = rawData->getUint32(byteCounter);
		UINT32 formAddr = rawData->ULongSwapBytes(tempAddr);
		formsList_.push_back((FormRecord *) ((long long)formAddr));		//get the pointers to the form data
		byteCounter += sizeof(UINT32);
		if ( formPosition && (formAddr - formPosition) > 26 )
			oldFormat = false;

		formPosition = formAddr;
	}
	if ( numberOfForms_ == 1 )
	{
		// In case there is only one label form we still don't know whether this is an old
		// format or a new one. Therefore the following test.
		iter_ = formsList_.begin();
		// This test may fail if the single label form has "data" which makes the label
		// be larger than the mere label form.
		if ( rawData->getSize() - (long long)(*iter_) > 63 )
			oldFormat = false;
	}

	if ( oldFormat )
	{
		if ( exitIfOldFormat )
		{
			Debug::trace("LabelType.binary", "createLabelFromBlob():  oldFormat && exitIfOldFormat are both true. Exiting!");

			// SOF-3583
			// delete raw forms otherwise we will attempt to delete something that doesnt exist
			formsList_.clear();

			return false;
		}
		else
		{
			// If this is the old format, create SystemFontConversionTable in order to convert
			// old font information into new font information.
			Debug::trace("LabelType.binary", "createLabelFromBlob():  oldFormat but not exitIfOldFormat. Carry on.");

			fontConversion = new SystemFontConversionTable();
		}
	}

	// Reset the label stock descriptor in order to perform the correct coordinate
	// conversion
	setLabelStockDesriptor(rawData, savePlace0, oldFormat);

	iter_ = formsList_.begin();
	for(int i = 0; i < numberOfForms_; i++)
	{
		bool graphicReference = false;
		bool usesMod2UPCABarcode = false;
		INT16 data, originX, lengthX, originY, lengthY, graphicNumber = 0;
		INT16 newOriginX = 0, newLengthX = 0, newOriginY = 0, newLengthY = 0;
		FormRecord* form  = new FormRecord;
		byteCounter =(UINT32) ((long long)*iter_);

		// Origin.h
		originX = rawData->ShortSwapBytes(rawData->getInt16(byteCounter));
		byteCounter += sizeof(originX);

		// Origin.v
		originY = rawData->ShortSwapBytes(rawData->getInt16(byteCounter));
		byteCounter += sizeof(originY);

		// Size.h
		lengthX = rawData->ShortSwapBytes(rawData->getInt16(byteCounter));
		byteCounter += sizeof(lengthX);

		if ( oldFormat )
		{
			newLengthX = convertSegment(originX, lengthX, &newOriginX);
			form->setOriginHorizontal(newOriginX);     //update the form origin
			form->setSizeHorizontal(newLengthX);       //update the form size
		}
		else
		{
			form->setOriginHorizontal(originX);     //update the form origin
			form->setSizeHorizontal(lengthX);       //update the form size
		}

		// Size.v
		lengthY = rawData->ShortSwapBytes(rawData->getInt16(byteCounter));
		byteCounter += sizeof(lengthY);
		if ( oldFormat )
		{
			newLengthY = convertSegment(originY, lengthY, &newOriginY);
			form->setOriginVertical(newOriginY);       //update the form origin
			form->setSizeVertical(newLengthY);         //update the form size
		}
		else
		{
			form->setOriginVertical(originY);       //update the form origin
			form->setSizeVertical(lengthY);         //update the form size
		}

		// Flag
		data = rawData->getUint16(byteCounter);
		data = rawData->UShortSwapBytes(data);
		form->setFlag(data);					//update the form flag
		byteCounter += sizeof(data);

		// Form Type
		UINT8 type = rawData->getUint8(byteCounter);
		form->setFieldType((FieldType) type);
		byteCounter += sizeof(type);

		// Horizontal Justification
		data = rawData->getInt8(byteCounter);
		form->setHorizontalJustify((HorizontalJustify) data);
		byteCounter += 1; // This is the size of the enum in the structure

		// Vertical Justification/Label Form Number
		data = rawData->ShortSwapBytes(rawData->getInt16(byteCounter));
		if ( oldFormat )
		{
			if ( form->getFieldType() == INDIRECT_GRAPHIC_REFERENCE )
			{
				// If it is a graphic reference, the graphic number needs to be converted
				// to a graphic name (same value but character based) and stored in the
				// Font Family Name
				graphicReference = true;
				graphicNumber = data;
				data = 0;
			}
			else if ( form->getFieldType() != INDIRECT_LABEL_REFERENCE )
				data = convertSegment(originY, data, 0);
		}
		form->setVerticalJustify(data);
		byteCounter += sizeof(data);

		// (Sub-)Type + padding
		type = rawData->getUint8(byteCounter);
		if ( oldFormat &&
				(form->getFieldType() == USER_LINE || form->getFieldType() == DIVIDING_LINE) )
		{
			if ( lengthX == 0 )
				type = (UINT8)convertSegment(originX, type, 0);
			else if ( lengthY == 0 )
				type = (UINT8)convertSegment(originY, type, 0);
		}
		form->setFieldSubType(type);
		byteCounter += sizeof(type) + 1;  // the 1 is because of padding

		// Rotation
		data = rawData->getUint16(byteCounter);
		data = rawData->UShortSwapBytes(data);
		form->setFieldRotation(data);
		byteCounter += sizeof(data);

		if ( oldFormat && form->getFieldType() == BARCODE )
		{
			// Minimum UPC A size in order to have a modSize = 2
			int minUPCAMod2Size = (LeftMargModsUPC + ModsUPCA + RightMargModsUPC) * 2;
			int minUPCAMod3Size = (LeftMargModsUPC + ModsUPCA + RightMargModsUPC) * 3;

			// Do not expand the size of the barcode form if from old format
			form->setSizeHorizontal(lengthX);        // update the form size
			form->setSizeVertical(lengthY);          // update the form size

/*			// If the barcode form is rotated, shift the unexpanded form
			// within the expanded rectangle
			switch ( form->getFieldRotation() )
*/			// If the label is rotated, shift the unexpanded form
			// within the expanded rectangle
			switch ( lsd_.getLabelOrientationDegrees() )
			{
				case 90:
				{
					// Move it to the lower left corner
					form->setOriginVertical(newOriginY + (newLengthY - lengthY));

					if ( form->getFieldSubType() == 0 ) // UPC A
					{
						// Ensure that the barcode form is long enough to allow for modSize = 2
						if ( lengthY < minUPCAMod2Size )
						{
							form->setSizeVertical((INT16)minUPCAMod2Size);
							form->setOriginVertical((INT16)(newOriginY + (newLengthY - minUPCAMod2Size)));
						}

						// If using Mod2 UPCA barcode mark it to later set font to Hobart 8
						if ( lengthY < minUPCAMod3Size )
							usesMod2UPCABarcode = true;
					}

					break;
				}
				case 180:
				{
					// Move it to the lower right corner
					form->setOriginHorizontal(newOriginX + (newLengthX - lengthX));
					form->setOriginVertical(newOriginY + (newLengthY - lengthY));

					if ( form->getFieldSubType() == 0 ) // UPC A
					{
						// Ensure that the barcode form is wide enough to allow for modSize = 2
						if ( lengthX < minUPCAMod2Size )
						{
							form->setSizeHorizontal((INT16)minUPCAMod2Size);
							form->setOriginHorizontal((INT16)(newOriginX + (newLengthX - minUPCAMod2Size)));
						}

						// If using Mod2 UPCA barcode mark it to later set font to Hobart 8
						if ( lengthX < minUPCAMod3Size )
							usesMod2UPCABarcode = true;
					}

					break;
				}
				case 270:
				{
					// Move it to the upper right corner
					form->setOriginHorizontal(newOriginX + (newLengthX - lengthX));

					if ( form->getFieldSubType() == 0 ) // UPC A
					{
						// Ensure that the barcode form is long enough to allow for modSize = 2
						if ( lengthY < minUPCAMod2Size )
							form->setSizeVertical((INT16)minUPCAMod2Size);

						// If using Mod2 UPCA barcode mark it to later set font to Hobart 8
						if ( lengthY < minUPCAMod3Size )
							usesMod2UPCABarcode = true;
					}

					break;
				}
				default:
				case 0:
				{
					// Leave it in the upper left corner


					if ( form->getFieldSubType() == 0 ) // UPC A
					{
						// Ensure that the barcode form is wide enough to allow for modSize = 2
						if ( lengthX < minUPCAMod2Size )
							form->setSizeHorizontal((INT16)minUPCAMod2Size);

						// If using Mod2 UPCA barcode mark it to later set font to Hobart 8
						if ( lengthX < minUPCAMod3Size )
							usesMod2UPCABarcode = true;
					}

					break;
				}
			}
		}

		if ( !oldFormat )
		{
			// Font Family Name
			UINT8 familyName[MaxFamilyName];
			rawData->getBytes(byteCounter, familyName, MaxFamilyName);
			form->setFamilyName(familyName);
			byteCounter += MaxFamilyName;

			// Font Point Size
			data = rawData->getUint16(byteCounter);
			data = rawData->UShortSwapBytes(data);
			form->setPointSize(data);
			byteCounter += sizeof(data);
		}
		else
		{
			if ( graphicReference )
			{
				char familyName[MaxFamilyName];

				memset(familyName, '\0', MaxFamilyName);
				sprintf(familyName, "%d", graphicNumber);
				form->setFamilyName((UINT8 *)familyName);
				form->setPointSize(0);

				// Skip fontUsage and padding, as well as font ID
				byteCounter += 4;
			}
			else
			{
				UINT16 pointSize;
				char familyName[MaxFamilyName];
				std::string evolFontSequenceStr;

				if ( form->getFieldType() == BARCODE && usesMod2UPCABarcode )
				{
					// On a barcode with modSize = 2 we use font Hobart 8
//					evolFontSequenceStr = fontConversion->evolHobart12;
					evolFontSequenceStr = "\x1B""\x1B""fn=Hobart;ps=8\x1B";

					// Skip fontUsage and padding, as well as font ID
					byteCounter += 4;
				}
				else
				{
 					// In order to convert an old Ultima font into a new Evolution font we need to
					// create an Ultima font sequence (ESCAPE character followed by a character that
					// represents the font number) and call the corresponding method in the
					// SystemFontConversionTable class. What we get back is an Evolution font sequence
					// that looks like
					//    [ESCAPE][ESCAPE]fn='font family name';ps='font point size'[ESCAPE]
					char ultimaFontSequence[3] = "\x1B ";

					// Font Usage + padding
					byteCounter += 2; // Skip the fontUsage field (0 = fixed, 1 = proportional)

					// Font ID
					data = rawData->UShortSwapBytes(rawData->getUint16(byteCounter));
					byteCounter += sizeof(data);
					ultimaFontSequence[1] = (char)data;

					evolFontSequenceStr = fontConversion->findEvolMatch(string(ultimaFontSequence));
					if ( evolFontSequenceStr.size() == 0 )
						evolFontSequenceStr = fontConversion->defaultEvolutionFont();
				}

				const char *evolutionFontSequence = evolFontSequenceStr.c_str();
				const char* ps = strchr(evolutionFontSequence + 5, ';');
				int size = (int)(ps - evolutionFontSequence - 5);
				strncpy(familyName, evolutionFontSequence + 5, size);
				memset(familyName + size, '\0', MaxFamilyName - size);
				pointSize = (INT16)atoi(ps + 4);

				form->setFamilyName((UINT8 *)familyName);
				form->setPointSize(pointSize);
			}
		}

		//erase the  pointer to the form data and replace with new form *
		// Note: we perform this operation here so that we can find out
		// whether this label has already the tag field in it or not
		iter_ = formsList_.erase(iter_);
		formsList_.push_back(form);

		// Read the tag if this is the new format
		{
			char tag[MaxTagChars];
			memset(tag, '\0', MaxTagChars);

			if ( !oldFormat )
			{
				if ( i + 1 < numberOfForms_ )
				{  // If the current form is not the last one
					if ( tagSize == -1 )
						tagSize = (INT32)(((long long)*iter_) - byteCounter - 4); 
				}
				else
				{  // If the current form is the last one
					if ( tagSize == -1 )
						tagSize = MaxTagChars;                   // Assume version 1.0 format
				}

				if ( tagSize > 0 )
				{
					rawData->getBytes(byteCounter, (UINT8 *)tag, tagSize);
					form->setTag((UINT8 *)tag);
					byteCounter += tagSize;
				}
				else
				{
					form->setTag((UINT8 *)tag);
				}
			}
			else
			{
				form->setTag((UINT8 *)tag);
			}
		}

		// Data
		UINT32 address = rawData->getUint32(byteCounter);
		address = rawData->ULongSwapBytes(address);

		if (address != 0)
		{
			form->setConstantData(rawData->getBlobBuffer() + address);
		}

//		form->show();
	}

	// reorder forms by location (render top to bottom)
	std::multimap<unsigned int, FormRecord*> formMap;

	for ( auto formptr : formsList_ )
		formMap.insert( std::make_pair( zorder( formptr ), formptr ) );

	formsList_.clear();

	for ( auto formiter : formMap )
	{
		Debug::trace( "LabelType.zorder", "zorder: " +
				Hex::toString( (void*) formiter.second ) +
				" x " + util_string::inttostring(formiter.second->getOriginHorizontal()) +
				" y " + util_string::inttostring(formiter.second->getOriginVertical()) );

		formsList_.push_back( formiter.second );
	}

	iter_ = formsList_.begin();

	if ( oldFormat && fontConversion )
		delete fontConversion;

	return true;
}

unsigned int HobartLabel::zorder( FormRecord *form ) const
{
	FormsListType forms;
	forms.push_back( form );

	Hobart::Rectangle rect( form->getRect() );

	return zorder( forms, rect );
}

unsigned int HobartLabel::zorder( FormsListType& forms, Hobart::Rectangle& boundingRect ) const
{
	const bool lineEntity(( 1 == forms.size() ) && ( forms.front()->isFormFlagLineEntity() ));

	// check overlap of other forms
	for ( auto formptr : formsList_ )
	{
		const bool found( forms.end() != std::find( forms.begin(), forms.end(), formptr ) );

		if ( found )
			continue;

		// ignore lines overlapping lines
		if (( lineEntity ) && ( formptr->isFormFlagLineEntity() ))
			continue;

		// check overlap
		const Hobart::Rectangle rect( formptr->getRect() );

		if ( boundingRect.doesOverlap( rect ) )
		{
#if defined(PAL_WIN32_RUN)
			boundingRect.setRectangleTop(min(boundingRect.getRectangleTop(), rect.getRectangleTop()));
			boundingRect.setRectangleBottom(max(boundingRect.getRectangleBottom(), rect.getRectangleBottom()));
			boundingRect.setRectangleLeft(min(boundingRect.getRectangleLeft(), rect.getRectangleLeft()));
			boundingRect.setRectangleRight(max(boundingRect.getRectangleRight(), rect.getRectangleRight()));
#else
			boundingRect.setRectangleTop( std::min( boundingRect.getRectangleTop(), rect.getRectangleTop() ) );
			boundingRect.setRectangleBottom( std::max( boundingRect.getRectangleBottom(), rect.getRectangleBottom() ) );
			boundingRect.setRectangleLeft( std::min( boundingRect.getRectangleLeft(), rect.getRectangleLeft() ) );
			boundingRect.setRectangleRight( std::max( boundingRect.getRectangleRight(), rect.getRectangleRight() ) );
#endif

			forms.push_back( formptr );

			return zorder( forms, boundingRect );
		}
	}

	return (boundingRect.getRectangleTop() << 16) + boundingRect.getRectangleLeft();
}

INT32 HobartLabel::setLabelStockDesriptor(Blob *blob, INT32 counter, bool oldFormat)
{
	INT16 data = blob->ShortSwapBytes(blob->getInt16(counter));
	if ( oldFormat )
	// Since data (= horizontal size) is a length, not a point, needs to be converted
	// to new format using convertSegment with 0 as the reference point since that is
	// the reference for the original length.
		data = convertSegment(0, data, 0);

	// We don't need to update 'widthDots' since setWidth will update it below
	lsd_.setOriginalWidthDots(data);

	// PJR-444 - SOF-5063 - Print wider labels on Zebra printer
	if (data > UFWLabelWidth)
		lsd_.setWidth(ExternalLabel);

	else if (data > WideLabelWidth)
		lsd_.setWidth(UFWLabel);
	else if ( data > NarrowLabelWidth )
		lsd_.setWidth(WideLabel);
	else
		lsd_.setWidth(NarrowLabel);
	counter += sizeof(INT16);

	data = blob->ShortSwapBytes(blob->getInt16(counter));
	if ( oldFormat )
	// Since data (= vertical size) is a length, not a point, needs to be converted to
	// new format using convertSegment with 0 as the reference point since that is that
	// is the reference for the original length.
		data = convertSegment(0, data, 0);
	lsd_.setLength(data);
	counter += sizeof(INT16);

	data = blob->getInt16(counter);
	data = blob->ShortSwapBytes(data);
	if(data == 0 || data == 180)
	{
		lsd_.setLabelOrientation(PortraitLabel);
	}
	else
	{
		lsd_.setLabelOrientation(LandscapeLabel);
	}
	lsd_.setLabelOrientationDegrees(data);
	counter += sizeof(INT16);

	data = blob->ShortSwapBytes(blob->getInt16(counter));
	if ( oldFormat )
	// Since data (= skew) is a length, not a point, needs to be converted to new
	// format using convertSegment with 0 as the reference point since that is that
	// is the reference for the original length.
		data = convertSegment(0, data, 0);
	lsd_.setSkew(data);
	return counter += sizeof(INT16);
}


const Blob& HobartLabel::getBinaryData()
{
	// Don't perform these computations if they have already been performed.
	if ( !binaryData_.getSize() )
	{
		// Determine the size of the binary and ensure enough room in it
		UINT32 /*position =*/ size = ResourceHeader::getSize() + LabelStockDescriptor::getSize() +
				(numberOfForms_ + 1) * sizeof(UINT32);
		UINT32 position = size;
		for ( FormsIteratorType iter_ = formsList_.begin(); iter_ != formsList_.end(); ++iter_ )
			size += FormRecord::getFormRecordSize() + (*iter_)->getConstantDataSize();
		binaryData_.ensureSize(size);
		// Also set the ResourceLength in the header_
		header_.setResourceLength(size);


		// Set the header_
		header_.writeBinaryData(binaryData_);


		// Set the label stock descriptor
		lsd_.writeBinaryData(binaryData_);


		// Set the form pointers
		// 'position' points to the beginning of each form in the binary
		for ( int i = 0; i < numberOfForms_; ++i, position += FormRecord::getFormRecordSize() )
		{
			UINT32 temp = binaryData_.ULongSwapBytes(position);
			binaryData_.append(temp);
		}

		binaryData_.append((UINT32)0);


		// Set the forms
		// 'position' points to the first byte after the last form (where constant text goes)
		for ( FormsIteratorType iter_ = formsList_.begin(); iter_ != formsList_.end(); ++iter_ )
		{
			(*iter_)->writeBinaryData(binaryData_, position);
			// If there is no constant data, then increment is 0, otherwise the increment
			// covers the text including the NULL character
			position += (*iter_)->getConstantDataSize();
		}


		// Set the forms' constant text
		for ( FormsIteratorType iter_ = formsList_.begin(); iter_ != formsList_.end(); ++iter_ )
		{
			if ( (*iter_)->getConstantData() )
				binaryData_.append((*iter_)->getConstantData(), (*iter_)->getConstantDataSize());
		}
	}

	return binaryData_;
}

void HobartLabel::show()
{
	Debug::trace("HobartLabel", 24, "*****   show()   *****");
	Debug::trace("HobartLabel", 24, "numberOfForms_ = " + util_string::inttostring(numberOfForms_));
	Debug::trace("HobartLabel", 24, "formatChanged_ = " + util_string::booltostring(formatChanged_));
	header_.show();
	lsd_.show();
	int i = 0;
	for ( FormsIteratorType iter_ = formsList_.begin(); iter_ != formsList_.end(); ++iter_ )
	{
		Debug::trace("HobartLabel", 24, "    FormRecord # " + util_string::inttostring(i++));
		(*iter_)->show();
	}
}
