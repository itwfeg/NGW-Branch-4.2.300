/**
 * @file aboutdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "aboutdialog.h"
#include "featuresdialog.h"
#include "licensingdialog.h"

#include "../sessioninfo.h"

#include <common/bitmap.h>
#include <common/buffer.h>
#include <ean/eanapi.h>

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTemporaryFile>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
AboutDialog::AboutDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "AboutDialog" );

    // initialize interface
    initialize();
    createLayout();

    // default values
    controllerVersion_->setText( "0.0.0" );
    printerHardware_->setText( "0.0.0" );

    // move to end so we dont block on anyone else
    updateVersionInfo( SnVersionInfo );
    updateVersionInfo( PrSysStatus );

    // These 2 are fast
    // request serial number
    session_->send( MfgTestDev, RequestMfgData );

    // request loader version
    session_->send( MfgTestDev, RequestLoaderVersion );

    // request version info
    //session_->send( PrinterSystem, PrSysReqStatus, "0" );     // REQ this - get this PrinterSystem,PrSysStatus moved to onSessionMessage()

    // request version info
    //session_->send( SessionDev, SnVersionInfo );              // locked updates of messages above it, so move it to the onSessionUpdate() method

    // FT-415
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
AboutDialog::~AboutDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *AboutDialog::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int AboutDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "AboutDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // version info
    if (( SessionDev == device ) && ( SnVersionInfo == event ))
    {
        LOG_DEBUG( "got version info...", NULL );

        updateVersionInfo( SnVersionInfo );
    }

    // printer version info
    else if (( PrinterSystem == device ) && ( PrSysStatus == event ))           // PrinterSystem,PrSysStatus
    {
        LOG_DEBUG( "got printer version info...", NULL );

        updateVersionInfo( PrSysStatus );
    }

    // scale serial info
    else if (( MfgTestDev == device ) && ( ReadMfgData == event ))
    {
        LOG_DEBUG( "got scale version info...", NULL );
        DomMfgReadData m( msg );

        scaleSerialNumber_->setText( m.serialNumber() );
    }

    // loader version
    else if (( MfgTestDev == device ) && ( ReadLoaderVersion == event ))
    {
        LOG_DEBUG( "got loader version...", NULL );
        DomMfgReadLoaderVersion m( msg );

        // FT-1396: show hwrev
        if ( hwRev_.length() )
        {
            loaderVersionLabel_->setText( tr( "System Controller" ) + " / " + tr( "Loader Version:" ) );
            loaderVersion_->setText( hwRev_ + " / " + m.version().simplified() );
        }
        else
        {
            loaderVersion_->setText( m.version().simplified() );
        }

        // request Printer Status info
        session_->send( PrinterSystem, PrSysReqStatus, "0" );       // REQ this - get this PrinterSystem,PrSysStatus

        // request version info - finally do it now because all other messages have been processed
        session_->send( SessionDev, SnVersionInfo );
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::updateVersionInfo( int versiondata )
{
    LOG_CLASS_INFO( "AboutDialog", "updateVersionInfo" );

    if( PrSysStatus == versiondata )
    {
        DomPrinterPowerupTests *versioninfo_ = &session_->scaleConfig->printerPowerupTests;

        if( controllerVersion_->text() != versioninfo_->softwareVersion() )
            controllerVersion_->setText( versioninfo_->softwareVersion() );

        if( printerHardware_->text() != versioninfo_->hardwareVersion() )
            printerHardware_->setText( versioninfo_->hardwareVersion() );
    }
    else if( SnVersionInfo == versiondata )
    {
        scaleName_->setText( session_->uiUtilities->scaleName() );
        scaleModel_->setText( session_->uiUtilities->scaleModel() );

        // SOF-5319
        QString specialCharacter = session_->uiUtilities->checkSpecialConfig();

        DomCurrentScaleSetup *versioninfo_ = &session_->scaleConfig->currentScaleSetup;

        if( versioninfo_ == NULL )
            return;

        if( !specialCharacter.isEmpty() )
        {
            LOG_ERROR("Custom configuration file found", NULL);
            QString newBuildNum = versioninfo_->buildNumber() + " " + specialCharacter;
            if( newBuildNum != buildNumberLabel_->text() )
                buildNumber_->setText(newBuildNum);
        }
        else
        {
            LOG_ERROR("Custom configuration file not found", NULL);
            if( buildNumber_->text() != versioninfo_->buildNumber() )
                buildNumber_->setText(versioninfo_->buildNumber());
        }

        //=== SOF-5319 ====

        if( version_->text() != versioninfo_->version() )
            version_->setText( versioninfo_->version() );

        if( osVersion_->text() != versioninfo_->osVersion() )
            osVersion_->setText( versioninfo_->osVersion() );

        if( copyright_->text() != session_->uiUtilities->scaleCopyright() )
            copyright_->setText( session_->uiUtilities->scaleCopyright() );

        //Use below code to get from DB file
        if( ip_->text() != versioninfo_->scaleIpAddress() )
            ip_->setText( versioninfo_->scaleIpAddress() );

        // SOF-353
        if ( session_->uiUtilities->isFeatureLicensed( SCALE_REMOTE_PRINTING ) )
        {
            const QString address( versioninfo_->scaleIpAddress() );

            if (( address.length() ) && ( "0.0.0.0" != address ))
            {
                QStringList octet( address.split( '.' ) );

                if ( 4 == octet.size() )
                {
                    // encode ip address to barcode
                    QString barcode;

                    for ( unsigned int i( 0 ); i < 4; ++i )
                    {
                        while ( 3 != octet[i].size() )
                            octet[i].prepend( '0' );

                        barcode.append( octet[i] );
                    }

                    // render barcode
                    bitmap b( 128, 64 );


                    bool thisIsReal = true;
                    #ifdef QT_DEBUG
                        thisIsReal = false;
                    #endif

                    if ( thisIsReal && eanapi::encode( barcode.toStdString(), ean::EAN13, b, 0, 0, b.width(), b.height() ) )
                    {
                        QTemporaryFile f;
                        f.setAutoRemove( true );

                        if ( f.open() )
                        {
                            const QString filename( f.fileName() );

                            const buffer img( b.toImage( "png" ) );
                            img.save( filename.toStdString() );

                            eanAddress_->setPixmap( QPixmap( filename ) );

                            f.close();
                        }
                    }
                }
            }
        }

        if( hostname_->text() != versioninfo_->scaleHostName() )
            hostname_->setText( versioninfo_->scaleHostName() );

        // gateway is now populated from the db file, use below code to get from DB file
        if(  gateway_->text() != versioninfo_->gatewayAddress() )
            gateway_->setText( versioninfo_->gatewayAddress() );

        if( dns_->text() != versioninfo_->scaleDNSAddress() )
            dns_->setText( versioninfo_->scaleDNSAddress() );

        if( lastComm_->text() != versioninfo_->lastComm() )
            lastComm_->setText( versioninfo_->lastComm() );

        if( lastItemChange_->text() != versioninfo_->lastItemComm() )
            lastItemChange_->setText( versioninfo_->lastItemComm() );

        // FT-1364
        if( smBackendVersion_->text() != versioninfo_->smBackendVersion().trimmed() )
            smBackendVersion_->setText( versioninfo_->smBackendVersion().trimmed() );

        // FT-1396: save off hwrev
        hwRev_ = versioninfo_->hwRev().trimmed();

        // FT-1566
        installedFeatures_ = versioninfo_->field<QStringList>( versioninfo_->features() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString AboutDialog::getGatewayValue()
{
   QString gateway = "0.0.0.0"; // fail safe value
   QString databaseFileName = "settingswizard/scalesettings";
   QString databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

   QFile file(databaseFileLocation);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return gateway;

   QStringList theLine;
   bool foundIt = false;
   QTextStream in(&file);
   while (!in.atEnd() && !foundIt) {
       QString line = in.readLine();
       theLine = line.split("=", QString::KeepEmptyParts);
       if(theLine.at(0) == "gateway")
       {
           gateway = theLine.at(1);
           foundIt = true;
       }
   }
   file.close();
   if(gateway == "64641964" || gateway == "0.0.0.0")
   {
       gateway = tr("No gateway assigned");
   }
   return gateway;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString AboutDialog::getIPValue()
{
   QString ipAddress = "0.0.0.0"; // fail safe value
   QString databaseFileName = "settingswizard/scalesettings";
   QString databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

   QFile file(databaseFileLocation);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return ipAddress;

   QStringList theLine;
   bool foundIt = false;
   QTextStream in(&file);
   while (!in.atEnd() && !foundIt) {
       QString line = in.readLine();
       theLine = line.split("=", QString::KeepEmptyParts);
       if(theLine.at(0) == "ipAddress")
       {
           ipAddress = theLine.at(1);
           foundIt = true;
       }
   }
   file.close();
   if(ipAddress == "64641964" || ipAddress == "0.0.0.0")
   {
       ipAddress = tr("No IP assigned");
   }
   return ipAddress;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::onFeaturesPressed()
{
    FeaturesDialog *dialog( new FeaturesDialog( installedFeatures_, mainWindow_ ) );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::onLicensingPressed()
{
    LicensingDialog *dialog( new LicensingDialog( mainWindow_ ) );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::initialize()
{
    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );
    setForegroundPalette( this );

    // create widgets
    scaleNameLabel_ = new QLabel( tr( "Name:" ), this );
    scaleNameLabel_->setObjectName( "scaleNameLabel" );

    scaleName_ = new QLabel( this );
    scaleName_->setObjectName( "scaleName" );

    scaleModelLabel_ = new QLabel( tr( "Scale Model:" ), this );
    scaleModelLabel_->setObjectName( "scaleModelLabel" );

    scaleModel_ = new QLabel( this );
    scaleModel_->setObjectName( "scaleModel" );

    buildNumberLabel_ = new QLabel( tr( "Build Number:" ), this );
    buildNumberLabel_->setObjectName( "buildNumberLabel" );

    buildNumber_ = new QLabel( this );
    buildNumber_->setObjectName( "buildNumber" );

    versionLabel_ = new QLabel( tr( "Application Version:" ), this );
    versionLabel_->setObjectName( "versionLabel" );

    version_ = new QLabel( this );
    version_->setObjectName( "version" );

    osVersionLabel_ = new QLabel( tr( "Operating System Version:" ), this );
    osVersionLabel_->setObjectName( "osVersionLabel" );

    osVersion_ = new QLabel( this );
    osVersion_->setObjectName( "osVersion" );

    loaderVersionLabel_ = new QLabel( tr( "Loader Version:" ), this );
    loaderVersionLabel_->setObjectName( "loaderVersionLabel" );

    loaderVersion_ = new QLabel( this );
    loaderVersion_->setObjectName( "loaderVersion" );

    controllerVersionLabel_ = new QLabel( tr( "Peripheral Software Version:" ), this );
    controllerVersionLabel_->setObjectName( "controllerVersionLabel" );

    controllerVersion_ = new QLabel( this );
    controllerVersion_->setObjectName( "controllerVersion" );

    printerHardwareLabel_ = new QLabel( tr( "Peripheral Hardware Version:" ), this );
    printerHardwareLabel_->setObjectName( "printerHardwareLabel" );

    printerHardware_ = new QLabel( this );
    printerHardware_->setObjectName( "printerHardware" );

    scaleSerialNumberLabel_ = new QLabel( tr( "Scale Serial Number:" ), this );
    scaleSerialNumberLabel_->setObjectName( "scaleSerialNumberLabel" );

    scaleSerialNumber_ = new QLabel( this );
    scaleSerialNumber_->setObjectName( "scaleSerialNumber" );

    copyrightLabel_ = new QLabel( tr( "Copyright:" ), this );
    copyrightLabel_->setObjectName( "copyrightLabel" );

    copyright_ = new QLabel( this );
    copyright_->setObjectName( "copyright" );
    setFontPointSize12( copyright_ );

    ipLabel_ = new QLabel( tr( "IP Address:" ), this );
    ipLabel_->setObjectName( "ipLabel" );

    ip_ = new QLabel( this );
    ip_->setObjectName( "ip" );

    hostnameLabel_ = new QLabel( tr( "Host Name:" ), this );
    hostnameLabel_->setObjectName( "hostnameLabel" );

    hostname_ = new QLabel( this );
    hostname_->setObjectName( "hostname" );

    gatewayLabel_ = new QLabel( tr( "Gateway Address:" ), this );
    gatewayLabel_->setObjectName( "gatewayLabel" );

    gateway_ = new QLabel( this );
    gateway_->setObjectName( "gateway" );

    dnsLabel_ = new QLabel( tr( "DNS Address:" ), this );
    dnsLabel_->setObjectName( "dnsLabel" );

    dns_ = new QLabel( this );
    dns_->setObjectName( "dns" );

    // FT-1364
    smBackendLabel_ = new QLabel( tr( "Scale Management Backend Version:" ), this );
    smBackendLabel_->setObjectName( "smBackendLabel" );
    smBackendVersion_ = new QLabel( this );
    smBackendVersion_->setObjectName( "smBackendVersion" );

    lastCommLabel_ = new QLabel( tr( "Last Communication Occurred:" ), this );
    lastCommLabel_->setObjectName( "lastCommLabel" );

    lastComm_ = new QLabel( this );
    lastComm_->setObjectName( "lastComm" );

    lastItemChangeLabel_ = new QLabel( tr( "Last Item Change Received:" ), this );
    lastItemChangeLabel_->setObjectName( "lastItemChangeLabel" );

    lastItemChange_ = new QLabel( this );
    lastItemChange_->setObjectName( "lastItemChange" );

    swrevision_ = new QLabel( this );
    swrevision_->setObjectName( "swrevision" );
    eanAddress_ = new QLabel( this );

    features_ = new QPushButton( tr( "Features" ), this );
    features_->setObjectName( "features" );
    connect( features_, SIGNAL( pressed() ), SLOT( onFeaturesPressed() ) );

    licensing_ = new QPushButton( tr( "Licensing" ), this );
    licensing_->setObjectName( "licensing" );
    connect( licensing_, SIGNAL( pressed() ), SLOT( onLicensingPressed() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AboutDialog::createLayout()
{
    QFormLayout *topLayout = new QFormLayout();
    topLayout->addRow( scaleNameLabel_, scaleName_ );
    topLayout->addRow( scaleModelLabel_, scaleModel_ );
    topLayout->addRow( buildNumberLabel_, buildNumber_ );
    topLayout->addRow( versionLabel_, version_ );
    topLayout->addRow( osVersionLabel_, osVersion_ );
    topLayout->addRow( loaderVersionLabel_, loaderVersion_ );
    topLayout->addRow( controllerVersionLabel_, controllerVersion_ );
    topLayout->addRow( printerHardwareLabel_, printerHardware_ );
    topLayout->addRow( scaleSerialNumberLabel_, scaleSerialNumber_ );
    topLayout->addRow( copyrightLabel_, copyright_ );
    topLayout->addItem( new QSpacerItem( 16, 16, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    topLayout->addRow( ipLabel_, ip_ );
    topLayout->addRow( hostnameLabel_, hostname_ );
    topLayout->addRow( gatewayLabel_, gateway_ );
    topLayout->addRow( dnsLabel_, dns_ );
    // FT-1364
    topLayout->addRow( smBackendLabel_, smBackendVersion_ );
    topLayout->addItem( new QSpacerItem( 16, 16, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    topLayout->addRow( lastCommLabel_, lastComm_ );
    topLayout->addRow( lastItemChangeLabel_, lastItemChange_ );

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QString rev_file( "/etc/hobart/software_revision.ini" );
    #ifdef QT_DEBUG
        rev_file = "C:/hobart/software_revision.ini";
    #endif

    if ( QFile::exists( rev_file ) )
    {
        QFile f( rev_file );
        if ( f.open( QIODevice::ReadOnly ) )
        {
            QTextStream is( &f );
            QString rev( is.readLine().trimmed() );
            if( ( rev.length() > 0 ) && ( rev != "A" ) )
                swrevision_->setText( rev );
            f.close();
        }
    }

    if(swrevision_->text().length() > 0 )
    {
        //bottomLayout->addStretch();
        bottomLayout->addWidget( swrevision_ );
        bottomLayout->addStretch();
    }
    else
        bottomLayout->addStretch();

    bottomLayout->addWidget( eanAddress_ );
    bottomLayout->addStretch();
    bottomLayout->addWidget( features_ );
    bottomLayout->addWidget( licensing_ );

    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addLayout( topLayout );
    screenLayout->addLayout( bottomLayout );
}
