#ifndef HFCEVMMEMORYSTATISTICS_H
#define HFCEVMMEMORYSTATISTICS_H



#include "EVMMemoryStatistics.h"
#include "HfcObject.h"



typedef enum HfcEVMMemoryStatisticsFieldOffsets
{
	fieldOffsetUsedMemory = 1,
	fieldOffsetFreeMemory,

	endHfcEVMMemoryStatisticsFieldOffsets	//	MUST be last!
} HfcEVMMemoryStatisticsFieldOffsets_t;



class HfcEVMMemoryStatistics : public EVMMemoryStatistics, public HfcObject
{
	public:
		// --- Constructors and destructor ---
		HfcEVMMemoryStatistics();
		HfcEVMMemoryStatistics(const char *data);
		HfcEVMMemoryStatistics(const char *data, int len, int formatType);
		~HfcEVMMemoryStatistics();


		// --- Accessor methods ---


		// --- Processing methods ---
		void init();
		virtual const char *serialize(int format = InternalFormat) const;
		

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//HFCEVMMEMORYSTATISTICS_H
