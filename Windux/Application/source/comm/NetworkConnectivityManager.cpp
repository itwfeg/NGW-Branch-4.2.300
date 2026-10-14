/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NetworkConnectivityManager.h"
#include "NetworkConnectivityEvents.h"
#include "NetworkStatus.h"
#include "Configuration.h"
#include "ConfigurationEvent.h"
#include "Hex.h"
#include "TimerManager.h"
#include "Ping.h"
#include "SessionMessage.h"
#include "GenericHfcObject.h"
#include "HobString.h"
#include "PythonScripter.h"
#include "HobTokenizerCSV.h"

#ifdef PAL_WIN32_RUN
#include <Sensapi.h>
#include <iphlpapi.h>
#endif

#define SECONDS_PER_MINUTE 60

enum NetworkType { wired, wireless };
#define MAX_WIRELESS_STRENGTH 10


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NetworkConnectivityManager::_dataType_ = 0;
std::string NetworkConnectivityManager::_dataTypeName_("NetworkConnectivityManager"); 
#endif



NetworkConnectivityManager::NetworkConnectivityManager()
	: Hobart::CEvolMsgReceiver("NetworkConnectivityManager"),
		online_(true), checkEnabled_(false), networkTrafficOnly_(false), networkType_(-1), wirelessStrength_(-1),
		onlineTimerId_(0), onlinePingTimerId_(0), offlineCheckingPingTimerId_(0),
		commOutState_(FORCE_ONE_COMM_MSG), onlineStateKnown_(true, false, _T ("onlineStateKnown")),
		offLineTimeCounter_(0), offlinePingCounter_(0), pingAttempts_(1), pingTarget_(""), weAreBootedUp_(false), lastActivity_(Activity_Normal)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

	Debug::trace("NetworkConnectivityManager", "NetworkConnectivityManager()");

	lastInBusyStat_ = 0;
	lastOutBusyStat_ = 0;

#if defined ( __linux__ )
	inBusyThreshold_ = 500;
	outBusyThreshold_ = 500;
#else
	inBusyThreshold_ = 1000;
	outBusyThreshold_ = 1000;
#endif

	networkConnectivityManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "NetworkConnectivityManager Thread" , false, false, false);			
    if (networkConnectivityManagerThread)
        networkConnectivityManagerThread->RestartThread();
}


NetworkConnectivityManager::~NetworkConnectivityManager()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

    Debug::trace("NetworkConnectivityManager", "~NetworkConnectivityManager");

	if (onlinePingTimerId_)
	{
		TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
		onlinePingTimerId_ = 0;
	}
	if (offlineCheckingPingTimerId_)
	{
		TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
		offlineCheckingPingTimerId_ = 0;
	}

    deregisterAllEvents();

	if (networkConnectivityManagerThread)
	{
		networkConnectivityManagerThread->StopThread();
		networkConnectivityManagerThread->WaitForCompletion();
	}
	ShutdownNicely();
}


Util::CSmartPtr< EvolMessage> NetworkConnectivityManager::processEventExternal(const Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	Configuration *config = Configuration::getInstance();
	if (!config) { Util::CSmartPtr<EvolMessage> evolMsg ( NULL ); return evolMsg; }

	bool isTracing = Debug::isTracing("NetworkConnectivityManager");

    EvolMessage evolMessage( msg->Data() );
    switch ( evolMessage.getDeviceType() )
    {
		case ConfigurationDev:
		{
			switch ( evolMessage.getEventType() )
			{
				case CfgSaved:
				{
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - Received CfgSaved message");

					if ( onlinePingTimerId_ && currentPingIntervalValue_ != config->getPingIntervalValue() )
					{
						// Delete the timer used for multiple pings to verify we are still online.
						// It will be recreated the next time the network is checked in the Run method.
						TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
						onlinePingTimerId_ = 0;
					}

					if (config->toBoolCfg(fieldOffsetCfgCheckNetworkConnectivity))
					{
						setupPingTarget();

						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "process CfgSaved - PING target is '" + getPingTarget() + "'");

						if (!checkEnabled_)
						{
							checkEnabled_ = true;

							EvolMessage offContNetworkStatus( SessionDev, StartContinuousNetworkStatus, this );
							dispatchEvent( &offContNetworkStatus );

							if (online_)
							{
								if (isTracing)
									Debug::trace("NetworkConnectivityManager", "process CfgSaved - Status Online so dispatch GoodComm message to session.");

								EvolMessage pCommStatus( NetworkConnectivityDev, GoodComm, this );
								dispatchEvent( &pCommStatus );
							}
							else
							{
								if (isTracing)
									Debug::trace("NetworkConnectivityManager", "process CfgSaved - Status Offline so dispatch BadComm message to session.");

								EvolMessage pCommStatus( NetworkConnectivityDev, BadComm, this );
								dispatchEvent( &pCommStatus );
							}

							dispatchNetworkStatus();
						}

						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "process CfgSaved - Network Connectivity Check enabled in configuration.");
					}
					else
					{
						/*we need to check to see if this is a transition from enabled to disabled.
						  if this is a transition then we need to stop the thread that does the checking.*/

						if (checkEnabled_)
						{
							checkEnabled_ = false;
							online_ = true;

							if (onlinePingTimerId_)
							{
								// Delete the timer used for pinging to verify we are online/offline.
								TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
								onlinePingTimerId_ = 0;
							}
							if (offlineCheckingPingTimerId_)
							{
								// Delete the timer used for pinging to verify we are online/offline.
								TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
								offlineCheckingPingTimerId_ = 0;
							}

							if (isTracing)
								Debug::trace("NetworkConnectivityManager", "process CfgSaved - Network Connectivity now disabled (previously enabled) in configuration.");

							// We are no longer checking. Thus we need to ensure that the background color will never be red.
			                
							EvolMessage * offContNetworkStatus = new EvolMessage(SessionDev, StopContinuousNetworkStatus, this);
							dispatchEvent(offContNetworkStatus);

							EvolMessage * pCommStatus = new EvolMessage(NetworkConnectivityDev, GoodComm, this);
							dispatchEvent(pCommStatus);

							dispatchNetworkStatus();
						}
					}
					break;
				}

				default:
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - received unknown ConfigurationDev event.");
					break;
			}
			break;
        }

        case NetworkConnectivityDev:
        {
			switch ( evolMessage.getEventType() )
			{
				case RequestNetworkType:
				{
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - Received RequestNetworkType message.");

					// NetworkType:  0 = wired, 1 = wireless
					int networkType = 0;
					if ( config->toBoolCfgI(CfgInternal::PYTHONENABLED) )
					{
						const string command( PythonScripter::buildPythonCall("getConnectionType.py") );
						string connectionTypeOutput;
						PAL_absclass::PAL_executeWithResults(command, connectionTypeOutput);
						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "process RequestNetworkType - Data received from getConnectionType.py: " + connectionTypeOutput);

						if ( !connectionTypeOutput.empty() )
							networkType = Integer::valueOf(connectionTypeOutput.substr(0,1));
					}

					NetworkStatus ns( this, 0, networkType, 0 );
					ns.setEventType(NetworkType);
					dispatchEvent(&ns);
					break;
				} 

				case RequestCurrentStatus:
				{
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - Received RequestCurrentStatus message.");

					dispatchNetworkStatus(false);
						
					/* This block has been commented out and replaced by the line directly above, to just dispatch the current status.
					if (online_ || !checkEnabled_)
					{
						Debug::trace("NetworkConnectivityManager.processEvent", "Status Online (or network check not enabled) so dispatch GoodComm message to session.");

						EvolMessage * pCommStatus = new EvolMessage(NetworkConnectivityDev, GoodComm, this);
						Util::CSmartPtr<EvolMessage> evolMsg ( pCommStatus );
						return evolMsg;
					}
					else
					{
						Debug::trace("NetworkConnectivityManager.processEvent", "Status Offline so dispatch BadComm message to session.");

						EvolMessage * pCommStatus = new EvolMessage(NetworkConnectivityDev, BadComm, this);
						Util::CSmartPtr<EvolMessage> evolMsg ( pCommStatus );
						return evolMsg;
					}
					*/
					break;
				} 

				case NetworkSyncStarted:
				{
					weAreBootedUp_ = true;			
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - NetworkSyncStarted event - start networkAvailable timer for synchronization.");
					onlineStateKnown_.EventReset();
					onlineTimerId_ = TimerManager::getInstance()->addTimer(this, 5, Timer::RepeatNO, timedFuncNetworkOnlineCheck);	
					break;					
				}			

				default:
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - received unknown NetworkConnectivityDev message.");
					break;
			}
            break;
		}

        case TimerDev:
		{
			if ( evolMessage.getEventType() == TimerExpired )
			{
				Timeout timeout( msg->Data() );
				if ( timeout.getTimerId() == onlineTimerId_ )
				{
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "processEventExternal - onlineTimerId_ timer timed out.");

					TimerManager::getInstance()->removeTimer(onlineTimerId_);
					onlineTimerId_ = 0;

					onlineStateKnown_.EventSet();

					if (online_)
					{
						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "process onlineTimerId_ - We are still online so dispatch NetworkAvailable.");

						EvolMessage commStatus(NetworkConnectivityDev, NetworkAvailable, this);
						dispatchEvent(&commStatus);
					}
				}
				else if ( timeout.getTimerId() == onlinePingTimerId_ || timeout.getTimerId() == offlineCheckingPingTimerId_ )
				{
					if (networkTrafficOnly_)
					{
						// This timer shouldn't even exist, so ignore it and delete it.
						if (onlinePingTimerId_)
						{
							TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
							onlinePingTimerId_ = 0;
						}
						if (offlineCheckingPingTimerId_)
						{
							TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
							offlineCheckingPingTimerId_ = 0;
						}
					}
					else
					{
						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "process xxxPingTimerId_ - Will now ping due to timer going off - offlinePingCounter_: " + inttostring(offlinePingCounter_) );

						bool pingable = isNetworkPingable();
						if ( online_ && !pingable )
						{
							if ( 0 == offlinePingCounter_ )
							{
								if (isTracing)
									Debug::trace("NetworkConnectivityManager", "process nextPingTimerId_ - During online ping, the ping failed, so starting process to go offline");

								offlinePingCounter_++;

								if (onlinePingTimerId_)
								{
									// Delete the timer used for periodic online pings.
									TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
									onlinePingTimerId_ = 0;
								}

								// Create a timer that will initiate the next offline ping, to verify we're offline.
								offlineCheckingPingTimerId_ = TimerManager::getInstance()->addTimer(this, config->toIntCfgI(CfgInternal::OFFLINEPINGCONNECTIVITYFREQUENCY), Timer::RepeatInMinutes, timedFuncNetworkOnlineCheck);
							}
							else
							{
								offlinePingCounter_++;

								if (isTracing)
									Debug::trace("NetworkConnectivityManager", "process nextPingTimerId_ - incremented offlinePingCounter_ to: " + inttostring(offlinePingCounter_) );

								if ( offlinePingCounter_ >= config->toIntCfgI(CfgInternal::OFFLINEPINGCONNECTIVITYATTEMPTS) )
								{
									if (isTracing)
										Debug::trace("NetworkConnectivityManager", "process nextPingTimerId_ - Now we're changing to offline: ");

									if (onlinePingTimerId_)
									{
										// Delete the timer used for periodic online pings.
										TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
										onlinePingTimerId_ = 0;
									}
									if (offlineCheckingPingTimerId_)
									{
										// Delete the timer used for multiple pings to verify we are offline.
										TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
										offlineCheckingPingTimerId_ = 0;
									}

									if (commOutState_ == CHECK_FOR_TRANSITIONS)
										transitionFromOnlineToOffline();

									online_ = false;
								}
							}
						}
						else
						{
							// We were able to ping, so remain online

							// If we detected that the network interface was dead and were checking our pingability before officially going offline,
							// then we need to delete the 'offline checking' ping timer, and create the normal 'online checking' ping timer.
							if (offlinePingCounter_)
							{
								offlinePingCounter_ = 0;

								if (offlineCheckingPingTimerId_)
								{
									// Delete the timer used for multiple pings to verify we are offline.
									TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
									offlineCheckingPingTimerId_ = 0;
								}

								// Create a timer that will initiate the next ping of the network, to see if we're still up.
								if ( checkEnabled_ )
								{
									currentPingIntervalValue_ = config->getPingIntervalValue();
									onlinePingTimerId_ = TimerManager::getInstance()->addTimer(this, (currentPingIntervalValue_ * SECONDS_PER_MINUTE), Timer::RepeatInMinutes, timedFuncNetworkOnlineCheck);
								}
							}
						}
					}
				}
			}
			break;
		}
		
		default:
			if (isTracing)
				Debug::trace("NetworkConnectivityManager", "received unknown DevType message.");
			break;
    }

    Util::CSmartPtr<EvolMessage> evolMsg ( NULL );
    return evolMsg;
}

void NetworkConnectivityManager::dispatchNetworkStatus(bool pingToCheckIfConnected)
{
	bool isTracing = Debug::isTracing("NetworkConnectivityManager");

	Configuration *config = Configuration::getInstance();
	if (!config) return;

	if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
	{
		HobString pingTarget;
		string cmd;

		if (!pingToCheckIfConnected || !checkEnabled_ || networkTrafficOnly_)
			cmd.append(" -P 0 ");
		else
		{
			if (!config->toBoolCfg(fieldOffsetCfgPingGateway))
				pingTarget = getPingTarget();
			if ( !pingTarget.empty() && pingTarget != "0.0.0.0")
			{
				pingTarget.quote();
				cmd.append(" -A ").append(pingTarget);
			}
		}

		const HobString command( PythonScripter::buildPythonCall("getConnectionStatus.py", -1, cmd) );
		string connectionStatusOutput;

		if (isTracing)
	 		Debug::trace("NetworkConnectivityManager", "dispatchNetworkStatus: calling: " + command);

		PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);

		if (connectionStatusOutput.size())
		{
			if (isTracing)
				Debug::trace("NetworkConnectivityManager", "dispatchNetworkStatus: output from command (signalStrength,networkType,onlineFlag): " + connectionStatusOutput);

			HobTokenizerCSV tok(&connectionStatusOutput, BrowserFormatSeparator);
			HobString token;
			if (tok.next(&token) != -1)  // First is wireless strength
			{
				wirelessStrength_ = Integer::valueOf(token);

				if (tok.next(&token) != -1)  // Second is networkType:  0 = wired, 1 = wireless
				{
					networkType_ = Integer::valueOf(token);

					if (tok.next(&token) != -1)  // Third and last is online flag, which we aren't going to use for now; we get the online flag from online_ var.
					{
						bool alive = Bool::valueOf(token);
					}
				}
			}
		}
	}

	// Commented this out; the signal strength and online status are considered to be independent of each other.
	// force network to zero strength when not connected
	//if (( wireless == networkType_ ) && ( !online_ ))
	//	wirelessStrength_ = 0;

	NetworkStatus ns( this, online_, networkType_, wirelessStrength_ );
	dispatchEvent(&ns);
}

void NetworkConnectivityManager::setupPingTarget()
{
	Configuration *config = Configuration::getInstance();
	if (!config) return;
	
    pingAttempts_ = config->toIntCfgI(CfgInternal::PINGCONNECTIVITYATTEMPTS);
    Debug::trace("NetworkConnectivityManager", "Ping attempts set to " + util_string::inttostring(pingAttempts_));

	std::string pingTargetAddress("");
	networkTrafficOnly_ = false;

    if (config->toBoolCfg(fieldOffsetCfgPingPrimary))
    {
        if ( (ScaleSynchronizationType)config->toIntCfg(fieldOffsetCfgScaleSynchronizationType) == ScaleSynchronizationTypeSecondary )
			pingTargetAddress = config->toStringCfg(fieldOffsetCfgPrimaryScale);
    }
    else if (config->toBoolCfg(fieldOffsetCfgPingGateway))
    {
		getDefaultGateway(pingTargetAddress);
    }
    else if (config->getPingTarget())		//	GCDR-138
	{
		HobIpAddress pingTarget = config->getPingTargetIPAddress();
		pingTarget.getAsString(pingTargetAddress);
	}
	else
		networkTrafficOnly_ = true;

	Debug::trace("NetworkConnectivityManager", "setupPingTarget():  pingTargetAddress = " + pingTargetAddress);

	setPingTarget(pingTargetAddress);
}


void NetworkConnectivityManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    Util::CSmartPtr< EvolMessage> evolMsg( processEventExternal( msg ));

    if ( evolMsg.notNull() )
        dispatchEvent(evolMsg.get());
}


DWORD NetworkConnectivityManager::Run()
{
	bool isTracing = Debug::isTracing("NetworkConnectivityManager");

	Configuration *config = Configuration::getInstance();
	if (!config)
		return 0;

    registerEvent(NetworkConnectivityDev, RequestCurrentStatus);
	registerEvent(NetworkConnectivityDev, RequestNetworkType);
	registerEvent(NetworkConnectivityDev, NetworkSyncStarted);
    registerEvent(ConfigurationDev, CfgSaved);

    if (config->toBoolCfg(fieldOffsetCfgCheckNetworkConnectivity))
    {
        setupPingTarget();

        checkEnabled_ = true;
		online_ = false;  // Start out as if we're offline

		if (isTracing)
	        Debug::trace("NetworkConnectivityManager", "Network Connectivity startup, checking enabled in configuration.");
    }
    else
    {
        checkEnabled_ = false;
		online_ = true;

		if (isTracing)
	        Debug::trace("NetworkConnectivityManager", "Network Connectivity startup, checking disabled in configuration.");
    }
 
	if (isTracing)
		Debug::trace("NetworkConnectivityManager.Run", std::string("Starting Thread. Id = ") + Hex::toString(networkConnectivityManagerThread->GetThreadID()));

    commOutState_ = FORCE_ONE_COMM_MSG;

	bool pingable = true;

    while (!m_ExitEvent.IsEventSet())
    {
        if (isNetworkAlive())
        {
			if (isTracing)
				Debug::trace("NetworkConnectivityManager", "Network interface is Alive.");

			if ( online_ )
			{
				if ( 0 == onlinePingTimerId_ && 0 == offlineCheckingPingTimerId_ && checkEnabled_ && !networkTrafficOnly_ )
				{
					// Create a timer that will initiate the next ping of the network, to see if we're still up.
					// This is here just in case this timer doesn't already exist, which it should.
					currentPingIntervalValue_ = config->getPingIntervalValue();
					onlinePingTimerId_ = TimerManager::getInstance()->addTimer(this, (currentPingIntervalValue_ * SECONDS_PER_MINUTE), Timer::RepeatInMinutes, timedFuncNetworkOnlineCheck);
				}
			}
			else
			{
				// If we were offline, but now the network interface is alive, do a ping to verify we are truly online.
				pingable = isNetworkPingable();
				if (pingable)
				{
					// If we detected that the network interface was dead and were checking our pingability before officially going offline,
					// then we need to delete the 'offline checking' ping timer, and create the normal 'online checking' ping timer.
					if (offlineCheckingPingTimerId_)
					{
						// Delete the timer used for multiple pings to verify we are offline.
						TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
						offlineCheckingPingTimerId_ = 0;
					}

					if ( 0 == onlinePingTimerId_ && checkEnabled_ )
					{
						// Create a timer that will initiate the next ping of the network, to see if we're still up.
						currentPingIntervalValue_ = config->getPingIntervalValue();
						onlinePingTimerId_ = TimerManager::getInstance()->addTimer(this, (currentPingIntervalValue_ * SECONDS_PER_MINUTE), Timer::RepeatInMinutes, timedFuncNetworkOnlineCheck);
					}

					offlinePingCounter_ = 0;

					// We are transitioning from off to on so dispatch goodComm message to session.
					if ( commOutState_ == FORCE_ONE_COMM_MSG || (commOutState_ == CHECK_FOR_TRANSITIONS && !online_) )
					{
						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "Transition from off to on."); 

						if (checkEnabled_)
						{
							if (isTracing)
								Debug::trace("NetworkConnectivityManager", "dispatch GoodComm message to session."); 

							EvolMessage commStatus(NetworkConnectivityDev, GoodComm, this);
							dispatchEvent(&commStatus);
						}

						if ( onlineTimerId_ == 0 && weAreBootedUp_)
						{
							if (isTracing)
								Debug::trace("NetworkConnectivityManager", "start networkAvailable timer.");

							onlineStateKnown_.EventReset();

							onlineTimerId_ = TimerManager::getInstance()->addTimer(this, 5, Timer::RepeatNO, timedFuncNetworkOnlineCheck);
						}
					}

					online_ = true;
				}
			}
        }
        else
        {
			if (isTracing)
				Debug::trace("NetworkConnectivityManager", "Network interface is Dead.");

			// Initialize this to false since we won't expect it to be successful, and because we won't ping if we're only checking for network traffic.
			pingable = false;
			// If the network interface says it's offline, verify it with a ping to be sure, but only if we were just online;
			// i.e. only do the ping when we are transtioning to offline, not if we've been offline.
			// We will try to ping the configurable number of times before we fully commit to being offline.
			// If we get just one ping response from these tries, we will remain 'online'.
			if ( online_ && !networkTrafficOnly_ )
			{
				if ( 0 == offlinePingCounter_ )
				{
					if (isTracing)
						Debug::trace("NetworkConnectivityManager", "Try first 'offline' ping since we were just online - offlinePingCounter_=0");

					pingable = isNetworkPingable();

					// If network interface indicates we're offline, and we can't ping, ...
					if (!pingable)
					{
						offlinePingCounter_++;

						if (onlinePingTimerId_)
						{
							// Delete the timer used for periodic online pings.
							TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
							onlinePingTimerId_ = 0;
						}

						// Create a timer that will initiate the next offline ping, to verify we're offline.
						offlineCheckingPingTimerId_ = TimerManager::getInstance()->addTimer(this, config->toIntCfgI(CfgInternal::OFFLINEPINGCONNECTIVITYFREQUENCY), Timer::RepeatInMinutes, timedFuncNetworkOnlineCheck);
					}
				}
			}
			else
			{
				if (isTracing)
					Debug::trace("NetworkConnectivityManager", "No network interface, and already offline or networkTrafficOnly_");

				if (isTracing)
					Debug::trace("NetworkConnectivityManager", "No network interface, and already offline or networkTrafficOnly_, and not pingable - so go offline");
				// Only do this block if we can't ping.  If the interface said we aren't communicating on the network,
				// but the ping succeeded (doesn't seem logical, but OK), then remain online.

				// If network interface is now down and we had a ping-timer setup, get rid of it; we're not going to ping if the interface is down.
				if (offlineCheckingPingTimerId_)
				{
					TimerManager::getInstance()->removeTimer(offlineCheckingPingTimerId_);
					offlineCheckingPingTimerId_ = 0;
				}
				if (onlinePingTimerId_)
				{
					TimerManager::getInstance()->removeTimer(onlinePingTimerId_);
					onlinePingTimerId_ = 0;
				}

				// We are transitioning from online to offline so dispatch a message to primary session.
				if (commOutState_ == FORCE_ONE_COMM_MSG || (commOutState_ == CHECK_FOR_TRANSITIONS && online_))
				{
					transitionFromOnlineToOffline();
				}
				else if (checkEnabled_ && (commOutState_ == CHECK_FOR_TRANSITIONS))
				{
					// We are already off line so just increment counter to see if we need to 
					// resend badComm message to session. this will cause the annoyance dialog to popup.

					// Note: if we don't want the annoyance dialog to popup, then the Session shouldn't register for BadCommRepeat messages,
					// which it doesn't register for for the HTi, because we have the blinking icon on the screen as the constant reminder.

					offLineTimeCounter_ += config->toIntCfgI(CfgInternal::NETWORKCONNECTIVITYOFFLINEWAITPERIODSECS);

					if (offLineTimeCounter_ >= config->toIntCfgI(CfgInternal::NETWORKCONNECTIVITYOFFLINERETRANSMISSIONPERIODSECS))
					{
						if (isTracing)
							Debug::trace("NetworkConnectivityManager", "offline for 5mins so dispatch BadCommRepeat message to session to notify UI.");

						EvolMessage commStatus(NetworkConnectivityDev, BadCommRepeat, this);
						dispatchEvent(&commStatus);
						offLineTimeCounter_ = 0;
					}
				}

				online_ = false;
			}
        }

        if (commOutState_ == FORCE_ONE_COMM_MSG)
            commOutState_ = CHECK_FOR_TRANSITIONS;
		
		Activity activity = checkNetworkActivity();
        if (lastActivity_ != activity)
        {
			if (Activity_Busy == activity)
			{
				EvolMessage commStatus(NetworkConnectivityDev, ActivityBusy, this);
				dispatchEvent(&commStatus);
			}
			else
			{
				EvolMessage commStatus(NetworkConnectivityDev, ActivityNormal, this);
				dispatchEvent(&commStatus);
			}
			lastActivity_ = activity;
        }

        PAL_absclass::PAL_waitForSingleObj(m_ExitEvent, config->toIntCfgI(CfgInternal::NETWORKCONNECTIVITYINTERFACEDETECTIONTIME) * 1000);
    }

    online_ = true;
    return 0;
}


bool NetworkConnectivityManager::isNetworkAlive()
{
	if ( !checkEnabled_ )
		return true;

	DWORD dwFlags;
    bool networkAlive = false;

    try
	{
		networkAlive = PAL_absclass::PAL_isNetworkAlive( &dwFlags );
    }
    catch(...)
    {
        Debug::trace("NetworkConnectivityManager.Run", "Got an exception in isNetworkAlive(). Continue!");
        LogCatch;
    }

	Debug::trace("NetworkConnectivityManager", "isNetworkAlive() - result is " + util_string::booltostring(networkAlive));
    return networkAlive;
}


bool NetworkConnectivityManager::isNetworkPingable()
{
	if ( !checkEnabled_ || networkTrafficOnly_ )
		return true;

    bool networkPingable = false;
    try
	{
		std::string pingTarget;
		if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgPingGateway) )
		{
			getDefaultGateway(pingTarget);
			if (pingTarget != getPingTarget())
				setupPingTarget();
		}
		else
			pingTarget = getPingTarget();

		if ( !pingTarget.empty() && pingTarget != "0.0.0.0")
		{
			//if we have some valid IP that isn't nothing, try to ping			
			networkPingable = Hobart::CPing::Ping(pingTarget, !HobIpAddress::isValidIPAddress(pingTarget), pingAttempts_);

			Debug::trace("NetworkConnectivityManager", "Ping of " + pingTarget + ((networkPingable) ? " SUCCEEDED!" : " FAILED!"));
		}
    }
    catch(...)
    {
        Debug::trace("NetworkConnectivityManager.Run", "Got an exception on ping. Continue!");
        LogCatch;
    }

	Debug::trace("NetworkConnectivityManager", "isNetworkPingable() - returning " + util_string::booltostring(networkPingable));
    return networkPingable;
}


void NetworkConnectivityManager::transitionFromOnlineToOffline()
{
    if (checkEnabled_)
    {
        Debug::trace("NetworkConnectivityManager", "Transition on to off, so dispatch BadComm message to session.");

        EvolMessage commStatus(NetworkConnectivityDev, BadComm, this);
        dispatchEvent(&commStatus);
    }

    Debug::trace("NetworkConnectivityManager", "Transition on to off, so dispatch NetworkUnavailable message.");

    EvolMessage commStatus(NetworkConnectivityDev, NetworkUnavailable, this);
    dispatchEvent(&commStatus);
    offLineTimeCounter_ = 0;

    onlineStateKnown_.EventSet();
}


void NetworkConnectivityManager::getDefaultGateway(std::string & defGWay)
{
	Configuration *config = Configuration::getInstance();
	if (!config) return;

	if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
	{	
		try
		{
			// Get Gateway from python call
			defGWay = "";

			const string command( PythonScripter::buildPythonCall("getGateway.py") );
			string getGatewayOutput;
			PAL_absclass::PAL_executeWithResults(command, getGatewayOutput);
			Debug::trace("NetworkConnectivityManager", "Python results of calling getGateway.py: " + getGatewayOutput);
			if (getGatewayOutput.size())
			{
				HobTokenizerCSV tok(&getGatewayOutput, '\n');
				HobString token;
				if (tok.next(&token) != -1)  // First line will be the gateway.
				{				
					defGWay = token;
					Debug::trace("NetworkConnectivityManager", "Python getDefaultGateway():  pingTargetAddress = " + defGWay);
				}
			}
		}
		catch(...)
		{
			Debug::trace("NetworkConnectivityManager.Run", "Got an exception on python gateway. Continue!");
			LogCatch;
		}
	}
	else
	{
		// This is what we used to do before the Python script method above.

		PIP_ADAPTER_INFO AdapterAddresses = NULL;
		ULONG OutBufferLength = 0;
		ULONG RetVal = 0, i;    

#if defined ( __linux__ )
		AdapterAddresses = new IP_ADAPTER_INFO;					
#endif

		for (i = 0; i < 4; i++) 
		{
			RetVal = PAL_absclass::PAL_getAdaptersInfo(AdapterAddresses, 
						&OutBufferLength);

			if (RetVal != ERROR_BUFFER_OVERFLOW) 
			{
				break;
			}

			if (AdapterAddresses != NULL) 
			{
				free(AdapterAddresses);
			}

			AdapterAddresses = (PIP_ADAPTER_INFO) malloc(OutBufferLength);
			if (AdapterAddresses == NULL) 
			{
				break;
			}
		}

		if (RetVal == ERROR_SUCCESS) 
		{
			PIP_ADAPTER_INFO AdapterList = AdapterAddresses;

			while (AdapterList)
			{
				if (strcmp(AdapterList->GatewayList.IpAddress.String, "") != 0)
				{
					defGWay = AdapterList->GatewayList.IpAddress.String;
					break;
				}
				AdapterList = AdapterList->Next;
			}
		}	
		delete AdapterAddresses;
	}
}


