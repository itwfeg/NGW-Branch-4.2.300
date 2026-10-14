// Label applier configuration on two printer systems

#include "serviceNewWrapperLAConfig.h"

#include "ui_serviceNewWrapperLAConfig.h"

#include "logmanager.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

serviceNewWrapperLAConfig::serviceNewWrapperLAConfig(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui( new Ui::serviceNewWrapperLAConfig ),
    labelApplierConfigData( new DomLabelApplierConfig() ),
    init( false ),
    hasChanged( false ),
    prePrintStateChanged( false )
{
    ui->setupUi(this);
    scaleScreen();

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
    connect(ui->cbPrimaryLabelPreprint, SIGNAL(clicked()), this, SLOT(prePrintStateChange()));
    connect(ui->cbSecondLabelPreprint, SIGNAL(clicked()), this, SLOT(prePrintStateChange()));

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    update();
    // Set the applier is configuration mode
    session_->send(LabelApplier, LaMode, "5");  // Set label applier mode
    session_->send(LabelApplier, LaReqInquire); // request config from label applier
    session_->send(Wrapper, WrReqCalibration, QString::number(1)); // request configuration

    session_->send( ConfigurationDev, CfgPrepackRead ); // update ui (sends back a CfgPrepackWrite)

    this->initialize();
    translate();
}

serviceNewWrapperLAConfig::~serviceNewWrapperLAConfig()
{
    delete ui;

    if ( labelApplierConfigData )
    {
        delete labelApplierConfigData;
        labelApplierConfigData = NULL;
    }
}

void serviceNewWrapperLAConfig::prePrintStateChange()
{
    prePrintStateChanged = true;
    //this->saveConfig();
}

int serviceNewWrapperLAConfig::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceNewWrapperLAConfig::initialize()
{
    hasChanged = false;
}

void serviceNewWrapperLAConfig::doSave()
{
    // nothing to do until we get config
    if ( !init )
        return;

    hasChanged = true;
    // Not really since if you change it back to what it was then it isn't different, but everyone seems to be doing anyway.
    // set the saving state for the destructor
}

void serviceNewWrapperLAConfig::saveConfig()
{
    LOG_CLASS_INFO( "serviceLabelApplierConfig", "saveConfig" );

    if(prePrintStateChanged)
    {
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        operatorDisplayFields->setPriLabelStockRotated( ui->cbPrimaryLabelPreprint->isChecked() );
        operatorDisplayFields->setSecLabelStockRotated( ui->cbSecondLabelPreprint->isChecked() );

        LOG_INFO( "Sending Primary Pre-Print: %d", ui->cbPrimaryLabelPreprint->isChecked() );
        LOG_INFO( "Sending Secondary Pre-Print: %d", ui->cbSecondLabelPreprint->isChecked() );

        session_->send( *operatorDisplayFields );
        session_->send( ConfigurationDev, CfgPrepackSave );

        session_->send( ConfigurationDev, CfgPrepackRead ); // update ui

        prePrintStateChanged = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperLAConfig::translate()
{
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperLAConfig::onScreenChanging()
{
    this->saveConfig();

    // also set label applier to conveyor mode since we are leaving the screen
    session_->send(LabelApplier, LaMode, "3");
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperLAConfig::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO("serviceLabelApplierConfig", "onSessionMessage()");

    if(ConfigurationDev == msg[0].toInt() && CfgPrepackWrite == msg[1].toInt())
    {
        MadpOperatorDisplayFields m(msg);

        LOG_INFO( "Received Primary Pre-Print: %d", m.priLabelStockRotated() );
        LOG_INFO( "Received Secondary Pre-Print: %d", m.secLabelStockRotated() );

        ui->cbPrimaryLabelPreprint->setChecked( m.priLabelStockRotated() == 0 ? false : true );
        ui->cbSecondLabelPreprint->setChecked( m.secLabelStockRotated() == 0 ? false : true );
    }
    else if( Wrapper == msg[0].toInt() && WrPPPConfig == msg[1].toInt() )
    {
        LOG_INFO( "Received WrPPPConfig", NULL );

        QString sampleRate = QString::number( msg[4].toInt() );
        QString primarySlope = QString::number( msg[5].toFloat() / 1000.0f );
        QString secondarySlope = QString::number( msg[6].toFloat() / 1000.0f );
        QString primaryHomeDist = QString::number( msg[7].toInt() );
        QString secondaryHomeDist = QString::number( msg[8].toInt() );
        QString mechTolerance = QString::number( msg[9].toInt() );

        ui->lblDistPlate1->setText( primaryHomeDist );
        ui->lblDistPlate2->setText( secondaryHomeDist );
        ui->lblSlope1->setText( primarySlope );
        ui->lblSlope2->setText( secondarySlope );
        ui->lblSampleRate->setText( sampleRate );
        ui->lblMechTol1->setText( mechTolerance );
        ui->lblMechTol2->setText( mechTolerance );
    }
}
