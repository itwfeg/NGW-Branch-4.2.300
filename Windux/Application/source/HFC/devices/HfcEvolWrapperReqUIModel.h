#ifndef HFCEVOLWRAPPERREQUIMODEL_H
#define HFCEVOLWRAPPERREQUIMODEL_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperReqUIModel.h"

class HfcEvolWrapperReqUIModel : public EvolWrapperReqUIModel, public HfcObject
{
	public:
		HfcEvolWrapperReqUIModel(const char *data);	
		~HfcEvolWrapperReqUIModel();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
