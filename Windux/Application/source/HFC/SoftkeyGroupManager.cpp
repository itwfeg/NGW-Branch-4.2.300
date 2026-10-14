/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyGroupManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcError.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Softkey.h"
#include "SoftkeyBasics.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductsRM.h"
#include "DBCategoriesRM.h"

#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"

#include "ProductManager.h"
#include "OperatorManager.h"
#include "Category.h"
#include "Special.h"
#include "WeigherRequestStatus.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyGroupManager::dataType = 0;
std::string SoftkeyGroupManager::dataTypeName("SoftkeyGroupManager"); 
#endif


SoftkeyGroupManager::SoftkeyGroupManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SoftkeyGroupManager::SoftkeyGroupManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

SoftkeyGroupManager::~SoftkeyGroupManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	/* This is now done only in the SoftkeyManager
	if (restoreTrackingFlag && mySession)
	{
		//	Force a Config message to all Sessions (no super login required).
		SessionMessage msg(SoftkeyDev, LoadAllSoftkeyGroups);
		msg.setFormatType(InternalFormat);
		mySession->send(&msg, Messenger::DEST_RTC);
	} */

	if (currentSoftkeyGroup)
		delete currentSoftkeyGroup;

	if (currentGridData)
		delete currentGridData;

	destroyQueriesAndIters();
}


void SoftkeyGroupManager::init()
{
	mySession = 0;

	currentSoftkeyGroup = 0;
	allSoftkeyGroupsQuery = 0;
	allSoftkeyGroupsIter  = 0;

	currentGridData = 0;

//	restoreTrackingFlag = false;
}


DBSession *SoftkeyGroupManager::getMySession()
{
	return mySession;
}
void SoftkeyGroupManager::setMySession(DBSession *session)
{
	mySession = session;
}


void SoftkeyGroupManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();

	/* This is now only done in the SoftkeyManager
	if (mySession && mySession->isCommSession() && event == RestoreSoftkeyGroup)
		restoreTrackingFlag = true; */

	SoftkeyGroup *msgSoftkeyGroup = dynamic_cast<SoftkeyGroup *> (msg->getHfcObject());

	bool traceOn = Debug::isTracing("SoftkeyGroupManager");
	if (traceOn)
	{
		HobString myDebug = "handleMessage() - SoftkeyGroup event: ";
		myDebug.append(Integer::toString(event));
		Debug::trace("SoftkeyGroupManager", myDebug);
	}

	switch (event)
	{
		case LoadSoftkeyGroup:
		case LoadSoftkeyGroupForEditor:
			handleLoadSoftkeyGroup(msgSoftkeyGroup, event);
			break;

		case ReadSoftkeyGroup:
			handleRead(msgSoftkeyGroup);
			break;

		case ReadAllSoftkeyGroups:
			handleReadAll();
			break;

		case WriteSoftkeyGroup:
			handleWrite(msgSoftkeyGroup);
			break;

		case DeleteSoftkeyGroup:
		case BatchApplyDeleteSoftkeyGroup:
			handleDelete(msg);
			break;

		case DeleteAllSoftkeyGroups:
			handleDeleteAll();
			break;

		case SaveSoftkeyGroup:
			handleUpdateAndSave(msgSoftkeyGroup);
			break;

		case RestoreSoftkeyGroup:
			handleRestore(msg);
			break;

		case CreateNewSoftkeyGroup:
			handleCreate(msgSoftkeyGroup);
			break;

		default:
			if (traceOn)
			{
				HobString myDebug = "handleMessage():  UNKNOWN SoftkeyGroup EVENT: ";
				myDebug.append(Integer::toString(event));
				Debug::trace("SoftkeyGroupManager", myDebug);
			}
			break;
	}
}



void SoftkeyGroupManager::handleLoadAllSoftkeyGroups()
{
	if (!dbAccess)
		return;

	bool traceOn = Debug::isTracing("SoftkeyGroupManager");

	SoftkeyGroup *skGroup = new SoftkeyGroup(dbAccess);
	if (skGroup)
	{
		if (traceOn)
			Debug::trace("SoftkeyGroupManager", "Loading Softkey Group 999001");

		skGroup->setFeatureNumber((HobRecordNum)Reserved_ProductGrid);
		handleLoadSoftkeyGroup(skGroup);
		delete skGroup;
	}

	skGroup = new SoftkeyGroup(dbAccess);
	if (skGroup)
	{
		if (traceOn)
			Debug::trace("SoftkeyGroupManager", "Loading Softkey Group 999002");

		skGroup->setFeatureNumber((HobRecordNum)Reserved_StartScreenButtons);
		handleLoadSoftkeyGroup(skGroup);
		delete skGroup;
	}

	skGroup = new SoftkeyGroup(dbAccess);
	if (skGroup)
	{
		if (traceOn)
			Debug::trace("SoftkeyGroupManager", "Loading Softkey Group 999003");

		skGroup->setFeatureNumber((HobRecordNum)Reserved_RunScreenButtons);
		handleLoadSoftkeyGroup(skGroup);
		delete skGroup;
	}

}

