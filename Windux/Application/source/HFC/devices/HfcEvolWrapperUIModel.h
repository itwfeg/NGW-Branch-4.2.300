#ifndef HFCEVOLWRAPPERUIMODEL_H
#define HFCEVOLWRAPPERUIMODEL_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperUIModel.h"

class HfcEvolWrapperUIModel : public EvolWrapperUIModel, public HfcObject
{
	public:
		HfcEvolWrapperUIModel(const char *data);	
		~HfcEvolWrapperUIModel();

		virtual const char *serialize(int format = InternalFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif