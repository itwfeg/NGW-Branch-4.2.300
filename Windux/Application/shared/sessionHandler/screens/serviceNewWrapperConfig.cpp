/**
 * @file serviceNewWrapperConfig.cpp
 * @brief Shows & Edits Wrapper Config
 *
 * @copyright Copyright (c) 2014-2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author: Todd Davis, Randy Blankley
 */

#include <logmanager.h>

#include "serviceNewWrapperConfig.h"

#include "../sessionmainwindow.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/servicemode.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <logmanager.h>
#include <numericlineedit.h>
#include <palette.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperConfig::serviceNewWrapperConfig( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ready_( false ),
    hasChanged_( false ),
    isTwoPrinterSystem_( false )
{
    LOG_CLASS_INFO( "snwSetConfig", "serviceNewWrapperConfig" );

    LOG_TRACE( "serviceNewWrapperConfig() Constructor START", NULL );

    internalConfigData_ = &session_->scaleConfig->internalConfig;
    internalConfigData_->messagesSuppressed = true;

    evolWrapperConfigData_ = &session_->scaleConfig->evolWrapperConfig;
    //evolWrapperHeaterConfigData_ = &session_->scaleConfig->evolWrapperHeaterConfig;
    evolWrapperSystemInformation_ = &session_->scaleConfig->evolWrapperSystemInformation;
    peripheralList_ = &session_->scaleConfig->peripheralList;

    // initialize
    LOG_TRACE( "serviceNewWrapperConfig() Calling initialize()", NULL );
    initialize();
    LOG_TRACE( "serviceNewWrapperConfig() Calling createLayout", NULL );
    createLayout();
    LOG_TRACE( "serviceNewWrapperConfig() Calling translate()", NULL );
    translate();

    toggleWrapperConfigs();

    // request system info (when we dont have it)
    if ( 0 == evolWrapperSystemInformation_->awsManufactureDate().toInt() )
        session_->send( Wrapper, WrReqEvolSystemInfo );

    // request configuration
    session_->send( Wrapper, WrReqInquire );

    // get label applier configuration
    session_->send( LabelApplier, LaReqInquire );

    // request heater information
    // session_->send( Wrapper, WrReqFilmHeater );

    LOG_TRACE( "serviceNewWrapperConfig() Constructor END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperConfig::~serviceNewWrapperConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::translate()
{
    LOG_CLASS_INFO( "snwSetConfig", "translate" );

    LOG_TRACE( "serviceNewWrapperConfig() translate() START", NULL );

    title_->setText( tr( "Wrapper Configuration" ) );

    sealerBeltSettingLabel_->setText( tr( "Sealer belt setting" ) );
    lowerFilmWidthLabel_->setText( tr( "Lower film width" ) );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
        upperFilmWidthLabel_->setText( tr( "Film width" ) );
    else
        upperFilmWidthLabel_->setText( tr( "Upper film width" ) );

    priLabelOffsetLabel_->setText( tr( "Primary label offset" ) );
    priRotationCalibrationOffsetLabel_->setText( tr( "Primary rotation calibration offset (0 degree)" ) );
    priRotationCalibrationOffset_90DegLabel_->setText( tr( "Primary rotation calibration offset (90 degree)" ) );
    priRotationCalibrationOffset_180DegLabel_->setText( tr( "Primary rotation calibration offset (180 degree)" ) );
    priRotationCalibrationOffset_270DegLabel_->setText( tr( "Primary rotation calibration offset (270 degree)" ) );
    priWandCalibrationOffsetLabel_->setText( tr( "Primary wand calibration offset" ) );

    packageDirectionLabel_->setText( tr( "Package direction" ) );
    secondCLALabel_->setText( tr( "Second CLA Attached" ) );
    wrapperOperationLabel_->setText( tr( "Wrapper operation" ) );
    hardwareAddOnLabel_->setText( tr( "Hardware add on" ) );

    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        secLabelOffsetLabel_->setText( tr( "Secondary label offset" ) );
        secRotationCalibrationOffsetLabel_->setText( tr( "Secondary rotation calibration offset (0 degree)" ) );
        secRotationCalibrationOffset_90DegLabel_->setText( tr( "Secondary rotation calibration offset (90 degree)" ) );
        secRotationCalibrationOffset_180DegLabel_->setText( tr( "Secondary rotation calibration offset (180 degree)" ) );
        secRotationCalibrationOffset_270DegLabel_->setText( tr( "Secondary rotation calibration offset (270 degree)" ) );
        secWandCalibrationOffsetLabel_->setText( tr( "Secondary wand calibration offset" ) );
        secApplierHandAdjustLabel_->setText( tr( "Secondary Applier Hand Adjust" ) );
    }
    priApplierHandAdjustLabel_->setText( tr( "Primary Applier Hand Adjust" ) );


    automaticRotationLabel_->setText( tr( "Automatic rotation" ) );
    sealerBeltPkgSensorTypeLabel_->setText( tr( "Sealer belt package sensor type" ) );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
        upperFilmPullTensionLabel_->setText( tr( "Film pull tension adjust" ) );
    else
        upperFilmPullTensionLabel_->setText( tr( "Upper film pull tension adjust" ) );
    lowerFilmPullTensionLabel_->setText( tr( "Lower film pull tension adjust" ) );

    lblFilmHeater_->setText( tr("Film Heater") );

    if ( 9 <= lowerFilmWidth_->count() )
        lowerFilmWidth_->setItemText( 8, ready_ ? tr( "Unknown" ) : tr( "Waiting..." ) );

    if ( 9 <= upperFilmWidth_->count() )
        upperFilmWidth_->setItemText( 8, ready_ ? tr( "Unknown" ) : tr( "Waiting..." ) );

    packageDirection_->setItemText( 0, tr( "Right" ) );
    packageDirection_->setItemText( 1, tr( "Left" ) );
    if ( 3 <= packageDirection_->count() )
        packageDirection_->setItemText( 2, ready_ ? tr( "Unknown" ) : tr( "Waiting..." ) );

    secondCLA_->setItemText( 0, tr( "No" ) );
    secondCLA_->setItemText( 1, tr( "Yes" ) );

    wrapperOperation_->setItemText( 0, tr( "Automatic" ) );
    wrapperOperation_->setItemText( 1, tr( "Manual" ) );

    hardwareAddOn_->setItemText( 0, tr( "None" ) );
    hardwareAddOn_->setItemText( 1, tr( "Second Printer/Applier" ) );

    automaticRotation_->setItemText( 0, tr( "No" ) );
    automaticRotation_->setItemText( 1, tr( "Yes" ) );

    sealerBeltPkgSensorType_->setItemText( 0, tr( "Beam Breaker" ) );
    sealerBeltPkgSensorType_->setItemText( 1, tr( "Reflective" ) );

    upperFilmPullTension_->setItemText( 0, tr( "0 - Default" ) );
    upperFilmPullTension_->setItemText( 1, tr( "+1" ) );
    upperFilmPullTension_->setItemText( 2, tr( "+2" ) );
    upperFilmPullTension_->setItemText( 3, tr( "+3" ) );
    upperFilmPullTension_->setItemText( 4, tr( "+4" ) );
    upperFilmPullTension_->setItemText( 5, tr( "+5" ) );

    lowerFilmPullTension_->setItemText( 0, tr( "0 - Default" ) );
    lowerFilmPullTension_->setItemText( 1, tr( "+1" ) );
    lowerFilmPullTension_->setItemText( 2, tr( "+2" ) );
    lowerFilmPullTension_->setItemText( 3, tr( "+3" ) );
    lowerFilmPullTension_->setItemText( 4, tr( "+4" ) );
    lowerFilmPullTension_->setItemText( 5, tr( "+5" ) );

    cboFilmHeater_->setItemText( 0, tr( "Off" ) );
    cboFilmHeater_->setItemText( 1, tr( "On" ) );

    toggleGeneralWrapperConfigButton_->setText( tr( "Previous" ) );
    toggleLabelApplierConfigButton_->setText( tr( "Next" ) );

    LOG_TRACE( "serviceNewWrapperConfig() translate() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::resizeEvent( QResizeEvent *e )
{
    title_->setFixedSize( (width() * 80) / 100, 64 );

    _Mybase::resizeEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperConfig::onScreenChanging()
{
    saveWrapperConfig();
    internalConfigData_->messagesSuppressed = false;
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // 9,56
    if (( Wrapper == device ) && ( WrEvolInquire == event ))
    {
        DomEvolWrapperConfig m( msg );


        int tmpIndex = 0;

        if( upperFilmWidth_ )
        {
            tmpIndex = qMax( 0, upperFilmWidth_->findData( m.upperRollWidth() ) );
            if( upperFilmWidth_->count() >= tmpIndex )
                upperFilmWidth_->setCurrentIndex( tmpIndex );
        }

        if( lowerFilmWidth_ )
        {
            tmpIndex = qMax( 0, lowerFilmWidth_->findData( m.lowerRollWidth() ) );
            if( lowerFilmWidth_->count() >= tmpIndex )
                lowerFilmWidth_->setCurrentIndex( tmpIndex );
        }

        if( sealerBeltSetting_)
            sealerBeltSetting_->setText( m.sealerBeltSetting() );

        if( packageDirection_ )
        {
            tmpIndex = qMax( 0, packageDirection_->findData( m.packageDirectionRaw() ) );
            if( packageDirection_->count() >= tmpIndex )
            {
                packageDirection_->setCurrentIndex( tmpIndex );

                if(!isTwoPrinterSystem_)
                {
                    if (packageDirection_->currentIndex() == 0)
                    { 
			//SOF-5163
                        // right
                        priRotationCalibrationOffsetLabel_->setText( tr( "Primary rotation calibration offset (0 degree)" ) );
                        priRotationCalibrationOffset_90DegLabel_->setText( tr( "Primary rotation calibration offset (90 degree)" ) );
                        priRotationCalibrationOffset_180DegLabel_->setText( tr( "Primary rotation calibration offset (180 degree)" ) );
                        priRotationCalibrationOffset_270DegLabel_->setText( tr( "Primary rotation calibration offset (270 degree)" ) );

                        secRotationCalibrationOffsetLabel_->setText( tr( "Secondary rotation calibration offset (0 degree)" ) );
                        secRotationCalibrationOffset_90DegLabel_->setText( tr( "Secondary rotation calibration offset (90 degree)" ) );
                        secRotationCalibrationOffset_180DegLabel_->setText( tr( "Secondary rotation calibration offset (180 degree)" ) );
                        secRotationCalibrationOffset_270DegLabel_->setText( tr( "Secondary rotation calibration offset (270 degree)" ) );
                    }
                    else if (packageDirection_->currentIndex() == 1)
                    {     
			//SOF-5163
                        // left
                        priRotationCalibrationOffsetLabel_->setText( tr( "Primary rotation calibration offset (0 degree)" ) );
                        priRotationCalibrationOffset_90DegLabel_->setText( tr( "Primary rotation calibration offset (90 degree)" ) );
                        priRotationCalibrationOffset_180DegLabel_->setText( tr( "Primary rotation calibration offset (180 degree)" ) );
                        priRotationCalibrationOffset_270DegLabel_->setText( tr( "Primary rotation calibration offset (270 degree)" ) );

                        secRotationCalibrationOffsetLabel_->setText( tr( "Secondary rotation calibration offset (0 degree)" ) );
                        secRotationCalibrationOffset_90DegLabel_->setText( tr( "Secondary rotation calibration offset (90 degree)" ) );
                        secRotationCalibrationOffset_180DegLabel_->setText( tr( "Secondary rotation calibration offset (180 degree)" ) );
                        secRotationCalibrationOffset_270DegLabel_->setText( tr( "Secondary rotation calibration offset (270 degree)" ) );
                    }
                }
            }
        }

        if( priLabelOffset_ )
            priLabelOffset_->setText( m.primaryPlacementOffset() );

        if( priRotationCalibrationOffset_ )
            priRotationCalibrationOffset_->setText( m.priRotationCalibOffset() );

        if( priRotationCalibrationOffset_90Deg_ )
            priRotationCalibrationOffset_90Deg_->setText( m.priRotationCalibrationOffset_90Deg() );

        if( priRotationCalibrationOffset_180Deg_ )
            priRotationCalibrationOffset_180Deg_->setText( m.priRotationCalibrationOffset_180Deg() );

        if( priRotationCalibrationOffset_270Deg_ )
            priRotationCalibrationOffset_270Deg_->setText( m.priRotationCalibrationOffset_270Deg() );

        tmpIndex = qMax( 0, secondCLA_->findData( m.secondCLAAttached() ) );
        if( secondCLA_->count() >= tmpIndex )
            secondCLA_->setCurrentIndex( tmpIndex );

        if( wrapperOperation_ )
        {
            tmpIndex = qMax( 0, wrapperOperation_->findData( m.wrapperOperation() ) );
            if( wrapperOperation_->count() >= tmpIndex )
                wrapperOperation_->setCurrentIndex( tmpIndex );
        }

        if( hardwareAddOn_ )
        {
            tmpIndex = qMax( 0, hardwareAddOn_->findData( m.hardwareAddOnRaw() ) );
            if( hardwareAddOn_->count() >= tmpIndex )
                hardwareAddOn_->setCurrentIndex( tmpIndex );
        }

        if( secLabelOffset_ )
            secLabelOffset_->setText( m.secondaryPlacementOffset() );

        if( secRotationCalibrationOffset_ )
            secRotationCalibrationOffset_->setText( m.secRotationCalibOffset() );

        if( secRotationCalibrationOffset_90Deg_ )
            secRotationCalibrationOffset_90Deg_->setText( m.secRotationCalibrationOffset_90Deg() );

        if( secRotationCalibrationOffset_180Deg_ )
            secRotationCalibrationOffset_180Deg_->setText( m.secRotationCalibrationOffset_180Deg() );

        if( secRotationCalibrationOffset_270Deg_ )
            secRotationCalibrationOffset_270Deg_->setText( m.secRotationCalibrationOffset_270Deg() );

        if( priApplierHandAdjust_ )
        {
            priApplierHandAdjust_->setText( m.priApplierHandAdjust() );
        }

        if( secApplierHandAdjust_ )
        {
            secApplierHandAdjust_->setText( m.secApplierHandAdjust() );
        }

        if( automaticRotation_ )
        {
            tmpIndex = qMax( 0, automaticRotation_->findData( m.autoRotationRaw() ) );
            if( automaticRotation_->count() >= tmpIndex )
                automaticRotation_->setCurrentIndex( tmpIndex );
        }

        if( priWandCalibrationOffset_ )
            priWandCalibrationOffset_->setText( m.priWandCalibOffset() );

        if( secWandCalibrationOffset_ )
            secWandCalibrationOffset_->setText( m.secWandCalibOffset() );

        if( sealerBeltPkgSensorType_ )
        {
            tmpIndex = qMax( 0, sealerBeltPkgSensorType_->findData( m.sealerBeltPkgSensorType() ) );
            if( sealerBeltPkgSensorType_->count() >= tmpIndex )
                sealerBeltPkgSensorType_->setCurrentIndex( tmpIndex );
        }

        if( upperFilmWidth_ )
        {
            if( upperFilmWidth_->count() >= 8 )
                upperFilmWidth_->removeItem( 8 );
        }

        if( lowerFilmWidth_ )
        {
            if( lowerFilmWidth_->count() >= 8 )
                lowerFilmWidth_->removeItem( 8 );
        }

        if( packageDirection_ )
        {
            if( packageDirection_->count() >= 2 )
                packageDirection_->removeItem( 2 );
        }

        if( upperFilmPullTension_ )
        {
            tmpIndex = m.upperFilmPullTension().toInt();
            if( upperFilmPullTension_->count() >= tmpIndex )
                upperFilmPullTension_->setCurrentIndex( tmpIndex );
            else
                upperFilmPullTension_->setCurrentIndex( 0 );
        }

        if( lowerFilmPullTension_ )
        {
            tmpIndex = m.lowerFilmPullTension().toInt();
            if( lowerFilmPullTension_->count() >= tmpIndex )
                lowerFilmPullTension_->setCurrentIndex( tmpIndex );
            else
                lowerFilmPullTension_->setCurrentIndex( 0 );
        }

        if( cboFilmHeater_ )
        {
            tmpIndex = m.heaterIsUsed().toInt();
            if( cboFilmHeater_->count() >= tmpIndex )
                cboFilmHeater_->setCurrentIndex( tmpIndex );
            else
                cboFilmHeater_->setCurrentIndex( 0 );
        }


        // NGW-43
        ready_ = true;

        // update!
        updateFormFields();
    }
    else if (( LabelApplier == device ) && (( LaConfig == event ) || ( LaInquire == event )))
    {
        DomLabelApplierConfig m( msg );

        isTwoPrinterSystem_ = m.secondaryStationInstalled();

    }

    // label applier config (evolution)
    else if (( LabelApplier == device ) && ( EvolLaConfig == event ))
    {
        DomEvolLabelApplierConfig m( msg );

        isTwoPrinterSystem_ = m.hasSecondPrinter();
    }

    /* not needed
    else if (( Wrapper == device ) && ( WrEvolFilmHeater == event ))
    {
        DomEvolWrapperHeaterConfig m( msg );

        int tmpIndex = m.heaterIsUsed();
        if( cboFilmHeater_->count() >= tmpIndex )
            cboFilmHeater_->setCurrentIndex( tmpIndex );
        else
            cboFilmHeater_->setCurrentIndex( 0 );
    }
    */
    _Mybase::onSessionMessage( msg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::onFieldUpdated()
{
    // data is different
    hasChanged_ = ready_;

    // check numeric limits
    hobart::numericLineEdit *edit( qobject_cast<hobart::numericLineEdit*>( sender() ) );

    if ( edit )
    {
        const QIntValidator *v( edit->validator() );

        if ( v )
        {
            const int value( edit->toInt() );

            if (( value < v->bottom() ) || ( v->top() < value ))
            {
                int nominalValue( 10 );

                if ( sealerBeltSetting_ == sender() )
                    nominalValue = 8;

                if ( packageDirection_ == sender() )
                {
                    if(!isTwoPrinterSystem_)
                    {
                        if (packageDirection_->currentIndex() == 0)
                        { 
			    //SOF-5163
                            // right
                            priRotationCalibrationOffsetLabel_->setText( tr( "Primary rotation calibration offset (0 degree)" ) );
                            priRotationCalibrationOffset_90DegLabel_->setText( tr( "Primary rotation calibration offset (90 degree)" ) );
                            priRotationCalibrationOffset_180DegLabel_->setText( tr( "Primary rotation calibration offset (180 degree)" ) );
                            priRotationCalibrationOffset_270DegLabel_->setText( tr( "Primary rotation calibration offset (270 degree)" ) );

                            secRotationCalibrationOffsetLabel_->setText( tr( "Secondary rotation calibration offset (0 degree)" ) );
                            secRotationCalibrationOffset_90DegLabel_->setText( tr( "Secondary rotation calibration offset (90 degree)" ) );
                            secRotationCalibrationOffset_180DegLabel_->setText( tr( "Secondary rotation calibration offset (180 degree)" ) );
                            secRotationCalibrationOffset_270DegLabel_->setText( tr( "Secondary rotation calibration offset (270 degree)" ) );
                        }
                        else if (packageDirection_->currentIndex() == 1)
                        {                       
			  //SOF-5163   
			  // left
                            priRotationCalibrationOffsetLabel_->setText( tr( "Primary rotation calibration offset (0 degree)" ) );
                            priRotationCalibrationOffset_90DegLabel_->setText( tr( "Primary rotation calibration offset (90 degree)" ) );
                            priRotationCalibrationOffset_180DegLabel_->setText( tr( "Primary rotation calibration offset (180 degree)" ) );
                            priRotationCalibrationOffset_270DegLabel_->setText( tr( "Primary rotation calibration offset (270 degree)" ) );

                            secRotationCalibrationOffsetLabel_->setText( tr( "Secondary rotation calibration offset (0 degree)" ) );
                            secRotationCalibrationOffset_90DegLabel_->setText( tr( "Secondary rotation calibration offset (90 degree)" ) );
                            secRotationCalibrationOffset_180DegLabel_->setText( tr( "Secondary rotation calibration offset (180 degree)" ) );
                            secRotationCalibrationOffset_270DegLabel_->setText( tr( "Secondary rotation calibration offset (270 degree)" ) );
                        }
                    }
                }

                edit->blockSignals( true );
                edit->setText( QString::number( nominalValue ) );
                edit->blockSignals( false );
            }
        }
    }

    updateFormFields();
}

void serviceNewWrapperConfig::onHeaterClicked()
{
    // If this changes we want to send a save message to Application
    ready_ = true;
    hasChanged_ = true;
}

void serviceNewWrapperConfig::toggleApplierConfigs()
{
    title_->setText( tr( "Label Applier Configuration" ) );

    for( size_t i = 0; i < applierConfigWidgets_.size(); ++i )
    {
        applierConfigWidgets_[i]->show();
    }

    for( size_t i = 0; i < wrapperConfigWidgets_.size(); ++i )
    {
        wrapperConfigWidgets_[i]->hide();
    }

    toggleLabelApplierConfigButton_->setEnabled( false );
    toggleGeneralWrapperConfigButton_->setEnabled( true );
    checkSecondaryLAConfigs();

}

void serviceNewWrapperConfig::toggleWrapperConfigs()
{
    title_->setText( tr( "Wrapper Configuration" ) );

    for( size_t i = 0; i < applierConfigWidgets_.size(); ++i )
    {
        applierConfigWidgets_[i]->hide();
    }

    for( size_t i = 0; i < wrapperConfigWidgets_.size(); ++i )
    {
        wrapperConfigWidgets_[i]->show();
    }

    toggleLabelApplierConfigButton_->setEnabled( true );
    toggleGeneralWrapperConfigButton_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::initialize()
{
    LOG_CLASS_INFO( "snwSetConfig", "initialize" );

    LOG_TRACE( "serviceNewWrapperConfig() initialize() START", NULL );

    modifyFontPointSize setFontPointSize14( 14 );
    modifyFontPointSize setFontPointSize20( 20 );
    modifyFontPointSize setFontPointSize22( 22 );

    modifyPaletteBackground setBackgroundBlack( hobart::colorPalette::black );
    modifyPaletteForeground setForegroundWhite( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );

    sealerBeltSettingValidator_ = new QIntValidator( 0, makeValidatorTop( 3 ), this );
    labelOffsetValidator_ = new QIntValidator( 1, 30, this );
    rotationCalibrationOffsetValidator_ = new QIntValidator( 1, 100, this ); // PJR-183: increase to 100 (from 80) as per Carlos
    wandCalibrationOffsetValidator_ = new QIntValidator( 1, 100, this );
    applierAdjustValidator_ = new QIntValidator( 1, 250, this );

    setFontPointSize14( this );

    //
    // title
    //

    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setAutoFillBackground( true );
    title_->setObjectName( "title" );

    setBackgroundBlack( title_ );
    setForegroundWhite( title_ );

    //
    // sealer belt
    //

    sealerBeltSettingLabel_ = new QLabel( this );
    sealerBeltSettingLabel_->setObjectName( "sealerBeltSettingLabel" );

    sealerBeltSetting_ = new hobart::numericLineEdit( this );
    sealerBeltSetting_->setDecimalPlaces( 0 );
    sealerBeltSetting_->setInputTruncation( true );
    sealerBeltSetting_->setInputWrapping( true );
    sealerBeltSetting_->setObjectName( "sealerBeltSetting" );
    sealerBeltSetting_->setValidator( sealerBeltSettingValidator_ );

    setFontPointSize20( sealerBeltSetting_ );

    wrapperConfigWidgets_.push_back( sealerBeltSettingLabel_ );
    wrapperConfigWidgets_.push_back( sealerBeltSetting_ );

    connect( sealerBeltSetting_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // lower film width
    //

    lowerFilmWidthLabel_ = new QLabel( this );
    lowerFilmWidthLabel_->setObjectName( "lowerFilmWidthLabel" );

    lowerFilmWidth_ = new QComboBox( this );
    lowerFilmWidth_->setObjectName( "lowerFilmWidth" );

    lowerFilmWidth_->addItem( "13\"", QString( "130" ) );
    lowerFilmWidth_->addItem( "14\"", QString( "140" ) );
    lowerFilmWidth_->addItem( "15\"", QString( "150" ) );
    lowerFilmWidth_->addItem( "16\"", QString( "160" ) );
    lowerFilmWidth_->addItem( "17\"", QString( "170" ) );
    lowerFilmWidth_->addItem( "18\"", QString( "180" ) );
    lowerFilmWidth_->addItem( "19\"", QString( "190" ) );
    lowerFilmWidth_->addItem( "20\"", QString( "200" ) );
    lowerFilmWidth_->addItem( "", QString( "118" ) );
    lowerFilmWidth_->setCurrentIndex( 8 );

    setFontPointSize22( lowerFilmWidth_ );

    wrapperConfigWidgets_.push_back( lowerFilmWidthLabel_ );
    wrapperConfigWidgets_.push_back( lowerFilmWidth_ );

    connect( lowerFilmWidth_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // upper film width
    //

    upperFilmWidthLabel_ = new QLabel( this );
    upperFilmWidthLabel_->setObjectName( "upperFilmWidthLabel" );

    upperFilmWidth_ = new QComboBox( this );
    upperFilmWidth_->setObjectName( "upperFilmWidth" );

    upperFilmWidth_->addItem( "13\"", QString( "130" ) );
    upperFilmWidth_->addItem( "14\"", QString( "140" ) );
    upperFilmWidth_->addItem( "15\"", QString( "150" ) );
    upperFilmWidth_->addItem( "16\"", QString( "160" ) );
    upperFilmWidth_->addItem( "17\"", QString( "170" ) );
    upperFilmWidth_->addItem( "18\"", QString( "180" ) );
    upperFilmWidth_->addItem( "19\"", QString( "190" ) );
    upperFilmWidth_->addItem( "20\"", QString( "200" ) );
    upperFilmWidth_->addItem( "", QString( "118" ) );
    upperFilmWidth_->setCurrentIndex( 8 );

    setFontPointSize22( upperFilmWidth_ );

    wrapperConfigWidgets_.push_back( upperFilmWidthLabel_ );
    wrapperConfigWidgets_.push_back( upperFilmWidth_ );

    connect( upperFilmWidth_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // primary label offset
    //

    priLabelOffsetLabel_ = new QLabel( this );
    priLabelOffsetLabel_->setObjectName( "priLabelOffsetLabel" );

    priLabelOffset_ = new hobart::numericLineEdit( this );
    priLabelOffset_->setDecimalPlaces( 0 );
    priLabelOffset_->setInputTruncation( true );
    priLabelOffset_->setInputWrapping( true );
    priLabelOffset_->setObjectName( "priLabelOffset" );
    priLabelOffset_->setValidator( labelOffsetValidator_ );

    setFontPointSize20( priLabelOffset_ );

    applierConfigWidgets_.push_back( priLabelOffsetLabel_ );
    applierConfigWidgets_.push_back( priLabelOffset_ );

    connect( priLabelOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // primary rotation calibration offset
    //

    priRotationCalibrationOffsetLabel_ = new QLabel( this );
    priRotationCalibrationOffsetLabel_->setObjectName( "priRotationCalibrationOffsetLabel" );

    priRotationCalibrationOffset_ = new hobart::numericLineEdit( this );
    priRotationCalibrationOffset_->setDecimalPlaces( 0 );
    priRotationCalibrationOffset_->setInputTruncation( true );
    priRotationCalibrationOffset_->setInputWrapping( true );
    priRotationCalibrationOffset_->setObjectName( "priRotationCalibrationOffset" );
    priRotationCalibrationOffset_->setValidator( rotationCalibrationOffsetValidator_ );

    setFontPointSize20( priRotationCalibrationOffset_ );

    applierConfigWidgets_.push_back( priRotationCalibrationOffsetLabel_ );
    applierConfigWidgets_.push_back( priRotationCalibrationOffset_ );

    connect( priRotationCalibrationOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // offsets for other rotation settings

    // 90 degree
    priRotationCalibrationOffset_90DegLabel_ = new QLabel( this );
    priRotationCalibrationOffset_90DegLabel_->setObjectName( "priRotationCalibrationOffset_90DegLabel" );

    priRotationCalibrationOffset_90Deg_ = new hobart::numericLineEdit( this );
    priRotationCalibrationOffset_90Deg_->setDecimalPlaces( 0 );
    priRotationCalibrationOffset_90Deg_->setInputTruncation( true );
    priRotationCalibrationOffset_90Deg_->setInputWrapping( true );
    priRotationCalibrationOffset_90Deg_->setObjectName( "priRotationCalibrationOffset_90Deg" );
    priRotationCalibrationOffset_90Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( priRotationCalibrationOffset_90Deg_ );

    applierConfigWidgets_.push_back( priRotationCalibrationOffset_90DegLabel_ );
    applierConfigWidgets_.push_back( priRotationCalibrationOffset_90Deg_ );

    connect( priRotationCalibrationOffset_90Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // 180 degree
    priRotationCalibrationOffset_180DegLabel_ = new QLabel( this );
    priRotationCalibrationOffset_180DegLabel_->setObjectName( "priRotationCalibrationOffset_180DegLabel" );

    priRotationCalibrationOffset_180Deg_ = new hobart::numericLineEdit( this );
    priRotationCalibrationOffset_180Deg_->setDecimalPlaces( 0 );
    priRotationCalibrationOffset_180Deg_->setInputTruncation( true );
    priRotationCalibrationOffset_180Deg_->setInputWrapping( true );
    priRotationCalibrationOffset_180Deg_->setObjectName( "priRotationCalibrationOffset_180Deg" );
    priRotationCalibrationOffset_180Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( priRotationCalibrationOffset_180Deg_ );

    applierConfigWidgets_.push_back( priRotationCalibrationOffset_180DegLabel_ );
    applierConfigWidgets_.push_back( priRotationCalibrationOffset_180Deg_ );

    connect( priRotationCalibrationOffset_180Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // 270 degree
    priRotationCalibrationOffset_270DegLabel_ = new QLabel( this );
    priRotationCalibrationOffset_270DegLabel_->setObjectName( "priRotationCalibrationOffset_270DegLabel" );

    priRotationCalibrationOffset_270Deg_ = new hobart::numericLineEdit( this );
    priRotationCalibrationOffset_270Deg_->setDecimalPlaces( 0 );
    priRotationCalibrationOffset_270Deg_->setInputTruncation( true );
    priRotationCalibrationOffset_270Deg_->setInputWrapping( true );
    priRotationCalibrationOffset_270Deg_->setObjectName( "priRotationCalibrationOffset_270Deg" );
    priRotationCalibrationOffset_270Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( priRotationCalibrationOffset_270Deg_ );

    applierConfigWidgets_.push_back( priRotationCalibrationOffset_270DegLabel_ );
    applierConfigWidgets_.push_back( priRotationCalibrationOffset_270Deg_ );

    connect( priRotationCalibrationOffset_270Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // wand calibration offset
    //

    priWandCalibrationOffsetLabel_ = new QLabel( this );
    priWandCalibrationOffsetLabel_->setObjectName( "priWandCalibrationOffsetLabel" );

    priWandCalibrationOffset_ = new hobart::numericLineEdit( this );
    priWandCalibrationOffset_->setDecimalPlaces( 0 );
    priWandCalibrationOffset_->setInputTruncation( true );
    priWandCalibrationOffset_->setInputWrapping( true );
    priWandCalibrationOffset_->setObjectName( "priWandCalibrationOffset" );
    priWandCalibrationOffset_->setValidator( wandCalibrationOffsetValidator_ );

    setFontPointSize20( priWandCalibrationOffset_ );

    applierConfigWidgets_.push_back( priWandCalibrationOffsetLabel_ );
    applierConfigWidgets_.push_back( priWandCalibrationOffset_ );

    connect( priWandCalibrationOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // package direction
    //

    packageDirectionLabel_ = new QLabel( this );
    packageDirectionLabel_->setObjectName( "packageDirectionLabel" );

    packageDirection_ = new QComboBox( this );
    packageDirection_->setObjectName( "packageDirection" );

    packageDirection_->addItem( "", QString( "0" ) );
    packageDirection_->addItem( "", QString( "1" ) );
    packageDirection_->addItem( "", QString( "" ) );
    packageDirection_->setCurrentIndex( 2 );

    setFontPointSize22( packageDirection_ );

    wrapperConfigWidgets_.push_back( packageDirectionLabel_ );
    wrapperConfigWidgets_.push_back( packageDirection_ );

    connect( packageDirection_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // second CLA attached
    //

    secondCLALabel_ = new QLabel( this );
    secondCLALabel_->setObjectName( "secondCLALabel" );

    secondCLA_ = new QComboBox( this );
    secondCLA_->setObjectName( "secondCLA" );

    secondCLA_->addItem( "", QString( "0" ) );
    secondCLA_->addItem( "", QString( "1" ) );

    setFontPointSize22( secondCLA_ );

    wrapperConfigWidgets_.push_back( secondCLALabel_ );
    wrapperConfigWidgets_.push_back( secondCLA_ );

    connect( secondCLA_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // wrapper operation
    //

    wrapperOperationLabel_ = new QLabel( this );
    wrapperOperationLabel_->setObjectName( "wrapperOperationLabel" );

    wrapperOperation_ = new QComboBox( this );
    wrapperOperation_->setObjectName( "wrapperOperation" );

    wrapperOperation_->addItem( "", QString( "0" ) );
    wrapperOperation_->addItem( "", QString( "1" ) );

    setFontPointSize22( wrapperOperation_ );

    wrapperConfigWidgets_.push_back( wrapperOperationLabel_ );
    wrapperConfigWidgets_.push_back( wrapperOperation_ );

    connect( wrapperOperation_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // hardware add on
    //

    hardwareAddOnLabel_ = new QLabel( this );
    hardwareAddOnLabel_->setObjectName( "hardwareAddOnLabel" );

    hardwareAddOn_ = new QComboBox( this );
    hardwareAddOn_->setObjectName( "hardwareAddOn" );

    hardwareAddOn_->addItem( "", QString( "0" ) );
    hardwareAddOn_->addItem( "", QString( "1" ) );

    setFontPointSize22( hardwareAddOn_ );

    wrapperConfigWidgets_.push_back( hardwareAddOnLabel_ );
    wrapperConfigWidgets_.push_back( hardwareAddOn_ );

    connect( hardwareAddOn_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // secondary label offset
    //

    secLabelOffsetLabel_ = new QLabel( this );
    secLabelOffsetLabel_->setObjectName( "secLabelOffsetLabel" );

    secLabelOffset_ = new hobart::numericLineEdit( this );
    secLabelOffset_->setDecimalPlaces( 0 );
    secLabelOffset_->setInputTruncation( true );
    secLabelOffset_->setInputWrapping( true );
    secLabelOffset_->setObjectName( "secLabelOffset" );
    secLabelOffset_->setValidator( labelOffsetValidator_ );

    setFontPointSize20( secLabelOffset_ );

    applierConfigWidgets_.push_back( secLabelOffsetLabel_ );
    applierConfigWidgets_.push_back( secLabelOffset_ );

    connect( secLabelOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // secondary rotation calibration offset
    //

    secRotationCalibrationOffsetLabel_ = new QLabel( this );
    secRotationCalibrationOffsetLabel_->setObjectName( "secRotationCalibrationOffsetLabel" );

    secRotationCalibrationOffset_ = new hobart::numericLineEdit( this );
    secRotationCalibrationOffset_->setDecimalPlaces( 0 );
    secRotationCalibrationOffset_->setInputTruncation( true );
    secRotationCalibrationOffset_->setInputWrapping( true );
    secRotationCalibrationOffset_->setObjectName( "secRotationCalibrationOffset" );
    secRotationCalibrationOffset_->setValidator( rotationCalibrationOffsetValidator_ );

    setFontPointSize20( secRotationCalibrationOffset_ );

    applierConfigWidgets_.push_back( secRotationCalibrationOffsetLabel_ );
    applierConfigWidgets_.push_back( secRotationCalibrationOffset_ );

    connect( secRotationCalibrationOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // offsets for other rotation settings

    // 90 degree
    secRotationCalibrationOffset_90DegLabel_ = new QLabel( this );
    secRotationCalibrationOffset_90DegLabel_->setObjectName( "secRotationCalibrationOffset_90DegLabel" );

    secRotationCalibrationOffset_90Deg_ = new hobart::numericLineEdit( this );
    secRotationCalibrationOffset_90Deg_->setDecimalPlaces( 0 );
    secRotationCalibrationOffset_90Deg_->setInputTruncation( true );
    secRotationCalibrationOffset_90Deg_->setInputWrapping( true );
    secRotationCalibrationOffset_90Deg_->setObjectName( "secRotationCalibrationOffset_90Deg" );
    secRotationCalibrationOffset_90Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( secRotationCalibrationOffset_90Deg_ );

    applierConfigWidgets_.push_back( secRotationCalibrationOffset_90DegLabel_ );
    applierConfigWidgets_.push_back( secRotationCalibrationOffset_90Deg_ );

    connect( secRotationCalibrationOffset_90Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // 180 degree
    secRotationCalibrationOffset_180DegLabel_ = new QLabel( this );
    secRotationCalibrationOffset_180DegLabel_->setObjectName( "secRotationCalibrationOffset_180DegLabel" );

    secRotationCalibrationOffset_180Deg_ = new hobart::numericLineEdit( this );
    secRotationCalibrationOffset_180Deg_->setDecimalPlaces( 0 );
    secRotationCalibrationOffset_180Deg_->setInputTruncation( true );
    secRotationCalibrationOffset_180Deg_->setInputWrapping( true );
    secRotationCalibrationOffset_180Deg_->setObjectName( "secRotationCalibrationOffset_180Deg" );
    secRotationCalibrationOffset_180Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( secRotationCalibrationOffset_180Deg_ );

    applierConfigWidgets_.push_back( secRotationCalibrationOffset_180DegLabel_ );
    applierConfigWidgets_.push_back( secRotationCalibrationOffset_180Deg_ );

    connect( secRotationCalibrationOffset_180Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    // 270 degree
    secRotationCalibrationOffset_270DegLabel_ = new QLabel( this );
    secRotationCalibrationOffset_270DegLabel_->setObjectName( "secRotationCalibrationOffset_270DegLabel" );

    secRotationCalibrationOffset_270Deg_ = new hobart::numericLineEdit( this );
    secRotationCalibrationOffset_270Deg_->setDecimalPlaces( 0 );
    secRotationCalibrationOffset_270Deg_->setInputTruncation( true );
    secRotationCalibrationOffset_270Deg_->setInputWrapping( true );
    secRotationCalibrationOffset_270Deg_->setObjectName( "secRotationCalibrationOffset_270Deg" );
    secRotationCalibrationOffset_270Deg_->setValidator( rotationCalibrationOffsetValidator_ );  // ok to reuse validator? has same requirements

    setFontPointSize20( secRotationCalibrationOffset_270Deg_ );

    applierConfigWidgets_.push_back( secRotationCalibrationOffset_270DegLabel_ );
    applierConfigWidgets_.push_back( secRotationCalibrationOffset_270Deg_ );

    connect( secRotationCalibrationOffset_270Deg_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // secondary wand calibration offset
    //

    secWandCalibrationOffsetLabel_ = new QLabel( this );
    secWandCalibrationOffsetLabel_->setObjectName( "secWandCalibrationOffsetLabel" );

    secWandCalibrationOffset_ = new hobart::numericLineEdit( this );
    secWandCalibrationOffset_->setDecimalPlaces( 0 );
    secWandCalibrationOffset_->setInputTruncation( true );
    secWandCalibrationOffset_->setInputWrapping( true );
    secWandCalibrationOffset_->setObjectName( "secWandCalibrationOffset" );
    secWandCalibrationOffset_->setValidator( wandCalibrationOffsetValidator_ );

    setFontPointSize20( secWandCalibrationOffset_ );

    applierConfigWidgets_.push_back( secWandCalibrationOffsetLabel_ );
    applierConfigWidgets_.push_back( secWandCalibrationOffset_ );

    connect( secWandCalibrationOffset_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    priApplierHandAdjustLabel_ = new QLabel( this );
    priApplierHandAdjustLabel_->setObjectName( "priApplierHandAdjustLabel" );

    priApplierHandAdjust_ = new hobart::numericLineEdit( this );
    priApplierHandAdjust_->setDecimalPlaces( 0 );
    priApplierHandAdjust_->setInputTruncation( false );
    priApplierHandAdjust_->setInputWrapping( true );
    priApplierHandAdjust_->setObjectName( "priApplierHandAdjust" );
    priApplierHandAdjust_->setValidator( applierAdjustValidator_ ); // we can just use this validator since its still 1 - 100

    setFontPointSize20( priApplierHandAdjust_ );

    applierConfigWidgets_.push_back( priApplierHandAdjustLabel_ );
    applierConfigWidgets_.push_back( priApplierHandAdjust_ );

    connect( priApplierHandAdjust_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    secApplierHandAdjustLabel_ = new QLabel( this );
    secApplierHandAdjustLabel_->setObjectName( "secApplierHandAdjustLabel" );

    secApplierHandAdjust_ = new hobart::numericLineEdit( this );
    secApplierHandAdjust_->setDecimalPlaces( 0 );
    secApplierHandAdjust_->setInputTruncation( true );
    secApplierHandAdjust_->setInputWrapping( true );
    secApplierHandAdjust_->setObjectName( "secApplierHandAdjust" );
    secApplierHandAdjust_->setValidator( applierAdjustValidator_ ); // we can just use this validator since its still 1 - 100

    setFontPointSize20( secApplierHandAdjust_ );

    applierConfigWidgets_.push_back( secApplierHandAdjustLabel_ );
    applierConfigWidgets_.push_back( secApplierHandAdjust_ );

    connect( secApplierHandAdjust_, SIGNAL(editingFinished()), SLOT(onFieldUpdated()) );

    //
    // automatic rotation
    //

    automaticRotationLabel_ = new QLabel( this );
    automaticRotationLabel_->setObjectName( "automaticRotationLabel" );

    automaticRotation_ = new QComboBox( this );
    automaticRotation_->setObjectName( "automaticRotation" );

    automaticRotation_->addItem( "", QString( "0" ) );
    automaticRotation_->addItem( "", QString( "1" ) );

    setFontPointSize22( automaticRotation_ );

    applierConfigWidgets_.push_back( automaticRotationLabel_ );
    applierConfigWidgets_.push_back( automaticRotation_ );

    connect( automaticRotation_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // sealer belt package sensor type
    //

    sealerBeltPkgSensorTypeLabel_ = new QLabel( this );
    sealerBeltPkgSensorTypeLabel_->setObjectName( "sealerBeltPkgSensorTypeLabel" );

    sealerBeltPkgSensorType_ = new QComboBox( this );
    sealerBeltPkgSensorType_->setObjectName( "sealerBeltPkgSensorType" );

    sealerBeltPkgSensorType_->addItem( "", QString( "1" ) );
    sealerBeltPkgSensorType_->addItem( "", QString( "2" ) );

    setFontPointSize22( sealerBeltPkgSensorType_ );

    wrapperConfigWidgets_.push_back( sealerBeltPkgSensorTypeLabel_ );
    wrapperConfigWidgets_.push_back( sealerBeltPkgSensorType_ );

    connect( sealerBeltPkgSensorType_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // Upper Film Pull Tension
    //

    upperFilmPullTensionLabel_ = new QLabel( this );
    upperFilmPullTensionLabel_->setObjectName( "upperFilmPullTensionLabel" );

    upperFilmPullTension_ = new QComboBox( this );
    upperFilmPullTension_->setObjectName( "upperFilmPullTension" );

    upperFilmPullTension_->addItem( "", QString( "0 - Default" ) );
    upperFilmPullTension_->addItem( "", QString( "+1" ) );
    upperFilmPullTension_->addItem( "", QString( "+2" ) );
    upperFilmPullTension_->addItem( "", QString( "+3" ) );
    upperFilmPullTension_->addItem( "", QString( "+4" ) );
    upperFilmPullTension_->addItem( "", QString( "+5" ) );

    setFontPointSize22( upperFilmPullTension_ );

    wrapperConfigWidgets_.push_back( upperFilmPullTensionLabel_ );
    wrapperConfigWidgets_.push_back( upperFilmPullTension_ );

    connect( upperFilmPullTension_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated()) );

    //
    // Lower Film Pull Tension
    //

    lowerFilmPullTensionLabel_ = new QLabel( this );
    lowerFilmPullTensionLabel_->setObjectName( "lowerFilmPullTensionLabel" );

    lowerFilmPullTension_ = new QComboBox( this );
    lowerFilmPullTension_->setObjectName( "lowerFilmPullTension" );

    lowerFilmPullTension_->addItem( "", QString( "0 - Default" ) );
    lowerFilmPullTension_->addItem( "", QString( "+1" ) );
    lowerFilmPullTension_->addItem( "", QString( "+2" ) );
    lowerFilmPullTension_->addItem( "", QString( "+3" ) );
    lowerFilmPullTension_->addItem( "", QString( "+4" ) );
    lowerFilmPullTension_->addItem( "", QString( "+5" ) );

    setFontPointSize22( lowerFilmPullTension_ );

    wrapperConfigWidgets_.push_back( lowerFilmPullTensionLabel_ );
    wrapperConfigWidgets_.push_back( lowerFilmPullTension_ );

    connect( lowerFilmPullTension_, SIGNAL(currentIndexChanged(int)), SLOT(onFieldUpdated(int)) );

    //
    // Film Heater
    //

    lblFilmHeater_ = new QLabel( this );
    lblFilmHeater_->setObjectName( "lblFilmHeater" );

    cboFilmHeater_ = new QComboBox( this );
    cboFilmHeater_->setObjectName( "cboFilmHeater" );

    cboFilmHeater_->addItem( "", QString( "0" ) );
    cboFilmHeater_->addItem( "", QString( "1" ) );

    // SOF-3969: Film Heater not available
    lblFilmHeater_->hide();
    cboFilmHeater_->hide();


    setFontPointSize22( cboFilmHeater_ );

    // SOF-3969: Film Heater not available
    //wrapperConfigWidgets_.push_back( lblFilmHeater_ );
    //wrapperConfigWidgets_.push_back( cboFilmHeater_ );

    connect( cboFilmHeater_, SIGNAL(currentIndexChanged(int)), SLOT(onHeaterClicked()) );

    toggleGeneralWrapperConfigButton_ = new QToolButton( this );
    toggleGeneralWrapperConfigButton_->setArrowType(Qt::LeftArrow);
    connect( toggleGeneralWrapperConfigButton_, SIGNAL( clicked() ), SLOT(toggleWrapperConfigs()) );

    toggleLabelApplierConfigButton_ = new QToolButton( this );
    toggleLabelApplierConfigButton_->setArrowType(Qt::RightArrow);
    connect( toggleLabelApplierConfigButton_, SIGNAL( clicked() ), SLOT(toggleApplierConfigs()) );

    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        LOG_INFO( "Wrapper is two printer. Hiding fields", NULL );

        priLabelOffsetLabel_->hide();
        priLabelOffset_->hide();

        secLabelOffsetLabel_->hide();
        secLabelOffset_->hide();

        sealerBeltPkgSensorTypeLabel_->hide();
        sealerBeltPkgSensorType_->hide();

        upperFilmPullTensionLabel_->hide();
        upperFilmPullTension_->hide();

        lowerFilmPullTensionLabel_->hide();
        lowerFilmPullTension_->hide();

        lblFilmHeater_->hide();
        cboFilmHeater_->hide();
    }

    // ---- //

    //
    // PJR-145: Wrap-only
    //
    if( session_->uiUtilities->isWrapOnly() )
    {
        lowerFilmWidthLabel_->hide();
        lowerFilmWidth_->hide();

        priLabelOffsetLabel_->hide();
        priLabelOffset_->hide();

        priRotationCalibrationOffsetLabel_->hide();
        priRotationCalibrationOffset_->hide();

        priRotationCalibrationOffset_90DegLabel_->hide();
        priRotationCalibrationOffset_90Deg_->hide();

        priRotationCalibrationOffset_180DegLabel_->hide();
        priRotationCalibrationOffset_180Deg_->hide();

        priRotationCalibrationOffset_270DegLabel_->hide();
        priRotationCalibrationOffset_270Deg_->hide();

        priWandCalibrationOffsetLabel_->hide();
        priWandCalibrationOffset_->hide();

        secondCLALabel_->hide();
        secondCLA_->hide();

        wrapperOperationLabel_->hide();
        wrapperOperation_->hide();

        hardwareAddOnLabel_->hide();
        hardwareAddOn_->hide();

        secLabelOffsetLabel_->hide();
        secLabelOffset_->hide();

        secRotationCalibrationOffsetLabel_->hide();
        secRotationCalibrationOffset_->hide();

        secRotationCalibrationOffset_90DegLabel_->hide();
        secRotationCalibrationOffset_90Deg_->hide();

        secRotationCalibrationOffset_180DegLabel_->hide();
        secRotationCalibrationOffset_180Deg_->hide();

        secRotationCalibrationOffset_270DegLabel_->hide();
        secRotationCalibrationOffset_270Deg_->hide();

        priApplierHandAdjustLabel_->hide();
        priApplierHandAdjust_->hide();

        secApplierHandAdjustLabel_->hide();
        secApplierHandAdjust_->hide();

        secWandCalibrationOffsetLabel_->hide();
        secWandCalibrationOffset_->hide();

        automaticRotationLabel_->hide();
        automaticRotation_->hide();

        // SOF-1066: Remove "Sealer belt package sensor type" for NGS
        sealerBeltPkgSensorTypeLabel_->hide();
        sealerBeltPkgSensorType_->hide();

        lowerFilmPullTensionLabel_->hide();
        lowerFilmPullTension_->hide();

        lblFilmHeater_->hide();
        cboFilmHeater_->hide();
    }
    //
    // EWS
    //
    else if ( evolWrapperSystemInformation_->isEWS() )
    {
        lowerFilmWidthLabel_->hide();
        lowerFilmWidth_->hide();

        priWandCalibrationOffsetLabel_->hide();
        priWandCalibrationOffset_->hide();

        secondCLALabel_->hide();
        secondCLA_->hide();

        wrapperOperationLabel_->hide();
        wrapperOperation_->hide();

        secWandCalibrationOffsetLabel_->hide();
        secWandCalibrationOffset_->hide();

        lowerFilmPullTensionLabel_->hide();
        lowerFilmPullTension_->hide();
    }

    //
    // AWS
    //
    else
    {
        lowerFilmWidthLabel_->setVisible( evolWrapperSystemInformation_->isDoubleRoll() );
        lowerFilmWidth_->setVisible( evolWrapperSystemInformation_->isDoubleRoll() );

        priWandCalibrationOffsetLabel_->setVisible( evolWrapperSystemInformation_->isClosedLoop() );
        priWandCalibrationOffset_->setVisible( evolWrapperSystemInformation_->isClosedLoop() );

        secondCLALabel_->setVisible( evolWrapperSystemInformation_->isOpenLoop() );
        secondCLA_->setVisible( evolWrapperSystemInformation_->isOpenLoop() );

        wrapperOperationLabel_->setVisible( evolWrapperSystemInformation_->isOpenLoop() );
        wrapperOperation_->setVisible( evolWrapperSystemInformation_->isOpenLoop() );

        secWandCalibrationOffsetLabel_->setVisible( evolWrapperSystemInformation_->isClosedLoop() );
        secWandCalibrationOffset_->setVisible( evolWrapperSystemInformation_->isClosedLoop() );

        automaticRotationLabel_->hide();
        automaticRotation_->hide();
    }

    // SOF-2871
    if( evolWrapperSystemInformation_->isSingleRoll() )
    {
        lowerFilmWidthLabel_->hide();
        lowerFilmWidth_ ->hide();

        lowerFilmPullTensionLabel_->hide();
        lowerFilmPullTension_->hide();
    }

    // NGW
    if ( !evolWrapperSystemInformation_->isNextGenModel() )
    {
        sealerBeltPkgSensorTypeLabel_->hide();
        sealerBeltPkgSensorType_->hide();
    }

    LOG_TRACE( "serviceNewWrapperConfig() initialize() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::createLayout()
{
    LOG_CLASS_INFO( "snwSetConfig", "createLayout() START" );

    LOG_TRACE( "serviceNewWrapperConfig() createLayout() START", NULL );

    QFormLayout *form( new QFormLayout() );
    form->addRow( sealerBeltSettingLabel_, sealerBeltSetting_ );
    form->addRow( upperFilmWidthLabel_, upperFilmWidth_ );
    form->addRow( lowerFilmWidthLabel_, lowerFilmWidth_ );
    form->addRow( priLabelOffsetLabel_, priLabelOffset_ );
    form->addRow( priRotationCalibrationOffsetLabel_, priRotationCalibrationOffset_ );
    form->addRow( priRotationCalibrationOffset_90DegLabel_, priRotationCalibrationOffset_90Deg_ );
    form->addRow( priRotationCalibrationOffset_180DegLabel_, priRotationCalibrationOffset_180Deg_ );
    form->addRow( priRotationCalibrationOffset_270DegLabel_, priRotationCalibrationOffset_270Deg_ );
    form->addRow( priWandCalibrationOffsetLabel_, priWandCalibrationOffset_ );
    form->addRow( packageDirectionLabel_, packageDirection_ );
    form->addRow( secondCLALabel_, secondCLA_ );
    form->addRow( wrapperOperationLabel_, wrapperOperation_ );
    form->addRow( hardwareAddOnLabel_, hardwareAddOn_ );
    form->addRow( secLabelOffsetLabel_, secLabelOffset_ );
    form->addRow( secRotationCalibrationOffsetLabel_, secRotationCalibrationOffset_ );
    form->addRow( secRotationCalibrationOffset_90DegLabel_, secRotationCalibrationOffset_90Deg_ );
    form->addRow( secRotationCalibrationOffset_180DegLabel_, secRotationCalibrationOffset_180Deg_ );
    form->addRow( secRotationCalibrationOffset_270DegLabel_, secRotationCalibrationOffset_270Deg_ );
    form->addRow( secWandCalibrationOffsetLabel_, secWandCalibrationOffset_ );
    form->addRow( priApplierHandAdjustLabel_, priApplierHandAdjust_ );
    form->addRow( secApplierHandAdjustLabel_, secApplierHandAdjust_ );
    form->addRow( automaticRotationLabel_, automaticRotation_ );
    form->addRow( sealerBeltPkgSensorTypeLabel_, sealerBeltPkgSensorType_ );
    form->addRow( upperFilmPullTensionLabel_, upperFilmPullTension_ );
    form->addRow( lowerFilmPullTensionLabel_, lowerFilmPullTension_ );
    //form->addRow( lblFilmHeater_, cboFilmHeater_ ); // SOF-3969: Film Heater not supported

    QGridLayout *buttonGrid( new QGridLayout );
    buttonGrid->addWidget( toggleGeneralWrapperConfigButton_, 0, 0 );
    buttonGrid->addWidget( toggleLabelApplierConfigButton_, 0, 1 );
    buttonGrid->setHorizontalSpacing( 50 );

    QVBoxLayout *formLayout( new QVBoxLayout() );
    formLayout->addWidget( title_ );
    formLayout->addLayout( buttonGrid );
    //formLayout->addWidget( toggleGeneralWrapperConfigButton_ );
    //formLayout->addWidget( toggleLabelApplierConfigButton_ );
    formLayout->addLayout( form );
    formLayout->addStretch();

    QHBoxLayout *screenLayout( new QHBoxLayout( this ) );
    screenLayout->addStretch();
    screenLayout->addLayout( formLayout );
    screenLayout->addStretch();

    LOG_TRACE( "serviceNewWrapperConfig() createLayout() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::saveWrapperConfig()
{
    QString msg;

    // NGW-43
    if ( !ready_ )
        return;
    else if ( !hasChanged_ )
        return;
    else if ( !peripheralList_->isWrapperAttached() )
        return;

// REMOVED FOR SOF-1228
//    if ( 375 < sealerBeltSetting )
//    {
//        sealerBeltSetting_->setText( "375" );
//        promptSealerBeltError = true;
//    }

    msg += upperFilmWidth_->currentData().toString();

    if ( evolWrapperSystemInformation_->isSingleRoll() )
        msg += ",";
    else
        msg += "," + lowerFilmWidth_->currentData().toString();

    msg += "," + sealerBeltSetting_->text();
    msg += "," + packageDirection_->currentData().toString();
    msg += "," + priLabelOffset_->text();
    msg += "," + priRotationCalibrationOffset_->text();

    if (( evolWrapperSystemInformation_->isEWS() ) || ( evolWrapperSystemInformation_->isClosedLoop() ))
        msg += ",";
    else
        msg += "," + secondCLA_->currentData().toString();

    if (( evolWrapperSystemInformation_->isEWS() ) || ( evolWrapperSystemInformation_->isClosedLoop() ))
        msg += ",";
    else
        msg += "," + wrapperOperation_->currentData().toString();

    msg += "," + hardwareAddOn_->currentData().toString();
    msg += "," + secLabelOffset_->text();
    msg += "," + automaticRotation_->currentData().toString();

    if ( evolWrapperSystemInformation_->isOpenLoop() )
        msg += ",";
    else
        msg += "," + priWandCalibrationOffset_->text();

    if ( evolWrapperSystemInformation_->isOpenLoop() )
        msg += ",";
    else
        msg += "," + secWandCalibrationOffset_->text();

    if ( !evolWrapperSystemInformation_->isNextGenModel() )
        msg += ",";
    else
        msg += "," + sealerBeltPkgSensorType_->currentData().toString();

    msg += "," + QString::number( upperFilmPullTension_->currentIndex() );

    msg += "," + QString::number( lowerFilmPullTension_->currentIndex() );

    msg += "," + QString::number( cboFilmHeater_->currentIndex() );

    msg += "," + priRotationCalibrationOffset_90Deg_->text();
    msg += "," + priRotationCalibrationOffset_180Deg_->text();
    msg += "," + priRotationCalibrationOffset_270Deg_->text();

    msg += "," + secRotationCalibrationOffset_90Deg_->text();
    msg += "," + secRotationCalibrationOffset_180Deg_->text();
    msg += "," + secRotationCalibrationOffset_270Deg_->text();
    msg += "," + secRotationCalibrationOffset_->text();
    msg += "," + priApplierHandAdjust_->text();
    msg += "," + secApplierHandAdjust_->text();

    session_->send( Wrapper, WrEvolConfig, msg );
    session_->send( Wrapper, WrEvolInquire );

    // NGW-214
    // popup warning message
// REMOVED FOR SOF-1228
//    if ( promptSealerBeltError )
//    {
//        MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
//        dialog->setTitle( tr( "Temperature Warning" ) );
//        dialog->setText( tr( "Invalid sealer belt temperature.\n\nTemperature was reset to its maximum allowed setting." ) );
//        dialog->setStandardButtons( QDialogButtonBox::Ok );

//        dialog->open();
//    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperConfig::updateFormFields()
{
    LOG_CLASS_INFO( "serviceNewWrapperConfig", "updateFormFields" );

    if ( !ready_ )
        return;

    //
    // EWS
    //
    if ( evolWrapperSystemInformation_->isEWS() )
    {
        LOG_INFO( "Wrapper is EWS", NULL );

        priRotationCalibrationOffset_->setEnabled( 1 == automaticRotation_->currentIndex() );
        priRotationCalibrationOffset_90Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );
        priRotationCalibrationOffset_180Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );
        priRotationCalibrationOffset_270Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );

        secRotationCalibrationOffset_->setEnabled( 1 == automaticRotation_->currentIndex() );
        secRotationCalibrationOffset_90Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );
        secRotationCalibrationOffset_180Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );
        secRotationCalibrationOffset_270Deg_->setEnabled( 1 == automaticRotation_->currentIndex() );

        secLabelOffset_->setEnabled( 1 == hardwareAddOn_->currentIndex() );
        lowerFilmPullTension_->setEnabled( false );
    }

    //
    // AWS
    //
    else
    {
        LOG_INFO( "Wrapper is AWS", NULL );

        if ( 1 == wrapperOperation_->currentIndex() )
        {
            LOG_INFO( "Manual Operation", NULL );

            priLabelOffset_->setEnabled( false );
            priRotationCalibrationOffset_->setEnabled( false );
            priRotationCalibrationOffset_90Deg_->setEnabled( false );
            priRotationCalibrationOffset_180Deg_->setEnabled( false );
            priRotationCalibrationOffset_270Deg_->setEnabled( false );

            secRotationCalibrationOffset_->setEnabled( false );
            secRotationCalibrationOffset_90Deg_->setEnabled( false );
            secRotationCalibrationOffset_180Deg_->setEnabled( false );
            secRotationCalibrationOffset_270Deg_->setEnabled( false );

            priWandCalibrationOffset_->setEnabled( false );
            secondCLA_->setEnabled( false );
            hardwareAddOn_->setEnabled( false );
            secLabelOffset_->setEnabled( false );
            secWandCalibrationOffset_->setEnabled( false );
        }
        else
        {
            LOG_INFO( "Automatic Operation", NULL );

            upperFilmPullTension_->setEnabled( true );
            lowerFilmPullTension_->setEnabled( true );

            priLabelOffset_->setEnabled( true );
            priRotationCalibrationOffset_->setEnabled( true );
            priRotationCalibrationOffset_90Deg_->setEnabled( true );
            priRotationCalibrationOffset_180Deg_->setEnabled( true );
            priRotationCalibrationOffset_270Deg_->setEnabled( true );

            secRotationCalibrationOffset_->setEnabled( true );
            secRotationCalibrationOffset_90Deg_->setEnabled( true );
            secRotationCalibrationOffset_180Deg_->setEnabled( true );
            secRotationCalibrationOffset_270Deg_->setEnabled( true );

            priWandCalibrationOffset_->setEnabled( true );

            secLabelOffset_->setEnabled( false );
            secWandCalibrationOffset_->setEnabled( false );

            secondCLA_->setEnabled( 0 == hardwareAddOn_->currentIndex() );
            hardwareAddOn_->setEnabled( 0 == secondCLA_->currentIndex() );

            // second printer attached
            if ( 1 == hardwareAddOn_->currentIndex() )
            {
                LOG_INFO( "Two printers attached", NULL );

                secLabelOffset_->setEnabled( true );
                secWandCalibrationOffset_->setEnabled( true );
            }

            // second cla attached
            else if ( 1 == secondCLA_->currentIndex() )
            {
                LOG_INFO( "Secondary CLA attached", NULL );

                priLabelOffset_->setEnabled( 1 == packageDirection_->currentIndex() );
                secLabelOffset_->setEnabled( 0 == packageDirection_->currentIndex() );
                secWandCalibrationOffset_->setEnabled( true );
            }
        }
    }

    // NGW
    if ( evolWrapperSystemInformation_->isNextGenModel() )
    {
        sealerBeltPkgSensorTypeLabel_->setEnabled( true );
        sealerBeltPkgSensorType_->setEnabled( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperConfig::makeValidatorTop( int digits )
{
    int top( 0 );

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    return top;
}

void serviceNewWrapperConfig::checkSecondaryLAConfigs()
{
    const QString modelName = session_->scaleConfig->getScaleModelName();
    //if ("NGW2" != modelName.left( 4 ).toUpper() && "NGW" == modelName.left(3).toUpper()

    if( !session_->scaleConfig->isWrapperTwoPrinter() )
    {
        secLabelOffsetLabel_->hide();
        secLabelOffset_->hide();
        secRotationCalibrationOffsetLabel_->hide();
        secRotationCalibrationOffset_->hide();
        secRotationCalibrationOffset_90DegLabel_->hide();
        secRotationCalibrationOffset_90Deg_->hide();
        secRotationCalibrationOffset_180DegLabel_->hide();
        secRotationCalibrationOffset_180Deg_->hide();
        secRotationCalibrationOffset_270DegLabel_->hide();
        secRotationCalibrationOffset_270Deg_->hide();
        secApplierHandAdjustLabel_->hide();
        secApplierHandAdjust_->hide();
        secWandCalibrationOffsetLabel_->hide();
        secWandCalibrationOffset_->hide();
        priApplierHandAdjustLabel_->hide();
        priApplierHandAdjust_->hide();
        automaticRotationLabel_->hide();
        automaticRotation_->hide();
    }
    else if( !session_->scaleConfig->isWrapperTwoPrinter() )
    {
        secLabelOffsetLabel_->hide();
        secLabelOffset_->hide();
        secRotationCalibrationOffsetLabel_->hide();
        secRotationCalibrationOffset_->hide();
        secRotationCalibrationOffset_90DegLabel_->hide();
        secRotationCalibrationOffset_90Deg_->hide();
        secRotationCalibrationOffset_180DegLabel_->hide();
        secRotationCalibrationOffset_180Deg_->hide();
        secRotationCalibrationOffset_270DegLabel_->hide();
        secRotationCalibrationOffset_270Deg_->hide();
        secApplierHandAdjustLabel_->hide();
        secApplierHandAdjust_->hide();
        secWandCalibrationOffsetLabel_->hide();
        secWandCalibrationOffset_->hide();
    }
}
