#ifndef EVOLWRAPPERUIMODEL_H
#define EVOLWRAPPERUIMODEL_H


#include "WrapperGeneric.h"


class EvolWrapperUIModel : public WrapperGeneric
{
	public:

		typedef enum MfgImageType
		{
			_AWS,
			_EWS,
			_CLA
		} MfgImageType_t;

		EvolWrapperUIModel(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperUIModel(const char *data);	
		~EvolWrapperUIModel();
		
		bool isModelSelectionAWS() {return (selection_ == _AWS) ? true : false;}		
		bool isModelSelectionEWS() {return (selection_ == _EWS) ? true : false;}		
		bool isModelSelectionCLA() {return (selection_ == _CLA) ? true : false;}		

		int getModelSelection(){return selection_;}
		void setModelSelection(int selection){selection_ = selection;}

		const char *serialize() const;

	private:
		int selection_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif