#ifndef HOBARTLABEL_H
#define HOBARTLABEL_H



#include <list>


#include "Blob.h"
#include "ResourceHeader.h"
#include "LabelStockDescriptor.h"
#include "Rectangle.h"

class FormRecord;


class HobartLabel
{
	public:
		// --- Types and subtypes ---
		typedef std::list<FormRecord*> FormsListType;
		typedef std::list<FormRecord*>::iterator FormsIteratorType;


		// --- Constructors and destructor ---
		HobartLabel(Blob *rawData, bool exitIfOldFormat = false);
		// Beware: If you delete the elements of the list passed in as the third
		// parameter, you may get a crash when the HobartLabel destructor gets
		// called since the same list elements will be deleted at that point.
		HobartLabel(const ResourceHeader& header, const LabelStockDescriptor& lsd,
				const FormsListType& formsList);
		~HobartLabel();


		// --- Getter and setter methods ---
		LabelStockDescriptor *getLabelStockDescriptor();
		INT32 getNumberOfForms();
		const Blob& getBinaryData();
		FormsListType* getFormsList();
		bool isFormatChanged() { return formatChanged_; }
		bool isBinaryValid() { return binaryValid_; }

		void startAtListTop();
		FormRecord * getFormFromList();

		void show();

		ResourceHeader &getResourceHeader() {return header_;}


	private:
		ResourceHeader header_;
		LabelStockDescriptor lsd_;
		INT32 numberOfForms_;	      // number of forms in the list that makeup this label
		FormsListType formsList_;
		FormsIteratorType iter_;
		bool formatChanged_;
		Blob binaryData_;
		bool binaryValid_;

		bool createLabelFromBlob(Blob *rawData, bool exitIfOldFormat);  // returns false if invalid
		INT32 setLabelStockDesriptor(Blob *blob, INT32 counter, bool oldFormat);

		unsigned int zorder( FormRecord *form ) const;
		unsigned int zorder( FormsListType& forms, Hobart::Rectangle& boundingRect ) const;

		// The following conversion factor is based on the Ultima magic factor 0.0065
		// The conversion factor is used to convert lines on an Ultima label first into
		// inches (the Ultima magic factor), then those inches into milimeters (25.4)
		// and these into lines on an Evolution label (8).
		static const double _ultimaConversionFactor_;
		static INT16 convertCoordinate(INT16 x)
				{ return (INT16)(x * _ultimaConversionFactor_); }
		static INT16 convertSegment(INT16 x, INT16 l, INT16* newx)
				{ INT16 y = convertCoordinate(x); if ( newx ) *newx = y; return convertCoordinate(l); }
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//HOBARTLABEL_H