void SoftkeyGroupManager::handleLoadSoftkeyGroup(SoftkeyGroup *msgSG, int event)
{
	bool traceOn = Debug::isTracing("SoftkeyGroupManager");
	SoftkeyFeature *feature = (SoftkeyFeature *) NULL;
	SoftkeyStyle *style = (SoftkeyStyle *) NULL;
	bool isSelfServMode = Configuration::getInstance()->isSelfServiceScale();
	bool isGen2UI = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe);
	HobString myDebug = "handleLoadSoftkeyGroup():   ";
	bool doContinue = true;

	if (!mySession || !msgSG || !dbAccess)
	{
		Debug::trace("SoftkeyGroupManager", myDebug + "mySession, msgSG and/or dbAccess is NULL!");
		return;
	}

	Session *s = dynamic_cast<Session *> (mySession);
	short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;


	HobRecordNum featureNum = msgSG->getFeatureNumber();

	// For Feature Reserved_FixedRowColGrid, we always use the grid dimensions specified by the Softkey Group with the
	// Fixed Softkey Feature's default group number, which overrides any groupNumber passed in with the msgSG parameter!
	HobRecordNum groupNum = msgSG->getGroupNumber();

	SoftkeyGroup *mySoftkeyGroup = 0;

	FixedRowColMethod fixedRowColMethod = FRCM_None;

	int defaultButtonToExecute = 0;
	if ( ( featureNum == Reserved_FixedRowColGrid ) || ( featureNum == Reserved_FreshServeButtons ) )
	{
		// have to get the current deptNum bc the msg doesnt contain it
		HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);

		SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
		if (!feature)
			return;  // Don't need to continue if the feature isn't in the database.
		else
		{
			if (isSelfServMode && isGen2UI)
			{
				//fixedRowColMethod = FRCM_FirstColumn;
				fixedRowColMethod = FRCM_None;
			}
			else
				fixedRowColMethod = feature->getFixedRowColMethod();

			defaultButtonToExecute = feature->getDefaultButton();

			  // stored in the db 1-based, but to execute the button it needs to be 0-based.
			delete feature;
			feature = 0;
		}
	}

	if ( fixedRowColMethod == FRCM_None )
	{
		mySoftkeyGroup = prepareToLoadSoftkeyGroup(featureNum, groupNum, langNum, dbAccess);
		loadSoftkeyGroup(mySoftkeyGroup, dbAccess);
	}
	else
	{
		// This branch is for when the UI is loading the Feature for display in Operate Mode.
		// In this case, the UI asks for a Feature with the groupNum being unspecified (RECORD_NOT_FOUND).
		// So if the UI is asking to load the Reserved_FixedRowColGrid Feature, and groupNum RECORD_NOT_FOUND, and one of the rows/columns is fixed, then
		// we need to load the fixed row/col, and then execute the default Softkey.

		SoftkeyGroup *fixedSoftkeyGroup = loadFixedRowColSoftkeyGroup(RECORD_NOT_FOUND, langNum, dbAccess);
		if (fixedSoftkeyGroup)
		{
			if (groupNum == RECORD_NOT_FOUND)
			{
				// Now determine which is the default button to execute, and execute it
				Softkey *softkeyButtonToExecute = fixedSoftkeyGroup->getSoftkey(defaultButtonToExecute);
				int action = Action_NoAction;
				if (softkeyButtonToExecute)
					action = softkeyButtonToExecute->getAction();
				if (softkeyButtonToExecute && (action == Action_LoadSoftkeyGroup || action == Action_LoadCategory_First || action == Action_LoadSpecials_First || action == Action_LoadMostCommon_First))
				{
					SessionMessage *executeThisSoftkeyMsg = new SessionMessage(SoftkeyDev, ExecuteSoftkey);
					executeThisSoftkeyMsg->setHfcObject(softkeyButtonToExecute);
					s->handleMessage(executeThisSoftkeyMsg);
					return;
				}
				else
					// If we can't determine the default button to execute, just return the fixedRowCol grid.
					mySoftkeyGroup = fixedSoftkeyGroup;
			}
			else
			{
				// We're being asked to load a specific group.  Since we're using the Reserved_FixedRowColGrid Feature,
				// we'll load it and concatenate it with the fixed row/col.

				mySoftkeyGroup = prepareToLoadSoftkeyGroup(featureNum, RECORD_NOT_FOUND, langNum, dbAccess);
				if (isSelfServMode && isGen2UI)
				{
					//fixedRowColMethod = FRCM_FirstColumn;
					fixedRowColMethod = FRCM_None;
				}

				if (mySoftkeyGroup)
				{
					// Only change the size if we're in Operate Mode, since we'll be concatenating with the fixed row/col later.
					if (event == LoadSoftkeyGroup)
					{
						//Change size, if the fixed row/col method is specified
						if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
							mySoftkeyGroup->setVerticalGridSize(mySoftkeyGroup->getVerticalGridSize() - 1);
						else  // is FRCM_FirstColumn || FRCM_LastColumn
							mySoftkeyGroup->setHorizontalGridSize(mySoftkeyGroup->getHorizontalGridSize() - 1);
					}

					// Just above, we prepared the 'variable' softkey group to load based on the default Group for the Reserved_FixedRowColGrid Feature, to set the size correctly.
					// But we really need to load the data for the group number passed into this method, so reset it if the group number passed is specified (not RECORD_NOT_FOUND),
					// and then load it.
					if (groupNum != RECORD_NOT_FOUND)
						mySoftkeyGroup->setGroupNumber(groupNum);

					loadSoftkeyGroup(mySoftkeyGroup, dbAccess);

					// Only concatenate if we're in Operate Mode.  When in Supervisor mode, we just load the group, since we're setting up the fixed row/col.
					if (event == LoadSoftkeyGroup)
						mySoftkeyGroup->concatenateSoftkeyGroup(fixedSoftkeyGroup, fixedRowColMethod);
				}
			}
		}
	}

	if (mySoftkeyGroup)
	{
		//	Send the SoftkeyGroup found back to the Browser.
		//	We will send an empty SoftkeyGroup back even if no feature, no style or no softkey group is found.
		Debug::trace("SoftkeyGroupManager", "Sending LoadSoftkeyGroup msg");

		SessionMessage featureData(SoftkeyDev, LoadSoftkeyGroup);
		featureData.setFormatType(BrowserFormat);
		mySoftkeyGroup->setFormatType(SoftkeyGroupFormat);
		featureData.setHfcObject(mySoftkeyGroup);

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send(&featureData, Messenger::DEST_UI);
	}
}


// static
SoftkeyGroup *SoftkeyGroupManager::prepareToLoadSoftkeyGroup(HobRecordNum featureNum, HobRecordNum groupNum, short langNum, DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("SoftkeyGroupManager", "loadSoftkeyGroup() - dbAccess is NULL!");
		return 0;
	}

	bool traceOn = Debug::isTracing("SoftkeyGroupManager");
	SoftkeyFeature *feature = (SoftkeyFeature *)NULL;
	SoftkeyStyle *style = (SoftkeyStyle *)NULL;
	HobString myDebug = "handleLoadSoftkeyGroup():   ";
	bool doContinue = true;

	SoftkeyGroup *mySoftkeyGroup = new SoftkeyGroup();

	// have to get the current deptNum bc the msg doesnt contain it
	HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);

	feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
	if (feature)
		mySoftkeyGroup->setFeatureNumber(featureNum);
	else
	{
		if (traceOn)
			Debug::trace("SoftkeyGroupManager", myDebug + "Creating phoney feature # " + longtostring(featureNum));

		feature = new SoftkeyFeature(dbAccess);
		doContinue = false;
	}

	HobRecordNum groupNumber = groupNum;

	if (traceOn)
		Debug::trace("SoftkeyGroupManager", myDebug + "groupNumber = " + longtostring(groupNumber) + "  doContinue = " + booltostring(doContinue));

	if (doContinue)
	{
		if (groupNumber == RECORD_NOT_FOUND)
		{
			groupNumber = feature->getDefaultGroupNumber();		//	Get the default group number for this feature.
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Got groupNumber from the feature. groupNumber = ").append(Long::toString(groupNumber));
				Debug::trace("SoftkeyGroupManager", myDebug2);
			}
		}
		style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
	}
	if (!style)
	{
		if (traceOn)
			Debug::trace("SoftkeyGroupManager", myDebug + "Creating a phoney style.");
		style = new SoftkeyStyle(dbAccess);
		doContinue = false;
	}

	mySoftkeyGroup->setSoftkeyFeature(feature);
	mySoftkeyGroup->setSoftkeyStyle(style);

	if (doContinue && (groupNumber > 0))
	{
		SoftkeyGroup *readSoftkeyGroup = SoftkeyGroup::find(groupNumber, lDept, dbAccess);
		if (readSoftkeyGroup)
		{
			//	Copy the group dimensions to the new SoftkeyGroup
			mySoftkeyGroup->setHorizontalGridSize(readSoftkeyGroup->getHorizontalGridSize());
			mySoftkeyGroup->setVerticalGridSize(readSoftkeyGroup->getVerticalGridSize());
			mySoftkeyGroup->setHorizontalSpacing(readSoftkeyGroup->getHorizontalSpacing());
			mySoftkeyGroup->setVerticalSpacing(readSoftkeyGroup->getVerticalSpacing());
			mySoftkeyGroup->setDeptNum(lDept);
			if (traceOn)
				Debug::trace("SoftkeyGroupManager", "Read the softkeyGroup.");

			delete readSoftkeyGroup;
		}
		else
		{
			//	Use the feature dimensions for the SoftkeyGroup
			mySoftkeyGroup->setHorizontalGridSize(feature->getDefaultHorizontalGridSize());
			mySoftkeyGroup->setVerticalGridSize(feature->getDefaultVerticalGridSize());
			mySoftkeyGroup->setHorizontalSpacing(feature->getHorizontalSpacing());
			mySoftkeyGroup->setVerticalSpacing(feature->getVerticalSpacing());
			if (traceOn)
				Debug::trace("SoftkeyGroupManager", "No SoftkeyGroup found. Setting the SoftkeyGroup info based on the SoftkeyFeature.");
		}
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Grid size is ").append(Long::toString(mySoftkeyGroup->getHorizontalGridSize()));
			myDebug2.append(" x ").append(Long::toString(mySoftkeyGroup->getVerticalGridSize()));
			myDebug2.append(". HorizontalSpacing is ").append(Long::toString(mySoftkeyGroup->getHorizontalSpacing()));
			myDebug2.append(", VerticalSpacing is ").append(Long::toString(mySoftkeyGroup->getVerticalSpacing()));
			Debug::trace("SoftkeyGroupManager", myDebug2);
		}

		mySoftkeyGroup->setGroupNumber(groupNumber);
		mySoftkeyGroup->setLanguageSelection(langNum);
	}

	return mySoftkeyGroup;
}


