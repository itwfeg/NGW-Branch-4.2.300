#ifndef LABELPRINTINGCOUNTS_H
#define LABELPRINTINGCOUNTS_H

#include "CEvolMsgReceiver.h"
#include "EvolMessage.h"
#include "../../HFC/MessageFormat.h"

class LabelPrintingCounts : public EvolMessage
{
	public:
		LabelPrintingCounts(Hobart::CEvolMsgReceiver *pHobThread = NULL, const int currentPrinted = 0, const int totalLabels = 0);
		LabelPrintingCounts(const char *buffer);
		~LabelPrintingCounts();

		const char *serialize() const;

		int getCurrentPrintedCount() const { return currentPrintedCount; }
		void setCurrentPrintedCount(const int count) { currentPrintedCount = count; }

		int getTotalLabelsCount() const { return totalLabelsCount; };
		void setTotalLabelsCount(const int count) { totalLabelsCount = count; }

	protected:
		int currentPrintedCount;
		int totalLabelsCount;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //LABELPRINTINGCOUNTS_H
