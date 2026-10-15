#include <iostream>
using namespace std;

#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "CommandOperationList.h"
#include "CommandPrintOperation.h"
#include "CommandStepOperation.h"
#include "CommandStepUntilOperation.h"
#include "CommandWaitOperation.h"
#include "CommandWaitUntilOperation.h"
#include "CommandTestOperation.h"
#include "CommandStatusOperation.h"
#include "CommandCounterOperation.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CommandOperationList::dataType = 0;
std::string CommandOperationList::dataTypeName("CommandOperationList"); 
#endif

CommandOperationList::CommandOperationList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

CommandOperationList::~CommandOperationList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

UINT16 CommandOperationList::getNumEntries()
{
	return (UINT16)size();
}

UINT16 CommandOperationList::getCommandBlock(CommandOperationMsg *pCOM, bool inBigEndian)
{
	UINT16 i = 0;
	CommandOperationList::iterator iter = begin();
	CommandOperationMsg *operation = pCOM;

	while (iter != end())
	{
		CommandOperation *pCO = const_cast<CommandOperation *> (dynamic_cast<const CommandOperation *> (*iter));
		operation[i].generic.commandDirective = (UINT8)pCO->getCommandDirective();

		switch (operation[i].generic.commandDirective)
		{
			case 	IdleDirective		:	
											break;
			case 	PrintDirective		:	{
												CommandPrintOperation *pCPO = dynamic_cast<CommandPrintOperation *> (const_cast<CommandOperation *> (*iter));
												operation[i].print.type = (UINT8)pCPO->getCommandDataType(); 

												if (inBigEndian == true)
												{
													operation[i].print.data = (UINT16) htons((u_short)pCPO->getCommandData());
												}
												else
												{
													operation[i].print.data = (UINT16)pCPO->getCommandData();
												}

												operation[i].print.labelFeedDirection = (UINT8)pCPO->getLabelFeedDirection();
											}
											break;
			case 	StepDirective		:	{
												CommandStepOperation *pCSO = const_cast<CommandStepOperation *> (dynamic_cast<CommandStepOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].step.type = (UINT8)pCSO->getCommandDataType(); 

												if (inBigEndian == true)
												{
													operation[i].step.data = (UINT16) htons((u_short)pCSO->getCommandData());
												}
												else
												{
													operation[i].step.data = (UINT16)pCSO->getCommandData();
												}
											}
											break;
			case 	StepUntilDirective	:	{
												CommandStepUntilOperation *pCSUO = const_cast<CommandStepUntilOperation *> (dynamic_cast<CommandStepUntilOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].stepUntil.type = (UINT8)pCSUO->getCommandDataType(); 

												if (inBigEndian == true)
												{
													operation[i].stepUntil.data = (UINT16) htons((u_short)pCSUO->getCommandData());
												}
												else
												{
													operation[i].stepUntil.data = (UINT16)pCSUO->getCommandData();
												}

												operation[i].stepUntil.bitOperation = (UINT8)pCSUO->getBitComparator();
												operation[i].stepUntil.bits = pCSUO->getBits();
												operation[i].stepUntil.result = pCSUO->getResult();
											}
											break;
			case 	WaitDirective		:	{
												CommandWaitOperation *pCWO = const_cast<CommandWaitOperation *> (dynamic_cast<CommandWaitOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].wait.type = (UINT8)pCWO->getCommandDataType(); 

												if (inBigEndian == true)
												{
													operation[i].wait.data = (UINT16) htons((u_short)pCWO->getCommandData());
												}
												else
												{
													operation[i].wait.data = (UINT16)pCWO->getCommandData();
												}
											}
											break;
			case 	WaitUntilDirective	:	{
												CommandWaitUntilOperation *pCWUO = const_cast<CommandWaitUntilOperation *> (dynamic_cast<CommandWaitUntilOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].waitUntil.bitOperation = (UINT8)pCWUO->getBitComparator(); 
												operation[i].waitUntil.bits = pCWUO->getBits();
												operation[i].waitUntil.result = pCWUO->getResult();
											}
											break;
			case 	TestDirective		:	{
												CommandTestOperation *pCTO = const_cast<CommandTestOperation *> (dynamic_cast<CommandTestOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].test.bitOperation = (UINT8)pCTO->getBitComparator(); 
												operation[i].test.bits = pCTO->getBits();
												operation[i].test.result = pCTO->getResult();
												operation[i].test.trueBits = pCTO->getTrueBits();
												operation[i].test.falseBits = pCTO->getFalseBits();
												operation[i].test.trueAction = (UINT8)pCTO->getTrueAction();
												operation[i].test.falseAction = (UINT8)pCTO->getFalseAction();
											}
											break;
			case 	StatusDirective		:	{
												CommandStatusOperation *pCSO = const_cast<CommandStatusOperation *> (dynamic_cast<CommandStatusOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].status.bitOperator = (UINT8)pCSO->getBitOperation(); 
												operation[i].status.bits = pCSO->getBits();
											}
											break;
			case 	MaskDirective		:
											break;
			case 	CounterDirective	:	{
												CommandCounterOperation *pCCO = const_cast<CommandCounterOperation *> (dynamic_cast<CommandCounterOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].counter.counterOperator = (UINT8)pCCO->getCounterOperation();
											}
											break;
			case 	DisableDirective	:
											break;
			case    CutDirective		:	{
												CommandOperation *pGCO = const_cast<CommandOperation *> (dynamic_cast<CommandOperation *> (const_cast<CommandOperation *> (*iter)));
												operation[i].generic.commandDirective = CutDirective;
											}
											break;
		}

		i++;
		iter++;
	}
	
	return (UINT16)size();
}

void CommandOperationList::show()
{
	Debug::trace("CommandOperationList", "CommandOperationList size: " + Integer::toString( size()));

	CommandOperationList::iterator i = begin();

	while (i != end())
	{
		(*i)->show();
		i++;
	}
}
