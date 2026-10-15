//! Session.cpp
/*! The Session() class is a singleton where only a single instance of this class ever exists.
 *  The class is responsible for funneling messages on to the socket class, and once messages
 *  are reeived from the socket, the messages are sent here to Session. Session is then
 *  responsible for parsing the message and forwarding it on to the handleMessage() method which
 *  will finally decide how to process and the message and call any forms or methods
 *  responsible for updating the user interface.
 * @Author: Joe E. Crew
 * @bugs: TBD
*/

#include <logmanager.h>
#include <ScaleClasses.h>

#include "Session.h"

#include "HobSocket.h"
#include "MessageParser.h"
#include "ScaleConfiguration.h"
#include "UiUtilities.h"
#include "../sessionHandlerWidgets/softkeybutton.h"
#include "../sessionHandlerWidgets/softkeyswidget.h"

#include <label.h>
#include <graphic.h>
#include <systemstring.h>

const int Session::HEADER_SIZE = 6;
const int Session::SOCKET_DATA_UTF8 = 1;
const int Session::SOCKET_DATA_ANSI = 2;

int sessionDateStyle = 1;

//! Session()
/*!
    The Session() Constructor, calls init()
    \  params:  device - device to use for this session
    \  returns: nothing
    \  Bugs\Issues: None
*/
Session::Session( QIODevice *device ) :
    _Mybase(),
    scaleConfig( NULL ),
    uiUtilities( NULL ),
    sessionDevice( device )
{
    scaleConfig = new ScaleConfiguration( this );
    uiUtilities = new UiUtilities( this );

    init();
}

//! ~Session()
/*!
    The Session() Destructor, calls cleans up memory
    \  params:  none
    \  returns: nothing
    \  Bugs\Issues: None
*/
Session::~Session()
{
    delete sessionDevice;
}

//! init()
/*!
    init() creates a new instance socket in order to pass messages back and
    forth between the user interface and the scale Application.
    \  params:  none
    \  returns: nothing
    \  Bugs\Issues: None
*/
void Session::init()
{
    zeros << "000000" << "00000" << "0000" << "000" << "00" << "0" << "";

    dataTransferType = SOCKET_DATA_ANSI;        // This is going to have to end up being a configuration once we can figure out the printer

    // Lets do something here to get this party started, create an instance of the HobSocket,
    // launch a form, a splash screen, some party invitations for anyone else who needs a session.

    HobSocket *socket = dynamic_cast<HobSocket*>( sessionDevice );

    // for hobart socket
    if ( socket )
    {
        connect( socket, SIGNAL( disconnected() ), SIGNAL( disconnected() ) );
        connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ), SLOT( deviceError(QAbstractSocket::SocketError) ) );
        connect( socket, SIGNAL( readyRead() ), SLOT( deviceReadyRead() ) );
    }
}

//!  handleMessage()
/*!
    handleMessage() is used to direct messages and perform necessary actions and activities when
    a message is received from the application. The update method will perform similar to
    handleMessage in Session.cpp where messages will be funneled based upon the device and event.
    This method in MADP and all previous DOMs was previously known as update(Device,Event).
    This method (right now) is intended to be only called from MessageParser.parseUpdate()
    \  params:  victor - a vector with each element holding a single arg of the comma seperated value list
    \  returns: nothing
    \  Bugs\Issues: None
 */
