//using namespace std;

#include "PrinterTeachTable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTeachTable::dataType = 0;
std::string PrinterTeachTable::dataTypeName("PrinterTeachTable"); 
#endif

// Generic Printer Commands
const CommandIdleOperation		PrinterTeachTable::idleOp;
const CommandPrintOperation		PrinterTeachTable::printLabelOp(IndirectCommandData, PrRam0, HeadFirst);
const CommandPrintOperation		PrinterTeachTable::reversePrintLabelOp(IndirectCommandData, PrRam0, HeelFirst);
// advance the label until it is synced.
const CommandStepUntilOperation PrinterTeachTable::syncLabelOp(DirectCommandData, (PrinterIndirection) 2312, BitsEqual,
															  MotorForwardBit | SynchedBit, MotorForwardBit | SynchedBit);
// Finds trailing edge of label or leading edge of black bar - gap
const CommandStepUntilOperation PrinterTeachTable::syncBarOp(DirectCommandData, (PrinterIndirection) 2312, BitsEqual,
															MotorForwardBit | SyncBarBit | SyncBarEdgeBit,
															MotorForwardBit | SyncBarBit | SyncBarEdgeBit);
const CommandStepUntilOperation PrinterTeachTable::findLabel2000Op(DirectCommandData, (PrinterIndirection) 3000, BitsEqual,
																  LabelTakenBit, 0);
const CommandStepOperation 		PrinterTeachTable::ejectLabelOp(IndirectCommandData, PrConfigWord3);
//const CommandStepOperation 		PrinterTeachTable::ejectShootOp(DirectCommandData, (PrinterIndirection) 75);
//const CommandStepOperation 		PrinterTeachTable::ejectShootOp(DirectCommandData, (PrinterIndirection) 35);
const CommandStepOperation 		PrinterTeachTable::ejectShootOp(DirectCommandData, (PrinterIndirection) 55);
const CommandStepOperation 		PrinterTeachTable::backupLabelOp(IndirectCommandData, PrConfigWord4);
const CommandStepOperation 		PrinterTeachTable::advanceLabelOp(IndirectCommandData, PrRam1);
const CommandStepOperation 		PrinterTeachTable::retractLabelOp(IndirectCommandData, PrRam2);
const CommandStepOperation 		PrinterTeachTable::retractShootLabelOp(DirectCommandData, (PrinterIndirection) -100);	//SOF-5307 -150 //SOF-5089 	-75
const CommandStepOperation 		PrinterTeachTable::cutterEjectLabelOp(IndirectCommandData, PrConfigWord5);
//const CommandStepOperation 		PrinterTeachTable::cutterEjectShootLabelOp(DirectCommandData, (PrinterIndirection) 150);
const CommandStepOperation 		PrinterTeachTable::cutterEjectShootLabelOp(IndirectCommandData, PrConfigWord3);
const CommandStepOperation		PrinterTeachTable::startLabelOp(DirectCommandData, (PrinterIndirection) 11);
const CommandStepOperation 		PrinterTeachTable::adjustLabelUpOp(DirectCommandData, (PrinterIndirection) 10);
const CommandStepOperation 		PrinterTeachTable::adjustLabelBackOp(DirectCommandData, (PrinterIndirection) -10);
const CommandStepOperation 		PrinterTeachTable::peelLabelOp(IndirectCommandData, PrConfigWord0);
const CommandWaitUntilOperation PrinterTeachTable::takeLabelOp(BitsEqual, LabelTakenBit, LabelTakenBit);
const CommandWaitOperation 		PrinterTeachTable::waitLabelOp(DirectCommandData, (PrinterIndirection) 50);
const CommandWaitOperation 		PrinterTeachTable::waitShootLabelOp(DirectCommandData, (PrinterIndirection) 300);
const CommandTestOperation 		PrinterTeachTable::abortSizeTestOp(BitsEqual, SyncBarBit, SyncBarBit, 0, 0, ContinueExecution, AbortExecution);
const CommandTestOperation 		PrinterTeachTable::missingLabelTestOp(BitsEqual, LabelTakenBit, LabelTakenBit,
																	 LabelMissingBit, 0, ContinueExecution, ContinueExecution);
const CommandTestOperation 		PrinterTeachTable::jammedLabelTestOp(BitsEqual, MotorForwardBit | SynchedBit, MotorForwardBit | SynchedBit,
																	0, MediaJammedBit, ContinueExecution, AbortExecution);
const CommandTestOperation 		PrinterTeachTable::jammedLabelTestOp0(BitsEqual, MotorForwardBit | SyncBarBit | SyncBarEdgeBit,
																	 MotorForwardBit | SyncBarBit | SyncBarEdgeBit,
																	 0, MediaJammedBit, ContinueExecution, AbortExecution);
const CommandTestOperation		PrinterTeachTable::checkStockOp(BitsNotEqual, OutOfMediaBit, OutOfMediaBit, 0, 0,
															   ContinueExecution, AbortExecution);
const CommandCounterOperation 	PrinterTeachTable::resetCounterOp(ResetCounter);
const CommandCounterOperation	PrinterTeachTable::enableCounterOp(EnableCounter);
const CommandCounterOperation	PrinterTeachTable::disableCounterOp(DisableCounter);
const CommandStatusOperation	PrinterTeachTable::setTakingOp(SetBits, TakingBit);
const CommandStatusOperation	PrinterTeachTable::clearTakingOp(ClearBits, TakingBit);
const CommandOperation			PrinterTeachTable::cutOp(CutDirective);

PrinterTeachTable::PrinterTeachTable()
{
	pConfig_ = Configuration::getInstance();

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterTeachTable::~PrinterTeachTable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

UINT16 PrinterTeachTable::getNumEntries()
{
	return (UINT16)ops.size();
}

void PrinterTeachTable::addTeachTableEntry(CommandOperationList *ops, PrinterCommandType command, PrinterModeType mode)
{
	this->ops.push_back(ops);
	this->command.push_back(command);
	this->mode.push_back(mode);
}

CommandOperationList *PrinterTeachTable::getCommandOperationList(PrinterCommandType command, PrinterModeType mode)
{
	CommandOperationList *rv = NULL;
	int i = 0;
	int size = (int)ops.size();
	bool found = false;
	
	while ((found == false) && (i < size))
	{
		if ((this->command[i] == command) && (this->mode[i] == mode))
		{
			rv = ops[i];
			found = true;
		}
		else
		{
			i++;
		}
	}

	return rv;
}

void PrinterTeachTable::createCommandList()
{
	createGenericAdvance();
	createGenericSize();
	createGenericFind();
	createGenericPrintHead();
	createGenericEject();
	createGenericSync();
	createGenericFindwoLabelTaken();

	createDieCutSize();
	createDieCutPrintHead();
	createDieCutShootHead();
	createDieCutPrintHeel();
	createDieCutShootHeel();
	createDieCutSync();
	createDieCutEject();

	createDieCutStreamPrintHead();
	createDieCutShootStreamPrintHead();
	createDieCutStreamPrintHeel();
	createDieCutShootStreamPrintHeel();
	createDieCutStreamSync();
	createDieCutStreamEject();
	createDieCutStreamBackup();

	createListingPrintHead();
	createListingPrintHeel();
	createListingPrintHeadCut();
	createListingPrintHeelCut();
	createListingPrintCombinedHead();
	createListingPrintCombinedHeel();
	createListingSync();
	createListingEject();

	createContinuousPrintHead();
	createContinuousPrintHeel();

	//Debug::trace("PrinterTeachTable.Cutter"," createCommandList creating cut tables");
	createContinuousPrintHeadCut();
	createContinuousPrintHeelCut();
	createContinuousShootPrintHeelCut();
	createContinuousPrintCombinedHead();
	createContinuousPrintCombinedHeel();


	createContinuousSync();
	createContinuousEject();
	createContinuousSize();

	createCalibrateSize();
	createCalibrateFind();
	createCalibratePrint();
	createCalibrateEject();
	createCalibrateSync();

	createSpeedKeyPrintMode();
}

void PrinterTeachTable::createGenericAdvance()
{
	genericAdvance.push_back(&advanceLabelOp);
	genericAdvance.push_back(&idleOp);
}

void PrinterTeachTable::createGenericSize()
{
	genericSize.push_back(&disableCounterOp);
	genericSize.push_back(&resetCounterOp);
	genericSize.push_back(&syncBarOp);
	genericSize.push_back(&checkStockOp);
	genericSize.push_back(&abortSizeTestOp);
	genericSize.push_back(&enableCounterOp);
	genericSize.push_back(&peelLabelOp);

	// See createDieCutPrintHead() for explanation
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		genericSize.push_back(&takeLabelOp);
	}

	genericSize.push_back(&syncBarOp);
	genericSize.push_back(&disableCounterOp);
	genericSize.push_back(&jammedLabelTestOp0);
	genericSize.push_back(&checkStockOp);
	genericSize.push_back(&idleOp);
}

void PrinterTeachTable::createGenericFind()
{
	genericFind.push_back(&findLabel2000Op);
	genericFind.push_back(&checkStockOp);
	genericFind.push_back(&missingLabelTestOp);
	genericFind.push_back(&syncLabelOp);
	genericFind.push_back(&jammedLabelTestOp);
	genericFind.push_back(&checkStockOp);

	// See createDieCutPrintHead() for explanation
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		genericFind.push_back(&takeLabelOp);
	}

	genericFind.push_back(&idleOp);
} 

