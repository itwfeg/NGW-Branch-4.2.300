#ifndef IMAGECONTROL_H
#define IMAGECONTROL_H


#include "PAL_platformheader.h"
#include "OffsetRecord.h"
#include "Rectangle.h"

#include <list>
#include <vector>
#include <map>


class FormRecord;
class LabelStockDescriptor;
class BitMap;
class Pen;



class ImageControl
{
	public:
		// --- Types and subtypes ---
		typedef std::list<FormRecord*> formContainer;
		typedef formContainer::const_iterator formContainerIterator;
		typedef std::map<UINT16, ImageControl*> embeddedContainer;
		typedef embeddedContainer::const_iterator embeddedContainerIterator;


		// --- Constructor and destructor ---
		ImageControl(formContainer *formList = NULL, const LabelStockDescriptor* typeOfLabel = NULL, BitMap *imageBitMap = NULL);
//		ImageControl(formContainer &formList, const LabelStockDescriptor* typeOfLabel = NULL, BitMap *imageBitMap = NULL);

		~ImageControl();


		// --- Accessor functions ---
		const formContainer *getFormsList() const;
		void setFormList(formContainer *formList);

		void initFormListIter() const;
		FormRecord *getFormFromList() const;
		bool isCurrentFormNull() const; 

		const LabelStockDescriptor* getLabelStockDescriptor() const;
		void setLabelStockDescriptor(const LabelStockDescriptor* typeOfLabel);

		UINT16 getLabelOrientationDegrees() const;

		UINT16 getImageOptions() const;
		bool isImageOptionsScrunch() const;
		bool isImageOptionsEditImage() const;
		void setImageOptions(UINT16 options);

		UINT16 getImageFlags() const;
		bool isImageFlagPreImage() const;
		bool isImageFlagPostImage() const;
		void setImageFlags(UINT16 imageStatus);

		INT16 getImageLines() const;
		void setImageLines(INT16 lines) const;

		bool getImageValid() const;
		void setImageValid(bool valid);

		Pen *getPen() const;
		void setPen(Pen *imagingPen);

		BitMap *getImageBitMap() const;
		Hobart::Rectangle getImageBitMapBounds() const;
		INT16 getImageBitMapBoundsLeft() const;
		INT16 getImageBitMapBoundsRight() const;
		INT16 getImageBitMapBoundsTop() const;
		INT16 getImageBitMapBoundsBottom() const;

		void setImageBitMap(BitMap *imageBitMap);
		INT16 getImageBitMapHeight() const;
		INT16 getImageBitMapWidth() const;
		UINT16 *getImageBitMapBase() const;
		UINT32 getImageBitMapSize() const;
		void setImageBitMapSize(UINT32 size);

		BitMap *getRotatedBitMap() const;
		Hobart::Rectangle getRotatedBitMapBounds() const;
		void setRotatedBitMap(BitMap *rotatedBitMap);
		UINT32 getRotatedBitMapSize() const;
		void setRotatedBitMapSize(UINT32 size);

		BitMap *getPrintBitMap() const;
		void setPrintBitMap(BitMap *printBitMap);

		void updateOffsetTable(INT16 line, INT16 offset);
		INT16 getOffsetFromTable(INT16 line) const;
		
		UINT16 getCenteringOffset() const;
		void setCenteringOffset(UINT16 offset);
		
		UINT16 getHorizFormOffset() const;
		void setHorizFormOffset(UINT16 offset);
		
		UINT16 getHorizFormWidth() const;
		void setHorizFormWidth(UINT16 width);

		UINT16 getTotalFormHorizOffset() const;

		bool getPrintingRotatedBitmap() const;
		void setPrintingRotatedBitmap(bool printRotated);

		UINT16 getRotatedImgOffset() const;
		void setRotatedImgOffset(UINT16 ImgOffset);

		bool isDigimarcEnabled() const {return digimarc;}
		void setDigimarcEnabled( bool value ) {digimarc = value;}

		int getDigimarcCount() const {return digimarcCount;}
		void setDigimarcCount( int value ) {digimarcCount = value;}

		int getDigimarcWeightLb() const {return digimarcWeightLb;}
		void setDigimarcWeightLb( int value ) {digimarcWeightLb = value;}

		int getDigimarcWeightKg() const {return digimarcWeightKg;}
		void setDigimarcWeightKg( int value ) {digimarcWeightKg = value;}

		int getDigimarcSellByDate() const {return digimarcSellByDate;}
		void setDigimarcSellByDate( int value ) {digimarcSellByDate = value;}

		int getDigimarcPrice() const {return digimarcPrice;}
		void setDigimarcPrice( int value ) {digimarcPrice = value;}


		const ImageControl* getEmbeddedImageControl(UINT16 labelNumber) const;
		const ImageControl* getEmbeddedImageControlAccumItem(UINT16 positionNum) const;
		ImageControl* addEmbeddedLabel(UINT16 labelNumber, formContainer* formList, 
				const LabelStockDescriptor *typeOfLabel = NULL, BitMap *imageBitMap = NULL);


		void show() const;


		std::string getSaveToFilenameString() const { return saveToFilenameString; }
		void setSaveToFilenameString(std::string filename) { saveToFilenameString = filename; }


	private:
		formContainer *formRecordList;		// list of the individual fields that make up this label 
	   	mutable formContainerIterator iter;

	   	typedef std::map<int, int> offsetMap;
	   	offsetMap offset_table;			// vertical offset for a particular line

		const LabelStockDescriptor* label_stock_descriptor;	// describes the type of label the image can printed on
	   													// this is checked agianst what is present in the printer
	 													// to see if the label can be printed
		UINT16 centeringOffset;							// this is new and is needed because we have to center EPP labels
		UINT16 horizFormOffset;							// this is used to horizontally offset all forms in the label 
		UINT16 horizFormWidth;							// this is to determine the width of the drawing area.
		UINT16 image_options;							// does the label image need scrunched, editImage or normal
		UINT16 image_flags;								// status of the image pre or post image phase
		mutable INT16 image_lines;						// number of lines imaged
		bool image_valid;								// is the image valid?
		Pen *pen;										// pen used to image the label
		BitMap *image_bitmap;							//what gets pouped out of the Renderer
		BitMap *rotated_bitmap;							
		BitMap *print_bitmap;

		//OffsetRecord		//see ultima

		UINT32 image_bitmap_size;
		UINT32 rotated_bitmap_size;
		bool printingRotatedBitmap;
		UINT16 rotatedImgOffset;						// rotatedImgOffset

		embeddedContainer embeddedImageControls_;

		bool digimarc;
		int digimarcCount;
		int digimarcWeightLb;
		int digimarcWeightKg;
		int digimarcSellByDate;
		int digimarcPrice;

		std::string saveToFilenameString;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif	//IMAGECONTROL_H
