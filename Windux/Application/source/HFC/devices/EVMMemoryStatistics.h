#ifndef EVMMEMORYSTATISTICS_H
#define EVMMEMORYSTATISTICS_H



#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"


class EVMMemoryStatistics : public EvolMessage
{
	public:
		// --- Constructors and destructor ---
		EVMMemoryStatistics(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EVMMemoryStatistics(const char *buffer);
		~EVMMemoryStatistics();


		// --- Accessor methods ---
		UINT32 getUsedMemory() const;
		void setUsedMemory(UINT32 usedMemory);

		UINT32 getFreeMemory() const;
		void setFreeMemory(UINT32 freeMemory);


		// --- Processing methods ---
		const char *serialize() const;

		void show();	


	private:
		// --- Data members ---
		UINT32 usedMemory_;
		UINT32 freeMemory_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //EVMMEMORYSTATISTICS_H