void PrinterTeachTable::createGenericFindwoLabelTaken()
{
	genericFindwoLabelTaken.push_back(&findLabel2000Op);
	genericFindwoLabelTaken.push_back(&checkStockOp);
	genericFindwoLabelTaken.push_back(&missingLabelTestOp);
	genericFindwoLabelTaken.push_back(&syncLabelOp);
	genericFindwoLabelTaken.push_back(&jammedLabelTestOp);
	genericFindwoLabelTaken.push_back(&checkStockOp);
	genericFindwoLabelTaken.push_back(&idleOp);
}

void PrinterTeachTable::createGenericPrintHead()
{
	genericPrintHead.push_back(&printLabelOp);
	genericPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createGenericEject()
{
	genericEject.push_back(&idleOp);
}

void PrinterTeachTable::createGenericSync()
{
	genericSync.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutSize()
{
	dieCutSize.push_back(&syncLabelOp);
	dieCutSize.push_back(&missingLabelTestOp);
	dieCutSize.push_back(&jammedLabelTestOp);
	dieCutSize.push_back(&setTakingOp);
	dieCutSize.push_back(&takeLabelOp);
	dieCutSize.push_back(&clearTakingOp);
	dieCutSize.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutPrintHead()
{
	dieCutPrintHead.push_back(&printLabelOp);
	dieCutPrintHead.push_back(&syncLabelOp);
	dieCutPrintHead.push_back(&missingLabelTestOp);
	dieCutPrintHead.push_back(&jammedLabelTestOp);
	dieCutPrintHead.push_back(&setTakingOp);

	// Label testing code. Prevents take label message when running the labels
	// out the front, onto the floor.
	// True when config (configc.dat or config.dat contains the following entry:
	// opModeStreamingLabelTest=F	# allows psuedo production mode for printing a continuous stream of labels
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutPrintHead.push_back(&takeLabelOp);
	}

	dieCutPrintHead.push_back(&clearTakingOp);
	dieCutPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutShootHead()
{
	dieCutShootPrintHead.push_back(&printLabelOp);
	dieCutShootPrintHead.push_back(&ejectShootOp);
	dieCutShootPrintHead.push_back(&missingLabelTestOp);
	dieCutShootPrintHead.push_back(&jammedLabelTestOp);
	dieCutShootPrintHead.push_back(&setTakingOp);

	// Label testing code. Prevents take label message when running the labels
	// out the front, onto the floor.
	// True when config (configc.dat or config.dat contains the following entry:
	// opModeStreamingLabelTest=F	# allows psuedo production mode for printing a continuous stream of labels
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutShootPrintHead.push_back(&takeLabelOp);
	}

	dieCutShootPrintHead.push_back(&clearTakingOp);
	dieCutShootPrintHead.push_back(&idleOp);

}

void PrinterTeachTable::createDieCutPrintHeel()
{
	dieCutPrintHeel.push_back(&reversePrintLabelOp);
	dieCutPrintHeel.push_back(&syncLabelOp);
	dieCutPrintHeel.push_back(&missingLabelTestOp);
	dieCutPrintHeel.push_back(&jammedLabelTestOp);
	dieCutPrintHeel.push_back(&setTakingOp);

	// See createDieCutPrintHead() for explanation
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutPrintHeel.push_back(&takeLabelOp);
	}

	dieCutPrintHeel.push_back(&clearTakingOp);
	dieCutPrintHeel.push_back(&idleOp);
}
void PrinterTeachTable::createDieCutShootHeel()
{
	dieCutShootPrintHeel.push_back(&reversePrintLabelOp);
	dieCutShootPrintHeel.push_back(&ejectShootOp);
	dieCutShootPrintHeel.push_back(&missingLabelTestOp);
	dieCutShootPrintHeel.push_back(&jammedLabelTestOp);
	dieCutShootPrintHeel.push_back(&setTakingOp);

	// See createDieCutPrintHead() for explanation
	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutShootPrintHeel.push_back(&takeLabelOp);
	}

	dieCutShootPrintHeel.push_back(&clearTakingOp);
	dieCutShootPrintHeel.push_back(&idleOp);
}
void PrinterTeachTable::createDieCutSync()
{
	dieCutSync.push_back(&syncLabelOp);
	dieCutSync.push_back(&jammedLabelTestOp);
	dieCutSync.push_back(&setTakingOp);
	dieCutSync.push_back(&clearTakingOp);

	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutSync.push_back(&takeLabelOp);
	}

	dieCutSync.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutEject()
{
	dieCutEject.push_back(&syncLabelOp);
	dieCutEject.push_back(&syncLabelOp);
	dieCutEject.push_back(&jammedLabelTestOp);
	dieCutEject.push_back(&setTakingOp);

	if( pConfig_ && !pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		dieCutEject.push_back(&takeLabelOp);
	}

	dieCutEject.push_back(&clearTakingOp);
	dieCutEject.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutStreamPrintHead()
{
	dieCutStreamPrintHead.push_back(&printLabelOp);
	dieCutStreamPrintHead.push_back(&syncLabelOp);
	dieCutStreamPrintHead.push_back(&jammedLabelTestOp);
	dieCutStreamPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutShootStreamPrintHead()
{
	dieCutShootStreamPrintHead.push_back(&printLabelOp);
	dieCutShootStreamPrintHead.push_back(&ejectShootOp);
	dieCutShootStreamPrintHead.push_back(&jammedLabelTestOp);
	dieCutShootStreamPrintHead.push_back(&waitShootLabelOp);
	dieCutShootStreamPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutStreamPrintHeel()
{
	dieCutStreamPrintHeel.push_back(&reversePrintLabelOp);
	dieCutStreamPrintHeel.push_back(&syncLabelOp);
	dieCutStreamPrintHeel.push_back(&jammedLabelTestOp);
	dieCutStreamPrintHeel.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutShootStreamPrintHeel()
{
	dieCutShootStreamPrintHeel.push_back(&reversePrintLabelOp);
	dieCutShootStreamPrintHeel.push_back(&ejectShootOp);
	dieCutShootStreamPrintHeel.push_back(&jammedLabelTestOp);
	dieCutShootStreamPrintHeel.push_back(&waitShootLabelOp);
	dieCutShootStreamPrintHeel.push_back(&idleOp);
}


void PrinterTeachTable::createDieCutStreamSync()
{
	dieCutStreamSync.push_back(&syncLabelOp);
	dieCutStreamSync.push_back(&jammedLabelTestOp);
	dieCutStreamSync.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutStreamEject()
{
	dieCutStreamEject.push_back(&ejectLabelOp);
	dieCutStreamEject.push_back(&idleOp);
}

void PrinterTeachTable::createDieCutStreamBackup()
{
	dieCutStreamBackup.push_back(&backupLabelOp);
	dieCutStreamBackup.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintHead()
{
	listingPrintHead.push_back(&startLabelOp);
	listingPrintHead.push_back(&printLabelOp);
	listingPrintHead.push_back(&ejectLabelOp);
	listingPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintHeadCut()
{
	listingPrintHeadCut.push_back(&startLabelOp);
	listingPrintHeadCut.push_back(&printLabelOp);
	listingPrintHeadCut.push_back(&ejectLabelOp);
	listingPrintHeadCut.push_back(&cutterEjectLabelOp);
	listingPrintHeadCut.push_back(&cutOp);
	listingPrintHeadCut.push_back(&retractLabelOp);
	listingPrintHeadCut.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintCombinedHead()
{
	listingPrintCombinedHead.push_back(&startLabelOp);
	listingPrintCombinedHead.push_back(&printLabelOp);
	listingPrintCombinedHead.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintCombinedHeadTake()
{
	listingPrintCombinedHeelTake.push_back(&setTakingOp);
	listingPrintCombinedHeelTake.push_back(&takeLabelOp);
	listingPrintCombinedHeelTake.push_back(&clearTakingOp);
	listingPrintCombinedHeelTake.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintHeel()
{
	listingPrintHeel.push_back(&startLabelOp);
	listingPrintHeel.push_back(&reversePrintLabelOp);
	listingPrintHeel.push_back(&ejectLabelOp);
	listingPrintHeel.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintHeelCut()
{
	listingPrintHeelCut.push_back(&startLabelOp);
	listingPrintHeelCut.push_back(&reversePrintLabelOp);
	listingPrintHeelCut.push_back(&ejectLabelOp);
	listingPrintHeelCut.push_back(&cutterEjectLabelOp);
	listingPrintHeelCut.push_back(&cutOp);
	listingPrintHeelCut.push_back(&retractLabelOp);
	listingPrintHeelCut.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintCombinedHeel()
{
	listingPrintCombinedHeel.push_back(&startLabelOp);
	listingPrintCombinedHeel.push_back(&reversePrintLabelOp);
	listingPrintCombinedHeel.push_back(&ejectLabelOp);
	listingPrintCombinedHeel.push_back(&idleOp);
}

void PrinterTeachTable::createListingPrintCombinedHeelTake()
{
	listingPrintCombinedHeelTake.push_back(&setTakingOp);
	listingPrintCombinedHeelTake.push_back(&takeLabelOp);
	listingPrintCombinedHeelTake.push_back(&clearTakingOp);
	listingPrintCombinedHeelTake.push_back(&idleOp);
}

void PrinterTeachTable::createListingSync()
{
	listingSync.push_back(&idleOp);
}

void PrinterTeachTable::createListingEject()
{
	listingEject.push_back(&ejectLabelOp);
	listingEject.push_back(&idleOp);
}


void PrinterTeachTable::createContinuousPrintHead()
{
	continuousPrintHead.push_back(&startLabelOp);
	continuousPrintHead.push_back(&printLabelOp);
	continuousPrintHead.push_back(&missingLabelTestOp);
	continuousPrintHead.push_back(&ejectLabelOp);
	continuousPrintHead.push_back(&setTakingOp);
	continuousPrintHead.push_back(&takeLabelOp);
	continuousPrintHead.push_back(&clearTakingOp);
	continuousPrintHead.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousPrintHeadCut()
{
	
	continuousPrintHeadCut.push_back(&startLabelOp);
	continuousPrintHeadCut.push_back(&printLabelOp);
	continuousPrintHeadCut.push_back(&cutterEjectLabelOp);

	continuousPrintHeadCut.push_back(&cutOp);

	continuousPrintHeadCut.push_back(&waitLabelOp);
	continuousPrintHeadCut.push_back(&waitLabelOp);

	continuousPrintHeadCut.push_back(&retractLabelOp);
	continuousPrintHeadCut.push_back(&advanceLabelOp);

	continuousPrintHeadCut.push_back(&missingLabelTestOp);
	continuousPrintHeadCut.push_back(&setTakingOp);
	continuousPrintHeadCut.push_back(&takeLabelOp);
	continuousPrintHeadCut.push_back(&clearTakingOp);
	continuousPrintHeadCut.push_back(&idleOp);
	
}

void PrinterTeachTable::createContinuousPrintCombinedHead()
{
	continuousPrintCombinedHead.push_back(&startLabelOp);
	continuousPrintCombinedHead.push_back(&printLabelOp);
	continuousPrintCombinedHead.push_back(&missingLabelTestOp);
	continuousPrintCombinedHead.push_back(&setTakingOp);
	continuousPrintCombinedHead.push_back(&takeLabelOp);
	continuousPrintCombinedHead.push_back(&clearTakingOp);
	continuousPrintCombinedHead.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousPrintHeel()
{
	continuousPrintHeel.push_back(&startLabelOp);
	continuousPrintHeel.push_back(&reversePrintLabelOp);
	continuousPrintHeel.push_back(&missingLabelTestOp);
	continuousPrintHeel.push_back(&ejectLabelOp);
	continuousPrintHeel.push_back(&setTakingOp);
	continuousPrintHeel.push_back(&takeLabelOp);
	continuousPrintHeel.push_back(&clearTakingOp);
	continuousPrintHeel.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousPrintHeelCut()
{
	continuousPrintHeelCut.push_back(&startLabelOp);
	continuousPrintHeelCut.push_back(&reversePrintLabelOp);
	continuousPrintHeelCut.push_back(&cutterEjectLabelOp);	

	continuousPrintHeelCut.push_back(&cutOp);

	/* knife stops ~100ms from when interrupt occurs */
	continuousPrintHeelCut.push_back(&waitLabelOp);
	continuousPrintHeelCut.push_back(&waitLabelOp);

	continuousPrintHeelCut.push_back(&retractLabelOp);
	continuousPrintHeelCut.push_back(&advanceLabelOp);

	continuousPrintHeelCut.push_back(&missingLabelTestOp);
	continuousPrintHeelCut.push_back(&setTakingOp);
	continuousPrintHeelCut.push_back(&takeLabelOp);
	continuousPrintHeelCut.push_back(&clearTakingOp);
	continuousPrintHeelCut.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousPrintCombinedHeel()
{
	continuousPrintCombinedHeel.push_back(&startLabelOp);
	continuousPrintCombinedHeel.push_back(&reversePrintLabelOp);
	continuousPrintCombinedHeel.push_back(&missingLabelTestOp);
	continuousPrintCombinedHeel.push_back(&setTakingOp);
	continuousPrintCombinedHeel.push_back(&takeLabelOp);
	continuousPrintCombinedHeel.push_back(&clearTakingOp);	
	continuousPrintCombinedHeel.push_back(&idleOp);	
}

void PrinterTeachTable::createContinuousShootPrintHeelCut()
{
	continuousShootPrintHeelCut.push_back(&startLabelOp);
	continuousShootPrintHeelCut.push_back(&reversePrintLabelOp);
	continuousShootPrintHeelCut.push_back(&cutterEjectShootLabelOp);
	continuousShootPrintHeelCut.push_back(&cutOp);
	continuousShootPrintHeelCut.push_back(&waitLabelOp);
	continuousShootPrintHeelCut.push_back(&waitLabelOp);
	continuousShootPrintHeelCut.push_back(&retractShootLabelOp);
	continuousShootPrintHeelCut.push_back(&missingLabelTestOp);
	continuousShootPrintHeelCut.push_back(&setTakingOp);
	continuousShootPrintHeelCut.push_back(&takeLabelOp);
	continuousShootPrintHeelCut.push_back(&clearTakingOp);	
	continuousShootPrintHeelCut.push_back(&idleOp);	
}

void PrinterTeachTable::createContinuousSync()
{
	continuousSync.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousEject()
{
	continuousEject.push_back(&ejectLabelOp);
	continuousEject.push_back(&idleOp);
}

void PrinterTeachTable::createContinuousSize()
{
	continuousSize.push_back(&idleOp);
}

void PrinterTeachTable::createCalibrateSize()
{
	calibrateSize.push_back(&idleOp);
}

void PrinterTeachTable::createCalibrateFind()
{
	calibrateFind.push_back(&setTakingOp);
	calibrateFind.push_back(&clearTakingOp);
	calibrateFind.push_back(&idleOp);
}

void PrinterTeachTable::createCalibratePrint()
{
	calibratePrint.push_back(&printLabelOp);
	calibratePrint.push_back(&idleOp);
}

void PrinterTeachTable::createCalibrateEject()
{
	calibrateEject.push_back(&idleOp);
}

void PrinterTeachTable::createCalibrateSync()
{
	calibrateSync.push_back(&idleOp);
}

void PrinterTeachTable::createSpeedKeyPrintMode()
{
	speedKeyPrintMode.push_back(&printLabelOp);
	speedKeyPrintMode.push_back(&idleOp);
}

// SOF-1593 - user needs to opt-in for saving continuous label stock - Needed to be able to recreate the teach tables, particulary for the size command.
void PrinterTeachTable::recreateTeachTable()
{
	bool isTracing = Debug::isTracing("PrinterManager.PrinterSize");

	if (isTracing)
	{
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - begin");
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - ops size " + inttostring(ops.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - command size " + inttostring(command.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - mode size " + inttostring(mode.size()));
	}

	ops.clear();
	command.clear();
	mode.clear();

	if (isTracing)
	{
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 2ops size " + inttostring(ops.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 2command size " + inttostring(command.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 2mode size " + inttostring(mode.size()));
	}

	createTeachTable();

	if (isTracing)
	{
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 3ops size " + inttostring(ops.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 3command size " + inttostring(command.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - 3mode size " + inttostring(mode.size()));
		Debug::trace("PrinterManager.PrinterSize", "recreateTeachTable() - end");
	}
}

void PrinterTeachTable::createTeachTable()
{
	// printerMode = UnsizedLabel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	UnsizedLabel);
	addTeachTableEntry(&genericPrintHead,		PrintCommand,	UnsizedLabel);
	addTeachTableEntry(&genericEject,			EjectCommand,	UnsizedLabel);
	addTeachTableEntry(&genericSync,			SyncCommand,	UnsizedLabel);
	addTeachTableEntry(&genericFind,			FindCommand,	UnsizedLabel);
	addTeachTableEntry(&genericSize,			SizeCommand,	UnsizedLabel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	UnsizedLabel);

	// printerMode = ContinuousLabelHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHead);
	addTeachTableEntry(&continuousPrintHead,	PrintCommand,	ContinuousLabelHead);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHead);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHead);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHead);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHead genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHead);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHead continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHead);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHead);

	// printerMode = ContinuousLabelHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHeel);
	addTeachTableEntry(&continuousPrintHeel,	PrintCommand,	ContinuousLabelHeel);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHeel);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHeel);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHeel);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHeel);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHeel);

	// printerMode = ContinuousLabelHeelCut
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHeelCut);
	addTeachTableEntry(&continuousPrintHeelCut,	PrintCommand,	ContinuousLabelHeelCut);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHeelCut);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHeelCut);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHeelCut);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHeelCut);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHeelCut);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHeelCut);


	// printerMode = ContinuousShootLabelHeelCut
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousShootLabelHeelCut);
	addTeachTableEntry(&continuousShootPrintHeelCut,	PrintCommand,	ContinuousShootLabelHeelCut);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousShootLabelHeelCut);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousShootLabelHeelCut);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousShootLabelHeelCut);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousShootLabelHeelCut);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousShootLabelHeelCut);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousShootLabelHeelCut);

	// printerMode = ContinuousLabelHeadCut
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHeadCut);
	addTeachTableEntry(&continuousPrintHeadCut,	PrintCommand,	ContinuousLabelHeadCut);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHeadCut);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHeadCut);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHeadCut);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHeadCut);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHeadCut);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHeadCut);

	// printerMode = ContinuousStreamHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	ContinuousStreamHead);
	addTeachTableEntry(&listingPrintHead,		PrintCommand,	ContinuousStreamHead);
	addTeachTableEntry(&listingEject,			EjectCommand,	ContinuousStreamHead);
	addTeachTableEntry(&listingSync,			SyncCommand,	ContinuousStreamHead);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousStreamHead);
	if ( !pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled() )
		addTeachTableEntry(&genericSize,			SizeCommand,	ContinuousStreamHead);
	else
		addTeachTableEntry(&continuousSize,			SizeCommand,	ContinuousStreamHead);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousStreamHead);

	// printerMode = ContinuousStreamHeadCut
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	ContinuousStreamHeadCut);
	addTeachTableEntry(&listingPrintHeadCut,		PrintCommand,	ContinuousStreamHeadCut);
	addTeachTableEntry(&listingEject,			EjectCommand,	ContinuousStreamHeadCut);
	addTeachTableEntry(&listingSync,			SyncCommand,	ContinuousStreamHeadCut);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousStreamHeadCut);
	if ( !pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled() )
		addTeachTableEntry(&genericSize,			SizeCommand,	ContinuousStreamHeadCut);
	else
		addTeachTableEntry(&continuousSize,			SizeCommand,	ContinuousStreamHeadCut);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousStreamHeadCut);
	
	// printerMode = ContinuousLabelHeadCombined
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHeadCombined);
	addTeachTableEntry(&continuousPrintCombinedHead,	PrintCommand,	ContinuousLabelHeadCombined);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHeadCombined);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHeadCombined);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHeadCombined);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeadCombined genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHeadCombined);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeadCombined continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHeadCombined);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHeadCombined);


	// printerMode = ContinuousLabelHeelCombined
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousLabelHeelCombined);
	addTeachTableEntry(&continuousPrintCombinedHeel,	PrintCommand,	ContinuousLabelHeelCombined);
	addTeachTableEntry(&continuousEject,		EjectCommand,	ContinuousLabelHeelCombined);
	addTeachTableEntry(&continuousSync,			SyncCommand,	ContinuousLabelHeelCombined);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousLabelHeelCombined);
	if (!pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled())
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeelCombined genericSize");
		addTeachTableEntry(&genericSize, SizeCommand, ContinuousLabelHeelCombined);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterSize", "createTeachTable() - ContinuousLabelHeel continuousSize");
		addTeachTableEntry(&continuousSize, SizeCommand, ContinuousLabelHeelCombined);
	}
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousLabelHeelCombined);


	// printerMode = ContinuousStreamHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousStreamHeel);
	addTeachTableEntry(&listingPrintHeel,	PrintCommand,	ContinuousStreamHeel);
	addTeachTableEntry(&listingEject,			EjectCommand,	ContinuousStreamHeel);
	addTeachTableEntry(&listingSync,			SyncCommand,	ContinuousStreamHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousStreamHeel);
	if ( !pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled() )
		addTeachTableEntry(&genericSize,			SizeCommand,	ContinuousStreamHeel);
	else
		addTeachTableEntry(&continuousSize,			SizeCommand,	ContinuousStreamHeel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousStreamHeel);

		 //printerMode = ContinuousStreamHeelCut
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, ContinuousStreamHeelCut);
	addTeachTableEntry(&listingPrintHeelCut,	PrintCommand,	ContinuousStreamHeelCut);
	addTeachTableEntry(&listingEject,			EjectCommand,	ContinuousStreamHeelCut);
	addTeachTableEntry(&listingSync,			SyncCommand,	ContinuousStreamHeelCut);
	addTeachTableEntry(&genericFind,			FindCommand,	ContinuousStreamHeelCut);
	if ( !pConfig_ || !pConfig_->isConserveContinuousStockByNotSizingEnabled() )
		addTeachTableEntry(&genericSize,			SizeCommand,	ContinuousStreamHeelCut);
	else
		addTeachTableEntry(&continuousSize,			SizeCommand,	ContinuousStreamHeelCut);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	ContinuousStreamHeelCut);

	// printerMode = DieCutLabelHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutLabelHead);
	addTeachTableEntry(&dieCutPrintHead,		PrintCommand,	DieCutLabelHead);
	addTeachTableEntry(&dieCutEject,			EjectCommand,	DieCutLabelHead);
	addTeachTableEntry(&dieCutSync,				SyncCommand,	DieCutLabelHead);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutLabelHead);
	//addTeachTableEntry(&dieCutSize,			SizeCommand,	DieCutLabelHead);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutLabelHead);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutLabelHead);

	// printerMode = DieCutShootHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutShootHead);
	addTeachTableEntry(&dieCutShootPrintHead,	PrintCommand,	DieCutShootHead);
	addTeachTableEntry(&dieCutEject,			EjectCommand,	DieCutShootHead);
	addTeachTableEntry(&dieCutSync,				SyncCommand,	DieCutShootHead);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutShootHead);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutShootHead);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutShootHead);

	// printerMode = DieCutLabelHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutLabelHeel);
	addTeachTableEntry(&dieCutPrintHeel,		PrintCommand,	DieCutLabelHeel);
	addTeachTableEntry(&dieCutSync,				SyncCommand,	DieCutLabelHeel);
	addTeachTableEntry(&dieCutEject,			EjectCommand,	DieCutLabelHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutLabelHeel);
	//addTeachTableEntry(&dieCutSize,			SizeCommand,	DieCutLabelHeel);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutLabelHeel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutLabelHeel);

	// printerMode = DieCutShootHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutShootHeel);
	addTeachTableEntry(&dieCutShootPrintHeel,	PrintCommand,	DieCutShootHeel);
	addTeachTableEntry(&dieCutSync,				SyncCommand,	DieCutShootHeel);
	addTeachTableEntry(&dieCutEject,			EjectCommand,	DieCutShootHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutShootHeel);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutShootHeel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutShootHeel);

	// printerMode = DieCutStreamHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, DieCutStreamHead);
	addTeachTableEntry(&dieCutStreamPrintHead,	PrintCommand,	DieCutStreamHead);
	addTeachTableEntry(&dieCutStreamEject,		EjectCommand,	DieCutStreamHead);
	addTeachTableEntry(&dieCutStreamSync,		SyncCommand,	DieCutStreamHead);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutStreamHead);
	//addTeachTableEntry(&dieCutSize,			SizeCommand,	DieCutStreamHead);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutStreamHead);
	addTeachTableEntry(&dieCutStreamBackup,		BackupCommand,	DieCutStreamHead);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutStreamHead);

	// printerMode = DieCutShootStreamHead
	addTeachTableEntry(&genericAdvance,			AdvanceCommand, DieCutShootStreamHead);
	addTeachTableEntry(&dieCutShootStreamPrintHead,	PrintCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&dieCutStreamEject,		EjectCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&dieCutStreamSync,		SyncCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutShootStreamHead);
	//addTeachTableEntry(&dieCutSize,			SizeCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&dieCutStreamBackup,		BackupCommand,	DieCutShootStreamHead);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutShootStreamHead);


	// printerMode = DieCutStreamHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutStreamHeel);
	addTeachTableEntry(&dieCutStreamPrintHeel,	PrintCommand,	DieCutStreamHeel);
	addTeachTableEntry(&dieCutStreamEject,		EjectCommand,	DieCutStreamHeel);
	addTeachTableEntry(&dieCutStreamSync,		SyncCommand,	DieCutStreamHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutStreamHeel);
	//addTeachTableEntry(&dieCutSize,			SizeCommand,	DieCutStreamHeel);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutStreamHeel);
	addTeachTableEntry(&dieCutStreamBackup,		BackupCommand,	DieCutStreamHeel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutStreamHeel);


	// printerMode = DieCutShootStreamHeel
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&dieCutShootStreamPrintHeel,	PrintCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&dieCutStreamEject,		EjectCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&dieCutStreamSync,		SyncCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&genericFind,			FindCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&genericSize,			SizeCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&dieCutStreamBackup,		BackupCommand,	DieCutShootStreamHeel);
	addTeachTableEntry(&genericFindwoLabelTaken,	FindwoLabelTakenCommand,	DieCutShootStreamHeel);


	// printerMode = PrinterCalibration
	addTeachTableEntry(&genericAdvance,			AdvanceCommand,	PrinterCalibration);
	addTeachTableEntry(&calibratePrint,			PrintCommand,	PrinterCalibration);
	addTeachTableEntry(&calibrateEject,			EjectCommand,	PrinterCalibration);
	addTeachTableEntry(&calibrateSync,			SyncCommand,	PrinterCalibration);
	addTeachTableEntry(&calibrateFind,			FindCommand,	PrinterCalibration);
	addTeachTableEntry(&calibrateSize,			SizeCommand,	PrinterCalibration);

	// printerMode = SpeedKeyPrint
	addTeachTableEntry(&speedKeyPrintMode,		PrintCommand,	SpeedKeyPrint);
}

void PrinterTeachTable::show(){
	int i = 0;
	int size = (int)ops.size();
	Debug::trace("PrinterTeachTable", "show():");
	while ((i < size))
	{
		Debug::trace("PrinterTeachTable", "Command: " + Integer::toString(this->command[i]));
		Debug::trace("PrinterTeachTable", "Mode: " + Integer::toString(this->mode[i]));
		Debug::trace("PrinterTeachTable", "Ops: ");
		ops[i]->show();
		i++;
	}
}