bool NetworkConnectivityManager::isThereADuplicateIpAddress()
{
    bool weHaveADuplicateIp = true;
    PIP_ADAPTER_INFO AdapterAddresses = NULL;
    ULONG OutBufferLength = 0;
    ULONG RetVal = 0, i;
    
#if defined ( __linux__ )
	AdapterAddresses = new IP_ADAPTER_INFO;					
#endif

    for (i = 0; i < 4; i++) 
    {
		RetVal = PAL_absclass::PAL_getAdaptersInfo(AdapterAddresses, 
                &OutBufferLength);

        if (RetVal != ERROR_BUFFER_OVERFLOW) 
        {
            break;
        }

        if (AdapterAddresses != NULL) 
        {
            free(AdapterAddresses);
        }

        AdapterAddresses = (PIP_ADAPTER_INFO) malloc(OutBufferLength);
        if (AdapterAddresses == NULL) 
        {
            break;
        }
    }

    if (RetVal == ERROR_SUCCESS) 
    {
        PIP_ADAPTER_INFO AdapterList = AdapterAddresses;

        while (AdapterList)
        {
            if (strcmp(AdapterList->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
            {
                weHaveADuplicateIp = false;
                break;
            }
            AdapterList = AdapterList->Next;
		}
	}
	delete AdapterAddresses;

	return weHaveADuplicateIp;
}


NetworkConnectivityManager::Activity NetworkConnectivityManager::checkNetworkActivity() const
{
    Activity level = Activity_Normal;

    DWORD inBusyStat = 0;
    DWORD outBusyStat = 0;

#if defined ( __linux__ )

	std::fstream devStream;
	devStream.open("/proc/net/dev", ios_base::in);
	unsigned int i;
	for (i = 0; !devStream.fail() && !devStream.eof(); ++ i)
	{
		char lineBuffer[1024] = { 0 };
		devStream.getline(lineBuffer, sizeof(lineBuffer));
		// start looking for interfaces at 3rd line
		if (i >= 2)
		{
			// parse out interface name
			std::string line = lineBuffer;
			size_t nonEmptyPos = line.find_first_not_of(" \t\n");
			size_t colonPos = line.find(':');
			if (nonEmptyPos < colonPos)
			{
				std::string inteface = line.substr(nonEmptyPos, colonPos - nonEmptyPos);
				// exclude loopback interface
				if (inteface != "lo")
				{
					DWORD unused;
				    DWORD interfaceInStat = 0;
				    DWORD interfaceOutStat = 0;
					sscanf(line.substr(colonPos + 1).c_str(), "%d%d%d%d%d%d%d%d%d%d",
							&unused, &interfaceInStat, &unused, &unused, &unused, &unused, &unused, &unused,
							&unused, &interfaceOutStat);
					inBusyStat += interfaceInStat;
					outBusyStat += interfaceOutStat;
				}
			}
		}
	}
	devStream.close();

#else

    MIB_IPSTATS stats;
    DWORD retVal = 0;
    retVal = GetIpStatistics(&stats);
    if (NO_ERROR != retVal)
    {
        Debug::trace("NetworkConnectivityManager::isNetworkActivityBusy", "Unable to get network statistics!");
    }
	
    inBusyStat = stats.dwInReceives;
	outBusyStat = stats.dwOutRequests;

#endif

	if (((inBusyStat - lastInBusyStat_) > inBusyThreshold_) ||
		((outBusyStat - lastOutBusyStat_) > outBusyThreshold_))
	{
		level = Activity_Busy;
	}

	lastInBusyStat_ = inBusyStat;
	lastOutBusyStat_ = outBusyStat;

    return level;
}


bool NetworkConnectivityManager::checkNetworkAvailability()
{
    onlineStateKnown_.Wait();

    return online_;
}


NetworkConnectivityManager *NetworkConnectivityManager::getInstance()
{
    if (instance == 0)
    {
        instance = new NetworkConnectivityManager();
    }
    return instance;
}


NetworkConnectivityManager *NetworkConnectivityManager::instance = (NetworkConnectivityManager *)NULL;