// static, returns 1 = success, 0 = failure
int SoftkeyGroupManager::loadSoftkeyGroup(SoftkeyGroup *mySoftkeyGroup, DBAccess *dbAccess, FixedRowColMethod fixedRowColMethod)
{
	if (!mySoftkeyGroup || !dbAccess)
		return 0;

	DBSoftKeysRM *dbSoftKeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftKeysRM)
	{
		Debug::trace("SoftkeyGroupManager", "SoftkeyGroupManager::loadSoftkeyGroup() - dbSoftKeysRM is NULL!");
		return 0;
	}

	int hGridSize = 0;
	int vGridSize = 0;
	int hSize, vSize;
	int counter = 0;

	int langNum = mySoftkeyGroup->getLanguageSelection();
	HobRecordNum featureToUse = mySoftkeyGroup->getFeatureNumber();

	FixedRowColMethod fixedRowColMethodOfFeature = FRCM_None;
	int maxRelevantPosition = 999;  // some initial value, doesn't matter
	//if ((featureToUse == Reserved_FixedRowColGrid) || (featureToUse == Reserved_FreshServeButtons))
	if ( featureToUse == Reserved_FixedRowColGrid )
	{
		SoftkeyFeature *f = mySoftkeyGroup->getSoftkeyFeature();
		fixedRowColMethodOfFeature = (f) ? f->getFixedRowColMethod() : FRCM_None;
	}

	DBQuery<DBSoftKey>* allSoftkeysQuery = dbSoftKeysRM->retrieveGroup( longtostring( mySoftkeyGroup->getGroupNumber() ), longtostring( mySoftkeyGroup->getDeptNum() ) );
	if (allSoftkeysQuery)
	{
		bool addIt = true;
		DBQuery<DBSoftKey>::const_iterator tempIter = allSoftkeysQuery->begin();
		while (tempIter != allSoftkeysQuery->end())
		{
			// Initially assume we are going to put the Softkey into the Group
			addIt = true;

			//	We will calculate the size of the grid just in case the SoftkeyGroup DB record does not exist.
			hSize = Integer::valueOf(tempIter->getHorizontalPosition()) + tempIter->getHorizontalSize();
			vSize = Integer::valueOf(tempIter->getVerticalPosition()) + tempIter->getVerticalSize();

			if (hSize > hGridSize)
				hGridSize = hSize;
			if (vSize > vGridSize)
				vGridSize = vSize;

			DBSoftKey *skey = new DBSoftKey(*tempIter);
			if (skey)
			{
				Softkey newSoftkey(skey, dbAccess, langNum);
				if (featureToUse == ReservedSK_SelfservCategoryButtons && newSoftkey.getAction() == Action_LoadCategory_First)
				{
					featureToUse = ReservedSK_SelfservCategoryResultsButtons;
					newSoftkey.setActionFeatureNumber(featureToUse);
				}

				if (fixedRowColMethod != FRCM_None)
				{
					// When loading the fixed row or column, only add the softkeys that are in the fixed row/col.
					switch (fixedRowColMethod)
					{
						case FRCM_TopRow:
							if ( newSoftkey.getVerticalPosition() != 0 )
								addIt = false;
							break;
						case FRCM_BottomRow:
							if ( newSoftkey.getVerticalPosition() != mySoftkeyGroup->getVerticalGridSize() - 1 )
								addIt = false;
							break;
						case FRCM_FirstColumn:
							if ( newSoftkey.getHorizontalPosition() != 0 )
								addIt = false;
							break;
						case FRCM_LastColumn:
							if ( newSoftkey.getHorizontalPosition() != mySoftkeyGroup->getHorizontalGridSize() - 1 )
								addIt = false;
							break;
						default:
							break;
					}
				}
				else if ( featureToUse == Reserved_FixedRowColGrid  && fixedRowColMethodOfFeature != FRCM_None)
				{
					// This is when we are loading a Softkey Group into the Reserved_FixedRowColGrid Feature that has a fixed row or column.
					// If it's fixed bottom row or fixed right column, we don't want to load any softkeys that would interfere with the
					// softkeys in the fixed row/col.  We also don't need to load any that are beyond the fixed row/col, since they won't be
					// displayed anyway.
					switch (fixedRowColMethodOfFeature)
					{
						case FRCM_BottomRow:
							if (newSoftkey.getVerticalPosition() >= mySoftkeyGroup->getVerticalGridSize() )
								addIt = false;
							break;
						case FRCM_LastColumn:
							if ( newSoftkey.getHorizontalPosition() >= mySoftkeyGroup->getHorizontalGridSize() )
								addIt = false;
							break;
						default:
							break;
					}
				}

				if (addIt)
				{
					mySoftkeyGroup->add(newSoftkey, featureToUse, langNum);
					++counter;
				}
			}
			++tempIter;
		}

		mySoftkeyGroup->setGridCellCount(counter);

		delete allSoftkeysQuery;
		allSoftkeysQuery = 0;

		Debug::trace("SoftkeyGroupManager", string("SoftkeyGroupManager::loadSoftkeyGroup() - Read ") + longtostring(counter) + " softkeys.");
	}
	return 1;
}


// static
SoftkeyGroup *SoftkeyGroupManager::loadFixedRowColSoftkeyGroup(HobRecordNum groupNum, short langNum, DBAccess *dbAccess)
{
	// have to get the current deptNum bc the msg doesnt contain it
	HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
	bool isSelfServMode = Configuration::getInstance()->isSelfServiceScale();
	bool isGen2UI = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe);
	SoftkeyFeature* feature = (SoftkeyFeature*)NULL;
	SoftkeyGroup* fixedSoftkeyGroup = (SoftkeyGroup*)NULL;
	FixedRowColMethod fixedRowColMethod = FRCM_None;

	//if( isGen2UI && isSelfServMode )
	//	feature = SoftkeyFeature::find(Reserved_FreshServeButtons, lDept, dbAccess);
	//else
	//	feature = SoftkeyFeature::find(Reserved_FixedRowColGrid, lDept, dbAccess);

	feature = SoftkeyFeature::find(Reserved_FixedRowColGrid, lDept, dbAccess);

	if (!feature)
		return 0;  // Don't need to continue if the feature isn't in the database.
	else
	{
		//if (isGen2UI && isSelfServMode)
		//{
		//	fixedRowColMethod = feature->getFixedRowColMethod();
		//	feature->setFixedRowColMethod(FRCM_FirstColumn);
		//}
		//else
		//	fixedRowColMethod = feature->getFixedRowColMethod();

		fixedRowColMethod = feature->getFixedRowColMethod();

		delete feature;
		feature = 0;
	}

	// For Feature Reserved_FixedRowColGrid, we always use the grid dimensions specified by the Softkey Group with the
	// Fixed Softkey Feature's default group number, which overrides any groupNumber passed in with the msgSG parameter!
	if (isGen2UI && isSelfServMode)
		fixedSoftkeyGroup = prepareToLoadSoftkeyGroup(Reserved_FreshServeButtons, RECORD_NOT_FOUND, langNum, dbAccess);
	else
		fixedSoftkeyGroup = prepareToLoadSoftkeyGroup(Reserved_FixedRowColGrid, RECORD_NOT_FOUND, langNum, dbAccess);

	if (fixedSoftkeyGroup)
	{
		// Load the fixed row/col into the Softkey Group
		loadSoftkeyGroup(fixedSoftkeyGroup, dbAccess, fixedRowColMethod);
	}
	return fixedSoftkeyGroup;
}


/* If the first record was not read, this method will return NULL.
 */
