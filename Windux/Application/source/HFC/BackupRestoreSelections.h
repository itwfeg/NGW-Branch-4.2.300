/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTORESELECTIONS_H
#define BACKUPRESTORESELECTIONS_H

#include "evolution.h"
#include "HfcObject.h"
#include "BackupRestore.h"

typedef enum BRSelectionFieldOffsets
{
	fieldOffsetBRFileName = 1,
	fieldOffsetBRMode,
	fieldOffsetBRFormat,
	fieldOffsetBRPluData,
	fieldOffsetBRResources,
	fieldOffsetBROperatorData,
	fieldOffsetBRStoreData,
	fieldOffsetBRScaleConfig,
	fieldOffsetBRWrapperSettings,
	fieldOffsetBRSoftkeyData,
	fieldOffsetBRCOOLData,
	fieldOffsetBRSelfServeData,
	fieldOffsetBRSynchronizeConfig,
	fieldOffsetBRaltFileName,
	fieldOffsetBRScaleStatus,
	fieldOffsetBRScaleStatistics,
	fieldOffsetBRPrinterSettings,
	endBRSelectionFieldOffsets
} BRSelectionFieldOffsets_t;

class BackupRestoreSelections : public HfcObject
{
	public:

		BackupRestoreSelections();
		BackupRestoreSelections(const char *data, int len, int formatType);

		~BackupRestoreSelections();

		HobString getFileName() const;
		void setFileName(HobString fileName);

		HobString getAltFileName() const;
		void setAltFileName(HobString altFileName);
		
		BRMode getMode() const;
		void setMode(BRMode mode);

		BRFormat getFormat() const;
		void setFormat(BRFormat format);

		bool isPluDataSelected() const;
		void setPluDataSelected(bool br);

		bool isResourcesSelected() const;
		void setResourcesSelected(bool br);

		bool isOperatorDataSelected() const;
		void setOperatorDataSelected(bool br);

		bool isStoreDataSelected() const;
		void setStoreDataSelected(bool br);

		bool isScaleConfigSelected() const;
		void setScaleConfigSelected(bool br);

		bool isWrapperSettingsSelected() const;
		void setWrapperSettingsSelected(bool br);

		bool isSoftkeyDataSelected() const;
		void setSoftkeyDataSelected(bool br);

		bool isCOOLDataSelected() const;
		void setCOOLDataSelected(bool br);

		bool isSelfServeDataSelected() const;
		void setSelfServeDataSelected(bool br);

		bool isSynchronizeConfigSelected() const;
		void setSynchronizeConfigSelected(bool br);

		bool isEveryDataTypeSelected() const;
		void setEveryDataTypeSelected(bool br);

		bool isScaleStatusSelected() const;
		void setScaleStatusSelected(bool br);

		bool isScaleStatisticsSelected() const;
		void setScaleStatisticsSelected(bool br);

		bool isDisplayStats() const;
		void setDisplayStats(bool ds);

		bool isPrinterSettingsSelected() const;
		void setPrinterSettingsSelected(bool br);

		BackupRestoreSelections& operator = (const BackupRestoreSelections& brs);

		virtual const char *serialize(int format = BrowserFormat) const;


	private:

		// Full path name. Up until the remote scale management feature was added this file was the only one used.
		// It was either the input or output file or in the case of a read both. The Backup and restore fnctions
		//always use this filename.
		HobString fileName;

		// Full path name. With the addition of the remote scale management feature we needed to have a input file, in the pending
		// directory and in the case of a read a output file in the outbox directory. This filename will represent the processing
		// filename in the case of reads as the output filename will have to be in the fileName field and the processing file will have to
		// be deleted when processing complete.
		HobString altFileName;

		/**
		 * BRMode defined in BackupRestoreManager.h
		 * overwrite, merge
		 */
		BRMode mode;

		/**
		 * BRFormat defined in BackupRestoreManager.h
		 * evolution format, Ultima Disk format
		 */
		BRFormat format;

		/**
		 * flags indicating which group(s) of data should be backed up/restored.
		 */

		// Process Products, Classes, Expanded Text, Special Messages, Nutri facts, Product Notes, Transaction Type Names
		bool brPluData;

		// Process Graphics, Fonts, Label Types, Label Text Names
		bool brResources;

		// Process Operators, Operator Notes
		bool brOperatorData;

		// Process Store Name, Store Number
		bool brStoreData;

		// Process Configuration (User), IP Addresses (?)
		bool brScaleConfig;

		// Process Wrapper Configuration
		bool brWrapperSettings;

		// Process Softkey Styles, Softkey Features, Softkeys
		bool brSoftkeyData;

		// Process COOLCategory, COOLCountry, COOLProductionStep, COOLPretext, COOLText, COOLShortlist
		bool brCOOLData;

		// Process Categories, PresentationMethods, Specials, StreamingGraphics, ProductStatistics, CategoryGraphics, ProductGraphics, SpecialsGraphics, StreamingGraphicsGraphics
		bool brSelfServeData;

		// Process Primary Secondary Scale Synchronization Configuration
		bool brSynchronizeConfigData;

		// Process Scale Status
		bool brScaleStatusData;

		// Process Scale Statistics
		bool brScaleStatisticsData;

		// Used for restoring the db-independent version data
		bool everyDataTypeSelected;

		// Process Printer Configuration
		bool brPrinterSettings;

		// Should backup/restore progress messages be sent to the UI for displaying for the user?
		bool displayStats;


		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //BACKUPRESTORESELECTIONS_H
