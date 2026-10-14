/**
 * @file NGWUpdateManager.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper firmware update manager
 * Created on: Dec 15, 2015
 *
 **/

#include "NGWUpdateManager.h"
#include "CanManager.h"
#include "FpmManager.h"
#include "NGWReqBootStatus.h"
#include "NGWRequestBootVersion.h"
#include "NGWBootStatus.h"
#include "NGWBootVersion.h"
#include "NGWBootNodeCMD.h"
#include "NGWBootFrame.h"
#include "NGWAck.h"
#include "NGWFlush.h"
#include "NGWBulkTransfer.h"
#include "NGWStartUpgrade.h"
#include "NGWUpgradeStatus.h"
#include "NGWUpgradeInfo.h"
#include "NGWUpgradeProgress.h"
#include "NGWUpgradeFinished.h"
#include "NGWUpgradeRequired.h"
#include "NGWUpgradeUIReady.h"
#include "NGWGeneric.h"
#include "GpioDeviceReset.h"
#include "filesys/file.h"

#include <algorithm>
#include <fstream>
#include <stdio.h>

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#define MAX_REC_SIZE	64
#define PROGRAM_FLASH_CMD	31

std::mutex NGWUpdateManager::instanceMutex_;
NGWUpdateManager *NGWUpdateManager::instance_( nullptr );

const std::string NGWUpdateManager::upgradePathNGW_( "/lib/firmware/hobart/ngw/" );
const std::string NGWUpdateManager::upgradePathNGS_( "/lib/firmware/hobart/ngs/" );

const std::string NGWUpdateManager::wrapperDirectory_( "wrapper/" );

int NGWUpdateManager::retryCounter;

