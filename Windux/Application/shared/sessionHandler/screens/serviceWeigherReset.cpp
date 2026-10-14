#include "serviceWeigherReset.h"
#include "ui_serviceWeigherReset.h"

#include "../sessionmainwindow.h"
#include "plumainscreen.h"
#include "../dialogs/messageboxdialog.h"
#include <qtimer.h>
#include <logmanager.h>
#include "python.h"


serviceWeigherReset::serviceWeigherReset(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceWeigherReset)

{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    translate();
}

serviceWeigherReset::~serviceWeigherReset()
{
    delete ui;
}

void serviceWeigherReset::translate()
{
    ui->retranslateUi(this);

    update();
}

void serviceWeigherReset::initialize()
{
    periphControllerVer = "";
    boardManf = "";
    haveFpcVersion = false;

    // Request the Weigher info
    session_->send( WeigherSystem, SysReqStatus );

    if( isLinux() )
    {
        populateBoardType();
    }
    else
    {
        boardManf = "Unknown";
    }

    connect( ui->btnYes, SIGNAL( clicked() ), this, SLOT( btnYesClicked() ) );
    connect( ui->btnNo, SIGNAL( clicked() ), this, SLOT( btnNoClicked() ) );
}

// v1 = fpcVersion, v2 = compareVersion (5.6.0)
bool serviceWeigherReset::compareSoftwareVersions(QString v1, QString v2)
{
    LOG_CLASS_INFO( "serviceWeigherReset", "compareSoftwareVersions" );

    LOG_DEBUG( "compareSoftwareVersions(): v1: %s", v1.toLatin1().constData() );
    LOG_DEBUG( "compareSoftwareVersions(): v2: %s", v2.toLatin1().constData() );

    if( 0 == v1.length() )
    {
        LOG_DEBUG( "compareSoftwareVersions(): FALSE-1", NULL );
        return false;
    }

    // in the future will need more flexability if undetermined number of digits

    if( v1.count(".") == 1 )    // if we get a 5.1 or something
        v1.append(".0");        // make sure we are comparing #.#.# to #.#.#

    // alright, lets split them up and compare the digits 1 by 1
    QStringList v1List( v1.split( "." ) );
    QStringList v2List( v2.split( "." ) );

    int v1Num = 0;  // originally I was comparing the element digits themselves. But this method (maybe) was crashing some
    int v2Num = 0;  // where so I pulled these out so I could see and debug them and print out debug on them easier

    for( int j = 0; j < v1List.size(); j++ )
    {
        v1Num = v1List[j].toInt();
        v2Num = v2List[j].toInt();

        if( v1Num > v2Num )
            return true;
        else if( v1Num < v2Num )
        {
            LOG_DEBUG( "compareSoftwareVersions(): FALSE-2", NULL );
            return false;
        }
    }

    // if you got to here it means that the peeled digits were equal all the way through
    // so return true because the original test was if fpc >= 5.6.  and is equal so true
    return true;
}

