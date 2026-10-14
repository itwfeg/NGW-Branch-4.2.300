#ifndef HFCEVOLWRAPPERRENAMEHEIGHTBOX_H
#define HFCEVOLWRAPPERRENAMEHEIGHTBOX_H



#include <string>

#include "HfcObject.h"



class HfcEvolWrapperRenameHeightBox : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperRenameHeightBoxFieldOffsets
		{
			fieldOffsetEvolWrRenameHeightBoxPrecedence = 1,
			fieldOffsetEvolWrRenameHeightBoxOldHeight,
			fieldOffsetEvolWrRenameHeightBoxNewHeight,

			endHfcEvolWrRenameHeightBoxFieldOffsets	//	MUST be last!
		} HfcEvolWrapperRenameHeightBoxFieldOffsets_t;


		// --- Constructors and destructor ---
		HfcEvolWrapperRenameHeightBox(const char* buffer, int len, int formatType);
		~HfcEvolWrapperRenameHeightBox();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		int getOldHeight() const { return oldHeight_; }
		void setOldHeight(int height) { oldHeight_ = height; }

		int getNewHeight() const { return newHeight_; }
		void setNewHeight(int height) { newHeight_ = height; }


		// --- Processing methods ---
		void init();
		void show() const;


	private:
		// --- Instance Members ---
		int precedence_;
		int oldHeight_;
		int newHeight_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERRENAMEHEIGHTBOX_H */
