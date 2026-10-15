#ifndef OPMODEEVENT_H
#define OPMODEEVENT_H

	#include "Event.h"

	//Operator Mode Generated Events - OperationMode
	typedef enum OpModeEvent
	{
		OpModeIdle,				             // 0 - initial operating mode
		OpModeOperate,
		OpModePluEntry,
		OpModeAutoWeigh,
		OpModeSemiWeigh,
		OpModeManualWeigh,	// 5            // Chose "Manual Operation" on Mode Dialog Counter Scale. ALSO selected "Normal" button while in Product Entry on Counter Scale 
		OpModeSupervisor,
		OpModeService,
		OpModeServiceLogin,
		OpModePowerDown,
		OpModeMonitor,			// 10
		OpModeWrapOnly,
		OpModeStatus,
		OpModeSelfService,                  // Chose "Self Service Operation" on Mode Dialog Counter Scale
		// OpModeAutoManualApply, when there is a label applier, will automatically print a label, but will not run the machine.
		OpModeAutoManualApply,              // Chose "Prepack Operation" on Mode Dialog Counter Scale
		OpModeProductionEntry,              // Chose "Production Entry" on Mode Dialog Counter Scale
		OpModePrintMultipleLabels,
		OpModeAccumulateItems,
		OpModeBuildAMeal,
		OpModeMeatSheets,   // Added; found it defined in Qt (ui code)
		OpModeMax,
		OpModeLabelsPrintedCounts
	}   OpModeEvent_t;

#endif // OPMODEEVENT_H
