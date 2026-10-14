#ifndef OPMODEPACKAGE_H
#define OPMODEPACKAGE_H

#include "Printer.h"
#include "LabelApplier.h"
#include "LabelApplierSetup.h"

class ProductTransaction;
class PrinterManager;

class OpModePackage
{
	typedef OpModePackage _Myt;

public:
	OpModePackage(
		const ProductTransaction *pTransaction = NULL,
		UINT8 packId = 0,
		LaPosLoc primaryPlacement = LocRight,
		LaRotation primaryRotation = Rotate0,
		LaPosLoc secondaryPlacement = LocLeft,
		LaRotation secondaryRotation = Rotate0,
		Hobart::CEvolMsgReceiver* pFirstPrinter = NULL,
		Hobart::CEvolMsgReceiver* pSecondPrinter = NULL,
		Hobart::CEvolMsgReceiver* pThirdPrinter = NULL,
		bool invalidPackage = false);

	virtual ~OpModePackage();

	void setProductTransaction(const ProductTransaction *pTransaction);
	const ProductTransaction *getProductTransaction() const;

	void setPackId(UINT8 packId);
	UINT8 getPackId() const;

	void setPrimaryPlacement(LaPosLoc primaryPlacement);
	LaPosLoc getPrimaryPlacement() const;

	void setPrimaryRotation(LaRotation primaryRotation);
	LaRotation getPrimaryRotation() const;

	void setSecondaryPlacement(LaPosLoc secondaryPlacement);
	LaPosLoc getSecondaryPlacement() const;

	void setSecondaryRotation(LaRotation secondaryRotation);
	LaRotation getSecondaryRotation() const;

	void setFirstPrinter(Hobart::CEvolMsgReceiver *pFirstPrinter);
	Hobart::CEvolMsgReceiver *getFirstPrinter() const;

	void setSecondPrinter(Hobart::CEvolMsgReceiver *pSecondPrinter);
	Hobart::CEvolMsgReceiver *getSecondPrinter() const;

	void setThirdPrinter(Hobart::CEvolMsgReceiver *pThirdPrinter);
	Hobart::CEvolMsgReceiver *getThirdPrinter() const;

	void setCompleted(Hobart::CEvolMsgReceiver *pPrinter);
	void clearCompleted(Hobart::CEvolMsgReceiver *pPrinter);
	bool isCompleted(Hobart::CEvolMsgReceiver *pPrinter) const;
	bool isCompleted(PrinterType prType) const;

	void setInvalidPackage(bool invalid = true);
	bool getInvalidPackage() const;
	void clearInvalidPackage();

	bool isTotalCompleted() const;

	bool newLabelApplierSetup(const LabelApplierSetup &laSetup) const;

	void show() const;

private:
	const ProductTransaction *pTransaction;
	UINT8 packId;
	LaPosLoc primaryPlacement;
	LaRotation primaryRotation;
	LaPosLoc secondaryPlacement;
	LaRotation secondaryRotation;
	Hobart::CEvolMsgReceiver *pFirstPrinter;
	Hobart::CEvolMsgReceiver *pSecondPrinter;
	Hobart::CEvolMsgReceiver *pThirdPrinter;
	bool firstPrinterCompleted;
	bool secondPrinterCompleted;
	bool thirdPrinterCompleted;
	bool invalidPackage;


#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};
#endif	//OPMODEPACKAGE_H