void serviceWeigherReset::btnYesClicked()
{
    LOG_CLASS_INFO( "serviceWeigherReset", "resetWeigherDefaults" );

    MessageBoxDialog dialog( real_time_priority, mainWindow_ );

    //all versions of KPC weigher support reset to defaults
    if( ( boardManf == "ARA" ) || ( boardManf == "ARK" ) )
    {
        //-------------------------------------------------------------------
        // SOF-3670: Before comparing the versions, need to make sure we have
        //           received the message with the FPC version on the scale
        //-------------------------------------------------------------------

        LOG_DEBUG( "btnYesClicked()-1: haveFpcVersion=%d", haveFpcVersion );

        if( !haveFpcVersion )
        {
            waitForFpcVersionTimer = new QTimer( this );
            waitForFpcVersionTimer->setSingleShot( true );
            waitForFpcVersionTimer->setInterval( 10000 );
            waitForFpcVersionTimer->start();

            while( !haveFpcVersion )
            {
                sleepFor( 1 );

                LOG_DEBUG( "btnYesClicked()-2: haveFpcVersion=%d", haveFpcVersion );
            }

            waitForFpcVersionTimer->stop();
        }

        LOG_DEBUG( "btnYesClicked()-3: haveFpcVersion=%d", haveFpcVersion );

        // 8/29/2019 - SOF-3047 - support 3 digit version numbers supercedes SOF-2837
        QString fpcVersion = periphControllerVer;
        QString stdVersion = "5.6.0";

        // new compare method to compare the current fpcVerion against 5.6.0
        bool isNewer = compareSoftwareVersions( fpcVersion, stdVersion);

        if( ( isNewer ) || ( boardManf == "ARK" ) )
        {
            dialog.setTitle( tr( "WEIGHER RESET" ) );
            dialog.setText( tr( "WARNING: You will have to recalibrate the weigher if you reset it to Factory Defaults.\n\nPlease remove All weight from the platter.\n\nPress OK to continue." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok|QDialogButtonBox::Cancel );

            int result = dialog.exec();

            if ( QDialog::Accepted == result )
            {
                ui->btnYes->hide();
                ui->btnNo->hide();

                weigherResetSuccessful = false;

                session_->send( Weigher, WgResetFactoryDefaults ); // 1,22
                session_->send( Weigher, WgReqInquire );           // Get the new weigher config

                LOG_DEBUG( "Sent message to reset weigher to factory default.", NULL );

                // Wait for confirmation that the weigher was successfully reset
                startWeigherResetTimer = new QTimer( this );
                startWeigherResetTimer->setSingleShot( true );
                startWeigherResetTimer->setInterval( 3000 );
                connect( startWeigherResetTimer, SIGNAL( timeout() ), SLOT( confirmWeigherReset() ) );
                weigherResetTimerDone = false;
                startWeigherResetTimer->start();

                while( !weigherResetTimerDone )
                {
                    sleepFor( 1 );
                }

                startWeigherResetTimer->stop();

                if( weigherResetSuccessful )
                {
                    dialog.setTitle( tr( "WEIGHER RESET" ) );
                    dialog.setText( tr( "Weigher was successfully set to Factory Defaults.\n\nPress OK to continue." ) );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );
                    dialog.exec();

                    LOG_DEBUG( "Weigher reset to Factory Defaults successful", NULL );

                    // In order for the Capacity Bar to update
                    session_->send( Weigher, WgReqInquire );
                }
                else
                {
                    dialog.setTitle( tr( "WEIGHER RESET" ) );
                    dialog.setText( tr( "Reset Weigher to Factory Defaults FAILED.\n\nPress OK to continue." ) );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );
                    dialog.exec();

                    LOG_DEBUG( "Weigher reset to Factory Defaults FAILED", NULL );
                }

                //----------------------------------------------------------------
                // Need to exit Service Mode after re-setting the Weigher to
                // Factory Defaults.  This forces the CAT-I W&M Configuration
                // Parameters and Last Changed date to be updated by the scale
                // application. To accomplish this we return to
                // Home (i.e. PluMainScreen).
                //----------------------------------------------------------------
                mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
            }
        }
        else
        {
            // Must have FPC version 5.6 or higher ...
            dialog.setTitle( tr( "WEIGHER RESET" ) );
            dialog.setText( tr( "Must have Peripheral Controller Version 5.6 or higher.\n\nCurrent version is " ) + periphControllerVer + tr( "\n\nPress OK to continue." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );
            dialog.exec();

            // Return to Home (i.e. PluMainScreen).
            mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
        }
    }
    else
    {
        // Only valid on ARA board ...
        dialog.setTitle( tr( "WEIGHER RESET" ) );
        dialog.setText( tr( "This option is only valid for a scale with an ARA board." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );
        dialog.exec();

        LOG_DEBUG( "This is not an ARA board", NULL );

        // Return to Home (i.e. PluMainScreen).
        mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
    }
}

void serviceWeigherReset::btnNoClicked()
{
    // Return to Home (i.e. PluMainScreen).
    mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
}

void serviceWeigherReset::confirmWeigherReset()
{
    weigherResetTimerDone = true;
}

void serviceWeigherReset::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceWeigherReset", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if (Weigher == device && WgResetFactoryDefaultsComplete == event ) // 1,23
    {
        DomWeigherResetFactoryDefaultsStatus m( msg );

        weigherResetSuccessful = ( m.resetStatus() == "1" ? true : false );

        LOG_DEBUG("WeigherResetFactoryDefaults: Result=%s", ( weigherResetSuccessful ? "SUCCESS" : "FAILED" ) );
    }

    if( ( WeigherSystem == device ) && ( SysUsbWgStatus == event ) )
    {
        DomUsbWeigherPowerupTests m( msg );

        periphControllerVer = m.softwareVersion();

        haveFpcVersion = true;

        LOG_DEBUG( "Got peripheral controller version info: %s", periphControllerVer.toLatin1().constData() );
    }
}

void serviceWeigherReset::populateBoardType()
{
    LOG_CLASS_INFO( "serviceWeigherReset", "populateBoardType" );

    if( !isLinux() )
        return;

    python pythonProcess;
    QStringList args;
    QString cpuCount;
    QString strCPUCount = "Unknown";

    if ( pythonProcess.execCommand("cpuCount", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            cpuCount = pythonProcess.readStdOut();

            if( cpuCount == "1" )
                strCPUCount = "Solo";
            else if( cpuCount == "2" )
                    strCPUCount = "Dual";
            else if( cpuCount == "4" )
                    strCPUCount = "Quad";

            LOG_DEBUG( "get CPU count successful, |%s|", cpuCount.toLatin1().constData() );
        }
        else
        {
            LOG_DEBUG( "Cannot get CPU count. Exit code = %d", pythonProcess.getExitCode() );
        }
    }

    //--------------------
    // getBoardType.py ...
    //--------------------
    args.clear();
    boardManf = "Unknown";

    if ( pythonProcess.execCommand("getBoardType", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "getBoardType successful", NULL );

            QString result = pythonProcess.readStdOut();
            LOG_DEBUG( "getBoardType results=|%s|", result.toLatin1().constData() );

            QStringList boardTypes;
            boardTypes = result.split( "," );

            if( boardTypes.count() >= 2 )
            {
                QString isComboBoard;
                QString isBcmBoard;

                isComboBoard = boardTypes[0];
                isBcmBoard = boardTypes[1];

                if( isComboBoard == "1" )
                    boardManf = "ARA";
                else if( isBcmBoard == "1" )
                    boardManf = "BCM";

                if( boardTypes.count() >= 3 )
                {
                    QString isArkBoard;

                    isArkBoard = boardTypes[2];

                    if( isArkBoard == "1" )
                        boardManf = "ARK";
                }
            }
        }
        else
        {
            LOG_DEBUG( "getBoardType failed. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
}

void serviceWeigherReset::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

bool serviceWeigherReset::isLinux()
{
    #if defined( Q_OS_LINUX )
        return true;
    #elif defined( Q_OS_WIN )
        return false;
    #endif
}
