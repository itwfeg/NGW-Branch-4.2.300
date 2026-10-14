#include "ImageControl.h"


#include "LabelType.h"
#include "LabelStockDescriptor.h"
#include "BitMap.h"
#include "Label.h"
#include "Configuration.h"



#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"

#include "utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ImageControl::dataType = 0;
std::string ImageControl::dataTypeName("ImageControl"); 
#endif

using namespace std;



ImageControl::ImageControl(formContainer *formList, const LabelStockDescriptor* typeOfLabel, BitMap *imageBitMap)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ImageControl", "ctor " +
		Hex::toString((void*)formList) + " " +
		Hex::toString((void*)typeOfLabel) + " " +
		Hex::toString((void*)imageBitMap));

	formRecordList = formList;
	label_stock_descriptor = typeOfLabel;
	image_bitmap = imageBitMap;

	init();
	initFormListIter();
}

void ImageControl::init()
{
	offset_table.clear();

	pen = NULL;
	rotated_bitmap = NULL;
	print_bitmap = NULL;
	centeringOffset = 0;
	horizFormOffset	= 0;
	horizFormWidth = 0;

	digimarc = false;
	digimarcCount = digimarcWeightLb = digimarcWeightKg =
			digimarcSellByDate = digimarcPrice = 0;

	saveToFilenameString = "";
}

ImageControl::~ImageControl()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("ImageControl", "dtor");

	embeddedContainerIterator eci( embeddedImageControls_.begin() );

	while ( eci != embeddedImageControls_.end() )
	{
		delete eci->second;
		++eci;
	}
}


const ImageControl::formContainer *ImageControl::getFormsList() const
{
	Debug::trace("ImageControl", "get form list " + Hex::toString((void*)formRecordList));
	return formRecordList;
}


void ImageControl::setFormList(formContainer *formList)
{
	Debug::trace("ImageControl", "set form list " + Hex::toString((void*)formList));
	formRecordList = formList;

	initFormListIter();
}


void ImageControl::initFormListIter() const
{
	if ( formRecordList )
		iter = formRecordList->begin();
}


FormRecord *ImageControl::getFormFromList() const
{
	Debug::trace("ImageControl", "get form from list" );

	FormRecord* form( NULL );

	// do not allow iterator run off!
	if (( formRecordList ) && ( iter != formRecordList->end() ))
	{
		form = *iter;	//initially, iter should point to the first element in the list
		++iter; 
	}
	// else error, user has over shot the list or list was initially empty!
	
	return form;	  
}


bool ImageControl::isCurrentFormNull() const
{
	Debug::trace("ImageControl", "is current null" );

	return (( !formRecordList ) || ( iter == formRecordList->end() ));
}


const LabelStockDescriptor* ImageControl::getLabelStockDescriptor() const
{
	return label_stock_descriptor;
}


void ImageControl::setLabelStockDescriptor(const LabelStockDescriptor* typeOfLabel)
{
	this->label_stock_descriptor = typeOfLabel;
}


UINT16 ImageControl::getLabelOrientationDegrees() const
{
	if ( label_stock_descriptor )
		return label_stock_descriptor->getLabelOrientationDegrees();

	return 0;
}


UINT16 ImageControl::getImageOptions() const
{
	return image_options;
}


void ImageControl::setImageOptions(UINT16 options)
{
	image_options = options;
}


bool ImageControl::isImageOptionsScrunch() const
{
	return (image_options == Scrunch);
}


bool ImageControl::isImageOptionsEditImage() const
{
	return (image_options == EditImage);
}


UINT16 ImageControl::getImageFlags() const
{
	return image_flags;
}


void ImageControl::setImageFlags(UINT16 imageStatus)
{
	image_flags = imageStatus;

	embeddedContainerIterator eci = embeddedImageControls_.begin();
	while ( eci != embeddedImageControls_.end() )
	{
		eci->second->setImageFlags(imageStatus);
		++eci;
	}
}


INT16 ImageControl::getImageLines() const
{
	return image_lines;
}


void ImageControl::setImageLines(INT16 lines) const
{
	image_lines = lines;
}


bool ImageControl::getImageValid() const
{
	return image_valid;
}


bool ImageControl::isImageFlagPreImage() const
{
	return (image_flags == PreImage);
}


