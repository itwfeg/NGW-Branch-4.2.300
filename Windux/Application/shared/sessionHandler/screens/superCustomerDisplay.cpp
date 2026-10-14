/**
 * @file superCustomerDisplay.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include <filesystem.h>
#include <logmanager.h>
#include <python.h>

#include "superCustomerDisplay.h"
#include "ui_superCustomerDisplay.h"

#include "supertextfileeditor.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/modewidget.h"

#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>

#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QIntValidator>

///////////////////////////////////////////////////////////////////////////////////////////////////
superCustomerDisplay::superCustomerDisplay( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ui( new Ui::superCustomerDisplay ),
    gcdSettings_( new DomGcdSettings )
{
    ui->setupUi(this);
    scaleScreen();

    // Set up validator for Playback Wait Time
    ui->txtPlaybackWaitTime->setValidator(playbackWaitTimeValidator_ = new QIntValidator(0, 900, this));
    ui->txtPlaybackWaitTime->setDecimalPlaces(0);
    ui->txtPlaybackWaitTime->setInputWrapping(true);

    // Set up validator for Dimmer Wait Time
    ui->leDimmerWaitTime->setValidator(dimmerWaitTimeValidator_ = new QIntValidator(0, 10, this));
    ui->leDimmerWaitTime->setDecimalPlaces(0);
    ui->leDimmerWaitTime->setInputWrapping(true);

    if (!session_->isPrepack())
	{
	    // Set up validator for Playback Wait Time
	    ui->txtPlaybackWaitTime->setValidator(playbackWaitTimeValidator_ = new QIntValidator(0, INT_MAX, this));
	    ui->txtPlaybackWaitTime->setDecimalPlaces(0);
	    ui->txtPlaybackWaitTime->setInputWrapping(true);
	    playbackWaitTimeValidator_->setTop( 900 );


	    // Set the scale range for the slider
	    ui->volumeSlider->setRange( 0, 100 );
	}

    initialize();

    if (session_->isServiceCounter())
	{
	    connect(ui->volumeSlider, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateVolumeDisplay( int ) ) );
    connect(ui->volumeSlider, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateVolumeDisplay( int ) ) );

    //------------------------------------------------------------------
    // Set the connections for when a one of the setup fields is changed.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //------------------------------------------------------------------
    connect( ui->txtPlaybackWaitTime, SIGNAL( textChanged(QString) ), this, SLOT( SetupChanged() ) );
    connect( ui->cbCustomerDisplayLanguage, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->cbCustomerDisplayPlaylist , SIGNAL( currentIndexChanged(int) ) , this, SLOT( SetupChanged() ) );
    connect( ui->cbLayoutNumber, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->volumeSlider, SIGNAL( valueChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->leDimmerWaitTime, SIGNAL( textChanged(QString) ), this, SLOT( SetupChanged() ) );

    connect( ui->chkUseFullScreenAdverts, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->cbIdleAdvertType, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->cbProductAdvertType, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->txtAdvertWebpage, SIGNAL( textChanged(QString) ), this, SLOT( SetupChanged() ) );

    //------------------------------------------------------------------
    // Set the connections for when Playback Wait Time is changed.
    //------------------------------------------------------------------
    connect( ui->txtPlaybackWaitTime, SIGNAL( textChanged(QString) ), this, SLOT( PlaybackWaitTimeChanged() ) );
    connect( ui->leDimmerWaitTime, SIGNAL( textChanged(QString) ), this, SLOT( DimmerTimeoutWaitTimeChanged(QString) ) );
    connect( ui->btnEditPlaylist, SIGNAL( clicked() ), this, SLOT( EditPlaylistFile() ) );

	    //------------------------------------------------------------------
	    // Set the connections for when Playback Wait Time is changed.
	    //------------------------------------------------------------------
	    connect( ui->txtPlaybackWaitTime, SIGNAL( editingFinished() ), this, SLOT( PlaybackWaitTimeChanged() ) );
	    connect( ui->btnEditPlaylist, SIGNAL( clicked() ), this, SLOT( EditPlaylistFile() ) );

	    //----------------------------------
	    // Request Customer Display Settings
	    //----------------------------------
	    session_->send( ConfigurationDev, CfgGCDRead );

	    bSetupChanged = false;
	    bPlaybackWaitTimeChanged = false;

		// FT-748: Start on the Customer Display tab
	    ui->tabWidget->setCurrentIndex( 0 );
	}
    connect(ui->leDimmerWaitTime, SIGNAL(textChanged(QString)), this, SLOT(DimmerTimeoutWaitTimeChanged(QString)));

    if (session_->isPrepack())
	{
        // Prepack does not have a Customer Promotions
        ui->tabWidget->removeTab( 1 );

        // Prepack does not have a Customer Display
        ui->tabWidget->removeTab(0);
//	    ui->tabWidget->setCurrentIndex( 1 );	//	Select the Operator Display tab
    }\
    expandTabs(ui->tabWidget);

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superCustomerDisplay::~superCustomerDisplay()
{
    delete gcdSettings_;
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superCustomerDisplay::translate()
{
    int index;

    ui->retranslateUi( this );

    if (session_->isServiceCounter())
	{
	    ui->cbLayoutNumber->setItemText( 0, tr( "Meat" ) );
	    ui->cbLayoutNumber->setItemText( 1, tr( "Bakery" ) );
	    ui->cbLayoutNumber->setItemText( 2, tr( "Deli" ) );
	    ui->cbLayoutNumber->setItemText( 3, tr( "Produce" ) );
	    ui->cbLayoutNumber->setItemText( 4, tr( "Seafood" ) );
	    ui->cbLayoutNumber->setItemText( 5, tr( "Floral" ) );
    	ui->cbLayoutNumber->setItemText( 6, tr( "Gray Layout" ) );

	    if ( 0 <= (index = ui->cbLayoutNumber->findData( "8" )) )
	        ui->cbLayoutNumber->setItemText( index, tr( "Layout #8" ) );

	    if ( 0 <= (index = ui->cbLayoutNumber->findData( "9" )) )
	        ui->cbLayoutNumber->setItemText( index, tr( "Layout #9" ) );
	}
    ui->cbIdleAdvertType->setItemText( 0, tr( "Playlist" ) );
    ui->cbIdleAdvertType->setItemText( 1, tr( "Website" ) );
    ui->cbIdleAdvertType->setItemText( 2, tr( "Advertisement Server") );

    ui->cbProductAdvertType->setItemText( 0, tr( "None" ) );				//	Returns 0
    ui->cbProductAdvertType->setItemText( 1, tr( "Category Promotion" ) );	//	Returns 5, but is second in the list.
    ui->cbProductAdvertType->setItemText( 2, tr( "Product Image 1" ) );		//	Returns 1, but is third in the list.
    ui->cbProductAdvertType->setItemText( 3, tr( "Product Image 2" ) );		//	Returns 2
    ui->cbProductAdvertType->setItemText( 4, tr( "Product Image 3" ) );		//	Returns 3
    ui->cbProductAdvertType->setItemText( 5, tr( "Product Image 4" ) );		//	Returns 4

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superCustomerDisplay::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString superCustomerDisplay::getDimmerTimeoutValue()
{
   QString minutes = "10"; // fail safe value
   QString databaseFileName = "settingswizard/scalesettings";
   databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

   QFile file(databaseFileLocation);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return minutes;

   QStringList theLine;
   QTextStream in(&file);
   while (!in.atEnd()) {
       QString line = in.readLine();
       theLine = line.split("=", QString::KeepEmptyParts);
       if(theLine.at(0) == "dimDisplayTimeout")  minutes = theLine.at(1);
   }
   file.close();
   return minutes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int superCustomerDisplay::getCustomerVolumeValue()
{
   QString volume = "50"; // fail safe value
   QString databaseFileName = "settingswizard/scalesettings";
   databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

   QFile file(databaseFileLocation);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return volume.toInt();

   QStringList theLine;
   QTextStream in(&file);
   while (!in.atEnd()) {
       QString line = in.readLine();
       theLine = line.split("=", QString::KeepEmptyParts);
       if(theLine.at(0) == "cVolume")  volume = theLine.at(1);
   }
   file.close();
   //qDebug() << "volume is : " << volume;
   return volume.toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superCustomerDisplay::onSessionMessage( const SessionMessage& msg )
{
    //----------------------------------
    // Customer Display Settings message
    //----------------------------------
    if (session_->isServiceCounter() && (ConfigurationDev == msg[0].toInt() ) && ( CfgGCDWrite == msg[1].toInt() ))
    {
        bool needToSaveSettings( false );

        int playlistIndex;
        int layoutIndex;
        int productAdvertIndex;

        DomGcdSettings m( msg );

        // HTs
        // this feature is disabled
        if ( !session_->uiUtilities->isFeatureLicensed( SCALE_TRANS_GCD ) )
        {
            if (( "0" != m.useTransactionalLayout() ) ||
                ( "0" != m.idleAdvertType() ))
            {
                needToSaveSettings = true;
            }

            m.useTransactionalLayout() = "0";
            m.idleAdvertType() = "0";
        }

        //
        // customer display
        //

        ui->cbCustomerDisplayLanguage->blockSignals( true );
        ui->cbCustomerDisplayLanguage->setCurrentIndex( ui->cbCustomerDisplayLanguage->findData( m.gcdLanguage() ) );
        ui->cbCustomerDisplayLanguage->blockSignals( false );

        if ( 0 <= (layoutIndex = ui->cbLayoutNumber->findData( m.gcdLayoutNumber() )) )
        {
            ui->cbLayoutNumber->blockSignals( true );
            ui->cbLayoutNumber->setCurrentIndex( layoutIndex );
            ui->cbLayoutNumber->blockSignals( false );
        }

        ui->chkUseFullScreenAdverts->blockSignals( true );
        ui->chkUseFullScreenAdverts->setChecked( ("1" == m.useTransactionalLayout()) ? false : true );
        ui->chkUseFullScreenAdverts->blockSignals( false );

        ui->txtPlaybackWaitTime->blockSignals( true );
        ui->txtPlaybackWaitTime->setText( m.gcdPlaybackWaitTime() );
        ui->txtPlaybackWaitTime->blockSignals( false );

        // The volume for the GCD can\will be set by the setup tool during the initial setup phase
        // of the scale. From there, the volume will be set in BOTH the database and a file named
        // ../hobart/db/settingswizard/scalesettings. Now if an operator goes to the GCD display
        // settings in Supervisor, the initial value for the GCD is read from the scalesettings file.
        // This is ok MOST of the time, b\c the volume slider, if changed, will be written to BOTH the DB
        // and the scale settings file.  HOWEVER, there are a couple of occassions that the DB value may
        // get changed, and the scalesettings file does not.  Either COMM, Scalemaster, HTe, etc, can push
        // down a new gcdVolume setting and it will get set in the DB.  But it doesn't get set in the file.
        // Therefore we cannot totally read the gcdVolume from the file and expect a valid setting since the
        // DB value may have changed.  So here we are going to read in the DB value and the scalesettings value.
        // If the values are different (bc the DB value changed due to COMM) go ahead and update the scalesettings
        // and make both the same again, and then proceed forward. This is just an effort to sync them up.

        int storedVolume = getCustomerVolumeValue();
        int dbVolume = m.gcdVolume().toInt();

        if ( storedVolume == dbVolume )
            ui->volumeSlider->setSliderPosition( storedVolume );
        else
        {
            ui->volumeSlider->setSliderPosition( storedVolume = dbVolume );

            // Save customer volume
            setCustomerVolume();
        }

        ui->lblVolumePercentage->setText( QString::number(storedVolume) + "%" );

        //
        // customer promotions
        //

        ui->cbIdleAdvertType->blockSignals( true );
        if ( "0" == m.idleAdvertType() )
            ui->cbIdleAdvertType->setCurrentIndex( 0 );
        else if ( "1" == m.idleAdvertType() )
            ui->cbIdleAdvertType->setCurrentIndex( 1 );
        else if ( "2" == m.idleAdvertType() )
            ui->cbIdleAdvertType->setCurrentIndex( 2 );
        ui->cbIdleAdvertType->blockSignals( false );

        if ( (playlistIndex = ui->cbCustomerDisplayPlaylist->findText( m.playlistFile() )) < 0 )
        {
            QString fileplaylist(m.playlistFile());
            //qDebug() << "fileplaylist3=|" << fileplaylist << "|";

            if (m.playlistFile() != QString("ad_service_playlist.txt"))
            {
                //qDebug() << "playlist file2 is " << m.playlistFile();
                ui->cbCustomerDisplayPlaylist->addItem( m.playlistFile(), m.playlistFile() );
            }
            playlistIndex = ui->cbCustomerDisplayPlaylist->count() - 1;
        }

        ui->cbCustomerDisplayPlaylist->blockSignals( true );
        ui->cbCustomerDisplayPlaylist->setCurrentIndex( playlistIndex );
        ui->cbCustomerDisplayPlaylist->blockSignals( false );

        ui->txtAdvertWebpage->blockSignals( true );
        ui->txtAdvertWebpage->setText( m.idleWebsiteAddress() );
        ui->txtAdvertWebpage->blockSignals( false );

        ui->cbProductAdvertType->blockSignals( true );
        if ( 0 <= (productAdvertIndex = ui->cbProductAdvertType->findData( m.productAdvertType() )) )
            ui->cbProductAdvertType->setCurrentIndex( productAdvertIndex );
        ui->cbProductAdvertType->blockSignals( false );

        // Save initial values for fields that are not used on the screen
        *gcdSettings_ = m;

        bSetupChanged =
            bPlaybackWaitTimeChanged =
            bDimmerWaitTimeChanged = false;

        if ( needToSaveSettings )
            bSetupChanged = true;

        UpdateUserInterface();
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------
// Initialize dropdowns
//---------------------
void superCustomerDisplay::initialize()
{
    QString layout;

    LOG_CLASS_INFO( "superCustomerDisplay", "initialize" );

    if (session_->isServiceCounter())
	{
	    ui->cbCustomerDisplayLanguage->addItem( tr( "English" ), "English" );
	    ui->cbCustomerDisplayLanguage->addItem( tr( "French"  ), "French" );
	    ui->cbCustomerDisplayLanguage->addItem( tr( "Spanish" ), "Spanish" );

	    ui->cbLayoutNumber->addItem( tr( "Meat" ), "1" );
	    ui->cbLayoutNumber->addItem( tr( "Bakery" ), "2" );
	    ui->cbLayoutNumber->addItem( tr( "Deli" ), "3" );
	    ui->cbLayoutNumber->addItem( tr( "Produce" ), "4" );
	    ui->cbLayoutNumber->addItem( tr( "Seafood" ), "5" );
	    ui->cbLayoutNumber->addItem( tr( "Floral" ), "6" );
    	ui->cbLayoutNumber->addItem( tr( "Gray Layout" ), "7" );

	    layout = filesystem::absoluteFilePath( "TLXML81.xml", folderType::Layouts );
	    if ( QFile::exists( layout ) )
	        ui->cbLayoutNumber->addItem( tr( "Layout #8" ), "8" );

	    layout = filesystem::absoluteFilePath( "TLXML91.xml", folderType::Layouts );
	    if ( QFile::exists( layout ) )
	        ui->cbLayoutNumber->addItem( tr( "Layout #9" ), "9" );

	    ui->cbIdleAdvertType->addItem( "", "0" );
	    ui->cbIdleAdvertType->addItem( "", "1" );
        ui->cbIdleAdvertType->addItem( "", "2" );

        ui->cbProductAdvertType->addItem( "", QVariant("0") );		//	None
        ui->cbProductAdvertType->addItem( "", QVariant("5") );		//	Category Promotion
        ui->cbProductAdvertType->addItem( "", QVariant("1") );		//	Product Image 1
        ui->cbProductAdvertType->addItem( "", QVariant("2") );		//	Product Image 2
        ui->cbProductAdvertType->addItem( "", QVariant("3") );		//	Product Image 3
        ui->cbProductAdvertType->addItem( "", QVariant("4") );		//	Product Image 4

	    // for remote session we need to sync playlist folder
	    if ( !mainWindow_->isPrimarySessionType() )
	    {
	        LOG_TRACE( "sync playlist folder...", NULL );

		        if ( !filesystem::syncFolder( folderType::Playlists ) )
		            LOG_WARN( "could not sync playlist folder!", NULL );
	    }

	    // populate combo with playlist files
	    QDir dir( filesystem::folder( folderType::Playlists ) );

	    QStringList filters;
	    filters << "*.txt";

	    dir.setFilter( QDir::Files );
	    dir.setNameFilters( filters );

	    QStringList files( dir.entryList() );

	    for( QStringList::ConstIterator file = files.begin(); file != files.end(); ++file )
	    {
	        QString filename( *file );
            //qDebug() << "SuperCustomerDisplay playlist filename = " << filename;

	        LOG_DEBUG( "found playlist '%s'", qPrintable( filename ) );
            // do not add the Advertisement Server playlist file to
            // playlist file textbox
            if (filename != QString("ad_service_playlist.txt"))
            {
                //qDebug() << "adding filenamed " << filename << " to option menu";
                ui->cbCustomerDisplayPlaylist->addItem( filename, filename );
            }
	    }

	    int volumeInitial = this->getCustomerVolumeValue();
	    ui->volumeSlider->setSliderPosition( volumeInitial );
	    ui->lblVolumePercentage->setText( QString::number(volumeInitial) + "%" );

	    ui->lblVolume->hide();
	    ui->lblVolumePercentage->hide();
	    ui->volumeSlider->hide();
	}

    ui->leDimmerWaitTime->setText(this->getDimmerTimeoutValue());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------
// Slot to update Volume percentage when slider is moved
//------------------------------------------------------
void superCustomerDisplay::UpdateVolumeDisplay( int newValue )
{
    ui->lblVolumePercentage->setText( QString::number( newValue) + "%" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superCustomerDisplay::SetupChanged()
{
    bSetupChanged = true;

    UpdateUserInterface();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superCustomerDisplay::UpdateUserInterface()
{
    ui->lblPlaybackWaitTimeDesc->setEnabled( ui->chkUseFullScreenAdverts->isChecked() );
    ui->txtPlaybackWaitTime->setEnabled( ui->chkUseFullScreenAdverts->isChecked() );
    ui->lblProductAdvertType->setEnabled( !ui->chkUseFullScreenAdverts->isChecked() );
    ui->cbProductAdvertType->setEnabled( !ui->chkUseFullScreenAdverts->isChecked() );

    // playlist
    ui->lblCustomerDisplayPlaylist->setEnabled( 0 == ui->cbIdleAdvertType->currentIndex() );
    ui->btnEditPlaylist->setEnabled( 0 == ui->cbIdleAdvertType->currentIndex() );
    ui->cbCustomerDisplayPlaylist->setEnabled( 0 == ui->cbIdleAdvertType->currentIndex() );

    // webpage
    ui->lblAdvertWebpage->setEnabled( 1 == ui->cbIdleAdvertType->currentIndex() );
    ui->txtAdvertWebpage->setEnabled( 1 == ui->cbIdleAdvertType->currentIndex() );

    // for remote session we need to disable edit button
    if (( !mainWindow_->isPrimarySessionType() ) && ( ui->btnEditPlaylist->isEnabled() ))
    {
#ifndef QT_DEBUG
        // disable edit button
        ui->btnEditPlaylist->setEnabled( false );
#endif
    }

    // HTs
    // this feature is disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_TRANS_GCD ) )
    {
        ui->chkUseFullScreenAdverts->setEnabled( false );

        ui->lblIdleAdvertType->setEnabled( false );
        ui->cbIdleAdvertType->setEnabled( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------
// Slot for when the Playback Wait Time has changed
//-------------------------------------------------
void superCustomerDisplay::PlaybackWaitTimeChanged()
{
    bPlaybackWaitTimeChanged = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------
// Slot for when the Operator brightness dimmer Time has changed
//-------------------------------------------------
void superCustomerDisplay::DimmerTimeoutWaitTimeChanged( const QString& )
{
    bDimmerWaitTimeChanged = true;

    // Save it too.
    SaveDimmerWaitTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------
// Slot to edit playlist file.
//-------------------------------------------------
void superCustomerDisplay::EditPlaylistFile()
{
    QString filename( (ui->cbCustomerDisplayPlaylist->itemData( ( ui->cbCustomerDisplayPlaylist->currentIndex() ) ) ).toString() );

    if ( filename.length() )
    {
        filename = filesystem::absoluteFilePath( filename, folderType::Playlists );

        if ( mainWindow_->switchToModeScreen<SupervisorMode, superTextFileEditor>() )
        {
            session_->scaleConfig->returnHomeLocation = OriginalScreen::TEXT_EDITOR_GCD_SCREEN;

            superTextFileEditor *editor = dynamic_cast<superTextFileEditor*>( mode_->screen() );

            if ( editor )
                editor->loadFile( filename );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void superCustomerDisplay::SaveDataOnExit()
{
    if ( session_->isServiceCounter())
    {
        if(bPlaybackWaitTimeChanged)
            SavePlaybackWaitTime();

        if ( bDimmerWaitTimeChanged )
            SaveDimmerWaitTime();

        if ( !bSetupChanged )
            return;

        QString param;

        QString gcdLayoutNumber = ui->cbLayoutNumber->itemData( ui->cbLayoutNumber->currentIndex() ).toString();

        QString playlistFile = ui->cbCustomerDisplayPlaylist->itemData( ui->cbCustomerDisplayPlaylist->currentIndex() ).toString();

        // if the idle time advertisment type is Advertisement Server
        // set playlist filename to ad_service_playlist.txt before saving
        if (2 == ui->cbIdleAdvertType->currentIndex())
            playlistFile = "ad_service_playlist.txt";

        param = gcdLayoutNumber;
        param += "," + QString::number( 1 ); // should always be 1 for fastrack; was m_gcdColorSchemeNumber;
        param += "," + gcdSettings_->gcdAllowOperReset();
        param += "," + QString::number( ui->volumeSlider->value() );
        param += "," + ui->txtPlaybackWaitTime->text();
        param += "," + gcdSettings_->mediaServerName();
        param += "," + gcdSettings_->mediaServerIP();
        param += "," + gcdSettings_->ftpUserName();
        param += "," + gcdSettings_->ftpPassword();
        param += "," + gcdSettings_->ftpDirectory();
        param += "," + playlistFile;
        param += "," + gcdSettings_->autoSearch();
        param += "," + gcdSettings_->searchHour();
        param += "," + gcdSettings_->searchMinute();
        param += "," + gcdSettings_->amOrPm();
        param += "," + gcdSettings_->isDnsEnabled();
        param += "," + gcdSettings_->gcdDownloadProtocol();
        param += "," + (ui->cbCustomerDisplayLanguage->itemData( ( ui->cbCustomerDisplayLanguage->currentIndex() ) ) ).toString();
        param += "," + (ui->chkUseFullScreenAdverts->isChecked() ? QString( "0" ) : QString( "1" ) );
        param += "," + (ui->cbIdleAdvertType->itemData( ( ui->cbIdleAdvertType->currentIndex() ) ) ).toString();
        param += "," + ui->txtAdvertWebpage->text();
        param += "," + (ui->cbProductAdvertType->itemData( ( ui->cbProductAdvertType->currentIndex() ) ) ).toString();

        session_->send( ConfigurationDev, CfgGCDSave, param );

        bSetupChanged =
            bPlaybackWaitTimeChanged =
            bDimmerWaitTimeChanged = false;

        if ( playlistFile != gcdSettings_->playlistFile() )
            session_->send( GCDSessionDev, GcdSessionUpdatePlaylist );

        if ( 1 == ui->cbCustomerDisplayLanguage->currentIndex() )
            session_->send( GCDSessionDev, GcdSessionFrench );
        else if ( 2 == ui->cbCustomerDisplayLanguage->currentIndex() )
            session_->send( GCDSessionDev, GcdSessionSpanish );
        else
            session_->send( GCDSessionDev, GcdSessionEnglish );

        // Save customer volume
        setCustomerVolume();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------
// Save the Playback Wait Time
//----------------------------
void superCustomerDisplay::SavePlaybackWaitTime()
{
    session_->send( GCDSessionDev, GcdSessionSetPlaybackWaitTime, ui->txtPlaybackWaitTime->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------
// Save the Dimmer Wait Time
//----------------------------
void superCustomerDisplay::SaveDimmerWaitTime()
{
    // This addition is a design afterthought.
    // This file is called CustomerDisplay but
    // it also has the dimmer timeout for the operator display.

    // Save to the DB first
    updateDB("dimDisplayTimeout", ui->leDimmerWaitTime->text());

    // Activate the new timer
    python pythonProcess;

    // Set backlight of both displays to max
    QStringList args;

    args.append(ui->leDimmerWaitTime->text());
    //        qDebug() << "customer volume is: " << ui->volumeLabel->text();

    if (pythonProcess.execCommand("setDimDisplayTimeout", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            ;
            // TBD
        }
        else
        {
            ;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superCustomerDisplay::setCustomerVolume()
{
    QString newVolume = QString::number(ui->volumeSlider->value());

    QString dbKey = "cVolume";
    // use setVolume.py
    python pythonProcess;

    // Set backlight of both displays to max
    QStringList args;

    args.append(newVolume);
    if (pythonProcess.execCommand("setVolume", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            ;//TBD - updateCustomerVolume();
        }
        else
        {
            ;// TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        ;// TODO:
        // What do we do here?
    }

    updateDB(dbKey, newVolume);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superCustomerDisplay::updateDB( const QString& key, const QString& value )
{
    QString databaseFileName = "settingswizard/scalesettings";
    QString databaseTempFileName = "settingswizard/scalesettingstemp";
    QString databasePath = filesystem::absoluteFilePath("settingswizard", folderType::DBs);
    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
    QString databaseTempFileLocation = filesystem::absoluteFilePath(databaseTempFileName, folderType::DBs);

    QFile file(databaseFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList theLine;
    QStringList fileContents;
    QTextStream in(&file);
    QString line;
    fileContents.clear();

    while (!in.atEnd())
    {
        line = in.readLine();
        theLine = line.split("=", QString::KeepEmptyParts);
        if(theLine.at(0) == key)
        {
            line = key + "=" + value;
        }
        fileContents.append(line);
    }

    file.close();
    QFile fileTemp(databaseTempFileLocation);
    if (!fileTemp.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream outStream(&fileTemp);

    for(int i = 0; i < fileContents.count(); i++)
    {
        outStream << fileContents.at(i) + "\n";
    }

    fileTemp.close();
    QDir dir;
    dir.remove(databaseFileLocation);
    dir.rename(databaseTempFileLocation,databaseFileLocation);
    return true;
}
