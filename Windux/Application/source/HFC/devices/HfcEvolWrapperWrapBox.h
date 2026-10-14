#ifndef HFCEVOLWRAPPERWRAPBOX_H
#define HFCEVOLWRAPPERWRAPBOX_H



#include <string>

#include "HfcObject.h"


class WrapBox;



class HfcEvolWrapperWrapBox : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperWrapBoxFieldOffsets
		{
			fieldOffsetEvolWrWrapBoxPrecedence = 1,
			fieldOffsetEvolWrWrapBoxName,
			fieldOffsetEvolWrWrapBoxPLUException,
			fieldOffsetEvolWrWrapBoxMinLength,
			fieldOffsetEvolWrWrapBoxMaxLength,
			fieldOffsetEvolWrWrapBoxMinWidth,
			fieldOffsetEvolWrWrapBoxMaxWidth,
			fieldOffsetEvolWrWrapBoxLipHeight,
			fieldOffsetEvolWrWrapBoxHeightBoxes,

			endHfcEvolWrWrapBoxFieldOffsets	//	MUST be last!
		} HfcEvolWrapperWrapBoxFieldOffsets;

		const static char DECIMAL_POINT = '.';


		// --- Constructors and destructor ---
		HfcEvolWrapperWrapBox(int precedence, const WrapBox& wrapBox);
		HfcEvolWrapperWrapBox(const char* buffer, int len, int formatType);
		~HfcEvolWrapperWrapBox();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		const std::string& getName() const { return name_; }
		void setName(const std::string& name) { name_.assign(name); }

		int getPLUException() const { return pluException_; }
		void setPLUException(int plu) { pluException_ = plu; }

		int getMinLength() const { return minLength_; }
		void setMinLength(int l) { minLength_ = l; }

		int getMaxLength() const { return maxLength_; }
		void setMaxLength(int l) { maxLength_ = l; }

		int getMinWidth() const { return minWidth_; }
		void setMinWidth(int w) { minWidth_ = w; }

		int getMaxWidth() const { return maxWidth_; }
		void setMaxWidth(int w) { maxWidth_ = w; }

		int getLipHeight() const { return lipHeight_; }
		void setLipHeight(int h) { lipHeight_ = h; }

		int getHeight(int index) const { return heights_[index]; }
		void setHeight(int height, int index) { heights_.reserve(index + 1); heights_[index] = height; }

		int getNumberHeights() const { return (int)heights_.size(); }

		bool isValid() const;


		// --- Processing methods ---
		void init();
		bool nameOrPLUChanged(const WrapBox& wrapBox) const;
		bool update(WrapBox& wrapBox) const;
		void show(const std::string& category = "") const;
		const char* serialize(int format)const;


	private:
		// --- Instance Members ---
		int precedence_;
		std::string name_;
		int pluException_;
		int minLength_;
		int maxLength_;
		int minWidth_;
		int maxWidth_;
		int lipHeight_;
		std::vector<int> heights_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERWRAPBOX_H */