SoftkeyGroup *SoftkeyGroupManager::firstSoftkeyGroup()
{
	if (!dbAccess)
		return (SoftkeyGroup *)NULL;

	DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = dbAccess->getDBSoftKeyGroupsRM();
	if (!dbSoftkeyGroupsRM)
		return (SoftkeyGroup *)NULL;

	// If the user calls firstSoftkeyGroup() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allSoftkeyGroupsQuery = dbSoftkeyGroupsRM->retrieveAll();

	// Couldn't get the query from which to access the SoftkeyGroups, just return false.
	if (!allSoftkeyGroupsQuery)
		return (SoftkeyGroup *)NULL;

	DBQuery<DBSoftKeyGroup>::const_iterator tempIter = allSoftkeyGroupsQuery->begin();
	if (tempIter == allSoftkeyGroupsQuery->end())
		return (SoftkeyGroup *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextSoftkeyGroup()
	 * method call below).
	 */
	allSoftkeyGroupsIter = new DBQuery<DBSoftKeyGroup>::const_iterator(tempIter);
	DBSoftKeyGroup *c = new DBSoftKeyGroup(*(*allSoftkeyGroupsIter));
	SoftkeyGroup *newSoftkeyGroup = new SoftkeyGroup(c, dbAccess);
	return newSoftkeyGroup;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstSoftkeyGroup() method, the first call to this method will
 * result in the first record being returned.
 */
SoftkeyGroup *SoftkeyGroupManager::nextSoftkeyGroup()
{
	if (!dbAccess)
		return (SoftkeyGroup *)NULL;

	if (allSoftkeyGroupsQuery && !allSoftkeyGroupsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allSoftkeyGroupsQuery || !allSoftkeyGroupsIter)
		return firstSoftkeyGroup();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allSoftkeyGroupsIter);
	if (*allSoftkeyGroupsIter == allSoftkeyGroupsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all SoftkeyGroups in the db. */
		destroyQueriesAndIters();
		return (SoftkeyGroup *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBSoftKeyGroup copy constructor in order for the Hfc SoftkeyGroup to
	 * control the destruction of the DBSoftKeyGroup record that the iterator contains; otherwise, the DBSoftKeyGroup will
	 * be deleted when the iterator goes out of scope.  The DBSoftKeyGroup copy will be deleted by the SoftkeyGroup record
	 * created below.
	 */
	DBSoftKeyGroup *c = new DBSoftKeyGroup(*(*allSoftkeyGroupsIter));
	SoftkeyGroup *newSoftkeyGroup = new SoftkeyGroup(c, dbAccess);
	return newSoftkeyGroup;
}


/* Provide a countAll method so the caller doesn't need to know about a dbSoftkeyGroupsRM.
 */
int SoftkeyGroupManager::countAllSoftkeyGroups()
{
	if (!dbAccess)
		return -1;

	DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = dbAccess->getDBSoftKeyGroupsRM();
	if (!dbSoftkeyGroupsRM)
		return -1;

	return dbSoftkeyGroupsRM->countAll();
}


SoftkeyGroup *SoftkeyGroupManager::getCurrentSoftkeyGroup()
{
	return currentSoftkeyGroup;
}
void SoftkeyGroupManager::setCurrentSoftkeyGroup(SoftkeyGroup *newSoftkeyGroup)
{
	if (currentSoftkeyGroup)
		delete currentSoftkeyGroup;
	currentSoftkeyGroup = newSoftkeyGroup;
}


void SoftkeyGroupManager::handleRead(SoftkeyGroup *msgSoftkeyGroup)
{
	if (!mySession || !msgSoftkeyGroup || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	HobRecordNum test_long_dept = msgSoftkeyGroup->getDeptNum();
	SoftkeyGroup *readSoftkeyGroup = SoftkeyGroup::find(msgSoftkeyGroup->getGroupNumber(), msgSoftkeyGroup->getDeptNum(), dbAccess);
	if (!readSoftkeyGroup)		//	This softkeyGroup does not exist
	{
		// Send error message to browser indicating SoftkeyGroup not found.
		if (s->isInSupervisor())
			mySession->addDialogMessage(ErrSoftkeyDev, SoftkeyGroupNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		return;
	}

	// We have a new current SoftkeyGroup.
	setCurrentSoftkeyGroup(readSoftkeyGroup);

	if (Debug::isTracing("SoftkeyGroupManager"))
	{
		HobString myDebug = "handleReadSoftkeyGroup(): currentSoftkeyGroup = ";
		myDebug.append(Hex::toString((long long) currentSoftkeyGroup));
		Debug::trace("SoftkeyGroupManager", myDebug);
	}

	// Send the SoftkeyGroup found back to the Browser.
	// Is this the behavior for ALL SoftkeyGroupRead messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
	SessionMessage featureData(SoftkeyDev, WriteSoftkeyGroup);
	featureData.setFormatType(BrowserFormat);
	featureData.setHfcObject(readSoftkeyGroup);

	// Send the SoftkeyGroup to the UI (Browser)
	mySession->send(&featureData, Messenger::DEST_UI);
}


void SoftkeyGroupManager::handleReadAll()
{
	Debug::trace("SoftkeyGroupManager", "handling ReadAllSoftkeyGroups event ...");
	if (!dbAccess)
		return;

	DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = dbAccess->getDBSoftKeyGroupsRM();
	if (!dbSoftkeyGroupsRM)
		return;

	HobList *groups = new HobList;

	// add all groups to list
	for ( SoftkeyGroup *i = firstSoftkeyGroup(); i; i = nextSoftkeyGroup() )
		groups->push_back(Long::toString(i->getGroupNumber()));

	SessionMessage msg(SoftkeyDev, WriteAllSoftkeyGroups);
	msg.setFormatType(BrowserFormat);
	msg.setHfcObject(groups);

	mySession->send(&msg, Messenger::DEST_UI);
	delete groups;
}


void SoftkeyGroupManager::handleWrite(SoftkeyGroup *modifiedSoftkeyGroup)
{
	if ((!modifiedSoftkeyGroup) || (!currentSoftkeyGroup))
		return;

	if (Debug::isTracing("SoftkeyGroupManager"))
	{
		HobString myDebug = "handleWriteSoftkeyGroup(): currentSoftkeyGroup = ";
		myDebug.append(Hex::toString((long long) currentSoftkeyGroup)).append("  , modifiedSoftkeyGroup = ").append(Hex::toString((long long) modifiedSoftkeyGroup));
		Debug::trace("SoftkeyGroupManager", myDebug);
	}
  	currentSoftkeyGroup->update(modifiedSoftkeyGroup);
}


void SoftkeyGroupManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyGroup *msgSoftkeyGroup = dynamic_cast<SoftkeyGroup *> (msg->getHfcObject());
	if (!msgSoftkeyGroup || !dbAccess)
		return;


	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyGroups))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobRecordNum test_long_dept = msgSoftkeyGroup->getDeptNum();
		SoftkeyGroup *existingSoftkeyGroup = SoftkeyGroup::find(msgSoftkeyGroup->getGroupNumber(), msgSoftkeyGroup->getDeptNum(), dbAccess);
		if (existingSoftkeyGroup)
		{
			existingSoftkeyGroup->deleteRecord();
			if (Debug::isTracing("SoftkeyGroupManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyGroup - SoftkeyGroup deleted - groupNum: ";
				myDebug.append(Long::toString(msgSoftkeyGroup->getGroupNumber()));
				Debug::trace("SoftkeyGroupManager", myDebug);
			}

			if (msg->getEventType() != BatchApplyDeleteSoftkeyGroup)
			{
				if (mySession && !mySession->isCommSession() && currentSoftkeyGroup && (currentSoftkeyGroup->getGroupNumber() == msgSoftkeyGroup->getGroupNumber()))
				{
					setCurrentSoftkeyGroup(0);
					//	Send the blank softkeyGroup to the browser
					sendCurrentSoftkeyGroup();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyGroups))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingSoftkeyGroup, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingSoftkeyGroup;
		}
		else
		{
			if (Debug::isTracing("SoftkeyGroupManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyGroup - SoftkeyGroup to be deleted was not found in the database - groupNum: ";
				myDebug.append(Long::toString(msgSoftkeyGroup->getGroupNumber()));
				Debug::trace("SoftkeyGroupManager", myDebug);
			}
		}
	}
}


void SoftkeyGroupManager::handleDeleteAll(bool createDefaultKeys, HobRecordNum deptNum)
{
	Debug::trace("SoftkeyGroupManager", "handleDeleteAllSoftkeyGroups - deleting all SoftkeyGroups");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyGroups))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(getDevice(), getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBSoftKeyGroupsRM *dbRM = dbAccess->getDBSoftKeyGroupsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		Debug::trace("SoftkeyGroupManager", "handleDeleteAll - will create reserved records next");
		
		//If we are doing a restore, we don't want to re-create default keys right away, since they 
		//may be coming in via the restore file, and may be in a different location. If no flashkeys are
		//read in on a restore and flashkey option was checked, we will then at that time create the default 
		//flashkeys
		SoftkeyGroup::verifyReservedRecords(dbAccess, createDefaultKeys);
	}

	Debug::trace("SoftkeyGroupManager", "handleDeleteAllSoftkeyGroups - leaving");
}


