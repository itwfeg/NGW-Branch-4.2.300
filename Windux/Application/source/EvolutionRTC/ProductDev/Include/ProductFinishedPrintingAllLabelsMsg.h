#ifndef PRODUCTFINISHEDPRINTINGALLLABELSMSG_H
#define PRODUCTFINISHEDPRINTINGALLLABELSMSG_H

#include "EvolMessage.h"
#include "evolution.h"

#include "CEvolMsgReceiver.h"

class ProductFinishedPrintingAllLabelsMsg : public EvolMessage
{
	public:
		ProductFinishedPrintingAllLabelsMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL,
						   						   bool printOk = true);

		ProductFinishedPrintingAllLabelsMsg(const char *buffer);
		~ProductFinishedPrintingAllLabelsMsg();

		const char *serialize() const;

		bool isPrintOk();
		void setPrintOk(bool value = true);
		void clearPrintOk();
		
	private:
		bool printOk;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif //PRODUCTFINISHEDPRINTINGALLLABELSMSG_H
