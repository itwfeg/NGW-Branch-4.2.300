#ifndef HFCEVOLWRAPPERSAVECYCLECOUNT_H
#define HFCEVOLWRAPPERSAVECYCLECOUNT_H

#include "EvolWrapperSaveCycleCount.h"
#include "HfcObject.h"

typedef enum HfcEvolWrapperSaveCycleCountFieldOffsets
{
	endHfcEvolSaveCycleCountFieldOffsets	
} HfcEvolWrapperSaveCycleCountFieldOffsets_t;

class HfcEvolWrapperSaveCycleCount : public EvolWrapperSaveCycleCount, public HfcObject
{
	public:
		HfcEvolWrapperSaveCycleCount(const char *data);

		~HfcEvolWrapperSaveCycleCount();

		void show();

		virtual const char *serialize(int format = InternalFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif
