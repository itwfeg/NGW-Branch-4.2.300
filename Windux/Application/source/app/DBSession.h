/* LX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DBSESSION_H
#define DBSESSION_H

#include "evolution.h"
#include "AbstractSession.h"
#include "DBAccess.h"
#include "HFCManagerContainer.h"

class HobList;

class ProductManager;
class ClassManager;
class DepartmentManager;
class ExpirationLabelManager;
class NutrifactsManager;
class ProductTextManager;
class LabelTypeManager;
class GraphicManager;
class CategoryManager;
class SpecialsManager;
class COOLManager;

namespace hfc
{
	class ClassTotalManager;
	class COOLTotalManager;
	class HourlyTotalManager;
	class OperatorChangeManager;
	class OperatorTotalManager;
	class ProductTotalManager;
	class TransactionManager;
	class TransactionTypeManager;
}
class ProductDefaultManager;

class SoftkeyManager;
class SoftkeyFeatureManager;
class SoftkeyGroupManager;
class SoftkeyStyleManager;

class OperatorManager;
class OperatorNoteManager;
class StreamingGraphicManager;
class GenericLabelTextManager;
class ConfigurationManager;
class EmailAlertsManager;
class PeripheralManager;
class FlashFileManager;

class LabelGraphicManager;
class CategoryGraphicManager;
class ProductGraphicManager;
class SoftkeyGraphicManager;
class SpecialsGraphicManager;
class PlanogramGraphicManager;

class EvolWrapperKnifeHistoryManager;
class EvolWrapperErrorHistoryManager;
class CutTestManager;
class CutTestTotalManager;

class RecallManager;

class DeviceManager;
class ProductTextCfgManager;
class BarcodeScannerManager;

class DeptManager;

/**
 * Because we don't know what type of session (primary, secondary, remote, ...)
 * should be created at the instant a Session is instantiated, all functionality that is
 * specific to any particular session type will check the sessionType property before
 * allowing the particular functionality.
 * @stereotype threaded
 */
class DBSession : public AbstractSession, public hfc::HFCManagerContainer
{
	public:

		DBSession(CommMechanism *uiClient = 0, const char* name = "DBSession");
		virtual ~DBSession();

		/* These methods display/remove dialog messages to the screen.  They are declared in the DBSession class,
		 * however, the implementation for them is provided by a Session, since it is the class with the
		 * ResponseMessageManager; thus, they are overridden in Session.
		 */
		virtual bool addDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK, const HobList *msgData = 0, bool allowDuplicates = false);
		virtual bool removeDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK);

		bool isBatching() const;
		int getBatchId() const;

		// virtual from HfcManagerContainer
		void dispatchMessage(EvolMessage *message);


		//PTR #747 -> Set that we received softkey COMM changes, need to reload softkeys		
		void setCommSessionSoftkeyReload() { CommSessionSoftkeyReload = true; }
		void setCommSessionOperatorReload() { CommSessionOperatorReload = true; }		

		//PTR #747 -> Broadcast out to any session that we need to reload flashkeys		
		void ProcessReloadSoftkeyUpdates();		
		void ProcessReloadOperatorUpdates();


	protected:

		/**
		 * The following defines the data managers the Session needs to handle
		 * the messages the Session will receive.
		 */
		virtual void initializationBeforeRun();  // override from AbstractSession
		virtual void finalizationAfterRun();  // override from AbstractSession
		virtual void handleResultOfRTC(UINT32 result);
		virtual void quitGracefully();  // override from AbstractSession

		ProductManager                * productManager;
		ClassManager                  * classManager;
		DepartmentManager             * departmentManager;
		ExpirationLabelManager        * expirationLabelManager;
		NutrifactsManager             * nutrifactsManager;
		ProductTextManager            * expandedTextManager;
		ProductTextManager            * specialMessageManager;
		ProductTextManager            * productNoteManager;
		ProductTextManager            * marqueeTextManager;
		ProductTextManager            * productText5Manager;
		ProductTextManager            * productText6Manager;
		ProductTextManager            * productText7Manager;
		ProductTextManager            * productText8Manager;
		ProductTextManager            * productText9Manager;
		LabelTypeManager              * labelTypeManager;
		GraphicManager                * graphicManager;
		CategoryManager               * categoryManager;
		SpecialsManager               * specialsManager;
		COOLManager	                  * coolManager;
		hfc::COOLTotalManager         * coolTotalManager;
		hfc::HourlyTotalManager       * hourlyTotalManager;
		hfc::OperatorTotalManager     * operatorTotalManager;
		hfc::ClassTotalManager        * classTotalManager;
		hfc::ProductTotalManager      * productTotalManager;
		hfc::TransactionTypeManager   * transactionTypeManager;
		hfc::TransactionManager       * transactionManager;
		ProductDefaultManager         * productDefaultManager;

		SoftkeyManager                * softkeyManager;
		SoftkeyFeatureManager         * softkeyFeatureManager;
		SoftkeyGroupManager           * softkeyGroupManager;
		SoftkeyStyleManager           * softkeyStyleManager;

		OperatorManager               * operatorManager;
		hfc::OperatorChangeManager    * operatorChangeManager;
		OperatorNoteManager           * operatorNoteManager;
		StreamingGraphicManager       * streamingGraphicManager;
		GenericLabelTextManager       * genericLabelTextManager;
		ConfigurationManager          * configurationManager;
		EmailAlertsManager            * emailAlertsManager;
		PeripheralManager             * peripheralManager;
		FlashFileManager              * flashFileManager;

		LabelGraphicManager           * labelGraphicManager;
		CategoryGraphicManager        * categoryGraphicManager;
		ProductGraphicManager         * productGraphicManager;
		SoftkeyGraphicManager         * softkeyGraphicManager;
		SpecialsGraphicManager        * specialsGraphicManager;
		PlanogramGraphicManager       * planogramGraphicManager;

		EvolWrapperKnifeHistoryManager *evolWrapperKnifeHistoryManager;
		EvolWrapperErrorHistoryManager *evolWrapperErrorHistoryManager;

		CutTestManager				  *cutTestManager;
		CutTestTotalManager			  *cutTestTotalManager;

		RecallManager				*recallManager;

		DeviceManager				*deviceManager;
		ProductTextCfgManager		*productTextCfgManager;
		BarcodeScannerManager       *barcodeScannerManager;

		DeptManager					*deptManager;


		int currentBatchId;
		void resetBatchId();

		//PTR #747 -> Set that we received softkey COMM changes, need to reload softkeys				
		bool CommSessionSoftkeyReload;
		bool CommSessionOperatorReload;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //DBSESSION_H
