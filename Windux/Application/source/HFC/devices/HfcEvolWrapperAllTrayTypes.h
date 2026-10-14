#ifndef HFCEVOLWRAPPERALLTRAYTYPES_H
#define HFCEVOLWRAPPERALLTRAYTYPES_H



#include <string>
#include <vector>

#include "HfcObject.h"


class HfcEvolWrapperPackageTimings;



class HfcEvolWrapperAllTrayTypes : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperAllTrayTypesFieldOffsets
		{
			fieldOffsetEvolWrAllTrayTypeNames = 1,
			fieldOffsetEvolWrAllTrayTypePLUExceptions,
			fieldOffsetEvolWrAllTrayTypeCanChange,

			endHfcEvolWrTrayTypeFieldOffsets	//	MUST be last!
		} HfcEvolWrapperAllTrayTypesFieldOffsets_t;


		// --- Constructors and destructor ---
		HfcEvolWrapperAllTrayTypes(const HfcEvolWrapperPackageTimings& timings);
		~HfcEvolWrapperAllTrayTypes();


		// --- Accessor methods ---
		const std::string& getName(int index) const { return names_[index]; }
		void setName(const std::string& name, int index) { names_.reserve(index + 1); names_[index] = name; }

		int getNumberNames() const { return (int)names_.size(); }

		int getPLUExceptions(int index) const { return pluExceptions_[index]; }
		void setPLUExceptions(int plu, int index) { pluExceptions_.reserve(index + 1); pluExceptions_[index] = plu; }

		int getNumberPLUExceptions() const { return (int)pluExceptions_.size(); }

		bool getCanChange(int index) const { return canChange_[index]; }
		void setCanChange(bool value, int index) { canChange_.reserve(index + 1); canChange_[index] = value; }

		int getNumberCanChanges() const { return (int)canChange_.size(); }


		// --- Processing methods ---
		void init();
		void show() const;
		const char* serialize(int format)const;


	private:
		// --- Instance Members ---
		std::vector<std::string> names_;
		std::vector<int> pluExceptions_;
		std::vector<bool> canChange_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERALLTRAYTYPES_H */
