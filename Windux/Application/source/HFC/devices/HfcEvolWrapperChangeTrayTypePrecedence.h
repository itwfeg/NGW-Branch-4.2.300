#ifndef HFCEVOLWRAPPERCHANGETRAYTYPEPRECEDENCE_H
#define HFCEVOLWRAPPERCHANGETRAYTYPEPRECEDENCE_H



#include <string>

#include "HfcObject.h"



class HfcEvolWrapperChangeTrayTypePrecedence : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperChangeTrayTypePrecedenceFieldOffsets
		{
			fieldOffsetEvolWrRenameHeightBoxPrecedence = 1,
			fieldOffsetEvolWrRenameHeightBoxDelta,

			endHfcEvolWrRenameHeightBoxFieldOffsets	//	MUST be last!
		} HfcEvolWrapperChangeTrayTypePrecedenceFieldOffsets_t;


		// --- Constructors and destructor ---
		HfcEvolWrapperChangeTrayTypePrecedence(const char* buffer, int len, int formatType);
		~HfcEvolWrapperChangeTrayTypePrecedence();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		int getDelta() const { return delta_; }
		void setDelta(int delta) { delta_ = delta; }


		// --- Processing methods ---
		void init();
		void show() const;


	private:
		// --- Instance Members ---
		int precedence_;
		int delta_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERCHANGETRAYTYPEPRECEDENCE_H */
