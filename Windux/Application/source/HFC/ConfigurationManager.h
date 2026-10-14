/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "CEvolMsgReceiver.h"
#include "PrinterInfo.h"
#include "Wrapper.h"

#include "ConfigurationEvent.h"

class SessionMessage;
class DBSession;
class WriteSingleField;

class WeigherInfo;
class HfcWeigherConfig;
class HfcLabelApplierConfig;
class HfcPrinterConfig;

class HfcWrapperConfig;
class HfcWrapperControl;
class HfcWrapperErrorTotals;

class HfcEvolWrapperConfig;
class HfcEvolWrapperControl;

class CfgSynchronization;
#include "CfgAll.h"

namespace hfc
{
	class HFCManagerContainer;
}


class ConfigurationManager : public DataManager
{
	public:

		ConfigurationManager();
		ConfigurationManager(hfc::HFCManagerContainer *container);
		virtual ~ConfigurationManager();

		void init();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		CfgAll* getFirstRecord();
		EvolDeviceType getDevice() const { return ConfigurationDev; }
		UINT8 getDeleteAllEvent() const { return InvalidEvent; }


		WeigherInfo *getCurrentWeigherInfo() const;
		void setCurrentWeigherInfo(WeigherInfo *weigherInfo);

		HfcWeigherConfig *getCurrentWeigherConfig() const;
		void setCurrentWeigherConfig(HfcWeigherConfig *weigherConfig);

		HfcLabelApplierConfig *getCurrentLabelApplierConfig() const;
		void setCurrentLabelApplierConfig(HfcLabelApplierConfig *labelApplierConfig);

		HfcPrinterConfig *getCurrentPrinterConfig(PrinterType printerType) const;
		void setCurrentPrinterConfig(PrinterType printerType, HfcPrinterConfig *printerConfig);

		PrinterInfo *getCurrentPrinterInfo(PrinterType printerType) const;
		void setCurrentPrinterInfo(PrinterInfo *printerConfig, PrinterType printerType);

		HfcWrapperConfig *getCurrentWrapperConfig() const;
		void setCurrentWrapperConfig(HfcWrapperConfig *wrapperConfig);

		HfcWrapperControl *getCurrentWrapperControl() const;
		void setCurrentWrapperControl(HfcWrapperControl *wrapperControl);

		HfcEvolWrapperConfig *getCurrentEvolWrapperConfig() const;
		void setCurrentEvolWrapperConfig(HfcEvolWrapperConfig *wrapperConfig);

		HfcEvolWrapperControl *getCurrentEvolWrapperControl() const;
		void setCurrentEvolWrapperControl(HfcEvolWrapperControl *wrapperControl);


		void handleMessage(SessionMessage *msg);

		// Method to handle automatically sending all config messages to the Browser.
		// They are individual messages, but they all get sent together.
		void handleReadAllConfigMsgs();

		void handleCfgReadSingleFieldMessage(WriteSingleField *configUpdate);
		void handleCfgWriteSingleFieldMessage(WriteSingleField *configUpdate);
		void handleCfgSaveSingleFieldMessage(WriteSingleField *configUpdate);

		// Dispatch a message to all Sessions to let them know that the configuration has been saved.
		// When they receive it, they will resend any configuration messages to the UI.
		// They don't need to reload Configuration, since it is a singleton, and all tasks have
		// access to the same Configuration object.
		void dispatchConfigurationSaved();
		void dispatchConfigurationSavedNow();

		void enterSeviceMode();
		void prepareToLeaveServiceMode();
		void leaveServiceMode();

		void handleDeleteAllFiles(UINT8 event);

		void processChangeSynchronizationType(bool syncTypeChanged, bool primaryScaleChanged,
			ScaleSynchronizationType newSyncType, std::string newPrimaryScale, bool bootingUp = false);
		bool isSynchronizationTypeChanging(CfgSynchronization *cfg, bool *syncTypeChanged,
			bool *primaryScaleChanged, std::string &newPrimaryScale);