void SoftkeyGroupManager::handleUpdateAndSave(SoftkeyGroup *modifiedSoftkeyGroup)
{
	Debug::trace("SoftkeyGroupManager", HobString("handleUpdateAndSave(): currentSoftkeyGroup = ") + HobString(Hex::toString((long long) currentSoftkeyGroup)));
	if (currentSoftkeyGroup)
	{
		if (modifiedSoftkeyGroup)
		  	currentSoftkeyGroup->update(modifiedSoftkeyGroup);
		handleSaveRecord(currentSoftkeyGroup);
	}
}

void SoftkeyGroupManager::handleSaveRecord(SoftkeyGroup *recordToSave)
{
	if (Debug::isTracing("SoftkeyGroupManager"))
	{
		HobString myDebug = "handleSaveSoftkeyGroup(): recordToSave = ";
		myDebug.append(Hex::toString((long long) recordToSave));
		Debug::trace("SoftkeyGroupManager", myDebug);
	}
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyGroups))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void SoftkeyGroupManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyGroup *restoreSoftkeyGroup = dynamic_cast<SoftkeyGroup *> (msg->getHfcObject());
	if (!restoreSoftkeyGroup || !dbAccess)
		return;

	if (restoreSoftkeyGroup->hasErrors())
	{
		if (mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = restoreSoftkeyGroup->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = restoreSoftkeyGroup->nextError();
			}
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyGroups))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("SoftkeyGroupManager");
		HobString myDebug = "";
		if (traceOn)
		{
			myDebug = "handleRestoreSoftkeyGroup():   ";
			HobString myDebug2 = myDebug;
			myDebug2.append("groupNumber = ").append(Integer::toString(restoreSoftkeyGroup->getGroupNumber()));
			Debug::trace("SoftkeyGroupManager", myDebug2);
		}

		restoreSoftkeyGroup->setDBAccess(dbAccess);
		//restoreSoftkeyGroup->show("Restoring this SoftkeyGroup:");

		SoftkeyGroup *usedSoftkeyGroup = (SoftkeyGroup *)NULL;
		HobRecordNum test_long_dept = restoreSoftkeyGroup->getDeptNum();
		SoftkeyGroup *existingSoftkeyGroup = SoftkeyGroup::find(restoreSoftkeyGroup->getGroupNumber(), restoreSoftkeyGroup->getDeptNum(), dbAccess);
		if (existingSoftkeyGroup)
		{
			if (existingSoftkeyGroup->isRecordValid())
			{
				Debug::trace("SoftkeyGroupManager", myDebug + "Existing record is valid.");
				existingSoftkeyGroup->update(restoreSoftkeyGroup);
				existingSoftkeyGroup->show("Updated this existing SoftkeyGroup:");
				existingSoftkeyGroup->saveRecord();
				usedSoftkeyGroup = existingSoftkeyGroup;
			}
			else
			{
				Debug::trace("SoftkeyGroupManager", myDebug + "Existing record is NOT valid! Using the record to restore.");
				existingSoftkeyGroup->deleteRecord();
				restoreSoftkeyGroup->saveRecord();
				usedSoftkeyGroup = restoreSoftkeyGroup;
			}
		}
		else
		{
			restoreSoftkeyGroup->saveRecord();
			usedSoftkeyGroup = restoreSoftkeyGroup;
		}

		if (existingSoftkeyGroup)
			delete existingSoftkeyGroup;
	}
}


