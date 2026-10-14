#ifndef HFCEVOLWRAPPERALLHTBOXESONEPARAM_H
#define HFCEVOLWRAPPERALLHTBOXESONEPARAM_H


#include <string>
#include <vector>

#include "HfcObject.h"
#include "HobList.h"


class HfcEvolWrapperPackageTimings;


class HfcEvolWrapperAllHtBoxesOneParam : public HfcObject
{
	public:
		// --- Constructors and destructor ---
		HfcEvolWrapperAllHtBoxesOneParam();
		HfcEvolWrapperAllHtBoxesOneParam(const HfcEvolWrapperPackageTimings& timings, int tray, int param);
		~HfcEvolWrapperAllHtBoxesOneParam();


		// --- Processing methods ---
		void show() const;
		const char* serialize(int format)const;


	private:
		// --- Instance Members ---
		int				attribute_;
		HobRecordNum	pluException_;
		HobList 		precedence_;
		HobList 		height_;
		HobList 		attributeValue_;


		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERALLHTBOXESONEPARAM_H */
