#include "OperatorChangeSummary.h"


#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorChangeSummary::dataType = 0;
	std::string OperatorChangeSummary::dataTypeName("hfc::OperatorChangeSummary"); 
#endif


	OperatorChangeSummary::OperatorChangeSummary(const std::string& operatoId, const std::string& operatorName)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		setOperatorId(operatoId);
		setOperatorName(operatorName);
	}


	OperatorChangeSummary::~OperatorChangeSummary()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void OperatorChangeSummary::init()
	{
		operatorId_.clear();
		operatorName_.clear();
		transQuantity_ = 0;
		oldTotal_ = 0;
		delta_ = 0;
	}


}
