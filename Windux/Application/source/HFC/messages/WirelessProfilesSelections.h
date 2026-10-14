/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WIRELESSPROFILESSELECTIONS_H
#define WIRELESSPROFILESSELECTIONS_H

#include "evolution.h"
#include "HfcObject.h"
#include "WindowsConfigurationManager.h"

typedef enum WPSelectionFieldOffsets
{
	fieldOffsetWPDirection = 1,
	fieldOffsetWPPath,
	endWPSelectionFieldOffsets
} WPSelectionFieldOffsets_t;

class WirelessProfilesSelections : public HfcObject
{
	public:

		WirelessProfilesSelections();
		WirelessProfilesSelections(const char *data, int len, int formatType);

		~WirelessProfilesSelections();

		HobString getFileName() const;
		void setFileName(HobString fileName);

		WPDirection getDirection() const;
		void setDirection(WPDirection dir);

		WirelessProfilesSelections& operator = (const WirelessProfilesSelections& wps);

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

		void init();

		// Full path name.
		HobString fileName;

		WPDirection direction;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //WIRELESSPROFILESSELECTIONS_H