bool ImageControl::isImageFlagPostImage() const
{
	return (image_flags == PostImage);
}


void ImageControl::setImageValid(bool valid)
{
	image_valid = valid;
}


Pen *ImageControl::getPen() const
{
	return pen;
}


void ImageControl::setPen(Pen *imagingPen)
{
   	this->pen = imagingPen;
}


BitMap *ImageControl::getImageBitMap() const
{
	return image_bitmap;
}


Hobart::Rectangle ImageControl::getImageBitMapBounds() const
{
	return image_bitmap->getBitMapBounds();
}


INT16 ImageControl::getImageBitMapBoundsLeft() const
{
	return image_bitmap->getBitMapBoundsLeft();
}


INT16 ImageControl::getImageBitMapBoundsRight() const
{
	return image_bitmap->getBitMapBoundsRight();
}


INT16 ImageControl::getImageBitMapBoundsTop() const
{
	return image_bitmap->getBitMapBoundsTop();
}


INT16 ImageControl::getImageBitMapBoundsBottom() const
{
	return image_bitmap->getBitMapBoundsBottom();
}


void ImageControl::setImageBitMap(BitMap *imageBitMap)
{
  	this->image_bitmap = imageBitMap;
}


BitMap *ImageControl::getRotatedBitMap() const
{
	return rotated_bitmap;
}


Hobart::Rectangle ImageControl::getRotatedBitMapBounds() const
{
	return rotated_bitmap->getBitMapBounds();
}


void ImageControl::setRotatedBitMap(BitMap *rotatedBitMap)
{
	this->rotated_bitmap = rotatedBitMap;
}


BitMap *ImageControl::getPrintBitMap() const
{
	return print_bitmap;
}


void ImageControl::setPrintBitMap(BitMap *printBitMap)
{
	this->print_bitmap = printBitMap; 
}


UINT32 ImageControl::getImageBitMapSize() const
{
	return image_bitmap_size;
}


void ImageControl::setImageBitMapSize(UINT32 size)
{
	image_bitmap_size = size; 
}


INT16 ImageControl::getImageBitMapHeight() const
{
	return(image_bitmap->getBitMapBoundsBottom() - image_bitmap->getBitMapBoundsTop());
}


INT16 ImageControl::getImageBitMapWidth() const
{
	return(image_bitmap->getBitMapBoundsRight() - image_bitmap->getBitMapBoundsLeft());
}


UINT16 *ImageControl::getImageBitMapBase() const
{
	return image_bitmap->getBitMapBase();
}


UINT32 ImageControl::getRotatedBitMapSize() const
{
	return rotated_bitmap_size;
}


void ImageControl::setRotatedBitMapSize(UINT32 size)
{
	rotated_bitmap_size = size;
}

void ImageControl::updateOffsetTable(INT16 line, INT16 offset)
{
    Debug::trace("PrinterManager.offsetTable", "Adding " + inttostring(line) + " " + inttostring(offset) );
    offset_table[line] = offset;
}


INT16 ImageControl::getOffsetFromTable(INT16 line) const
{
	INT16 offset( 0 );

	for ( offsetMap::const_iterator i( offset_table.begin() ); i != offset_table.end(); i++ )
	{
		if ( line < i->first )
			break;

		offset = i->second;
	}

    Debug::trace("PrinterManager.offsetTable", "Offset " + inttostring(line) + " " + inttostring(offset) );
	return offset;
}


UINT16 ImageControl::getCenteringOffset() const
{
	return centeringOffset;
}


void ImageControl::setCenteringOffset(UINT16 offset)
{
	centeringOffset = offset;
}

	
UINT16 ImageControl::getHorizFormOffset() const
{
	return horizFormOffset;
}


void ImageControl::setHorizFormOffset(UINT16 offset)
{
	horizFormOffset = offset;
}


UINT16 ImageControl::getHorizFormWidth() const
{
	return horizFormWidth;
}


void ImageControl::setHorizFormWidth(UINT16 width)
{
	horizFormWidth = width;
}

				 
UINT16 ImageControl::getTotalFormHorizOffset() const
{
	return centeringOffset + horizFormOffset;
}


bool ImageControl::getPrintingRotatedBitmap() const
{
	return printingRotatedBitmap;
}