		bool hasPendingSynchronizationChange();
		void applyPendingSynchronizationChange(bool bootingUp = false);

		void updateHookWeigherPrecision( bool dualRange = false ) const;

	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		WeigherInfo					*currentWeigherInfo;
		HfcWeigherConfig			*currentWeigherConfig;
		HfcLabelApplierConfig		*currentLabelApplierConfig;

		std::vector<HfcPrinterConfig *> currentPrinterConfig;
//		HfcPrinterConfig			*currentPrinterConfig;

		HfcWrapperConfig			*currentWrapperConfig;
		HfcWrapperControl			*currentWrapperControl;

		HfcEvolWrapperConfig		*currentEvolWrapperConfig;
		HfcEvolWrapperControl		*currentEvolWrapperControl;

		std::vector<PrinterInfo*> currentPrinterInfo;

		HfcWeigherConfig			*initialServiceWeigherConfig;
		bool enteringServiceMode;
		bool leavingServiceMode;
		bool savedWeigherConfig;
		bool commSessionSavedConfig;

		CfgSynchronization *pendingSynchronizationChange;

		std::string hookWeigherPrecision;
		std::string hookWeigherPrecisionDR;

 		void handleWeigherConfig(SessionMessage *msg);
		void handleWeigherInquire(SessionMessage *msg);
		void handleWeigherRequestInquire(SessionMessage *msg);
		void handleWeigherRequestInfo(SessionMessage *msg);
		void handleWeigherValueMax(SessionMessage *msg);
		void handleWeigherRequestValueMax(SessionMessage *msg);
		void handleWeigherInfo(SessionMessage *msg);
		void handleWeigherSaveInfo(SessionMessage *msg);
		void handleWeigherDoneInfo(SessionMessage *msg);
		void handleWeigherAttach(SessionMessage *msg);

		// SOF-1667
		void handleWeigherResetFactoryDefaults(SessionMessage *msg);
		void handleWeigherResetFactoryDefaultsComplete(SessionMessage *msg);


		void handleLabelApplierConfig(SessionMessage *msg);
		void handleLabelApplierInquire(SessionMessage *msg);
		void handleLabelApplierRequestInquire(SessionMessage *msg);

 		void handlePrinterConfig(SessionMessage *msg);
		void handlePrinterInquire(SessionMessage *msg);
		void handlePrinterRequestInquire(SessionMessage *msg);
		void handlePrinterRequestInfo(SessionMessage *msg);
		void handlePrinterInfo(SessionMessage *msg);
		void handlePrinterSaveInfo(SessionMessage *msg);
//		DeviceAddress getStationAddress(short printerNum);
//		void setStationAddress(short printerNum, DeviceAddress stationAddress);
//		bool getDidPrinterStationRespond(short printerNum);
//		bool didAllPrinterStationsRespond();

		void handleWrapperInquire(SessionMessage *msg);
		void handleWrapperConfig(SessionMessage *msg);
		void saveConfigToWrapper(WrConfigType configType);
		void handleWrapperResetErrorTotals();
		void handleWrapperResetPackageTotals();
		void handleWrapperWritePackCode(SessionMessage *msg);
		void handleWrapperSavePackCode(SessionMessage *msg);
		void handleWrapperResetConfig();

		void handleWrapperSettings(SessionMessage *msg);
		void handleWrapperStatus(SessionMessage *msg);
		void handleWrapperControl(SessionMessage *msg);

		void handleEvolWrapperSettings(SessionMessage *msg);
		void handleProductReloadWrapperControls(SessionMessage *msg);
		void handleReloadEvolWrapperControls(SessionMessage *msg);
		void handleReloadUltimaWrapperControls(SessionMessage *msg);
		
		void handleConfigurationDevMsg(SessionMessage *msg);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//CONFIGURATIONMANAGER_H
