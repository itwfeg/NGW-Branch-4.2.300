#ifndef HFCEVOLWRAPPERPACKAGETIMINGS_H
#define HFCEVOLWRAPPERPACKAGETIMINGS_H



#include <string>

#include "EvolWrapperPackageTimings.h"
#include "HfcObject.h"
#include "HfcEvolWrapperHeightBoxParam.h"


class HfcEvolWrapperWrapBox;
class HfcEvolWrapperHeightBox;

typedef enum HfcEvolWrapperPkgTimingsOffsets
{
	fieldOffsetEvolWrPkgTimingsVersion = 1,
	fieldOffsetEvolWrPkgTimingsTimings,
	endHfcEvolWrPkgTimingsOffsets	//	MUST be last!
} HfcEvolWrapperPkgTimingsOffsets_t;

class HfcEvolWrapperPackageTimings : public EvolWrapperPackageTimings, public HfcObject
{
	public:
		// --- Constructors and destructor ---
		HfcEvolWrapperPackageTimings();
		HfcEvolWrapperPackageTimings(const char* buffer);
		HfcEvolWrapperPackageTimings(EvolWrapperPackageTimings* ewpt);
		HfcEvolWrapperPackageTimings(const char* data, int len, int formatType);
		~HfcEvolWrapperPackageTimings();


		// --- Processing methods ---
		bool updateTrayType(int precedence, const std::string& name, int exception);
		void deleteTrayType(int precedence) { removeWrapBox(precedence); }
		bool changeTrayTypePrecedence(int precedence, int delta);
		bool updateWrapBox(const HfcEvolWrapperWrapBox& wrapBox);
		bool updateHeightBox(const HfcEvolWrapperHeightBox& heightBox);
		bool updateHeightBoxParam(const HfcEvolWrapperHeightBoxParam& heightBoxParam);

		void generateDummyData() { initDummy(); }

		static void initFieldList();


		virtual const char *serialize(int format = InternalFormat)const;


	private:
		// --- Class Members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

		static std::list<HfcEvolWrapperPkgTimingsOffsets> fieldList;
};



#endif /* HFCEVOLWRAPPERPACKAGETIMINGS_H */