void ImageControl::setPrintingRotatedBitmap(bool printRotated)
{
	printingRotatedBitmap = printRotated;
}


UINT16 ImageControl::getRotatedImgOffset() const
{
	return rotatedImgOffset;
}


void ImageControl::setRotatedImgOffset(UINT16 ImgOffset)
{
	rotatedImgOffset = ImgOffset;
}


const ImageControl* ImageControl::getEmbeddedImageControl(UINT16 labelNumber) const
{
	embeddedContainerIterator eci = embeddedImageControls_.find(labelNumber);
	if ( eci != embeddedImageControls_.end() )
		return eci->second;

	return 0;
}


const ImageControl* ImageControl::getEmbeddedImageControlAccumItem(UINT16 positionNum) const
{
	embeddedContainerIterator eci;

	Debug::trace("PrinterManager.embeddedLabels", " entering ImageControl::getEmbeddedImageControlAccumItem  position=" + util_string::inttostring(positionNum));

	eci = embeddedImageControls_.begin();
	while ( eci != embeddedImageControls_.end() )
	{
		const std::string f( util_string::inttostring( eci->first ) );
		const std::string s( util_string::ptrtostring( eci->second ) );

		Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, look at all embeddedImageControls_, first=" + f + "  second=" + s);
		eci++;
	}

	eci = embeddedImageControls_.find( Configuration::getInstance()->toIntCfgI( CfgInternal::ACCUMITEMSITEMLABELTYPE ) );
	if ( eci == embeddedImageControls_.end() )
		Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, DIDN'T FIND embeddedImageControls_ for 177 ");
	else
	{
		Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, found embeddedImageControls_ for 177, first=" + util_string::inttostring(eci->first) + "  second=" + util_string::ptrtostring(eci->second));

		for (int i = 1; i < positionNum && eci != embeddedImageControls_.end(); i++)
			eci++;

		if ( eci == embeddedImageControls_.end() )
			Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, after positioning, we're at the end of the iterator");
		else
		{
			Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, after positioning, found embeddedImageControls_ for 177, first=" + util_string::inttostring(eci->first) + "  second=" + util_string::ptrtostring(eci->second));

			if ( eci->first == Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMLABELTYPE) )
			{
				Debug::trace("PrinterManager.embeddedLabels", " ImageControl::getEmbeddedImageControlAccumItem, returning second=" + util_string::ptrtostring(eci->second));
				return eci->second;
			}
		}
	}

	return 0;
}


ImageControl* ImageControl::addEmbeddedLabel(
	UINT16 labelNumber,
	formContainer* formList,
	const LabelStockDescriptor* typeOfLabel,
	BitMap* imageBitMap )
{
	ImageControl* ic = new ImageControl(formList, typeOfLabel, imageBitMap);

	embeddedImageControls_.insert(make_pair(labelNumber, ic));

	return ic;
}


void ImageControl::show() const
{
	bool addedTC = false;

	if (!Debug::isTracing(HobString("ImageControl")))
	{
		addedTC = true;
		Debug::addTraceCategory(HobString("ImageControl"));
	}
	Debug::trace("ImageControl", "unrotated bitmap ");

	if (image_bitmap)
		image_bitmap->show();
	else
		Debug::trace("ImageControl", "ImageControl's image_bitmap is NULL ");

	Debug::trace("ImageControl", HobString("unrotated bitmap size ") + HobString(Integer::toString(image_bitmap_size)));


	Debug::trace("ImageControl", "rotated bitmap ");

	if (rotated_bitmap)
		rotated_bitmap->show();
	else
		Debug::trace("ImageControl", "ImageControl's rotated_bitmap is NULL ");

	Debug::trace("ImageControl", HobString("rotated bitmap size ") + HobString(Integer::toString(rotated_bitmap_size)));
	Debug::trace("ImageControl", HobString("centering offset ") + HobString(Integer::toString(centeringOffset)));
	Debug::trace("ImageControl", HobString("horizontal offset ") + HobString(Integer::toString(horizFormOffset)));
	Debug::trace("ImageControl", HobString("horizontal form width ") + HobString(Integer::toString(horizFormWidth)));
	Debug::trace("ImageControl", HobString("rotated img offset ") + HobString(Integer::toString(rotatedImgOffset)));

	if (addedTC)
		Debug::removeTraceCategory(HobString("ImageControl"));
}
