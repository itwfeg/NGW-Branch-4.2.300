/**
 * @file pluoperatescreen.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "plumainscreen.h"
#include "pluoperatescreen.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledpluedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

const int PluOperateScreen::pluValidatorBottom_ = 0;
const int PluOperateScreen::pluValidatorTop_ = 999999;

///////////////////////////////////////////////////////////////////////////////////////////////////
PluOperateScreen::PluOperateScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ignoreSignals_( false )
{
    setObjectName( "PluOperateScreen" );

    // set palette
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( this );

    // connect slots
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PluOperateScreen::~PluOperateScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::setForcedTrayEntry( bool value )
{
    plu_->setTrayTypeForced( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::setTrayName( const QString& name )
{
    plu_->setTrayTypeVisible( true, name );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::translate()
{
    plu_->setLabelText( tr( "PLU#" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *PluOperateScreen::currentProduct() const
{
    DomCurrentPLU *product( operateMode_ ? operateMode_->currentProduct() : NULL );

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::setCurrentItemNumber( const QString& value )
{
    LOG_CLASS_INFO( "PluOperateScreen", "setCurrentItemNumber" );

    // validate entry
    if ( value.isEmpty() )
        return;

    // check for different product number
    else if ( value == currentItemNumber_ )
    {
        plu_->resetInputTruncationFlag();
        return;
    }
    else if (( "0" == value ) && ( currentItemNumber_.isEmpty() ))
    {
        plu_->clear();
        return;
    }

    // FT-947
    currentItemNumber_ = value;

    // FT-732
    // also set plu text... if an error occurs loading plu focus will be taken away and we will
    // revert to the previous plu...
    if ( plu_->text() != value )
    {
        bool restorePluFocus;

        ignoreSignals_ = true;

        if ( (restorePluFocus = plu_->hasFocus()) )
            plu_->clearFocus();

        plu_->setText( currentItemNumber_ );

        if ( restorePluFocus )
            plu_->setFocus();

        ignoreSignals_ = false;
    }

    // request plu but first clear the price and weight fields
    LOG_INFO( "requesting plu %s...", qPrintable( currentItemNumber_ ) );
    operateMode_->clearPriceAndWeightFields(); // clearPriceAndWeightFields
	if( hasRemoteLic && mainWindow_->isRemotePrintItem(value) )
    {
        session_->send( ProductDev, ProductRead, "0" );
        session_->send( Error, ErrProductDevice, QString::number(101143) + ",5" );
    }
    else
    {
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        if( value.toInt() > 0 )
            operateMode_->clearPLU();
        session_->send( ProductDev, ProductRead, value + ",3" );
    }

    // reset plu input field
    plu_->resetInputTruncationFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // scale configuration
    if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        // set plu auto lookup parameters
        updatePluAutoLookup();
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::onCurrentProductChanged( bool nof )
{
    LOG_CLASS_INFO( "PluOperateScreen", "onCurrentProductChanged" );

    // product not on file, nothing to do
    if ( nof )
    {
        LOG_DEBUG( "cleared product", NULL );

        // lets clear the field also...
        plu_->clear();
        currentItemNumber_.clear();

        return;
    }

    // reset plu input field
    plu_->resetInputTruncationFlag();

    DomCurrentPLU *product( currentProduct() );

    // handle product
    if ( !product )
        return;

    ignoreSignals_ = true;

    // plu
    if ( plu_->text() != (currentItemNumber_ = product->itemNumber()) )
    {
        bool restorePluFocus;

        if ( (restorePluFocus = plu_->hasFocus()) )
            plu_->clearFocus();

        plu_->setText( currentItemNumber_ );

        if ( restorePluFocus )
            plu_->setFocus();
    }

    ignoreSignals_ = false;

    session_->scaleConfig->internalConfig.selfServicePreventClicks = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::onPluEditingFinished()
{
    if ( ignoreSignals_ )
        return;

    setCurrentItemNumber( plu_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::onPluReturnPressed()
{
    if ( ignoreSignals_ )
        return;

    // FT-957: keep track of print key
    // FT-958: keep track of print key
    // nothing to do when ENTER due to print key press
    if ( session_->scaleConfig->printKeyPressed )
        return;

    // FT-786
    // reset current item number... this will force a refresh of current plu
    if (( 0 != plu_->toInt() ) && ( !plu_->text().isEmpty() ))
        currentItemNumber_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::onPluTextEdited( const QString& text )
{
    // check for entry
    if ( !text.length() )
        return;

    MadpScaleConfig *config( &session_->scaleConfig->scaleConfig );

    // check for auto lookup
    if ( config->autoLookupProduct() )
    {
        int digits( config->autoLookupNumOfDigits() );

        // make sure valid number
        if ( digits )
        {
            QString overrideDigit( config->autoLookupOverrideDigit() );
            int overrideTop( qMin( pluValidatorTop_, makeIntValidatorTop( digits+1 ) ) );

            // look for override
            if (( digits == text.length() ) && ( overrideDigit == text.left( 1 ) ) && ( overrideTop != pluValidatorTop_ ))
                pluValidator_->setTop( overrideTop );

            // perform lookup
            else if ( digits <= text.length() )
                onPluEditingFinished();

            // reset validator top
            else
            {
                pluValidator_->setTop( makeIntValidatorTop( digits ) );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::initialize()
{
    static const QMargins margins( 10, 10, 10, 6 );

    const QSize fieldSize( scaleSizeForScreen( QSize( 270, 100 ) ) );

   const  hobart::backgroundWidgetRenderer renderField(
        new hobart::rectangleBackgroundWidget,
        new hobart::darkOperatorBackgroundWidgetRendererAppearance ); ;

    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    QFont scaledFont( font() );
    scaledFont.setPointSize( scaleFontPointSizeForScreen( scaledFont.pointSize() ) );

    setFont( scaledFont );

    // ---- //

    // plu
    if ( session_->isServiceCounter() )
        plu_ = new hobart::labeledPluEdit( this, renderField.drawControl( fieldSize, hobart::Normal ), false ); // FT-1466
    else
    {
        plu_ = new hobart::labeledPluEdit( this, renderField.drawControl( fieldSize, hobart::Normal ), true );	//	HA-176
        plu_->setModeUpdateEnabled( true );
    }

    plu_->setBackgroundFocused( renderField.drawControl( fieldSize, hobart::Focused ) );
    plu_->setBackgroundReadOnly( renderField.drawControl( fieldSize, hobart::ReadOnly ) );
    plu_->setDropShadow( renderField.drawDropShadow( fieldSize ) );
    plu_->setContentsMargins( margins );
    plu_->setObjectName( "plu" );
    plu_->setLabelObjectName( "plu_label" );

    plu_->setInputFocusZeroing( false );
    plu_->setInputTruncation( true );
    plu_->setInputWrapping( true );
    plu_->setValidator( pluValidator_ = new QIntValidator( pluValidatorBottom_, pluValidatorTop_, this ) );
    setFontPointSize32( plu_ );

    connect( plu_, SIGNAL( editingFinished() ), SLOT( onPluEditingFinished() ) );
    connect( plu_, SIGNAL( returnPressed() ), SLOT( onPluReturnPressed() ) ); // FT-786
    connect( plu_, SIGNAL( textEdited(QString) ), SLOT( onPluTextEdited(QString) ) );

    const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
	hasRemoteLic = false;
    QString licenses  = scaleConfig->licensedFeatures();
    if( licenses.contains( "SCALE_REMOTE_PRINTING" ) )
        hasRemoteLic = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluOperateScreen::updatePluAutoLookup()
{
    MadpScaleConfig *config( &session_->scaleConfig->scaleConfig );

    // no auto lookup set
    if ( !config->autoLookupProduct() )
        pluValidator_->setTop( pluValidatorTop_ );
    else
    {
        int digits( config->autoLookupNumOfDigits() );

        // set validator top
        if ( digits )
            pluValidator_->setTop( makeIntValidatorTop( digits ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PluOperateScreen::makeIntValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    return top;
}