///////////////////////////////////////////////////////////////////////////////////////////////////
NGWUpdateManager *NGWUpdateManager::getInstance()
{
	if ( !instance_ )
	{
		std::lock_guard<std::mutex> guard( instanceMutex_ );

		if ( !instance_ )
			instance_ = new NGWUpdateManager();
	}

	return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::shutdown()
{
	Debug::trace("NGWUpdateManager", "shutdown()");

	if ( instance_ )
	{
		std::lock_guard<std::mutex> guard( instanceMutex_ );

		if ( instance_ )
		{
			delete instance_;
			instance_ = nullptr;
		}
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NGWUpdateManager::NGWUpdateManager() :
	Hobart::CEvolMsgReceiver("NGWUpdateManager"),
	finished_( false )
{
    MtdManufacturingData *manfData( FpmManager::getInstance()->readMfgScaleData() );

	systemStyle = FpmManager::getInstance()->getMFGSystemStyle();

	if ( manfData )
	{
		scaleStyle = manfData->getScaleStyle();
		replaceAll( scaleStyle, "\"", "" );
	}

	Debug::trace( "NGWUpdateManager", "MfgSys Style: " + systemStyle);
	Debug::trace( "NGWUpdateManager", " Scale Style: " + scaleStyle);

	// initialize our list of known device nodes
	nodes_.push_back(_Node1);
	nodes_.push_back(_Node2);
	nodes_.push_back(_Node3);
	nodes_.push_back(_Node4);
	nodes_.push_back(_Node5);
	nodes_.push_back(_Node6);

	// get an instance of the CAN manager
	// assign my pointer to translator
#if defined ( __linux__ )				// no windows support for CAN at this time
	CanManager *pManager;

	if ( (pManager = CanManager::getInstance()) )
		pManager->setTranslatorUpdaterManager( this );
#endif // __linux__

	// register for upgrade messages
	registerEvent( Wrapper, WrNGWBootStatus );
	registerEvent( Wrapper, WrNGWAck );
	registerEvent( Wrapper, WrNGWBootVersion );
	registerEvent( NGWrapper, _UpgradeUIReady );
	registerEvent( NGWrapper, _StartUpgrade );
	registerEvent( NGWrapper, _UpgradeCancel );
	registerEvent( NGWrapper, _ReqUpgradeInfo );

	state_ = _Init;

	detected_ = false;

	myRunThread_ = new Util::CTLSThread( *this, 1, 0, "NGWUpdateManager" );

	retryCounter = 0;

	Debug::trace("NGWUpdateManager", "Update state = _Init" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NGWUpdateManager::~NGWUpdateManager()
{
	Debug::trace("NGWUpdateManager", "~NGWUpdateManager()");

	cancelTimer(1);

	deregisterAllEvents();

	ShutdownNicely();

	// tell the thread to exit
	m_ExitEvent.EventSet();

	// wait for the thread to exit before deleting all the resources!
	if ( myRunThread_ )
	{
		while ( myRunThread_->IsRunning() )
			PAL_absclass::PAL_sleep(1);

		Debug::trace("NGWUpdateManager", "~NGWUpdateManager() Thread has exited.");

        delete myRunThread_;
        myRunThread_ = nullptr;
	}

#if defined ( __linux__ )
	// get an instance of the CAN manager
	// clear the updater assignment in the translator
	// close the CAN interface
	CanManager *pManager;

	if ( (pManager = CanManager::getInstance()) )
	{
		pManager->clrTranslatorUpdateManager();

		//shutdown the upgrade interface so we can start the operational interface
		if ( ( systemStyle == "NGW3" ) || ( systemStyle == "NGW2" ) )
		{
			Debug::trace("NGWUpdate", "Shutting down the can manager");
			pManager->shutdown();
	    }
	}
#endif // __linux__

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::resetDevices()
{
	// setup the reset line for AWS3
	GpioDeviceReset gpioReset;
	gpioReset.setDeviceTypeAWS3Wrapper();

	// assert reset for wrapper nodes
	gpioReset.resetDevice();

	// keep the reset line low for a minimum of 200mSec
	PAL_absclass::PAL_sleep( 200 );

	// release reset on the nodes
	gpioReset.releaseResetDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD NGWUpdateManager::Run()
{
	Debug::trace("NGWUpdateManager", "Entering thread");

	do
	{
		// yield thread...
		PAL_absclass::PAL_sleep( 1 );

		std::lock_guard<std::mutex> guard( mutex_ );

		switch ( state_ )
		{
		case _Init:

			Debug::trace( "NGWUpdateManager", "_Init" );

			currentNodeIndex_ = 0;
			forceUpgrade_ = false;

			nodeFaults_.clear();

			// start response timer if no response, move to the next device in the list
			createTimer(responseTime_, 1, CHobartTimer::PERIODIC_TIMER);

			state_ = _RequestStatus;
			break;

		case _RequestStatus:
		{
			// request the loader status for the device on the list
			if ( nodes_.size() <= currentNodeIndex_ )
			{
				if ( nodes_.size() == nodeVersions_.size() )
					state_ = _CheckForUpgrade;
				else
				{
					Debug::trace( "NGWUpdateManager", "_RequestStatus  Failed to retrieve status from all nodes" );
					state_ = _StartNode;
				}

				break;
			}

			Debug::trace( "NGWUpdateManager", "_RequestStatus  Send request for status to node" );

			NGWRequestBootStatus reqStatus(this, currentNode());
			reqStatus.setSourceObject(this);
			dispatchEvent(&reqStatus);

			state_ = _WaitForStatus;
			break;
		}

		case _WaitForStatus:
			break;

		case _RequestVersion:
		{

			Debug::trace( "NGWUpdateManager", "_RequestVersion  Send request for version info to node" );

			NGWRequestBootVersion rbv(this, currentNode());
			rbv.setSourceObject(this);
			dispatchEvent(&rbv);

			state_ = _WaitForVersion;
			break;
		}

		case _WaitForVersion:
			break;

		case _CheckForUpgrade:
		{
			cancelTimer( 1 );

			Debug::trace( "NGWUpdateManager", "_CheckForUpgrade for nodes" );

			bool upgradeRequired( false );

			// node requires a firmware update
			for ( const auto &node : nodes_ )
				if ( checkForUpdate( node ) )
				{
					Debug::trace( "NGWUpdateManager", "Node requires an upgrade: " + Integer::toString(node));
					upgradeRequired = true;
				}

			if (( upgradeRequired ) || ( forceUpgrade_ ))
			{
				// tell the UI to show the Wrapper upgrade page.
				Debug::trace("NGWUpdateManager", "_CheckForUpgrade: Message to startup the UI");

				NGWGeneric genMsg(_UpgradeRequired, this);
				dispatchEvent(&genMsg);

				// start response timer if no response from UI, try up to three times
				createTimer(responseTime_, 1, CHobartTimer::PERIODIC_TIMER);

				state_ = _WaitForUI;
			}
			else
			{
				// we have made it through the list without errors or upgrade required.
				state_ = _StartNode;
			}

			break;
		}

		case _StartNode:
		{
			cancelTimer(1);

			Debug::trace( "NGWUpdateManager","_StartNode" );

			//wrapper detected
			detected_ = true;

			// the jump to app message does not evaluate node... so by sending this message
			// to any node all nodes will see it and process it.

			// send command for all nodes to start its application.
			NGWBootNodeCMD cmd( this, _Node2, NGWBootNodeCMD::JumpToApp );
			cmd.setSourceObject(this);
			dispatchEvent(&cmd);

			state_ = _Exit;
			break;
		}

		case _Exit:
		{
			cancelTimer(1);

			Debug::trace("NGWUpdateManager", "Entered Exit state." );

			// tell the UI that we are finished upgrading the wrapper
			NGWUpgradeFinished msg(this);
			msg.setSourceObject(this);
			dispatchEvent(&msg);

			m_ExitEvent.EventSet();
			break;
		}

		case _WaitForUI:
			break;

		case _StartUpgrade:
		{
			cancelTimer(1);

			// inform the UI of the number of nodes
			NGWStartUpgrade msg(this, (UINT8)nodes_.size());
			msg.setSourceObject(this);
			dispatchEvent(&msg);

			currentNodeIndex_ = 0;
			state_ = _NextNode;

			break;
		}

		case _NextNode:
		{
			cancelTimer(1);

			// find node to upgrade
			if ( nodes_.size() <= currentNodeIndex_ )
			{
				Debug::trace("NGWUpdateManager", "No more nodes to upgrade!");

				// we have made it through the list without errors or upgrade required.
				state_ = _StartNode;
			}

			const std::string firmwareVersion( firmwareVersions_[currentNode()] );
			const size_t firmwareSize( firmwareImages_[currentNode()].size() );

			Debug::trace( "NGWUpdateManager", "_NextNode: " + Integer::toString(currentNode()));
			Debug::trace( "NGWUpdateManager", "Node version: " + currentNodeVersion());
			Debug::trace( "NGWUpdateManager", "Firmware version: " + firmwareVersion);
			Debug::trace( "NGWUpdateManager", "Firmware size: " + Integer::toString(firmwareSize));

			if (( firmwareVersion.empty() ) || ( currentNodeVersion() == firmwareVersion ))
			{
				// inform the UI node state
				NGWUpgradeProgress msg(this, currentNode(), 100);
				msg.setSourceObject(this);
				dispatchEvent(&msg);

				++currentNodeIndex_;
			}
			else if ( firmwareSize )
			{
				// inform the UI node state
				NGWUpgradeProgress msg(this, currentNode(), 0);
				msg.setSourceObject(this);
				dispatchEvent(&msg);

				offset_ = 0;
				state_ = _SendUpgradeCommand;
			}
			else
			{
				Debug::trace("NGWUpdateManager.Error", "Missing firmware!");

				// notify the UI of the node error.
				NGWUpgradeStatus msg(this, currentNode());
				msg.setVersion(currentNodeVersion());
				msg.setStateError();
				msg.setErrorWriteFailure();
				dispatchEvent(&msg);

				++currentNodeIndex_;
			}

			break;
		}

		case _SendUpgradeCommand:
		{
			Debug::trace( "NGWUpdateManager", "_SendUpgradeCommand: " + Integer::toString(currentNode()));

			NGWBootNodeCMD cmd( this, currentNode(), NGWBootNodeCMD::UpdateImage );
			cmd.setSourceObject(this);
			dispatchEvent(&cmd);

			// start response timer
			createTimer(upgradeTime_, 1, CHobartTimer::PERIODIC_TIMER);

			state_ = _WaitForReady;
			break;
		}

		case _WaitForReady:
			break;

		case _Upgrade:
		{
			bool done( false );

			if ( !updateDevice(currentNode(), done) )
				state_ = _ErrorNode; 						// upgrade failed during image download
			else if ( !done )
			{
				retryFrameTimer_ = 0;
				state_ = _WaitForAck;
			}
			else
			{
				// inform the UI that node download is complete.
				NGWUpgradeProgress msg(this, currentNode(), 100);
				msg.setSourceObject(this);
				dispatchEvent(&msg);

				// upgrade complete
				state_ = _EndNodeProg;
			}

			break;
		}

		case _WaitForAck:

			// resend frame after 5s
			if (( 5000 == ++retryFrameTimer_ ) && ( offset_ ))
			{
				offset_ -= NGWBulkTransfer::MAX_PAYLOAD_SIZE;
				state_ = _Upgrade;
			}

			break;

		case _EndNodeProg:
		{
			Debug::trace( "NGWUpdateManager", "_EndNodeProg: " + Integer::toString(currentNode()));

			// instruct the node to verify its application
			NGWBootNodeCMD cmd(this, currentNode(), NGWBootNodeCMD::VerifyApplication);
			cmd.setSourceObject(this);
			dispatchEvent(&cmd);

			state_ = _WaitForVerify;
			break;
		}

		case _WaitForVerify:
			break;

		case _ResetNode:
		{
			cancelTimer(1);

			Debug::trace("NGWUpdateManager", "Reset Node: " + Integer::toString(currentNode()));

			// send command for node to reset.
			NGWBootNodeCMD cmd( this, currentNode(), NGWBootNodeCMD::ResetNode );
			dispatchEvent(&cmd);

			// TO DO: add reception of RTR (0x701,0x702...) to signal when ready
			// allow the node to reset and initialize before we begin again
			PAL_absclass::PAL_sleep( 500 );

			state_ = _ErrorNode;
			break;
		}

		case _ErrorNode:
		{
			cancelTimer(1);

			Debug::trace("NGWUpdateManager.Error", "Node failure: " + Integer::toString(currentNode()));

			++currentNodeIndex_;
			state_ = _NextNode;
			break;
		}

		case _Cancel:

			cancelTimer(1);

			Debug::trace("NGWUpdateManager", "_Cancel: Reset device.");

			// reset the wrapper into bootloader
			resetDevices();

			state_ = _Init;
			break;

		default:
			break;
		}

	} while ( !m_ExitEvent.IsEventSet() );

	Debug::trace("NGWUpdateManager", "Finished." );

	// tell the UI that we are finished upgrading the wrapper
	NGWUpgradeFinished msg(this);
	msg.setSourceObject(this);
	dispatchEvent(&msg);

	// we are finished with the list
	finished_ = true;

	Debug::trace("NGWUpdateManager", "Exiting thread" );
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::timeoutProcess(__int32 /*timer_id*/)
{
	std::lock_guard<std::mutex> g( mutex_ );

	Debug::trace( "NGWUpdateManager", "timer expired for state " + Integer::toString(state_) );

	if ( _WaitForStatus == state_ )
	{
		Debug::trace("NGWUpdateManager.Error",
				"Node has not responded to request for status message.");
		state_ = _Exit;

		++retryCounter;
		if( retryCounter < numRetry )
		{
			Debug::trace( "NGWUpdateManager", "retryCounter = " + Integer::toString(retryCounter) );
			state_ = _RequestStatus;
		}
	}

	else if ( _WaitForVersion == state_ )
	{
		Debug::trace("NGWUpdateManager.Error",
				"Node has not responded to request for version message.");

		state_ = _Exit;
	}

	else if ( _WaitForUI == state_ )
	{
		Debug::trace( "NGWUpdateManager", "Dispatch start UI message." );

		NGWGeneric genMsg(_UpgradeRequired, this);
		dispatchEvent(&genMsg);

		// remain in wait state... message is dispatched every n seconds...
	}

	else if (( _WaitForReady == state_ ) ||
		( _WaitForAck == state_ ) ||
		( _WaitForVerify == state_ ))
	{
		Debug::trace("NGWUpdateManager.Error",
				"Node has failed to respond during upgrade!");

		// notify the UI of the node error
		NGWUpgradeStatus msg(this, currentNode() );
		msg.setVersion(currentNodeVersion());
		msg.setStateError();
		msg.setErrorTimeout();

		dispatchEvent(&msg);

		state_ = _ErrorNode;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg )
{
	if ( !msg )
	{
        Debug::trace( "NGWUpdateManager.Error", "Received NULL message" );
        return;
	}

	std::lock_guard<std::mutex> guard( mutex_ );

	switch ( msg->getDeviceType() )
	{
	case Wrapper:
		handleUpdaterEvent((WrEvent) msg->getEventType(), msg->Data());
		break;

	case NGWrapper:
		handleUIEvent((NGWEvents) msg->getEventType(), msg->Data());
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::handleUpdaterEvent( WrEvent wrEvent, const char *recvBuffer )
{
	Debug::trace( "NGWUpdateManager", "handleUpdaterEvent for state " + Integer::toString(state_) );

	switch (wrEvent)
	{
	case WrNGWBootStatus:
	{
        const NGWBootStatus bs( recvBuffer );

        if ( bs.getNodeId() != currentNode() )
        {
			Debug::trace("NGWUpdateManager.Error", "Unexpected node response " +
					Integer::toString(bs.getNodeId()) + HobString( " expected ") + Integer::toString(currentNode()));
			return;
        }

        switch ( bs.getStatus() )
        {
        case NGWBootStatus::INITIALIZED:
			Debug::trace("NGWUpdateManager", "Node init " + Integer::toString(currentNode()));

        	if ( _WaitForStatus == state_ )
				state_ = _RequestVersion;

        	break;

        case NGWBootStatus::READY:
			Debug::trace("NGWUpdateManager", "Node loader Ready. Switching state to: _Upgrade");
        	state_ = _Upgrade;
        	break;

        case NGWBootStatus::FAULT:
        {
        	const NGWBootStatus::LoaderFaults fault( bs.getFault() );

			Debug::trace("NGWUpdateManager", "Node " + Integer::toString(currentNode()) + " fault " + Integer::toString(fault));

        	if ( _WaitForStatus == state_ )
			{
				// allow upgrade if fault is a node image mismatch or the image checksum does not match.
				if (( NGWBootStatus::NodeImageMismatch == fault ) ||
					( NGWBootStatus::ImageChecksumMismatch == fault ))
				{
					Debug::trace("NGWUpdateManager", "NodeImageMismatch or ImageChecksumMismatch fault.");

					forceUpgrade_ = true;
					state_ = _RequestVersion;

					nodeFaults_[currentNode()] = true;
				}
			}

        	else if (( _EndNodeProg == state_) || ( _WaitForVerify == state_ ))
        	{
				Debug::trace("NGWUpdateManager.Error", "NGWBootStatus::NodeImageMismatch: " + Integer::toString(NGWBootStatus::NodeImageMismatch));
				Debug::trace("NGWUpdateManager.Error", "NGWBootStatus::ImageChecksumMismatch: " + Integer::toString(NGWBootStatus::ImageChecksumMismatch));

				Debug::trace("NGWUpdateManager.Error", "Fault detected for Node: " + Integer::toString(currentNode()));
				Debug::trace("NGWUpdateManager.Error", "Loader Status: " + Integer::toString(bs.getStatus()));
				Debug::trace("NGWUpdateManager.Error", "Loader Fault: " + Integer::toString(fault));
				Debug::trace("NGWUpdateManager.Error", "Loader Fault Address: " + Integer::toString(bs.getFaultAddress()));
				Debug::trace("NGWUpdateManager.Error", "State Machine State: " + Integer::toString(state_));

				// notify the UI of the node error.
				NGWUpgradeStatus msg(this, currentNode());
				msg.setVersion(currentNodeVersion());
				msg.setStateError();

				if ( NGWBootStatus::Checksum == fault )
					msg.setErrorChecksum();
				else if ( NGWBootStatus::Timeout == fault )
					msg.setErrorTimeout();
				else if ( NGWBootStatus::WriteFailure == fault )
					msg.setErrorWriteFailure();
				else if ( NGWBootStatus::EraseFailureBank1 == fault )
					msg.setErrorEraseFailure1();
				else if ( NGWBootStatus::EraseFailureBank2 == fault )
					msg.setErrorEraseFailure2();
				else if ( NGWBootStatus::NodeIdError == fault )
					msg.setErrorNodeIdFailure();
				else if ( NGWBootStatus::WriteAddressOutOfRange == fault )
					msg.setErrorWriteOUtOfRange();

				Debug::trace("NGWUpdateManager", "Node Error Status:" + Integer::toString(msg.getError()));
				msg.show();

				dispatchEvent(&msg);

				state_ = _ResetNode;
			}

        	else
        	{
				Debug::trace("NGWUpdateManager.Error", "Caught Error while in: _EndNodeProg ");
			}

        	break;
		}

        case NGWBootStatus::VERIFY:
        {
        	const NGWBootStatus::LoaderFaults fault( bs.getFault() );

			Debug::trace("NGWUpdateManager", "Node " + Integer::toString(currentNode()) + " verify " + Integer::toString(fault));

			if (( NGWBootStatus::NodeImageMismatch == fault ) ||
				( NGWBootStatus::ImageChecksumMismatch == fault ))
			{
				Debug::trace("NGWUpdateManager", "Node verification of firmware image failed! " + Integer::toString(fault));

				// notify the UI of the node error.
				NGWUpgradeStatus msg(this, currentNode());
				msg.setVersion(currentNodeVersion());
				msg.setStateError();
				msg.setErrorChecksum();
				dispatchEvent(&msg);

				state_ = _ResetNode;
			}

			else
			{
				Debug::trace("NGWUpdateManager", "Valid Node image for node: " + Integer::toString(bs.getNodeId()));

				++currentNodeIndex_;
				state_ = _NextNode;
			}

			break;
        }

        default:
			Debug::trace("NGWUpdateManager.Error", "Bad boot loader status " + Integer::toString(bs.getStatus()));
			break;
        }

		break;
	}

	case WrNGWBootVersion:
	{
		const NGWBootVersion bv(recvBuffer);

        if ( bv.getNodeId() != currentNode() )
        {
			Debug::trace("NGWUpdateManager.Error", "Unexpected node response " +
					Integer::toString(bv.getNodeId()) + HobString( " expected ") + Integer::toString(currentNode()));
			return;
        }

       	Debug::trace("NGWUpdateManager",
       		"boot version received, state: " + Integer::toString(state_));

		std::string nodeAppVersion( bv.getAppVersionString() );

		if ( nodeAppVersion == "65535.65535" )
			nodeAppVersion = "0.0";

		nodeVersions_[currentNode()] = nodeAppVersion;

		// check for node fault
		if ( nodeFaults_.end() != nodeFaults_.find( currentNode() ) )
			nodeVersions_[currentNode()] = "FAULT";

		// notify the UI of the node version number
		NGWUpgradeStatus msg(this, currentNode());
		msg.setStateVersion();
		msg.setVersion(currentNodeVersion());
		msg.setErrorNone();
		dispatchEvent(&msg);

		Debug::trace("NGWUpdateManager",
				"Version message received for node: " + Integer::toString(currentNode()) +
				" Application version: " + currentNodeVersion());

		// process next node
		++currentNodeIndex_;
		state_ = _RequestStatus;

		break;
	}
	case WrNGWAck:
	{
		Debug::trace("NGWUpdateManager", "Ack: " + Integer::toString(currentNode()) );

		NGWFlush wrFlush(this);
		wrFlush.setNodeId( currentNode() );
		wrFlush.setSourceId(255);
		wrFlush.setSourceObject(this);
		dispatchEvent(&wrFlush);

		// transfer is complete node is ready for next record
		if ( _WaitForAck == state_ )
			state_ = _Upgrade;

		break;
	}

	case WrNGWFlush:
	{
		Debug::trace("NGWUpdateManager", "Flush: " + Integer::toString(currentNode()) );

		NGWAck wrAck(this);
		wrAck.setNodeId( currentNode() );
		wrAck.setSourceId(255);
		wrAck.setSourceObject(this);
		dispatchEvent(&wrAck);

		break;
	}

	default:
		break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::handleUIEvent( NGWEvents ngEvent, const char* /*recvBuffer*/ )
{
	Debug::trace( "NGWUpdateManager", "handleUIEvent" );

	switch (ngEvent)
	{
	case _UpgradeUIReady:

		// UI has signaled it is ready, we can start the upgrade process.
		if ( _WaitForUI == state_ )
		{
			Debug::trace( "NGWUpdateManager", "UI has signaled it is ready!" );
			state_ = _StartUpgrade;
		}

		break;

	case _UpgradeCancel:

		Debug::trace( "NGWUpdateManager", "UI has signaled to cancel upgrade! State: " + Integer::toString(state_));

		if ( _WaitForUI == state_ )
			state_ = _StartNode;

		break;

	case _ReqUpgradeInfo:

		Debug::trace( "NGWUpdateManager", "UI has requested upgrade info! State: " + Integer::toString(state_));

		if ( _WaitForUI == state_ )
		{
			NGWUpgradeInfo::nodeIdList nodeIds;
			NGWUpgradeInfo::nodeVersionList nodeVersions;

			NGWUpgradeInfo::nodeVersionList firmwareVersions;

			for ( size_t n( 0 ); n < nodes_.size(); ++n )
			{
				const NGWWrapperDevices node( nodes_[n] );

				nodeIds.push_back( node );
				nodeVersions.push_back( nodeVersions_[node] );

				firmwareVersions.push_back( firmwareVersions_[node] );
			}

			// notify the UI of the node version number
			NGWUpgradeInfo msg(this);
			msg.setNodeIds(nodeIds);
			msg.setNodeVersions(nodeVersions);
			msg.setFirmwareVersions(firmwareVersions);
			msg.setRequired(forceUpgrade_);
			dispatchEvent(&msg);
		}

		break;

	default:
		break;

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NGWUpdateManager::checkForUpdate(NGWWrapperDevices device)
{
	std::string availableVersion;
	std::vector<unsigned char> imageBuffer;

	bool updateRequired( false );
	bool scaleStyleError( false );

	// SOF-1063
	std::string upgradePath( upgradePathNGW_ );

	if (( 10 <= scaleStyle.length() ) && ( '-' == scaleStyle[4] ))
	{
		if ( "NGW" != scaleStyle.substr( 0, 3 ) )
		{
			scaleStyleError = true;
		}
	}
	else
	{
		scaleStyleError = true;
	}

	if ( scaleStyleError )
	{
		util_filesystem::File ngwImageFile( upgradePathNGW_ + wrapperDirectory_ + "node1/" + "source.bin" );
		util_filesystem::File ngsImageFile( upgradePathNGS_ + wrapperDirectory_ + "node1/" + "source.bin" );

		if ( ngwImageFile.exists() )
			upgradePath = upgradePathNGW_;
		else
			Debug::trace( "NGWUpdateManager", "UNKNOWN SCALE STYLE!!" );
	}

	std::string imagePath( upgradePath + wrapperDirectory_ );

	if ( device == _Node1 )
		imagePath += "node1/";
	else if ( device == _Node2 )
		imagePath += "node2/";
	else if ( device == _Node3 )
		imagePath += "node3/";
	else if ( device == _Node4 )
		imagePath += "node4/";
	else if ( device == _Node5 )
		imagePath += "node5/";
	else if ( device == _Node6 )
		imagePath += "node6/";

	Debug::trace( "NGWUpdateManager", "Source path = " + imagePath );

	util_filesystem::File imageFile( imagePath, "source.bin" );
	util_filesystem::File iniFile( imagePath,  "version.ini" );

	if (( !imageFile.exists() ) || ( !iniFile.exists() ))
		Debug::trace( "NGWUpdateManager", "No source image found." );
	else
	{
		// retrieve image version from file
		Debug::trace( "NGWUpdateManager", "Source image exists." );

		std::ifstream verFile(iniFile.getFullPath().c_str(),
			std::ifstream::in | std::ifstream::binary);
		std::getline( verFile, availableVersion );

		availableVersion.erase(
			std::remove(availableVersion.begin(), availableVersion.end(),
				'\n'), availableVersion.end());
		availableVersion.erase(
			std::remove(availableVersion.begin(), availableVersion.end(),
				'\r'), availableVersion.end());

		// wrapper node version is in the form X.Y with a major and minor version only... for some
		// reason source versions may have a third version (i.e. X.Y.Z) so lets check and strip if
		// it exists
		const size_t pos0( availableVersion.find_first_of( '.' ) );
		const size_t pos1( availableVersion.find_last_of( '.' ) );

		if ( pos0 != pos1 )
		{
			Debug::trace( "NGWUpdateManager", "badly formed version " + availableVersion );
			availableVersion.erase( pos1 );
		}

		// check versions
		Debug::trace( "NGWUpdateManager", "Target version: " + nodeVersions_[device] );
		Debug::trace( "NGWUpdateManager", "Source version: " + availableVersion );

		if ( availableVersion == nodeVersions_[device] )
			 Debug::trace( "NGWUpdateManager", "Versions match." );
		else
		{
			Debug::trace("NGWUpdateManager", "Version mismatch upgrade required.");

			std::ifstream upgradeFile(imageFile.getFullPath().c_str(),
					std::ifstream::in | std::ios::binary);

			/*stop removing my data! */
			upgradeFile.unsetf(std::ios::skipws);

			const long64 imageFileSize( imageFile.size() );

			if ( imageFileSize )
			{
				imageBuffer.reserve( imageFileSize );

				imageBuffer.assign(
					std::istream_iterator<unsigned char>(upgradeFile),
					std::istream_iterator<unsigned char>() );
			}

			updateRequired = true;
		}
	}

	firmwareVersions_[device] = availableVersion;
	firmwareImages_[device] = imageBuffer;

	return updateRequired;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NGWUpdateManager::NGWWrapperDevices NGWUpdateManager::currentNode() const
{
	if ( currentNodeIndex_ < nodes_.size() )
		return nodes_[currentNodeIndex_];

	return _InvalidNode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string NGWUpdateManager::currentNodeVersion() const
{
	DeviceListVersions::const_iterator n( nodeVersions_.find( currentNode() ) );

	if ( nodeVersions_.end() == n )
		return std::string();

	return n->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NGWUpdateManager::updateDevice(NGWWrapperDevices device, bool &done)
{
	// this section supports straight binary image file
	const size_t imageSize( firmwareImages_[device].size() );

	Debug::trace( "NGWUpdateManager",
		"updateDevice " + Integer::toString(device) +
		" offset " + Integer::toString(offset_) +
		" / " + Integer::toString(imageSize) );

	if ( imageSize <= offset_ )
	{
		Debug::trace( "NGWUpdateManager", "Download complete" );
		done = true;

		return true;
	}

	size_t chunk( imageSize - offset_ );

	if ( NGWBulkTransfer::MAX_PAYLOAD_SIZE < chunk )
		chunk = NGWBulkTransfer::MAX_PAYLOAD_SIZE;

	processImageRecord( device, &((firmwareImages_[device])[offset_]), chunk );

	const size_t oldProgress( (offset_ * 100) / imageSize );
	offset_ += NGWBulkTransfer::MAX_PAYLOAD_SIZE;

	const size_t newProgress( (offset_ * 100) / imageSize );

	// inform the UI node download progress
	if ( oldProgress != newProgress )
	{
		NGWUpgradeProgress msg(this, device, (UINT8)newProgress);
		msg.setSourceObject(this);
		dispatchEvent(&msg);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::processImageRecord(NGWWrapperDevices device, const unsigned char *record, size_t recordSize )
{
	//Debug::trace( "NGWUpdateManager", "processImageRecord " + Integer::toString(device) );

	// determine the number of frames needed to send record
	if ( 0 == (recordSize % 8) )
	{
		NGWBulkTransfer bulkMsg(this, record, PROGRAM_FLASH_CMD, (int)(recordSize / 8), 0, device);
		dispatchEvent(&bulkMsg);
	}
	else
	{
		NGWBulkTransfer bulkMsg(this, record, PROGRAM_FLASH_CMD, (int)((recordSize / 8) + 1), (int)(recordSize % 8), device);
		dispatchEvent(&bulkMsg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT8 NGWUpdateManager::asciiToDec( UINT8 value )
{
	UINT8 result;

	if (( '0' <= value ) && ( value <= '9' ))
		result = value - '0';
	else
		result = value - '7';

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NGWUpdateManager::replaceAll( std::string &s, const std::string &from, const std::string &to )
{
	const size_t len( from.length() );

	size_t pos( std::string::npos );

	while ( std::string::npos != (pos = s.find_last_of( from, pos )) )
		s.replace( pos, len, to );
}