void SoftkeyGroupManager::handleCreate(SoftkeyGroup *msgSoftkeyGroup)
{
	if (!mySession || !msgSoftkeyGroup || !dbAccess)
		return;

	SoftkeyGroup *newSoftkeyGroup = new SoftkeyGroup(dbAccess);
	if (newSoftkeyGroup && (msgSoftkeyGroup->getGroupNumber() > 0))
	{
		newSoftkeyGroup->setGroupNumber(msgSoftkeyGroup->getGroupNumber());

		newSoftkeyGroup->update(msgSoftkeyGroup);		//	Update with the size and spacing info, if supplied.

		//	Set this new category as the current category.
		setCurrentSoftkeyGroup(newSoftkeyGroup);

		// Send the new category to the Browser.
		SessionMessage featureData(SoftkeyDev, WriteSoftkeyGroup);
		featureData.setFormatType(BrowserFormat);
		featureData.setHfcObject(currentSoftkeyGroup);

		// Send the product to the UI (Browser)
		mySession->send(&featureData, Messenger::DEST_UI);

		handleSaveRecord(newSoftkeyGroup);
	}
}

void SoftkeyGroupManager::sendCurrentSoftkeyGroup()
{
	if (!currentSoftkeyGroup || !mySession)
	{
		Debug::trace("SoftkeyGroupManager", "sendCurrentSoftkeyGroup(): currentSoftkeyGroup or mySession is NULL!");
		return;
	}
	SessionMessage categoryMsg(SoftkeyDev, WriteSoftkeyGroup);
	categoryMsg.setFormatType(BrowserFormat);
	categoryMsg.setHfcObject(currentSoftkeyGroup);
	mySession->send(&categoryMsg, Messenger::DEST_UI);
}

void SoftkeyGroupManager::destroyQueriesAndIters()
{
	Debug::trace("SoftkeyGroupManager", "destroySoftkeyGroupQueriesAndIters()");
	if (allSoftkeyGroupsIter)
	{
		delete allSoftkeyGroupsIter;
		allSoftkeyGroupsIter = 0;
	}

	if (allSoftkeyGroupsQuery)
	{
		delete allSoftkeyGroupsQuery;
		allSoftkeyGroupsQuery = 0;
	}
}


void SoftkeyGroupManager::determinePLUsInSoftkeyGroup(const HobRecordNum groupNum, const HobRecordNum departmentNum, std::string &pluNumberList, std::string *groupNumberList)
{
	bool traceOn = Debug::isTracing("SoftkeyGroupManager");
	HobString myDebug = "determinePLUsInSoftkeyGroup():   ";

	// I would normally initialize the pluNumberList to be an empty string here, but since this method is recursive,
	// it will have to be up to the original caller to pass in a reference to a string that is empty.

	if ( groupNum <= 0 || !dbAccess)
	{
		Debug::trace("SoftkeyGroupManager", myDebug + "groupNum <= 0 or dbAccess is NULL!");
		return;
	}

	DBSoftKeysRM *dbSoftKeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftKeysRM)
	{
		Debug::trace("SoftkeyGroupManager", myDebug + "dbSoftKeysRM is NULL!");
		return;
	}

	// This groupNumberList is being kept so that we prevent circular recursive calls (a group that has a flashkey to load a group that has already been loaded).
	bool groupNumListCreated = false;
	if ( !groupNumberList )
	{
		groupNumberList = new std::string();
		if ( !groupNumberList )
		{
			Debug::trace("SoftkeyGroupManager", myDebug + "groupNumberList could not be allocated!");
			return;
		}
		groupNumberList->append(",");
		pluNumberList.append(",");
		groupNumListCreated = true;
	}

	groupNumberList->append(Long::toString(groupNum));
	groupNumberList->append(",");

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("groupNumber = ").append(Long::toString(groupNum));
		Debug::trace("SoftkeyGroupManager", myDebug2);
	}

	SoftkeyGroup *readSoftkeyGroup = SoftkeyGroup::find(groupNum, departmentNum, dbAccess);
	if (readSoftkeyGroup)
	{
		DBQuery<DBSoftKey>* allSoftkeysQuery = dbSoftKeysRM->retrieveGroup(longtostring(groupNum), longtostring(departmentNum));
		if (allSoftkeysQuery)
		{
			DBQuery<DBSoftKey>::const_iterator tempIter = allSoftkeysQuery->begin();
			while (tempIter != allSoftkeysQuery->end())
			{
				DBSoftKey *skey = new DBSoftKey(*tempIter);
				if (skey)
				{
					if ( skey->getAction() == Action_SelectProduct && Integer::valueOf(skey->getActionProductNumber()) > 0 )
						pluNumberList += skey->getActionProductNumber() + ",";
					else if ( skey->getAction() == Action_LoadSoftkeyGroup )
					{
						HobRecordNum gNum = skey->getActionIntegerParameter();
						string gNumS = "," + Long::toString(gNum) + ",";
						if ( groupNumberList->find( gNumS ) == std::string::npos )
						{
							// recursive call!
							determinePLUsInSoftkeyGroup( gNum, departmentNum, pluNumberList, groupNumberList );
						}
					}
					delete skey;
					skey = 0;
				}
				++tempIter;
			}
			delete allSoftkeysQuery;
			allSoftkeysQuery = 0;
		}

		delete readSoftkeyGroup;
	}

	if ( groupNumListCreated )
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("groupList =  ").append(*groupNumberList);
			Debug::trace("SoftkeyGroupManager", myDebug2);
			myDebug2 = myDebug;
			myDebug2.append("pluList =  ").append(pluNumberList);
			Debug::trace("SoftkeyGroupManager", myDebug2);
		}

		delete groupNumberList;
		groupNumberList = 0;
	}

}