void Session::handleMessage( const SessionMessage& victor )
{
    // validate message
    if ( victor.size() < 2 )
        return;

    // this will eventually be the master update method, similar to Session::handleMessage()
    const int SessionDevice( victor.at( 0 ).toInt() );      // Get the DEVICE
    const int SessionEvent( victor.at( 1 ).toInt() );       // Now lets grab the EVENT and tie it to a switch:case
    int SessionMode = 0;

    // update any struct that may be needing updated with this data
    switch ( SessionDevice )
    {
    case UnknownDevice:                     // 0
        // TODO: The only action here would be to print some DEBUG or something
        break;

    case Weigher:							// 1
        switch ( SessionEvent )
        {
        case WgInquire:          						// 1,2
            scaleConfig->weigherConfigRead = victor;
            break;
        case WgStatus:           						// 1,4
            scaleConfig->weightData = victor;
            break;
        case WgCalibrationStep:  						// 1,100
            break;
        case WgCalibrationEnd:	 						// 1,101
            break;
        case WgInfo:             						// 1,103
            scaleConfig->weigherInfoRead = victor;
            break;
        case WgStatusCounts:     						// 1,104
            break;
        case WgConfigurationEnd:						// 1,111
            break;
        case WgWriteAuditTrail:                         // 1,117
            scaleConfig->auditLogInformation = victor;
            break;
        case WgSaveAuditTrailToDiskFinished:            // 1,120
            break;
        }
        break;

    case WeigherSystem:						// 2

        switch (SessionEvent)
        {
            case SysUsbWgStatus:
                scaleConfig->weigherPowerupTests = victor;
            break;
        }
        break;

    case LabelApplier:                      // 5
        switch ( SessionEvent )
        {
        case LaConfig:           						// 5,1
        case LaInquire:          						// 5,2
            scaleConfig->labelApplierConfig = victor;
            break;
        case LaSensors:          						// 5,12
            break;
        case LaILASpeed:          						// 5,86
            break;
        case EvolLaConfig:        						// 5,100
            scaleConfig->evolLabelApplierConfig = victor;
            break;
        }
        break;

    case LabelApplierSystem:   				// 6
        break;

    case Printer:			  				// 7
        switch ( SessionEvent )
        {
        case PrInquire:          						// 7,2
            break;
        case PrSensors:          						// 7,8
            break;
        case PrPrintheadPower:   						// 7,9
            break;
        case PrNomSize:          						// 7,17
            break;
        case PrPrinterType:      						// 7,38
            break;
        case PrStockMetrics:     						// 7,41
            scaleConfig->printerStockMetrics = victor;
            break;
        case PrHeadType:                                // 7,60
            break;
        case PrStatusClear:                             // 7,108
            break;
        case PrWriteAllUsbDriverNames:                  // 7,112
            break;
        case PrWriteAllNetworkPrinterDriverNames:       // 7,114
            break;
        case PrWriteExternalPrinter:                    // 7,122
            break;
        case PrWriteAllExternalPrinter:                 // 7,123
            scaleConfig->allExternalPrinters = victor;
            break;
        case PrWriteProductTextPrintParam:              // 7,132
            break;
        case PrWriteAllProductTextPrintParams:          // 7,133
            break;

        }
        break;

    case PrinterSystem:		 				// 8
        switch (SessionEvent)
        {
            case PrSysStatus:
                scaleConfig->printerPowerupTests = victor;
            break;
        }
        break;

        case Wrapper:			 				// 9
            switch(SessionEvent)
            {
//            case WrInquire:              					// 9,2
//				break;
//            case WrStatus:               					// 9,4
//				break;
//            case WrSensors:              					// 9,15
//				break;
//            case WrErrorHistory:         					// 9,18
//				break;
            case WrEvolStatus:           					// 9,51
                scaleConfig->evolWrapperStatus = victor;
                if( scaleConfig->internalConfig.messagesSuppressed )
                    return;     // suppressess these messages in WrapperConfig and WrapperTimings
                break;
            case WrEvolSettings:         					// 9,55
                break;
            case WrEvolInquire:          					// 9,56
                scaleConfig->evolWrapperConfig = victor;
                break;
            case WrEvolErrorHistory:         				// 9,61
            break;
            case WrEvolErrorHistoryList:      				// 9,66
            break;
            case WrEvolSensors:                             // 9,115
                if( scaleConfig->internalConfig.messagesSuppressed )
                    return;     // suppressess these messages in WrapperConfig and WrapperTimings
            break;
            case WrEvolPackageDesc:        					// 9,125
            break;
            case WrCameraScan:                              // 9,139
            break;
            case WrCameraHeightScan:                        // 9,140
            break;
            case WrCameraCalibration:     					// 9,141
            break;
            case WrEvolTestComplete:                        // 9,165
            break;
            case WrEvolSystemInfo:        					// 9,144
                scaleConfig->evolWrapperSystemInformation = victor;
                break;
            case WrEvolCycleCount:                          // 9,171
            break;
            case WrEvolCameraThreshold:   					// 9,175
            break;
            case WrEvolEventCapture:                        // 9,180
                if( scaleConfig->internalConfig.messagesSuppressed )
                    return;     // suppressess these messages in WrapperConfig and WrapperTimings
            break;
            case WrKnifeTimeHistory:                        // 9,183
            break;
            case WrEvolWriteAllForAttribute:   				// 9,198
            break;
            case WrSettings:             					// 9,201
                break;
            case WrTestSense:            					// 9,203
                break;
            case WrErrorTotals:          					// 9,205
                break;
            case WrPackageTotals:        					// 9,208
                break;
            case WrAllPackCodeNames:     					// 9,212
                break;
            case WrWritePackCode:        					// 9,214
                break;
            case WrIntakePkgSize:        					// 9,217
                break;
            case WrPackageHistory:       					// 9,222
                break;
            case WrEvolWriteAllTrayTypes:         			// 9,237 - original WrNewWrTimings
            break;
            case WrEvolWriteWrapBox:        				// 9,241 - original WrWrapBoxParams
            break;
            case WrHeightBox:            					// 9,247
            break;
            case WrEvolPackageHistory:     					// 9,252
                break;
        }
        break;

    case WrapperSystem:						// 10
        break;

    case System:							// 11
        if ( SysPeripheralList == SessionEvent )               // 11, 100
            scaleConfig->peripheralList = victor;
        break;

    case TouchScreen:						// 14
        break;

    case RFIDReader:						// 15
        break;

    case BarcodeScanner:
        switch( SessionEvent )
        {
            case ScannerRead:                               // 16,0
                break;
            case ScannerReadAll:                   			// 16,1
                break;
            case ScannerWrite:                   			// 16,2
                break;
            case ScannerWriteAll:                  			// 16,3
                break;
            case ScannerDelete:                   			// 16,4
                break;
            case ScannerSave:                   			// 16,6
                break;
            case ScannerCreateNew:                   		// 16,12
                break;
            case ScannerEnabled:                   			// 16,20
                break;
            case ScannerDisabled:                   		// 16,21
                break;
            case ScannerReadProduct:                   		// 16,22
//                if( scaleConfig->weighModeTotalType.weighMode() == OpModeSelfService )
//                {
//                    // This will create a beep when ever an existing or nonexisting code is scanned in self service mode
//                    uiUtilities->boop();
//                }
                if(uiUtilities->isPlaying == false)
                {
                    uiUtilities->boop();
                }
                break;
            case ScannerReadPlu:
                if( victor.length() > 2)
                {
                    // grab the plu and do a plu lookup
                    const QString plunum = victor.at( 2 );
                    send( SessionDev, SnReadWeighModeTransactionType );
                    send( ProductDev, ProductRead, plunum );
                }
                break;
        }

        break;

    case BoardSite:                                     // 23
        switch ( SessionEvent )
        {
            // SOF-3516
            case BrdSitePercentCompete:                 // 23,8

                scaleConfig->boardSiteProgress = victor;
                break;
        }

        break;

    case SessionDev:						// 100
        switch ( SessionEvent )
        {
        case SnRegister:                                // 100,1
            break;
        case SnVersionInfo:                   			// 100,2
            scaleConfig->currentScaleSetup = victor;
            break;
        case SnTimedOut:                     			// 100,3
            break;
        case SnTerminate:                     			// 100,4
            break;
        case SnWriteDateTime:                 			// 100,6
            scaleConfig->dateAndTime = victor;
            break;
        case SnWriteWeighModeTransactionType: 			// 100,10
            scaleConfig->weighModeTotalType = victor;

            // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg
            SessionMode = victor.at(2).toInt();         // Now lets grab the Mode and tie it to a switch:case

            switch ( SessionMode )
            {
                case OpModeSelfService:             // 13
                    if ( scaleConfig->internalConfig.scaleBootComplete )
                        scaleConfig->internalConfig.ssactive_lock = true;
                    else
                        scaleConfig->internalConfig.ssactive_tenative_lock = true;
                    break;
                case OpModeManualWeigh:
                    if( scaleConfig->internalConfig.ssactive_lock )
                        return;     // because we are in self serv and dont want to do anything. we get a 100,10,5 going into SS

                    scaleConfig->internalConfig.ssactive_lock = false;
                    scaleConfig->internalConfig.ssactive_tenative_lock = false;
                    break;
                default:
                    //scaleConfig->internalConfig.ssactive_lock = false;
                    //scaleConfig->internalConfig.ssactive_tenative_lock = false;
                    break;
             }

            break;
        case SnWriteCurrentWebPage:          			// 100,20
            break;
        case SnTouchTypeSetting:             			// 100,23
            break;
        case SnPrintKeyPressed:              			// 100,25
            break;
        case SnClearKeyPressed:              			// 100,26
            break;
        case SnSelectCurrentLanguage:        			// 100,27
            break;
        case SnWriteStreamingGraphic:        			// 100,33
            break;
        case SnWriteStatistics:              			// 100,51
            break;
        case SnKeyActivationStates:          			// 100,63
            scaleConfig->keyActivation = victor;
            break;
        case SnUsbKbdAttachChange:            			// 100,69
            break;
        case SnSelectActiveWebPageFrame:      			// 100,70
            break;
        case SnSetFlashkeyInternetWebPage:    			// 100,82
            break;
        case SnTestMessage:                   			// 100,100
            break;
        case SnWriteLicensedPackages:                   // 100,135
            scaleConfig->setFeatureInstalled( MadpLicensedPackages( victor ) );
            break;
        }
        break;

    case ProductDev:						// 101
        switch ( SessionEvent )
        {
        case ProductComplete:                           // 101,3
        {
            if( scaleConfig->isWrapperTwoPrinter() && !( uiUtilities->isWrapOnly() ) )
            {
                if( scaleConfig->currentPLU.productType() > ProductType::PRODUCTTYPE_RANDOMWEIGHT )
                {
                    // means we looked a non random weight PLU with or without weight on platter, allow it all
                    scaleConfig->internalConfig.isOkToLookUpPlu = true;
                }

                // can look up another PLU once we print this one
                if( !scaleConfig->internalConfig.isOkToLookUpPlu )
                {
                    scaleConfig->internalConfig.isOkToLookUpPlu = true;

                    if( scaleConfig->internalConfig.poppedRezero )
                        send( Error, ErrRemoveAnyModalDialogDevice, QString::number( WeigherDevError::WeigherErrScaleNotZero ) + ",11,0" );

                    scaleConfig->internalConfig.poppedRezero = false;

                    // if there is a dropped PLU look it up
                    if( ( scaleConfig->internalConfig.isOkDroppedPLU > 0 ) && ( scaleConfig->internalConfig.isOkDroppedPLU != scaleConfig->currentPLU.itemNumber().toInt() ) )
                    {
                        scaleConfig->printProductComplete( scaleConfig->internalConfig.isOkDroppedPLU );
                        send( SessionDev, SnReadWeighModeTransactionType );
                        send( ProductDev, ProductRead, QString::number( scaleConfig->internalConfig.isOkDroppedPLU ) + ",3" );  
                    }

                    scaleConfig->internalConfig.isOkDroppedPLU = 0;
                }
            }

            break;
        }
        case ProductPrint:                       		// 101, 4
            break;
        case ProductWrite:                       		// 101, 7

            if( scaleConfig->isWrapperTwoPrinter() )
                scaleConfig->previousPLU = scaleConfig->currentPLU;

            scaleConfig->currentPLU = victor;    
            scaleConfig->internalConfig.selfservInvalidPlu = false;

            if( scaleConfig->isWrapperTwoPrinter() )
            {
                if( scaleConfig->currentPLU.productType() > ProductType::PRODUCTTYPE_RANDOMWEIGHT )
                {
                    // means we looked a non random weight PLU with or without weight on platter, allow it all
                    scaleConfig->internalConfig.isOkToLookUpPlu = true;
                }
            }

            break;
        case ProductWriteSupportData:            		// 101, 10
            scaleConfig->pluSupport = victor;
            break;
        case ProductWriteCurrentTotalValues:     		// 101, 15
            if( victor.length() > 2)
                scaleConfig->currentPLUTotals = victor;
            break;
        case ProductWriteSingleField:            		// 101, 16
            break;
        case ProductWriteDefault:                       // 101, 19
            scaleConfig->defaultCurrentPLU = victor;
            break;
        case ProductDeleteDefault:               		// 101, 20
            break;
        case ProductShowLinksToSupportData:      		// 101, 23
            break;
        case ProductCreate:                      		// 101, 24
            break;
        case ProductGetTree:                     		// 101, 29
            break;
        case ProductFindByDescription:           		// 101, 30
            break;
        case ProductFindSoftKeysByDescription:   		// 101, 31
            break;
        case ProductWriteLabelFormSize:          		// 101, 34
            break;
        case ProductStartPrintAllLabels:         		// 101, 42
            break;
        case ProductStopPrintAllLabels:          		// 101, 43
            break;
        case ProductClearUI:                            // 101, 47
        {
            uiUtilities->clearPLUData();
            break;
        }
        case ProductWriteAllSoftKeyGraphicNames: 		// 101, 53
            break;
        case ProductWriteAllExpirationLabels:    		// 101, 63
            break;
        case ProductRemoteRequestRWItem:                // 101, 93
            if( victor.length() > 2)
            {
                // grab the plu and do a plu lookup
                const QString plunum = victor.at( 2 );
                send( ProductDev, ProductRead, plunum );
            }
            break;
        }
        case ProductPausePrintAllLabels:          		// 101, 95
            break;
        break;

    case OperatorDev:						// 102
        switch ( SessionEvent )
        {
            case OpDtWrite:                  			 	// 102,2
                break;
            case OpDtWriteAll:               			 	// 102,3
                break;
            case OpLogin:                    			 	// 102,20
            {
                LOG_CLASS_INFO( "Session", "handleMessage - OpLogin" );

                DomOperatorLoginData m(victor);
                LOG_DEBUG( "OpLogin:  OpId = [%s],  OpName = [%s]", qPrintable(m.operatorID()), qPrintable(m.operatorName()) );

                if( m.operatorID() == "" || m.operatorID().isNull() )
                    scaleConfig->setIsOperatorLoggedIn(false);
                else
                    scaleConfig->setIsOperatorLoggedIn(true);
                scaleConfig->setLoggedOperatorId( m.operatorID() );
                scaleConfig->setLoggedOperatorName( m.operatorName() );
                scaleConfig->setLoggedOperatorAccess( m.operatorAccessLevel() );

                break;
            }
            case OpRFIDExists:               			 	// 102,27
                break;
            case OpRFIDDoesntExist:          				// 102,28
                break;
            case OpOperatorChanges:          				// 102,31
                break;
            case OpWriteNote:                			 	// 102,52
                break;
        }
        break;

    case Error:                             // 103 handle some errors
            // in case we want to handle a dialog when we click its buttons
            switch ( SessionEvent )
            {
                case Weigher:
                {
                    if( victor.length() > 2)
                    {
                        const int error = victor.at( 2 ).toInt();
                        if( 1021 == error )
                        {
                            if( !scaleConfig->internalConfig.isScaleCalibrating )
                                return;     // if we are NOT on the calibrate screen when we receive this error - dont pop it
                        }
                    }
                    break;
                }

                case SessionDev:
                {
                    if( victor.length() > 2)
                    {
                        const int product_error = victor.at( 2 ).toInt();
                        if ( SessionDevError::SessionDevBrowserBootupCompleted == product_error )               // 103,255,100018
                        {
                            // JHC NOTE: This block will handle if the user clicks the background or the OK button on the
                            // dialog that says initialization is complete and we emit this SIGNAL. There was a 3rd case I saw
                            // working on my laptop where the dialog went away on its own (due to console window getting focus)
                            // and then this code block was NEVER called.  If we start to see that happen we need to get the
                            // code block that does get called and emit a SIGNAL there.  see emit receiveErrorMessage( msg );
                            // Dont think this is going to be an error on a real scale, only on my laptop

                            if( scaleConfig->internalConfig.ssactive_tenative_lock || scaleConfig->internalConfig.ssactive_lock )
                            {
                                // if we are clicking the initial initializing dialog,
                                // lock up  everything if we are going to self service mode
                                emit receiveLockDownScreenMessage( true );

                                // Tell user what is happening.  No buttons to dismiss it.
                                send( Error, ErrSessionDevice, "100056, 3" );
                            }

                            send( Printer, PrReqCurrentStatus );
                            send( TransactionTypeDev, TransactionTypeWriteCurrentType, "1" );       // Production

                        }
                        //else if( 101001 == product_error )
                        //{
                        //    // received a PLU does not exist error while we are in cut test
                        //
                        //}
                    }
                    break;
                }

                case BackupRestoreDev:
                {
                    if( victor.length() > 2)
                    {
                        const int product_error = victor.at( 2 ).toInt();
                        if ( BackupRestoreDevError::BackupRestoreSelfServeModified == product_error )
                        {
                            // shutdown!
                            QString body( QString::number(SessionDevError::SessionDevShutdownAreYouSure) + ",1" );
                            send( Error, ErrSessionDevice, body );
                        }
                    }
                    break;
                }
            }

            break;

    case GraphicDev:						// 105
        break;

    case ExpandedTextDev:                                   // 106
        break;

    case SpecialMessageDev:                                 // 107
        break;

    case ProductNotesDev:                                   // 108
        break;

    case ClassDev:							// 110
        switch ( SessionEvent )
        {
        case ClassDtWrite:                                  // 110,2
            break;
        case ClassGetTree:                                  // 110,21
            break;
        case ClassGetOrphanTree:                            // 110,22
            break;
        }
        break;

    case LabelTypeDev:                      // 111

        if ( LabelTypeDtWrite == SessionEvent )             // 111,2
        {
            DomLabelType m( victor );

            // validate record
            if ( m.data().length() )
            {
                // add label into data store
                label l( m.data(), base16_encoding );

                    scaleConfig->dataStore_.store<label>()->insert( l );

                    // add label meta data
                ScaleConfiguration::metaData md;
                    md.width = m.field<QString>( m.width() );
                    md.height = m.field<QString>( m.height() );
                    md.custom = m.field<bool>( m.custom() );
                    md.rotationOnStock = m.field<QString>( m.rotationOnLabelStock() );

                scaleConfig->labelMetaDataMap_.insert( l.id(), md );
            }
        }

        break;

    case ConfigurationDev:					// 112
        switch ( SessionEvent )
        {
        case CfgWriteSingleField:                       // 112,17
            break;
        case CfgScaleWrite:              				// 112,21
        {
            scaleConfig->scaleConfig = victor;

            // save off date style
            sessionDateStyle = scaleConfig->scaleConfig.dateStyle();

            // save off licensing
            if ( scaleConfig->scaleConfig.licensedFeatures().length() )
                scaleConfig->setFeatureInstalled( scaleConfig->scaleConfig.licensedFeatures().split( "," ) );

            scaleConfig->setIsPick5Enabled( scaleConfig->scaleConfig.isPick5Enabled() );

            if( !scaleConfig->internalConfig.scaleBootComplete )
            {
                if( scaleConfig->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && !scaleConfig->scaleConfig.selfServeDisplayLabelOnScreenGO() )
                {
                    // if the combine feature is enabled, and the scale crashed or we received mismatched COMM or something, make sure this is set right, AT BOOT ONLY
                    // This says, if Combined turned on (which allows printing a label to a screen), and we are not AmazonGo (Print Label To the Screen), TURN ON
                    // AmazonGo (allow ability to scan). Field says [X]Print label to Screen. You get this config AUTOMATICALLY if Combine turned on even if you dont check it
                    int printToScreen = scaleConfig->scaleConfig.selfServeIgnorePrinterErrors();
                    // TODO FSUI - have to make sire this works under the other print conditions in case this is 1 and we need to turn it off
                    // See selfServeDisplayBarcodeLabelRaw too
                    if( ( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel ==  printToScreen ) ||
                        ( SelfServicePrintOption::SelfServOption_PrintToLabel == printToScreen ) )
                    {
                        scaleConfig->scaleConfig.selfServeDisplayLabelOnScreenGORaw() = "1";
                        QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
                        param += ",1,0";
                        send( ConfigurationDev, CfgSaveSingleField, param );
                     }
                }
            }

            if( scaleConfig->internalConfig.ssactive_lock )
                return;

            // save off homepage in case we need it
            if ( scaleConfig->scaleConfig.defaultHomePage().length() )
                scaleConfig->setConfiguredHomepage( scaleConfig->scaleConfig.defaultHomePage().trimmed() );

            hobart::softkeyButton::fontPointSize_ = scaleConfig->scaleConfig.defaultFlashkeyFontPointsize();

            break;
        }
        case CfgClassGetOrphanTree:         			// 112,31
            break;
        case CfgFrequentShopperWrite:     				// 112,41
            scaleConfig->frequentShopper = victor;
            break;
        case CfgPrintedFieldsWrite:       				// 112,51
            {
                scaleConfig->printSpecialFields = victor;
            }
            break;
        case CfgOperatorAccessWrite:      				// 112,61
            scaleConfig->operatorCanModify = victor;
            break;
        case CfgScaleOperationWrite:      				// 112,71
            scaleConfig->normalScaleOperation = victor;
            break;
        case CfgGenericTextWriteAll:      				// 112,81
            break;
        case CfgStoreInfoWrite:          				// 112,91
            scaleConfig->storeInfo = victor;
            break;
        case CfgCurrencyWrite:            				// 112,101
            scaleConfig->currencySettings = victor;
            break;
        case CfgSelfServiceWrite:         				// 112,111
            break;
        case CfgScaleUpgradeWrite:        				// 112,121
            break;
        case CfgEmailWrite:              				// 112,131
            break;
        case CfgPrepackWrite:             				// 112,141
            {
                scaleConfig->operatorDisplayFields = victor;
            }
            break;
        case CfgExpirationLabelNoteWrite: 				// 112,151
            break;
        case CfgMarqueeWrite:             				// 112,161
            break;
        case CfgFTPWrite:                 				// 112,171
            break;
        case CfgSynchronizationWrite:     				// 112,181
            break;
        case CfgSyncListWrite:            				// 112,185
            break;
        case CfgCommInterfaceWrite:       				// 112,191
            break;
        case CfgScaleManagementWrite:     				// 112,201
            break;
        case CfgRotationOverrideWrite:    				// 112,211
            break;
        case CfgReportsSetupWrite:        				// 112,221
            break;
        case CfgGCDWrite:         				        // 112,231
            break;
        case CfgAccumulateItemsWrite:                   // 112,241
            scaleConfig->accumulateItemsConfig = victor;
            break;
        }
        break;

    case FontDev:							// 113
        break;

    case ReportManagerDev:				    // 116
    {
        switch ( SessionEvent )
        {
            case TerminateReport:                       // 116,2
                break;
        }

        break;
    }
    case NutrifactsDev:						// 117
        break;

    case GenericTextDev:					// 119

        if ( AppletViewerWriteAll == SessionEvent )         // 119,21
        {
            DomAppletTextList m( victor );

            // validate record count
            if ( m.numOfRecordIds() == m.numOfTexts() )
            {
                // get records
                integerList ids = m.field<integerList>( m.allRecordIds() );
                QStringList strings = m.field<QStringList>( m.textList() );

                integerList::iterator id = ids.begin();

                // add each record into data store
                for ( QStringList::iterator string = strings.begin(); string != strings.end(); ++string )
                    scaleConfig->dataStore_.store<systemString>()->insert( systemString( *id++, *string ) );
            }
        }
        break;

    case VersionManagerDev:					// 120
        switch ( SessionEvent )
        {
        case InitiateUpgradeProcess:      				// 120,13
            break;
        case UsbWrapperUpgradeStatus:     				// 120,15
            break;
        case UpgradeXferStatus:          				// 120,16
            break;
        case PeripheralUpgradeList:       				// 120,17
                MadpUpgradeLists m(victor);
                scaleConfig->upgradeList = m;
                scaleConfig->receivedUpgradeList = true;
            break;
        }
        break;

    case CategoryDev:						// 121
        switch ( SessionEvent )
        {
        case CategoryDtWrite:                     		// 121,2
            break;
        case CategoryDtWriteAll:                  		// 121,3
            break;
        case CategoryWriteAllCatGraphicNames:     		// 121,21
            break;
        case CategoryWriteAllMainCategories:      		// 121,23
            break;
        case CategoryWriteSoftKeyList:            		// 121,51
            break;
        }
        break;

    case NetworkDev:						// 122
        switch ( SessionEvent )
        {
        case NetworkPingResponse:         				// 122,21
            break;
        case CfgTcpIpWrite:              				// 122,31
            break;
        case CfgDnsWrite:                	 			// 122,41
            break;
        case CfgWirelessCardWrite:       	 			// 122,51
            break;
        case NetworkSignalStrength:      	 			// 122,62
            break;
        case NetworkConnectionStatus:    	 			// 122,65
            break;
        case NetworkTftpUpgradeWrite:    	 			// 122,71
            break;
        case cfgLEAPWrite:               	 			// 122,81
            break;
        case cfgLEAPStatusWrite:         	 			// 122,84
            break;
        }
        break;

    case TotalsDev:							// 123
        switch ( SessionEvent )
        {
        case productRunTotal02:          				// 123,2
            break;
        case totalRecordData22:          				// 123,22
            break;
        case productTotalList37:          				// 123,37
            break;
        case totalRecordData42:          				// 123,42
            break;
        case operatorTotalList43:         				// 123,43
            break;
        case totalRecordData62:          				// 123,62
            break;
        case hourlyTotalList63:          				// 123,63
            break;
        case totalRecordData82:          				// 123,82
            break;
        case productTotalList97:          				// 123,97
            break;
        }
        break;

    case SpecialsDev:						// 124
        switch ( SessionEvent )
        {
        case SpecialsDtWrite:             				// 124,2
            break;
        case SpecialsDtWriteAll:          				// 124,3
            break;
        case ReadAllGraphics:             				// 124,20
            break;
        }
        break;

    case StreamingGraphicDev:				// 129
        switch(SessionEvent)
        {
        case StreamingGraphicDtWrite:                	 // 129,2
            break;
        case StreamingGraphicDtWriteAll:             	 // 129,3
            break;
        case StreamingGraphicWriteAllGraphicNames:   	 // 129,21
            break;
        }
        break;

    case OperatorChangesDev:				// 131
        break;

    case EmailAlertsDev:					// 134
        break;

    case COOLDev:							// 135
        switch ( SessionEvent )
        {
        case COOLDevWriteCountry:                     	// 135,22
            break;
        case COOLDevWriteAllCountries:                	// 135,23
            break;
        case COOLDevWriteCountriesByCategory:         	// 135,34
            break;
        case COOLDevWriteProductionStep:             	// 135,42
            break;
        case COOLDevWriteAllProductionSteps:          	// 135,43
            break;
        case COOLDevWriteProductionStepsByCategory:   	// 135,54
            break;
        case COOLDevWritePretext:                     	// 135,62
            break;
        case COOLDevWriteAllPretexts:                 	// 135,63
            break;
        case COOLDevWritePretextsByCategory:          	// 135,74
            break;
        case COOLDevWriteCategory:                    	// 135,82
            break;
        case COOLDevWriteAllCategories:              	// 135,83
            break;
        case COOLDevWriteCoolText:                    	// 135,102
            break;
        case COOLDevCreateNewCoolText:                	// 135,112
            break;
        case COOLDevShortlistWriteAllCoolText:        	// 135,114
            break;
        }
        break;

    case SoftkeyDev:						// 136
        switch ( SessionEvent )
        {
        case WriteSoftkey:                				// 136,22
            scaleConfig->softkeyItem = victor;
            break;
        case WriteSoftkeyFeature:         				// 136,42
        {
            DomSoftkeyFeature m( victor );
            scaleConfig->softkeyFeature = victor;

            if( ( !scaleConfig->internalConfig.scaleBootComplete ) && ( FixedFlashkeys == scaleConfig->scaleConfig.dcaRunScreenType() ) && ( hobart::softkeysWidget::FixedRowColumnGrid == m.defaultGroupNum() )  )
                scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = m.defaultGroupNum();
            break;
        }
        case WriteSoftkeyStyle:          				// 136,62
            break;
        case WriteAllSoftkeyStyleNumbers: 				// 136,74
            break;
        case WriteSoftkeyGroup:          				// 136,82
            break;
        case LoadSoftkeyGroup:            				// 136,93
        {
            DomLoadSoftkeyGroup m( victor );
            int feature = m.featureNum();

            if(  feature ==  hobart::softkeysWidget::FixedRowColumnGrid)
            {
                scaleConfig->toggledLastRow = m.horizGridSize();
                scaleConfig->toggledLastCol = m.vertGridSize();
            }
        }

            break;
        case LoadAllSoftkeyGroups:        				// 136,255
            scaleConfig->internalConfig.updateSelfServiceFlashkeys = true;
            break;
        }
        break;

    case MarqueeTextDev:					// 137
        break;

    case PeripheralDev:						// 138
        if(SessionEvent == PeripheralDtWrite)            	// 138,2
        {
            // TODO
        }
        else if(SessionEvent == WriteLostPeripheralsList)	// 138,20
        {
            scaleConfig->lostPeripheralList_ = victor;
        }
        break;

    case NumericSystem:						// 142
        break;

    case MarqueeSystem:						// 143
        break;

    case NetworkConnectivityDev:			// 145
        if ( CurrentStatus == SessionEvent )            // 145,23
            scaleConfig->networkConnectivity = victor;
        break;

    case CGICallManagerDev:					// 150
        break;

    case TransactionDev:					// 151
        break;

    case TransactionTypeDev:				// 152
        switch ( SessionEvent )
        {
        case TransactionTypeDtWrite:          			// 152,2
            break;
        case TransactionTypeWriteAllNames:    			// 152,21
            break;
        case TransactionTypeWriteCurrentType: 			// 152,31
            break;
        }
        break;

    case HourlyTotalDev:					// 153
        break;

    case OperatorTotalDev:					// 154
        break;

    case ProductTotalDev:					// 156
        break;

    case CutTestDev:						// 159
        switch ( SessionEvent )
        {
        case CuttingTestDtWrite:          			    // 159,2
            scaleConfig->cuttingTestWrite = victor;
            break;
        case BeginCuttingTest:    			            // 159,20
            break;
        case CancelCuttingTest: 			            // 159,21
            break;
        case EndCuttingTest:          			        // 159,22
            break;
        case CuttingTestDetailsRead:                    // 159,24
            break;
        case CuttingTestDetailsWrite:    			    // 159,24  ( details screen )
            scaleConfig->cuttingTestTotals = victor;    // used to display the last test
            break;
        case CuttingTestHistoryRead:   			        // 159,25
            break;
        case CuttingTestHistoryWrite:                   // 159,26
            scaleConfig->cuttingTestHistoryTotals = victor;
            break;
        case SuspendCuttingTest:          			    // 159,27
            break;
        case DeleteCuttingTest:          			    // 159,28
            break;
        case ResumeCuttingTest:          			    // 159,29
            break;
        case PrintCuttingTest:          			    // 159,30
            break;
        case SaveCuttingTestToUsb:          			// 159,31
            break;
        case CuttingTestHistoryLogin:                   // 159,32
            break;
        }
        break;

    case GCDSessionDev:						// 161
        break;

    case JobsDevice:						// 250
        switch ( SessionEvent )
        {
        case TestNetworkReport:              			// 250,12
            break;
        case TestOperatorNames:              			// 250,21
            break;
        case TestOperatorDataEntry:          			// 250,41
            break;
        case TestCreateCopiedPLU:             			// 250,97
            break;
        case TestTestMessage:                 			// 250,98
            break;
        case TestTestWeight:                  			// 250,99
            break;
        }
        break;

    case DeptDev:
        switch (SessionEvent)
        {
        case DeptDevDtWrite:
            scaleConfig->currentDept = DomCurrentDept(victor);
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    emit receive( victor );
    return;
}

void Session::handleSelfServModeMsg( bool isSelfServ )
{
    hobart::softkeyButton::selfServiceMode_ = isSelfServ;
}

//! send()
/*!
    send() is used to forward messages on to the socket class to send on to the scale Application
    \  params:  sessionMsg - a string that will be sent thru the socket to the Applciation
    \  returns: nothing
    \  Bugs\Issues: None
 */
void Session::send( int device, int event )
{
    // Similar to Client.send in the previous mom. Forwards the message on to the socket to send to the Application
    send( QString( QString::number(device) + "," + QString::number(event) ) );
}

//! send()
/*!
    send() is used to forward messages on to the socket class to send on to the scale Application
    \  params:  sessionMsg - a string that will be sent thru the socket to the Applciation
    \  returns: nothing
    \  Bugs\Issues: None
 */
void Session::send( int device, int event, const QString& msgBody )
{
    // Similar to Client.send in the previous mom. Forwards the message on to the socket to send to the Application
    send( QString( QString::number(device) + "," + QString::number(event) + "," + msgBody ) );
}

//! send()
/*!
    send() is used to forward messages on to the socket class to send on to the scale Application
    \  params:  victor - a string that will be sent thru the socket to the Applciation
    \  returns: nothing
    \  Bugs\Issues: None
 */
void Session::send( const SessionMessage& victor )
{
    bool first = true;
    QString data;

    for ( int i = 0; i < victor.size(); ++i )
    {
        if ( first )
            first = false;
        else
            data += ",";

        data += victor[i];
    }

    send( data );
}

//! send()
/*!
    send() is used to forward messages on to the socket class to send on to the scale Application
    \  params:  sessionMsg - a string that will be sent thru the socket to the Applciation
    \  returns: nothing
    \  Bugs\Issues: None
 */
void Session::send( const QString& data )
{
    LOG_CLASS_INFO( "Session", "send" );

    // Similar to Client.send in the previous mom. Forwards the message on to the socket to send to the Application

    //if( data.length() < 1000 )
        LOG_DEBUG( "Send: %s", data.toLatin1().constData() );

    unsigned int written = 0;
    unsigned int dataLen = 0;
    QByteArray newMsg;

    if( dataTransferType == SOCKET_DATA_UTF8 )
    {
        QByteArray utf8Data = data.toUtf8();
        dataLen = utf8Data.length() + 6;                       // have to add 6 characters for the header

        QString header( setHeader( dataLen ) );                // add on the 6 byte header which represents the size of the message

        newMsg = header.toUtf8();                              // create a QByteArray to send as a message, initializing it with the header

        newMsg = newMsg.append( utf8Data );
    }
    else if( dataTransferType == SOCKET_DATA_ANSI )
    {
        dataLen = data.length() + 6;                       // have to add 6 characters for the header

        QString header( setHeader( dataLen ) );                          // add on the 6 byte header which represents the size of the message

        newMsg = header.toLatin1();                          // create a QByteArray to send as a message, initializing it with the header

        newMsg = newMsg.append(data.toLatin1());
    }

    QMutexLocker lock( &writeMutex );

    if ( !sessionDevice->isWritable() )
        LOG_WARN( "HobSocket.send: NO SOCKET", NULL );                    // What did I write?
    else
    {
        written = sessionDevice->write( newMsg );                        // write the message out to the Application through the socket
    }

    if(written != dataLen)
    {
        LOG_ERROR( "write error!", NULL );
    }
}

//! deviceReadyRead()
/*!
    The deviceReadyRead() method reads bytes of data from the socket and places it in a buffer.
    Initially the first 6 bytes, the header, is read form the socket first. The header
    contains information how many bytes the current message contains. The remaining
    amount of bytes, minus the header, are then read from the buffer and passed on for parsing.
    \ params: None
    \ returns:    currently nothing, but in the future maybe the bytes read, success, or error.
      Bugs\Issues: Needs to be fully implemented. Implement a while loop? threading? better error
      handing? Right now this is triggered by a readyRead event, which seems not to require a typical
      endless while looping. Need to test this under full work load conditions to see if it remains true.
      Have to test if readyRead gets all data all the time out of the socket.
 */
void Session::deviceReadyRead()
{
    LOG_CLASS_INFO( "Session", "deviceReadyRead" );

    //
    // The scale service application sends data in the following format:
    // [---------- message ----------][---------- message ----------]
    // [<6 byte header><message data>][<6 byte header><message data>]
    //
    // Where the header is zero prefixed text containing the length of the entire message.
    //
    // For example:
    // 000011101,1000012112,12
    //
    // Is two seperate messages:
    // 101,1
    // 112,12
    //

    if ( !readMutex.tryLock() )
        return;

    for ( ;; )
    {
        // read all data from device
        // add received bytes to buffer
        buffer.append( sessionDevice->readAll() );

        const int bytesAvailable( buffer.size() );

        // make sure we at least have a header to read
        if ( HEADER_SIZE > bytesAvailable )
            break;

        const QByteArray header( buffer.left( HEADER_SIZE ) );

        // make sure header is numeric
        for ( int i = HEADER_SIZE; i--; )
            if (( header[i] < '0' ) || ( '9' < header[i] ))
            {
                LOG_WARN( "invalid header, clearing buffer!", NULL );
                LOG_WARN( "buffer: %s", buffer.constData() );

                buffer.clear();
                break;
            }

        const int messageSize( header.toInt() );

        // make sure we have a full message
        if ( messageSize > bytesAvailable )
            break;
        else if ( HEADER_SIZE >= messageSize )
        {
            LOG_WARN( "have malformed header, clearing buffer!", NULL );
            LOG_WARN( "buffer: %s", buffer.constData() );

            buffer.clear();
            break;
        }

        QString message;

        if ( SOCKET_DATA_UTF8 == dataTransferType )
            message = buffer.mid( HEADER_SIZE, messageSize - HEADER_SIZE );
        else if ( SOCKET_DATA_ANSI == dataTransferType )
            message = QString::fromLatin1(buffer.mid( HEADER_SIZE, messageSize - HEADER_SIZE ));

        buffer = buffer.right( bytesAvailable - messageSize );

        // Similar to Client.update(), where we received a message off of the
        // socket from the Application, and forward it on to be processed

        //if( message.length() < 1000 )
            LOG_DEBUG( "Update: %s", qPrintable( message ) );
        const SessionMessage msg( MessageParser::parseUpdate( message ) );

        if ( Error != msg[0].toInt() )
            handleMessage( msg );
        else
        {
            // error messages are handled a little differently because they generally pop
            // dialogs... emits error signal...
            emit receiveErrorMessage( msg );
        }
    }

    readMutex.unlock();
}

bool Session::isPrepack() const
{
    return ( "2" == scaleConfig->scaleConfig.scaleModel() );
}

bool Session::isServiceCounter() const
{
    return ( "2" != scaleConfig->scaleConfig.scaleModel() );
}

bool Session::isSelfServiceMode() const
{
    return( scaleConfig->internalConfig.ssactive_lock );
}

bool Session::isSelfServeAndPrintingToScreen() const
{
    // pjr383 - use original test
    if( scaleConfig->internalConfig.ssactive_lock && ( SelfServicePrintOption::SelfServOption_PrintToScreen != scaleConfig->scaleConfig.selfServeIgnorePrinterErrors() ) )
        return true;
    else
        return false;
}

//! deviceError()
void Session::deviceError( QAbstractSocket::SocketError socketError )
{
    LOG_CLASS_INFO( "Session", "deviceError" );

    LOG_ERROR( "have socket error %d", socketError );
}

//! setHeader()
/*!
    The setHeader() method creates the 6 byte header required for each message. Prepends the required number of
    0's to the front of the QString to fill out all 6 bytes of the header. For example, if the size of the msg
    is 256 bytes, the header, being 6 bytes, setHeader() would return 000256
    \ params:  none
    \ returns: void
    \ Bugs\Issues: None
*/
QString Session::setHeader(int msgSize) const
{
    QString msgSizeStr( QString::number(msgSize) );
    int numLen = msgSizeStr.length();

    return QString( zeros[numLen] + msgSizeStr );
}

bool Session::isLinux()
{
    #if defined( Q_OS_LINUX )
        return true;
    #elif defined( Q_OS_WIN )
        return false;
    #endif
}
