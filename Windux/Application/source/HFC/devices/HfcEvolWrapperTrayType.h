#ifndef HFCEVOLWRAPPERTRAYTYPE_H
#define HFCEVOLWRAPPERTRAYTYPE_H



#include <string>

#include "HfcObject.h"



class HfcEvolWrapperTrayType : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperTrayTypeFieldOffsets
		{
			fieldOffsetEvolWrTrayTypePrecedence = 1,
			fieldOffsetEvolWrTrayTypeName,
			fieldOffsetEvolWrTrayTypePLUException,

			endHfcEvolWrTrayTypeFieldOffsets	//	MUST be last!
		} HfcEvolWrapperTrayTypeFieldOffsets_t;


		// --- Constructors and destructor ---
		HfcEvolWrapperTrayType(const char* buffer, int len, int formatType);
		~HfcEvolWrapperTrayType();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		const std::string& getName() const { return name_; }
		void setName(const std::string& name) { name_.assign(name); }

		int getPLUException() const { return pluException_; }
		void setPLUException(int plu) { pluException_ = plu; }


		// --- Processing methods ---
		void init();
		void show() const;


	private:
		// --- Instance Members ---
		int precedence_;
		std::string name_;
		int pluException_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERTRAYTYPE_H */
