/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include "PAL_platformheader.h"

#include "evolution.h"
#include "HfcObject.h"

#include "util/platform/platform.h"

#include <list>

class DBSession;

typedef enum StatisticsFieldOffsets
{
	fieldOffsetUsedFlashMemory = 1,
	fieldOffsetFreeFlashMemory,
	fieldOffsetUsedRamMemory,
	fieldOffsetFreeRamMemory,
	fieldOffsetUsedEVMMemory,
	fieldOffsetFreeEVMMemory,
	fieldOffsetProductsCount,
	fieldOffsetClassesCount,
	fieldOffsetExpandedTextCount,
	fieldOffsetSpecialMessagesCount,
	fieldOffsetProductNotesCount,
	fieldOffsetNutrifactsCount,
	fieldOffsetLabelTypesCount,
	fieldOffsetGraphicsCount,
	fieldOffsetFontsCount,
	fieldOffsetOperatorsCount,
	fieldOffsetTotalsCount,
	fieldOffsetAvgNewProducts,
	fieldOffsetScaleName,
	fieldOffsetFirmwareVersionStat,
	fieldOffsetFirmwareRevisionStat,
	fieldOffsetTransactionsCount,
	fieldOffsetCutTestCount,
	fieldOffsetFlashkeysCount,
	endStatisticsFieldOffsets,
} StatisticsFieldOffsets_t;


class Statistics : public HfcObject
{
	public:

		Statistics(DBSession *mySession = 0);
		//Statistics(const char *data, int len, int formatType);

		~Statistics();

		UINT32 getUsedEVMmemory() const { return usedEVMMemory_; }
		void setUsedEVMMemory(UINT32 used) { usedEVMMemory_ = used; }

		UINT32 getFreeEVMmemory() const { return freeEVMMemory_; }
		void setFreeEVMMemory(UINT32 free) { freeEVMMemory_ = free; }


		virtual const char *serialize(int format) const;

		static void initFieldList();


	private:

		DBSession *mySession;

#ifdef LINUX_FIX
		PAL_MEMORYSTATUS memStats; 
#else
		MEMORYSTATUS memStats; 
#endif
		UINT32 usedEVMMemory_;
		UINT32 freeEVMMemory_;

		char *physMemTop;
		char *memTop;

		long64 flashUsedBytes;
		long64 flashUsedKb;
		long64 flashFreeBytes;
		long64 flashFreeKb;

		long64 ramDiskUsedBytes;
		long64 ramDiskUsedKb;
		long64 ramDiskFreeBytes;
		long64 ramDiskFreeKb;

		// Amount of used and free RAM memory
		long64 ramUsedBytes;
		long64 ramUsedKb;
		long64 ramFreeBytes;
		long64 ramFreeKb;

		int productCount;
		int classCount;
		int expTextCount;
		int spMsgCount;
		int productNotesCount;
		int nutrifactsCount;
		int labelTypeCount;
		int graphicCount;
		int operCount;
		int avgSpace;  // in bytes
		int additionalItemCount;

		int transactionsCount;
		int cuttingTestsCount;
		int softkeysCount;

		static std::list<StatisticsFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //STATISTICS_H
