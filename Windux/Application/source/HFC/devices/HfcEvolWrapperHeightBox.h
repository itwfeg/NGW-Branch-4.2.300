#ifndef HFCEVOLWRAPPERHEIGHTBOX_H
#define HFCEVOLWRAPPERHEIGHTBOX_H



#include <string>
#include <list>

#include "HfcObject.h"

#include "WrapBox.h"



class HfcEvolWrapperHeightBox : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperHeightBoxFieldOffsets
		{
			fieldOffsetEvolWrHeightBoxPrecedence = 1,
			fieldOffsetEvolWrHeightBoxHeight,
			fieldOffsetEvolWrHeightBoxAttributes,
			fieldOffsetEvolWrHeightBoxNewHeight,
			fieldOffsetEvolWrHeightBoxCanDelete,

			endHfcEvolWrHeightBoxFieldOffsets	//	MUST be last!
		} HfcEvolWrapperHeightBoxFieldOffsets_t;

		const static char DECIMAL_POINT = '.';


		// --- Constructors and destructor ---
		HfcEvolWrapperHeightBox(int precedence, const WrapBox::HeightBox& heightBox);
		HfcEvolWrapperHeightBox(const char* buffer, int len, int formatType);
		~HfcEvolWrapperHeightBox();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		int getHeight() const { return height_; }
		void setHeight(int height) { height_ = height; }

		int getNewHeight() const { return newHeight_; }
		void setNewHeight(int height) { newHeight_ = height; }

		int getNumberAttributes() const { return (int)attributes_.size(); }

		bool getCanDelete() const { return canDelete_; }
		void setCanDelete(bool value) { canDelete_ = value; }


		// --- Processing methods ---
		void init();
		void initFieldLists();
		bool update(WrapBox::HeightBox& heightBox) const;
		void show() const;
		const char* serialize(int format)const;


	private:
		// --- Instance Members ---
		int precedence_;
		int height_;
		int newHeight_;
		bool canDelete_;
		std::vector<int> attributes_;


		static std::list<HfcEvolWrapperHeightBox::HfcEvolWrapperHeightBoxFieldOffsets> browserFieldList, createFieldList;

		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};



#endif /* HFCEVOLWRAPPERHEIGHTBOX_H */
