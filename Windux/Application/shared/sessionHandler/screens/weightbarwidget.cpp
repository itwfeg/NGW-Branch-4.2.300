/**
 * @file weightbarwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <sessionhooks.h>

#include "weightbarwidget.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include "../screens/plumainscreen.h"

#include "../widgets/dynamiccontentwidget.h"
#include "../widgets/labeledlineedit.h"
#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledplaintextedit.h"
#include "../widgets/labeledunitpriceedit.h"
#include "../widgets/labeledupcedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <pixmap.h>
#include <softkeyswidget.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <Messages.h>
#include <ScaleClasses.h>

#include <QApplication>
#include <QIntValidator>
#include <QMouseEvent>
#include <QSignalMapper>

const bool WeightBarWidget::convertUppercase_ = true;
const bool WeightBarWidget::moveable_ = false;

const bool BC_UNIT_PRICE_DEVELOPMENT = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
WeightBarWidget::WeightBarWidget( SessionMainWindow *mainWindow, ModeWidget *parent, WeightBarArea area ) :
    _Mybase( mainWindow, parent ),
    area_( area ),
    displayMode_( WeightDisplayMode ),
    ignoreSignals_( false ),
    clearingFields_( false ),
    tareFlashkeysExist_( false ),
    isKeyboardNetWeightItem_( false ),
    moving_( false ),
    pick5PriceFieldEnabled_( false ),
    pick5WeightFieldEnabled_( false ),
    valuemaxIsEnabled_( false ),
    scaleWentOutOfTilt_( false )
{
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );
    config_ = session_->scaleConfig;
    curSeparator_ = ".";
    internalConfigData = &session_->scaleConfig->internalConfig;

    setObjectName( "WeightBarWidget" );

    // load pixmap
    const QSize backgroundSize( scaleSizeForScreen( QSize( 1024, 95 ) ) );

    const hobart::backgroundWidgetRenderer renderBackground(
        new hobart::rectangleBackgroundWidget(),
        new hobart::transparentBackgroundWidgetRendererAppearance() );

    const QPixmap background( renderBackground.drawControl( backgroundSize, hobart::Normal ) );

    // set palette
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::gray, hobart::colorPalette::gray )  );
    setForeground( this );

    modifyPaletteBackground setBackground( background );
    setBackground( this );

    setAutoFillBackground( true );
    setFixedSize( background.size() );

    // HW-75
    maximumTare_ = config_->normalScaleOperation.maxTare();

    // initialize
    initialize();
    createLayout();
    translate();

    // populate fields
    setWeigherConfig();
    setWeightText( "", "" );

    updateCurrencySettings();
    updateScaleConfig();

    clearFields();
    updateProgrammableFields();

    if ( operateMode_ )
    {
        connect( operateMode_, SIGNAL( clearPluForCutTest() ),            SLOT( onClearPluForCutTest() ) );
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ),     SLOT( onCurrentProductChanged(bool) ) );
        connect( operateMode_, SIGNAL( currentProductTotalsChanged() ),   SLOT( onCurrentProductTotalsChanged() ) );
        connect( operateMode_, SIGNAL( currentProductUpdateField(DomUpdateRunModeField) ), SLOT( onCurrentProductUpdateField(DomUpdateRunModeField) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WeightBarWidget::~WeightBarWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WeightBarWidget::isForcedFields() const
{
    LOG_CLASS_INFO( "WeightBarWidget", "isForcedFields" );

    if ( tare_->isForced() )
    {
        LOG_DEBUG( "tare is forced", NULL );
        return true;
    }
    else if ( byCount_->isForced() )
    {
        LOG_DEBUG( "by count is forced", NULL );
        return true;
    }
    else if ( unitPrice_->isForced() )
    {
        LOG_DEBUG( "unit price is forced", NULL );
        return true;
    }
    else if ( price_->isForced() )
    {
        LOG_DEBUG( "price is forced", NULL );
        return true;
    }

    LOG_TRACE( "no forced fields", NULL );
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins WeightBarWidget::margins() const
{
    QMargins value;

    if ( BottomWeightBarArea == area_ )
        value.setBottom( height() );
    else if ( TopWeightBarArea == area_ )
        value.setTop( height() );

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setByCount( const QString& value )
{
    // I think this is only called when updating self service. But it might be updating the main screen. Hmmm

    bool ok( false );
    int n( value.toInt( &ok ) );

    if (( !ok ) || ( n < 0 ))
        return;

    // set value
    byCount_->fromInt( n );             // I think self serv

    // update field
    onEditingFinished( byCount_ );

    if( byCountHalfie_->isVisible() )
    {
        byCountHalfie_->fromInt( n );
        onEditingFinished( byCountHalfie_ );
    }
}

// SOF-2806
///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setTareFocus()
{
    tare_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setDisplayMode( DisplayMode value )
{
    if ( displayMode_ == value )
        return;

    displayMode_ = value;

    updateProgrammableFields();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::processForcedFields()
{
    LOG_CLASS_INFO( "WeightBarWidget", "processForcedFields" );

    if (( tare_->isForced() ) && ( tare_->isVisible() ))
    {
        LOG_INFO( "processing forced tare...", NULL );
        tare_->setFocus();
    }
    else if (( byCount_->isForced() ) && ( byCount_->isVisible() ))
    {
        LOG_INFO( "processing forced by count...", NULL );
        byCount_->setFocus();
    }
    else if (( unitPrice_->isForced() ) && ( unitPrice_->isVisible() ))
    {
        LOG_INFO( "processing forced unit price...", NULL );
        unitPrice_->setFocus();
    }
    else if (( price_->isForced() ) && ( price_->isVisible() ))
    {
        LOG_INFO( "processing forced price...", NULL );
        price_->setFocus();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::translate()
{
    // label text
    QString weightText = (currentProduct() ? tr( "Net Weight" ) : tr( "Weight" ));
    QString tareText = tr( "Tare" );
    QString propTareText = tr( "Total Tare" );
    QString unitPriceText = tr( "Unit Price" );
    QString priceText = tr( "Total Price" );
    QString fixedWeightText = tr( "Net Weight" );
    QString volumeText = tr( "Net Weight" );
    QString displayWeightText = tr( "Net Weight" );
    QString byCountText = tr( "By Count" );
    QString byCountUnitPriceText = tr( "Unit Price" );
    QString exceptionPriceText = tr( "Exception Price" );

    QString upcText = tr( "Barcode Number" );
    QString discountUnitPriceText = tr( "Discount Unit Price" );
    QString discountPriceText = tr( "Discount Total Price" );
    QString savingsText = tr( "Savings" );

    // FT-881: manual weight item
    if (( currentProduct() ) && ( isKeyboardNetWeightItem_ ))
        weightText = tr( "Manual Weight" );

    // convert to upper
    if ( convertUppercase_ )
    {
        weightText = weightText.toUpper();
        tareText = tareText.toUpper();
        propTareText = propTareText.toUpper();
        unitPriceText = unitPriceText.toUpper();
        priceText = priceText.toUpper();
        fixedWeightText = fixedWeightText.toUpper();
        volumeText = volumeText.toUpper();
        displayWeightText = displayWeightText.toUpper();
        byCountText = byCountText.toUpper();
        byCountUnitPriceText = byCountUnitPriceText.toUpper();

        exceptionPriceText = exceptionPriceText.toUpper();

        upcText = upcText.toUpper();
        discountUnitPriceText = discountUnitPriceText.toUpper();
        discountPriceText = discountPriceText.toUpper();
        savingsText = savingsText.toUpper();
    }

    // set label text
    weight_->setLabelText( weightText );
    tare_->setLabelText( tareText );
    propTare_->setLabelText( propTareText );
    unitPrice_->setLabelText( unitPriceText );
    price_->setLabelText( priceText );
    fixedWeight_->setLabelText( fixedWeightText );
    volume_->setLabelText( volumeText );
    displayWeight_->setLabelText( displayWeightText );
    byCount_->setLabelText( byCountText );
    byCountHalfie_->setLabelText( "BC Halfie" );
    byCountUnitPrice_->setLabelText( byCountUnitPriceText );
    exceptionPrice_->setLabelText( exceptionPriceText );

    upc_->setLabelText( upcText );
    discountUnitPrice_->setLabelText( discountUnitPriceText );
    discountPrice_->setLabelText( discountPriceText );
    savings_->setLabelText( savingsText );

    // translate fields
    unitPrice_->translate();

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::netWeightString, weight_->labelText() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setWeightText( const QString& netWeight, const QString& dualRangeIndicator )
{
    LOG_CLASS_INFO( "WeightBarWidget", "setWeightText" );

    Q_UNUSED( dualRangeIndicator );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );
    DomCurrentPLU *product( currentProduct() );

/*
    // Note:
    // Dual range indicator does not work at the moment!!
    else if ( dualRangeIndicator.length() )
    {
        // dual range indicator is '2g' or '5g'
        bool lowRange = ("2g" == dualRangeIndicator);

        // precision
        setScalePrecision( lowRange );
    }

    else
    {
        int netWeightLength = netWeight.length();
        int netWeightIndexOfDecimal;

        if ( netWeightLength )
            if ( 0 < (netWeightIndexOfDecimal = netWeight.indexOf( "." )) )
            {
                // number of digits starting at the decimal (i.e. ".000")
                bool lowRange = (4 == (netWeightLength - netWeightIndexOfDecimal));

                // precision
                setScalePrecision( lowRange );
            }
    }
*/
    // for keyboard net weight set validator (for operator input)
    if ( isKeyboardNetWeightItem_ )
    {
        MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
        int valid;
        if(scaleConfig->isScaleMetric())
        {
            valid = 999999;
            weightPrecision_ = 3;
        }
        else
        {
            valid = ( (2 == scaleConfig->keyboardNetWeightPrecision()) ? 99999 : 999999 );      //  HTA-100:  999.99 or 999.999
        }
        weightValidator_->setTop( valid );
    }

    // weight
    //QString test1 = netWeight;
    //QString test2 = weight_->text();
    //LOG_DEBUG( "weightbar.setWeightText() - Update weight #1 netWeight               with: %s ", qPrintable( test1 ) );
    //LOG_DEBUG( "weightbar.setWeightText() - Update weight #2 weight_->text()         with: %s ", qPrintable( test2 ) );

    weight_->fromSessionField( netWeight, hobart::no_numeric_validation );

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::netWeight, weight_->text() );

    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5) )
    {
        // pixmaps
        const QSize angledFieldSize( scaleSizeForScreen( QSize( 270, 90 ) ) );
        const hobart::backgroundWidgetRenderer greenP5RenderAngledField(
            new hobart::leftSlantedWeightBarBackgroundWidget( ),
            new hobart::greenP5OperatorBackgroundWidgetRendererAppearance() );
        const hobart::backgroundWidgetRenderer renderAngledField(
            new hobart::leftSlantedWeightBarBackgroundWidget( ),
            new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

        const QPixmap angledFieldReadOnly( renderAngledField.drawControl( angledFieldSize, hobart::ReadOnly ) );
        const QPixmap angledFieldGreenP5( greenP5RenderAngledField.drawControl( angledFieldSize, hobart::Normal ) );
        flipPixmap pixmapFlipper( false, (BottomWeightBarArea == area()) );

        if( scaleConfig->getIsPick5Active() )
        {
            if( 0 == netWeight.length() )
            {
                pick5WeightFieldEnabled_  = false;
                weight_->setBackgroundReadOnly( pixmapFlipper( angledFieldReadOnly ) );
            }

            if( product )
            {
                QString tmpMinWt = product->minWeight();
                QString tmpMaxWt = product->maxWeight();

                if( tmpMinWt == "" && tmpMaxWt == "" && 0 == netWeight.length() )
                {
                    LOG_DEBUG( "we have no weight or maxs or mins", NULL);
                }
                else
                {
                    double minWt = product->minWeight().toDouble();
                    double maxWt = product->maxWeight().toDouble();
                    double actualWt = netWeight.toDouble();

                    if( ( minWt >= 0.00 && maxWt > 0.00 ) && (minWt <= actualWt && maxWt >= actualWt)  )
                    {
                        // ok we need to flip the field green
                        if( !pick5WeightFieldEnabled_ )      // falls in range and not green so make on screen field green
                        {
                            pick5WeightFieldEnabled_ = true;
                            weight_->setBackgroundReadOnly( pixmapFlipper( angledFieldGreenP5 ) );
                        }
                    }
                    else                                    // fell out of or never was in orage
                    {
                        weight_->setBackgroundReadOnly( pixmapFlipper( angledFieldReadOnly ) );
                        pick5WeightFieldEnabled_ = false;
                    }
                }
            }
        }
        else
        {
            weight_->setBackgroundReadOnly( pixmapFlipper( angledFieldReadOnly ) );
            pick5WeightFieldEnabled_ = false;
        }
    }
}

void WeightBarWidget::setOutOfTilt()
{
    // now test at 3% tilt - also dont know how fast these are coming in,
    // may have to up the mod value if coming in fast ( (weigherCts % 5) == 0 )
    if( internalConfigData->scaleWentOutOfTilt )
    {
        session_->send( Error, ErrWeigherDevice, QString::number(WeigherDevError::WeigherErrValMaxTiltReached) + ",5" );				// 1025
        updateWeightOutOfTilt(true);
        setWeightText( "", "" );
    }
    else
    {
        MadpWeightData *weightData( &config_->weightData );
        updateWeightOutOfTilt(false);
        setWeightText( weightData->netWeight(), weightData->dualRangeIndicator() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setWeigherConfig()
{
    DomWeigherConfigRead *weigherConfig( &config_->weigherConfigRead );

    QString weigherMaximumWt( weigherConfig->weigherMaximumWt() );
    weigherMaximumWt.replace( ".", "" );

    weightTop_ = weigherMaximumWt.toInt();
    dualRangeWeightTop_ = weigherConfig->dualRangeLimit();

    weightPrecision_ = dualRangeWeightPrecision_ = (weigherConfig->isScaleMetric() ? 3 : 2);
    weightStep_ = dualRangeWeightStep_ = weigherConfig->weigherScaleFactor();

    // dual range scale
    if ( session_->uiUtilities->isDualRangeCapable() )
        if (( 0 < weigherConfig->dualRangeLimit() ) && ( 1 != weightStep_ ))
        {
            // dr = 0.002
            // non-dr = 0.005
            if ( 2 == dualRangeWeightStep_ )
            {
                dualRangeWeightPrecision_ = 3;

                weightPrecision_ = 3;
                weightStep_ = 5;
            }
            // dr = 0.005
            // non-dr = 0.01
            else if ( 5 == dualRangeWeightStep_ )
            {
                dualRangeWeightPrecision_ = 3;

                weightPrecision_ = 2;
                weightStep_ = 1;
            }
        }

    // fix top values from whole number
    weightTop_ *= (2 == weightPrecision_ ? 100 : 1000);
    dualRangeWeightTop_ *= (2 == dualRangeWeightPrecision_ ? 100 : 1000);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::mousePressEvent( QMouseEvent *e )
{
    if ( Qt::LeftButton == e->button() )
        position_ = mapToParent( e->pos() );

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::mouseMoveEvent( QMouseEvent *e )
{
    // allow drag only when left button pressed and dragging enabled in operate mode
    if (( e->buttons().testFlag( Qt::LeftButton ) ) && ( moveable_ ))
        if (( operateMode_ ) && ( !operateMode_->widgetDragLock() ))
        {
            QPoint mousePos = mapToParent( e->pos() );

            // check for drag
            if ( !moving_ )
            {
                int distance = (mousePos - position_).manhattanLength();

                // drag initiated
                if ( distance >= QApplication::startDragDistance() )
                {
                    bool allowDrag = true;

                    // weight bar can only be moved when capacity bar is on the bottom
                    if ( operateMode_ )
                        allowDrag = (BottomCapacityBarArea == operateMode_->capacityBarArea());

                    moving_ = allowDrag;
                }
            }

            // move window
            if ( moving_ )
            {
                int x = pos().x();
                int y = pos().y() + (mousePos.y() - position_.y());

                // do not allow outside parent
                int min = 0;
                int max = mode_->height() - height();

                if ( operateMode_ )
                {
                    min += operateMode_->capacityBarMargins().top();
                    max -= operateMode_->capacityBarMargins().bottom();
                }

                if ( y < min )
                    y = min;
                else if ( y > max )
                    y = max;

                move( x, y );
                position_ = mousePos;
            }
        }

    _Mybase::mouseMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::mouseReleaseEvent( QMouseEvent *e )
{
    if (( Qt::LeftButton == e->button() ) && ( moving_ ))
    {
        moving_ = false;

        // snap window to edge
        int min = 0;
        int max = mode_->height() - height();

        WeightBarArea where;

        if ( operateMode_ )
        {
            min += operateMode_->capacityBarMargins().top();
            max -= operateMode_->capacityBarMargins().bottom();
        }

        // top
        if ( pos().y() < (min + ((max - min) / 2)) )
        {
            move( pos().x(), min );
            where = TopWeightBarArea;
        }

        // bottom
        else
        {
            move( pos().x(), max );
            where = BottomWeightBarArea;
        }

        // emit signal
        Q_EMIT moved( where );
    }

    _Mybase::mouseReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "WeightBarWidget", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // might end up being too extreme will have to see.
    //if(  session_->scaleConfig->internalConfig.ssactive_lock )
    //    return;

    // weigher status (i.e. weight)
    if (( Weigher == device ) && ( WgStatus == event ))
    {
        MadpWeightData *weightData( &config_->weightData );

        if( valuemaxIsEnabled_ )
            setOutOfTilt();
        else
            setWeightText( weightData->netWeight(), weightData->dualRangeIndicator() );
    }

    // weigher configuration
    else if (( Weigher == device ) && ( WgInquire == event ))
    {
        DomWeigherConfigRead m( msg );
        valuemaxIsEnabled_  = m.isScaleValuemax();

        MadpWeightData *weightData( &config_->weightData );

        setWeigherConfig();
        
        //LOG_DEBUG( "weightbar.onSessionMessage() - CALL case WgInquire with netWeight: %s ", qPrintable( weightData->netWeight() ) );
        ScaleConfiguration *scaleConfig( session_->scaleConfig );

        if( scaleConfig->internalConfig.ssactive_lock )
        {
            if( scaleConfig->scaleConfig.selfServeIgnorePrinterErrors() == 1 && scaleConfig->scaleConfig.selfServeDisplayBarcodeLabel() == 1)
                return;
            else
                setWeightText( weightData->netWeight(), weightData->dualRangeIndicator() );
        }
        else
            setWeightText( weightData->netWeight(), weightData->dualRangeIndicator() );
    }

    // currency configuration
    else if (( ConfigurationDev == device ) && ( CfgCurrencyWrite == event ))
    {
        updateCurrencySettings();
    }

    // scale configuration
    else if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        updateScaleConfig();
        updateProgrammableFields();
    }

    // normal scale operation
    else if (( ConfigurationDev == device ) && ( CfgScaleOperationWrite == event ))
    {
        DomNormalScaleOperation m( msg );

        maximumTare_ = m.maxTare();
        updateScaleConfig();
    }

    // weigh mode
    else if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        upc_->setText( updateUpcNumberField() );

        // WEH-14
        updateFieldPermissions();
    }

    // request platter tare
    else if (( SessionDev == device ) && ( SnRequestPlatterTare == event ))
    {
        DomCurrentPLU *product( currentProduct() );

        if ( !product )
            LOG_WARN( "not enabling platter tare, no product", NULL );
        else if ( !tare_->isVisible() )
            LOG_WARN( "not enabling platter tare, tare not active", NULL );
        else
        {
            // enable platter tare
            session_->send( SessionDev, SnPlatterTareOn );

            // clear current tare value
            tare_->clear();
            propTare_->clear();    // jhc proptare

            // update hooks
            sessionHooks::instance()->updateFieldValue( sessionHooks::itemTare, "" );
        }
    }

    // softkeys
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        if ( hobart::softkeysWidget::PresetTareGrid == m.featureNum() )
        {
            tareFlashkeysExist_ = (0 < m.totalNumOfSoftkeys());

            ScaleConfiguration *scaleConfig( session_->scaleConfig );

            // pjr383 TODO - Delhaize special, they wanted TARE turned on, but no containers. This needs replaced with ANOTHER config
            // check to see if we have any flashkeys
            int totalKeys = m.totalNumOfSoftkeys();
            if( totalKeys > 0 )
                scaleConfig->tareFlashkeysDoExist = true;
            else
                scaleConfig->tareFlashkeysDoExist = false;
        }
    }

    // product error
    else if (( Error == device ) && ( ErrProductDevice == event ))
    {
        returnedErrorsProductDev m( msg );

        // platter tare
        if ( ProductDevError::ProductErrPlatterTareMode == m.errorCode() )          // 101043
        {
            DomCurrentPLU *product( currentProduct() );

            // no product
            if ( !product )
                LOG_WARN( "bad platter tare, no product", NULL );

            // operator said YES
            else if ( returnedErrorsProductDev::ActionYes == m.errorData2() )
            {
                MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

                // HT-136 Send correct tare to application so it can re-calculate
                if( scaleConfig->useSecondTare() )
                    operateMode_->writeSingleField( OperateMode::OffsetTareB, product->tareB(), "" );
                else
                    operateMode_->writeSingleField( OperateMode::OffsetTare, product->tare(), "" );
            }

            // disable platter tare
            session_->send( SessionDev, SnPlatterTareOff );

            // retrieve new totals
            session_->send( ProductDev, ProductReadCurrentTotalValues );
        }
    }

    // weigher error
    else if( (Error == device) && (ErrWeigherDevice == event) )
    {
        returnedErrorsWeigherDev m( msg );

        if ( WeigherDevError::WeigherErrValMaxTiltReached == m.errorCode() )				// 1025
        {
            // when there is NO plu, all cases are handled. Whats not is when we have a PLU
            // on the screen and we go out of tilt. We receive NO 1,4 messages to test if we
            // are out of tilt, just a dialog warning FROM the application at some point. So
            // if we have a product AND we go out of tilt during that product, handle it here

            DomCurrentPLU *product( currentProduct() );

            // no product
            if ( !product )
                LOG_WARN( "Only want to do something if if we have a plu", NULL );
            else
            {
                // IF we receive this warning dialog - WE ARE OUT OF TILT - ONLY gets reset if we receive a 101,15
                if( valuemaxIsEnabled_ )                    // just have to make sure
                {
                    internalConfigData->scaleWentOutOfTilt = true;
                    setWeightText( "", "" );
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onByCountFocused( bool hasFocus )
{
    if ( !hasFocus )
        hideOperatorMessage();
    else if ( byCount_->isForced() )
    {
        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid by count for this item." ) );

        showOperatorMessage( type, message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onClearPluForCutTest()
{
    // clear current product details
    clearFields();
    updateProgrammableFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onCurrentProductChanged( bool nof )
{
    LOG_CLASS_INFO( "WeightBarWidget", "onCurrentProductChanged" );

    LOG_DEBUG( "product changed %d", nof );

    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    // Reveived a 101,7 - Current Product Write message

    // clear current product details
    clearFields();
    updateProgrammableFields();

    // product not on file, nothing to do
    if ( nof )
        return;
    else if ( !operateMode_ )
        return;

    DomCurrentPLU *product( currentProduct() );

    // no product
    if ( !product )
        return;

    // tare
    // HT-136 Use correct tare
    tare_->fromSessionField( (scaleConfig->useSecondTare() ? product->tareB() : product->tare() ) );

    // unit price
    unitPrice_->fromSessionField( product->unitPrice() );

    ignoreSignals_ = true;
    unitPrice_->setWeightFor( product->poundsForAmount() );
    unitPrice_->setFractional( product->fractionalPriceUnit() );
    updateDisplayUnitPriceField( product->unitPrice() );  // FT-472
    ignoreSignals_ = false;

    // fixed weight
    fixedWeight_->fromSessionField( product->fixedWeight() );

    // volume
    volume_->fromSessionField( product->fixedWeight() );

    // display weight

    if ( 2 == product->productType() ) // fixed weight
        updateDisplayWeightField( product->fixedWeight() );
    else if ( 4 == product->productType() ) // fluid ounces
        updateDisplayWeightField( volume_->text() );

    // by count
    byCount_->fromInt( product->byCount() );

    if( byCountHalfie_->isVisible() )
        byCountHalfie_->fromInt( product->byCount() );

    // exception price
    // This will NOT take a concatted string value.  The last line of this method does a toInt()
    exceptionPrice_->fromSessionField( product->exceptionPrice() );

    // upc
    upc_->setText( updateUpcNumberField() );

    // update hooks
    QString productExceptionPrice( product->exceptionPrice().trimmed() );
    productExceptionPrice.replace( "\"", "" );
    productExceptionPrice.replace( "0", "" );
    productExceptionPrice.replace( ".", "" );

    DomPrintSpecialFields *config = &session_->scaleConfig->printSpecialFields;

    sessionHooks::instance()->updateFieldValue( sessionHooks::itemTare, tare_->text() );
    if( ( 1 == config->unitPriceFormat().toInt() ) && ( 3 == product->productType() ) )
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, unitPrice_->text() );

    sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPriceCountFor, byCount_->text() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPriceFractional, unitPrice_->fractional() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPricePoundsFor, unitPrice_->weightFor() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemByCount, byCount_->text() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemExceptionPrice, exceptionPrice_->text() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemHasExceptionPricing, !productExceptionPrice.isEmpty() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::itemHasFreqShopperPricing, product->hasFreqShopper() );

    updateForcedFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onCurrentProductTotalsChanged()
{
    LOG_CLASS_INFO( "WeightBarWidget", "onCurrentProductTotalsChanged" );

    DomCurrentPLU *product( currentProduct() );
    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );
    DomPrintSpecialFields *config = &session_->scaleConfig->printSpecialFields;

    if ( !productTotals )
    {
        // I dont know how long this hasn't worked but it's had to been for awhile.  The problem is that when we
        // set  MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() ); if we dont
        // have a currentplu, ie, plu > 0, then productTotals is set to NULL and we exit.  The problem with this
        // when we want to clear the grandTotals on the screen (price, weight, etc) we don't have a way to clear
        // the screen fields like price b\c when we want to do this we don't have a PLU and we send a
        // 101,15 with a PLU of -1 and a cleared\blank\empty price.  But we need to do this action to clear stuff

        MadpUpdateCurrentPLUTotals *currentTotals = &session_->scaleConfig->currentPLUTotals;
        const int itemNumber = currentTotals->itemNumber().toInt();
        const QString totalPrice = currentTotals->totalPrice();

        if( itemNumber == -1 )
        {
            // then we received a 101,15 with a PLU of -1 which means clear price
            // we could also clear the weight here if there is an issue.
            price_->fromSessionField( totalPrice );

            // might need to reset the background here for PICK5
            updateTotalPriceBackgroundColor();
        }
        return;
    }

    QString weight( productTotals->totalNetWeight() );
    const QString tare( productTotals->totalTare() );
    const QString propTare( productTotals->totalTare() );
    const QString price( productTotals->totalPrice() );

    LOG_DEBUG( "have weight '%s' tare '%s' price '%s'",
        qPrintable( weight ),
        qPrintable( tare ),
        qPrintable( price ),
        qPrintable( propTare ));

    if( internalConfigData->scaleWentOutOfTilt )
    {
        if( ( "----" == weight ) || ("" == weight ) )
            setWeightText( "", "" );
        else
        {
            if( weight.toFloat() > 0 )
            {
                // Then the weight of come back and we are back in level again
                 internalConfigData->scaleWentOutOfTilt = false;
                 setWeightText( weight, productTotals->dualRangeIndicator() );
            }
        }
    }
    else
    {
        // SOF-3698: Build-a-Meal mode was displaying "0.001" in the Weight field on the Operator
        //           screen after meal label printed. This was the result of the "By Count"
        //           value (e.g. "1") being passed in the Net Weight field.
        //
        //           This will prevent that by count value from flashing on the screen
        //           after the label is printed.

        DomWeighModeTotalType *msg = &session_->scaleConfig->weighModeTotalType;
        if( msg->weighMode() == OpModeMealBuilder )
        {
            if( weight.length() > 0 && !( weight.contains( "." ) ) )
                weight = "0";
        }
        else if( config_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            // pile it on, same issue as above here for self serve new ui
           if( ( "" == weight ) || weight.contains( curSeparator_ ) )
               setWeightText( weight, productTotals->dualRangeIndicator() );        // eliminates bycounts
        }
        else
            setWeightText( weight, productTotals->dualRangeIndicator() );
    }

    // FT-158 [RandyB]: begin
    // tare comes from product record only, should not be taken from product totals
    //
    // do not set value if operator actively modifying
    //if ( !tare_->hasFocus() )
    //    tare_->fromSessionField( tare );
    // FT-158 [RandyB]: end

    // ah but the Proportional Tare comes from the Product Totals
    propTare_->fromSessionField( propTare );	// jhc proptare

    QString tmpPr = productTotals->unitPrice();
    int prodTypeInt     = product->productType();
    QString prodTypeStr = QString::number( product->productType() );
    LOG_DEBUG( "have product type as '%s'", qPrintable( prodTypeStr ) );

    if( prodTypeInt == 3 )        // this seems to fail reversed style
    {
        LOG_DEBUG( "Entered if prodType is 3 is TRUE", NULL );

        // if formattype is 1, display the bycount the way we have been displaying it for 25 years
        if( config->unitPriceFormat().toInt() > 1 && tmpPr.length() > 0 )
        {
            LOG_DEBUG( "Entered display bycount unit price is TRUE", NULL );

            int tmpFont = ( (byCount_->font().pointSize() / 2) );       // whats the size of something else?
            modifyFontPointSize byCountUnitPriceFont( tmpFont );
            byCountUnitPriceFont( byCountUnitPrice_);
            byCountUnitPrice_->show();
            byCountUnitPrice_->setText( tmpPr );

            if( BC_UNIT_PRICE_DEVELOPMENT )
            {
                byCountHalfie_->show();
                byCountHalfie_->setText( QString::number(product->byCount()) );

                byCount_->hide();

                //QRect r_bc     = byCount_->geometry();
                //QRect r_bcup   = byCountUnitPrice_->geometry();
                //QRect r_bchalf = byCountHalfie_->geometry();
            }
        }
        else
        {
            LOG_DEBUG( "Entered display bycount unit price is FALSE", NULL );
            byCount_->show();

            byCountUnitPrice_->hide();
            byCountHalfie_->hide();
        }
    }
    else
    {
        LOG_DEBUG( "Entered if prodType is 3 is FALSE", NULL );
        byCountUnitPrice_->hide();
        byCountHalfie_->hide();
    }

    // do not set value if operator actively modifying
    if ( !price_->hasFocus() )
    {
        price_->fromSessionField( price );

        // Pick5 Change Background COLOR
        updateTotalPriceBackgroundColor();

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemTotalPrice, price_->text() );

        // update unit price now if needed
        if( ( config->unitPriceFormat().toInt() > 1 ) && ( 3 == product->productType() ) )
            sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, tmpPr );
    }

    // update frequent shopper details
    if ( product->hasFreqShopper() )
    {
        discountUnitPrice_->fromSessionField( productTotals->fsUnitPrice() );
        discountPrice_->fromSessionField( productTotals->fsTotalPrice() );
        savings_->fromSessionField( productTotals->fsSavings() );

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemDiscountedUnitPrice,  discountUnitPrice_->text() );
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemDiscountedTotalPrice, discountPrice_->text() );
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemDiscountedSavings,    savings_->text() );
    }

    updateWeightTareFields();

    // FT-816 FPP PLU Weight showing blank
    // Update the weight in the DCA frame if we have some FPP action going on.
    // But do it last since it's likely the least important thing to do.
    updateFreshProductionPlanningWeight(weight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateTotalPriceBackgroundColor()
{
    LOG_CLASS_INFO( "WeightBarWidget", "updateTotalPriceBackgroundColor" );

    // pixmaps
    const QSize angledFieldSize( scaleSizeForScreen( QSize( 270, 90 ) ) );
    const hobart::backgroundWidgetRenderer greenP5RenderAngledField(
        new hobart::leftSlantedWeightBarBackgroundWidget( ),
        new hobart::greenP5OperatorBackgroundWidgetRendererAppearance() );
    const hobart::backgroundWidgetRenderer renderAngledField(
        new hobart::leftSlantedWeightBarBackgroundWidget( ),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );
    const hobart::backgroundWidgetRenderer renderAngledFieldForced(
        new hobart::leftSlantedWeightBarBackgroundWidget(),
        new hobart::forcedOperatorBackgroundWidgetRendererAppearance<1>() );

    const QPixmap angledFieldReadOnly( renderAngledField.drawControl( angledFieldSize, hobart::ReadOnly ) );
    const QPixmap angledFieldGreenP5( greenP5RenderAngledField.drawControl( angledFieldSize, hobart::Normal ) );
    const QPixmap angledField( renderAngledField.drawControl( angledFieldSize, hobart::Normal ) );

    flipPixmap horizontalPixmapFlipper( true, (BottomWeightBarArea == area()) );
    flipPixmap verticalPixmapFlipper( false, (BottomWeightBarArea == area()) );

    ScaleConfiguration *config( session_->scaleConfig );
    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );
    MadpUpdateCurrentPLUTotals *currentTotals = &session_->scaleConfig->currentPLUTotals;

    DomCurrentPLU *product( currentProduct() );
    const int itemNumber = currentTotals->itemNumber().toInt();
    const QString totalPrice = currentTotals->totalPrice();
    QString price( "" );

    if( productTotals )
        price = productTotals->totalPrice();

    if( itemNumber == -1 )
    {
        // then we received a 101,15 with a PLU of -1 which means clear price
        // we could also clear the weight here if there is an issue.
        // might need to reset the background here for PICK5

        price_->fromSessionField( totalPrice );

        if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5) )
        {
            if( pick5PriceFieldEnabled_ )
            {
                pick5PriceFieldEnabled_ = false;    // enabled when Price Field on screen is green (not bg)
                price_->setBackground( horizontalPixmapFlipper( angledField ), true );  // could use angledFieldReadOnly
                price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
            }

            if( pick5WeightFieldEnabled_ )
            {
                pick5WeightFieldEnabled_ = false;   // enabled when Weight Field on screen is green (not bg)
                weight_->setBackgroundReadOnly( verticalPixmapFlipper( angledFieldReadOnly ) );

            }
        }
        return;
    }

    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5) )
    {
        if( config->getIsPick5Enabled() && config->getIsPick5Active() )           // ok - its been toggled on
        {
            QString tmpMinPr = product->minPrice();
            QString tmpMaxPr = product->maxPrice();

            if( tmpMinPr == "" && tmpMaxPr == "" && 0 == price.length() )
            {
                LOG_DEBUG( "we have no price or maxs or mins", NULL);
            }
            else
            {
                float minPr = product->minPrice().toFloat();
                float maxPr = product->maxPrice().toFloat();
                float actualPr = price.toFloat();

                if( ( minPr >= 0 && maxPr > 0 ) && ( minPr <= actualPr && actualPr <= maxPr ) )
                {
                    price_->setBackground( horizontalPixmapFlipper( angledFieldGreenP5 ) );
                    price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldGreenP5 ) );
                    pick5PriceFieldEnabled_ = true;
                }
                else                                    // fell out of or never was in green
                {
                    price_->setBackground( horizontalPixmapFlipper( angledField ) );
                    price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
                    pick5PriceFieldEnabled_ = false;
                }
            }
        }
        else
        {
            // its been toggled off or was never on
            price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
            price_->setBackground( horizontalPixmapFlipper( angledField ) );
            pick5PriceFieldEnabled_ = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onCurrentProductUpdateField( const DomUpdateRunModeField& m )
{
    bool processed = true;

    const QString newValue( const_cast<DomUpdateRunModeField*>( &m )->newValue() );

    // FT-820 FPP Quantity not updating on a PLU with forced by count.
    // Didn't really like initializing these here but got compile errors if I didn't
    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );

    if ( !productTotals )
            return;

    const QString weight( productTotals->totalNetWeight() );

    // handle update
    switch ( (OperateMode::ProductWriteSingleFieldOffset) m.offsetIndex() )
    {
    case OperateMode::OffsetBarcodeProductNum:
        upc_->setText( updateUpcNumberField() );
        break;

    case OperateMode::OffsetByCount:
    {
        if (( byCount_->isForced() ) && ( m.isUpdateLegal() ))
        {
            ignoreSignals_ = true;
            operateMode_->setFocusForPopupNumberPad();
            ignoreSignals_ = false;
        }

        // here in the 101,16 newValue is a QString, where in a DomCurrentPLU BC is an int and uses fromInt()
        updateWeightTareFields();
        byCount_->fromSessionField( newValue );

        if( byCountHalfie_->isVisible() )
            byCountHalfie_->fromSessionField( byCount_->text() );

        // FT-820 FPP Quantity not updating on a PLU with forced by count.
        // On a single field write, need to refresh the FPP frame if its available.
        updateFreshProductionPlanningWeight(weight);

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemByCount, byCount_->text() );
    }
        break;

    case OperateMode::OffsetExceptionPrice:
    {
        exceptionPrice_->fromSessionField( newValue );
        break;
    }

    case OperateMode::OffsetUnitPrice:
    {
        if (( unitPrice_->isForced() ) || ( price_->isForced() ))
            if ( m.isUpdateLegal() )
            {
                ignoreSignals_ = true;
                operateMode_->setFocusForPopupNumberPad();
                ignoreSignals_ = false;
            }

        unitPrice_->fromSessionField( newValue );
        updateDisplayUnitPriceField( newValue );  // FT-472

        // update hooks
        DomCurrentPLU *product( currentProduct() );
        DomPrintSpecialFields *config = &session_->scaleConfig->printSpecialFields;
        QString tmpPr = productTotals->unitPrice();

        if( ( config->unitPriceFormat().toInt() > 1 ) && ( 3 == product->productType() ) )
            sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, tmpPr );
        else
            sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, unitPrice_->text() );

        break;
    }

    case OperateMode::OffsetTare:
    case OperateMode::OffsetTareB:  // HT-309
        if (( tare_->isForced() ) && ( m.isUpdateLegal() ))
        {
            ignoreSignals_ = true;
            operateMode_->setFocusForPopupNumberPad();
            ignoreSignals_ = false;
        }

        tare_->fromSessionField( newValue );

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemTare, tare_->text() );
        session_->scaleConfig->internalConfig.selfServicePreventClicks = false;
        break;

    case OperateMode::OffsetFixedWeight:
        fixedWeight_->fromSessionField( newValue );
        volume_->fromSessionField( newValue );
        updateDisplayWeightField( newValue );
        break;

    case OperateMode::OffsetFractionalPriceUnit:
        ignoreSignals_ = true;
        unitPrice_->setFractional( newValue.toInt() );
        ignoreSignals_ = false;

        {
            // FT-933: when fractional price changes we need to zero unit price
            bool restorePluFocus;

            if ( (restorePluFocus = unitPrice_->hasFocus()) )
                unitPrice_->clearFocus();

            unitPrice_->blockSignals( true );
            unitPrice_->fromInt( 0 );
            unitPrice_->blockSignals( false );

            if ( restorePluFocus )
                unitPrice_->setFocus();

            onEditingFinished( unitPrice_ );
        }

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPriceFractional, unitPrice_->fractional() );

        break;

    case OperateMode::OffsetPoundsForAmount:
        ignoreSignals_ = true;
        unitPrice_->setWeightFor( newValue.toInt() );
        ignoreSignals_ = false;

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPricePoundsFor, unitPrice_->weightFor() );

        break;

    default:
        processed = false;
        break;
    }

    // post field processing
    if ( processed )
    {
        // update forced fields
        updateForcedFields();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onEditingFinished( QWidget *field )
{
    if ( !field )
        return;
    else if ( ignoreSignals_ )
        return;

    DomCurrentPLU *product( currentProduct() );
    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    if ( !product )
        return;
    else if ( !productTotals )
        return;

    int index = 0;
    QString newValue = dynamic_cast<hobart::labeledNumericLineEdit*>( field )->toSessionField();
    QString oldValue;

    if (( weight_ == field ) && ( isKeyboardNetWeightItem_ ))
    {
        index = OperateMode::OffsetKeyboardNetWeight;
        oldValue = product->keyboardNetWeight();
    }
    else if (( fixedWeight_ == field ) || ( volume_ == field ))
    {
        index = OperateMode::OffsetFixedWeight;
        oldValue = product->fixedWeight();
    }
    else if ( tare_ == field )
    {
        // HT-136 Send correct tare to application so it can re-calculate
        index = (scaleConfig->useSecondTare() ? OperateMode::OffsetTareB : OperateMode::OffsetTare );
        oldValue = (scaleConfig->useSecondTare() ? product->tareB() : product->tare() );
    }
    else if ( unitPrice_ == field )
    {
        index = OperateMode::OffsetUnitPrice;
        oldValue = product->unitPrice();
    }
    else if ( byCount_ == field )
    {
        index = OperateMode::OffsetByCount;
        oldValue = product->byCountRaw();
    }
    else if( byCountHalfie_ == field )
    {
        index = OperateMode::OffsetByCount;
        oldValue = product->byCountRaw();
    }
    else if (( price_ == field ) && ( productTotals ))
    {
        // Note:
        // The price field (total price) is only editable for non random weight items where unit
        // price is not shown. This field is taking on the role of unit price.

        index = OperateMode::OffsetUnitPrice;
        oldValue = productTotals->totalPrice();

        productTotals->totalPrice() = newValue;

        // save product info first
        if ( newValue != oldValue )
        {
            session_->send( ProductDev, ProductWriteCurrentTotalValues,
                product->itemNumber() + ",\"\",\"" + byCount_->toSessionField() + "\",\"" + price_->toSessionField() + "\"" );
        }
    }

    // write
    operateMode_->writeSingleField( (OperateMode::ProductWriteSingleFieldOffset) index, newValue, oldValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onFixedWeightFocused( bool hasFocus )
{
    if ( !hasFocus )
        hideOperatorMessage();
    else if ( fixedWeight_->isForced() )
    {
        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid weight for this item." ) );

        showOperatorMessage( type, message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onInvalidAccessLevel( QWidget *field )
{
    if ( !field )
        return;
    else if ( ignoreSignals_ )
        return;

    operateMode_->processInvalidAccessLevel( field );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onPriceFocused( bool hasFocus )
{
    if ( !hasFocus )
        hideOperatorMessage();
    else if ( price_->isForced() )
    {
        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid price for this item." ) );

        showOperatorMessage( type, message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onTareFocused( bool hasFocus )
{
    // SOF-1913 Forced tare field shows 3-digit tare when scale
    // is past the 12 lb threshold if Search is used

    if ( !hasFocus )
        hideOperatorMessage();

    // tare flashkeys show up whenever tare is focused into
    else if ( tareFlashkeysExist_ )
        showOperatorMessage( hobart::dynamicContentWidget::TareFlashkeys, QString() );

    // no tare flashkeys, so so forced message if forced field
    else if ( tare_->isForced() )
    {
        // update the tare precission for the fields if necessary when forced.

        updateTareDigits();

        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid tare for this item." ) );

        showOperatorMessage( type, message );
    }

    else if ( !tare_->isForced() )
    {
        // update the tare precission for the fields if necessary when forced  or not.
        // Could be coming down from 12 and call up a PLU with a 3 digit tare not forced.
        updateTareDigits();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onUnitPriceFocused( bool hasFocus )
{
    if ( !hasFocus )
        hideOperatorMessage();
    else if ( unitPrice_->isForced() )
    {
        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid unit price for this item." ) );

        showOperatorMessage( type, message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onUnitPriceFractionalChanged( int value )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;
    else if ( clearingFields_ ) // FT-974
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetFractionalPriceUnit,
        QString::number( value ),
        product->fractionalPriceUnitRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onUnitPriceWeightForChanged( int value )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;
    else if ( clearingFields_ ) // FT-974
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetPoundsForAmount,
        QString::number( value ),
        product->poundsForAmountRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onUpcPrefixChanged( int prefix )
{
    // This method gets called when the UPC check digit changes
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;
    else if ( clearingFields_ ) // FT-974
        return;

    QString result;
	int maxChars = 5;
    OperateMode::ProductWriteSingleFieldOffset offset = OperateMode::OffsetBarcodeProductNum;

    if ( product )		//	HA-100
    {
        //int barcodeFields = product->barcodeFields();
        //if ( 1 == barcodeFields )
        //{
        //    result = product->productNumber();
        //    if( !session_->scaleConfig->scaleConfig.secondCheckDigit() )
        //	      maxChars = 6;
        //}
        //else if ( 3 == barcodeFields )
        //{
        //    result = product->vendorNumber();
        //    offset = OperateMode::OffsetBarcodeVendorNum;
        //}

        // had to test this differently moving forward - PJR-263 Switch barcode type on label when total price exceeds $99.99
        QString bc_prod = product->productNumber();
        QString bc_vend = product->vendorNumber();
        if( ( bc_prod.length() > 0 ) && (  bc_vend.length() > 0 ) )
        {
            // Clarification - This is the vendornumber even though we are testing 3 == barcodes bc when the
            // checkdigit changes on a v+n, its really just the vendor number that is getting changed here
            result = product->vendorNumber();
            offset = OperateMode::OffsetBarcodeVendorNum;
        }
        else if ( bc_prod.length() > 0 )
		{
            result = product->productNumber();
            if( !session_->scaleConfig->scaleConfig.secondCheckDigit() )
				maxChars = 6;
        }
		else
		{
            // Something is wrong here. For UPC the barcodeFields should either be 1 (ProductNum) or 3 (VendorNum + ProductNum)
        }

		if (result.length() < maxChars)
		{
			result = QString::number( prefix ) + result;
		}
		else
		{
            result = QString::number( prefix ) + result.right( result.length() - 1 );
		}

	    // write
	    operateMode_->writeSingleField(
	        offset,
			result,
	        product->productNumber() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::onVolumeFocused( bool hasFocus )
{
    if ( !hasFocus )
        hideOperatorMessage();
    else if ( volume_->isForced() )
    {
        int type = hobart::dynamicContentWidget::OperatorMessage;
        QString message( tr( "Please use the keypad to enter\na valid volume for this item." ) );

        showOperatorMessage( type, message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::initialize()
{
    static const QMargins margins( 10, 10, 10, 6 );
    static const QMargins marginsLargeLeft( scaleWidthForScreen( 20 ), 10, 5, 6 );
    static const QMargins marginsLargeRight( 5, 10, scaleWidthForScreen( 20 ), 6 );
    static const QMargins marginsLargeRight2( 5, 10, 10, 6 );
    static const QMargins marginsLargeTop(10, 36, 10, 6 ); // L,T,R,B - proptare 22=32
    static const QMargins marginsBcUpTop(10, 22, 10, 6 ); // L,T,R,B

    // pixmaps
    const QSize angledFieldSize( scaleSizeForScreen( QSize( 270, 90 ) ) );
    int scaled_field_width = ( ( width() - (scaleWidthForScreen( 250 ) * 2) ) / 2) - 5;
    int half_field_scaled_width = scaled_field_width / 2;
    const QSize tabbedFieldSize( scaled_field_width, angledFieldSize.height() );
    const QSize halfTabbedFieldSize( half_field_scaled_width, angledFieldSize.height() );    // proptare

    const QSize bcupFieldSize( scaled_field_width, angledFieldSize.height() );
    const QSize bcupHalfTabbedFieldSize( ( bcupFieldSize.width() / 2 ) + 30 , angledFieldSize.height() );    // bycount unit price
    const QSize squareFieldSize( (scaled_field_width - bcupHalfTabbedFieldSize.width()) + 5, angledFieldSize.height() );

    const hobart::backgroundWidgetRenderer renderAngledField(
        new hobart::leftSlantedWeightBarBackgroundWidget( ),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer greenP5RenderAngledField(
        new hobart::leftSlantedWeightBarBackgroundWidget( ),
        new hobart::greenP5OperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderAngledFieldForced(
        new hobart::leftSlantedWeightBarBackgroundWidget(),
        new hobart::forcedOperatorBackgroundWidgetRendererAppearance<1>() );

    const hobart::backgroundWidgetRenderer renderTabbedField(
        new hobart::leftNotchedWeightBarBackgroundWidget(  ),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderTabbedFieldForced(
        new hobart::leftNotchedWeightBarBackgroundWidget(),
        new hobart::forcedOperatorBackgroundWidgetRendererAppearance<1>() );

    // for proportional Tare only - proptare
    const hobart::backgroundWidgetRenderer renderPropTareField(
        new hobart::halfLeftNotchedWeightBarBackgroundWidget( ),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    // for ByCount Unit Price only - ByCountUP
    const hobart::backgroundWidgetRenderer renderByCountUPField(
        new hobart::halfMoreLeftNotchedWeightBarBackgroundWidget( ),
        new hobart::lightNeverOperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderSquareField(
        new hobart::squareWeightBarBackgroundWidget(  ),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderSquareFieldForced(
        new hobart::squareWeightBarBackgroundWidget(),
        new hobart::forcedOperatorBackgroundWidgetRendererAppearance<1>() );

    const QPixmap angledField( renderAngledField.drawControl( angledFieldSize, hobart::Normal ) );
    const QPixmap angledFieldFocused( renderAngledField.drawControl( angledFieldSize, hobart::Focused ) );
    const QPixmap angledFieldReadOnly( renderAngledField.drawControl( angledFieldSize, hobart::ReadOnly ) );
    const QPixmap angledFieldForced( renderAngledFieldForced.drawControl( angledFieldSize, hobart::Normal ) );
    const QPixmap angledFieldForcedFocused( renderAngledFieldForced.drawControl( angledFieldSize, hobart::Focused ) );
    const QPixmap angledFieldForcedReadOnly( renderAngledFieldForced.drawControl( angledFieldSize, hobart::ReadOnly ) );
    const QPixmap angledFieldShadow( renderAngledField.drawDropShadow( angledFieldSize ) );

    const QPixmap tabbedField( renderTabbedField.drawControl( tabbedFieldSize, hobart::Normal ) );
    const QPixmap tabbedFieldFocused( renderTabbedField.drawControl( tabbedFieldSize, hobart::Focused ) );
    const QPixmap tabbedFieldReadOnly( renderTabbedField.drawControl( tabbedFieldSize, hobart::ReadOnly ) );
    const QPixmap tabbedFieldForced( renderTabbedFieldForced.drawControl( tabbedFieldSize, hobart::Normal ) );
    const QPixmap tabbedFieldForcedFocused( renderTabbedFieldForced.drawControl( tabbedFieldSize, hobart::Focused ) );
    const QPixmap tabbedFieldForcedReadOnly( renderTabbedFieldForced.drawControl( tabbedFieldSize, hobart::ReadOnly ) );
    const QPixmap tabbedFieldShadow( renderTabbedField.drawDropShadow( tabbedFieldSize ) );

    // bycountHalfie
    const QPixmap squareField( renderSquareField.drawControl( squareFieldSize, hobart::Normal ) );
    const QPixmap squareFieldFocused( renderSquareField.drawControl( squareFieldSize, hobart::Focused ) );
    const QPixmap squareFieldReadOnly( renderSquareField.drawControl( squareFieldSize, hobart::ReadOnly ) );
    const QPixmap squareFieldForced( renderSquareFieldForced.drawControl( squareFieldSize, hobart::Normal ) );
    const QPixmap squareFieldForcedFocused( renderSquareFieldForced.drawControl( squareFieldSize, hobart::Focused ) );
    const QPixmap squareFieldForcedReadOnly( renderSquareFieldForced.drawControl( squareFieldSize, hobart::ReadOnly ) );
    const QPixmap squareFieldShadow( renderSquareField.drawDropShadow( squareFieldSize ) );

    // proptare
    const QPixmap propTareField(         renderPropTareField.drawControl(    halfTabbedFieldSize, hobart::Normal ) );
    const QPixmap propTareFieldReadOnly( renderPropTareField.drawControl(    halfTabbedFieldSize, hobart::Normal ) );
    const QPixmap propTareFieldShadow(   renderPropTareField.drawDropShadow( halfTabbedFieldSize ) );

    // bycountUnitPrice - bcup
    const QPixmap byCountUPField(         renderByCountUPField.drawControl(    bcupHalfTabbedFieldSize, hobart::Normal ) );
    const QPixmap byCountUPFieldReadOnly( renderByCountUPField.drawControl(    bcupHalfTabbedFieldSize, hobart::Normal ) );
    const QPixmap byCountUPFieldShadow(   renderByCountUPField.drawDropShadow( bcupHalfTabbedFieldSize ) );

    flipPixmap horizontalPixmapFlipper( true );

    // fonts
    QFont scaledFont( font() );
    scaledFont.setPointSize( scaleFontPointSizeForScreen( scaledFont.pointSize() ) );
    setFont( scaledFont );

    // signal mappers
    editingFinishedSignal_ = new QSignalMapper( this );
    connect( editingFinishedSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onEditingFinished(QWidget*) ) );

    invalidAccessLevelSignal_ = new QSignalMapper( this );
    connect( invalidAccessLevelSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onInvalidAccessLevel(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // create widgets

    // weight
    weight_ = new hobart::labeledNumericLineEdit( this, angledField );
    weight_->setBackgroundFocused( angledFieldFocused );
    weight_->setBackgroundReadOnly( angledFieldReadOnly );
    weight_->setDropShadow( angledFieldShadow );
    weight_->setContentsMargins( margins );
    weight_->setObjectName( "weight" );
    weight_->setLabelObjectName( "weight_label" );

    weight_->setInputTruncation( true );
    weight_->setInputWrapping( true );
    weight_->setValidator( weightValidator_ = new QIntValidator( -99999, 99999, this ) );

    editingFinishedSignal_->setMapping( weight_, weight_ );
    connect( weight_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( weight_, weight_ );
    connect( weight_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    widgets_.push_back( weight_ );

    weightTop_ = 3000;
    weightPrecision_ = 2;
    weightStep_ = 1;

    dualRangeWeightTop_ = 12000;
    dualRangeWeightPrecision_ = 3;
    dualRangeWeightStep_ = 5;

    // tare
    tare_ = new hobart::labeledNumericLineEdit( this, tabbedField );
    tare_->setBackgroundFocused( tabbedFieldFocused  );
    tare_->setBackgroundReadOnly( tabbedFieldReadOnly  );
    tare_->setBackground( tabbedFieldForced, true );
    tare_->setBackgroundFocused( tabbedFieldForcedFocused, true );
    tare_->setBackgroundReadOnly( tabbedFieldForcedReadOnly, true );
    tare_->setDropShadow( tabbedFieldShadow );
    tare_->setContentsMargins( marginsLargeLeft );
    tare_->setObjectName( "tare" );
    tare_->setLabelObjectName( "tare_label" );

    tare_->setInputTruncation( true );
    tare_->setInputWrapping( true );
    tare_->setValidator( tareValidator_ = new QIntValidator( 0, 99999, this ) );

    editingFinishedSignal_->setMapping( tare_, tare_ );
    connect( tare_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( tare_, tare_ );
    connect( tare_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( tare_, SIGNAL( focused(bool,QRect) ), SLOT( onTareFocused(bool) ) );

    widgets_.push_back( tare_ );

    // prop tare - this is not an editable field or forced but the reg tare still needs to be editable
    propTare_ = new hobart::labeledNumericLineEdit( this, propTareField );
    propTare_->setBackgroundReadOnly( propTareFieldReadOnly  );
    propTare_->setDropShadow( propTareFieldShadow );
    propTare_->setContentsMargins( marginsBcUpTop );
    propTare_->setObjectName( "propTare" );
    propTare_->setLabelObjectName( "propTare_label" );

    propTare_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );  // AlignRight
    propTare_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  ); // AlignRight

    widgets_.push_back( propTare_ );

    // unit price
    unitPrice_ = new hobart::labeledUnitPriceEdit( this, horizontalPixmapFlipper( tabbedField ) );
    unitPrice_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldFocused ) );
    unitPrice_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldReadOnly ) );
    unitPrice_->setBackground( horizontalPixmapFlipper( tabbedFieldForced ), true );
    unitPrice_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldForcedFocused ), true );
    unitPrice_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldForcedReadOnly ), true );
    unitPrice_->setDropShadow( horizontalPixmapFlipper( tabbedFieldShadow ) );
    unitPrice_->setContentsMargins( marginsLargeRight );
    unitPrice_->setObjectName( "unitPrice" );
    unitPrice_->setLabelObjectName( "unitPrice_label" );

    unitPrice_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    unitPrice_->setInputTruncation( true );
    unitPrice_->setInputWrapping( true );
    unitPrice_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    unitPrice_->setValidator( unitPriceValidator_ = new QIntValidator( 0, 99999, this ) );

    editingFinishedSignal_->setMapping( unitPrice_, unitPrice_ );
    connect( unitPrice_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( unitPrice_, unitPrice_ );
    connect( unitPrice_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( unitPrice_, SIGNAL( focused(bool,QRect) ), SLOT( onUnitPriceFocused(bool) ) );
    connect( unitPrice_, SIGNAL( fractionalChanged(int) ), SLOT( onUnitPriceFractionalChanged(int) ) );
    connect( unitPrice_, SIGNAL( weightForChanged(int) ), SLOT( onUnitPriceWeightForChanged(int) ) );

    widgets_.push_back( unitPrice_ );

    // price
    price_ = new hobart::labeledNumericLineEdit( this, horizontalPixmapFlipper( angledField ) );
    price_->setBackgroundFocused( horizontalPixmapFlipper( angledFieldFocused ) );
    price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
    price_->setBackground( horizontalPixmapFlipper( angledFieldForced ), true );
    price_->setBackgroundFocused( horizontalPixmapFlipper( angledFieldForcedFocused ), true );
    price_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldForcedReadOnly ), true );
    price_->setDropShadow( horizontalPixmapFlipper( angledFieldShadow ) );
    price_->setContentsMargins( margins );
    price_->setObjectName( "price" );
    price_->setLabelObjectName( "price_label" );

    price_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    price_->setInputTruncation( true );
    price_->setInputWrapping( true );
    price_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    price_->setValidator( priceValidator_ = new QIntValidator( 0, 999999, this ) );

    editingFinishedSignal_->setMapping( price_, price_ );
    connect( price_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( price_, price_ );
    connect( price_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( price_, SIGNAL( focused(bool,QRect) ), SLOT( onPriceFocused(bool) ) );

    widgets_.push_back( price_ );

    // fixed weight
    fixedWeight_ = new hobart::labeledNumericLineEdit( this, angledField );
    fixedWeight_->setBackgroundFocused( angledFieldFocused );
    fixedWeight_->setBackgroundReadOnly( angledFieldReadOnly );
    fixedWeight_->setBackground( angledFieldForced, true );
    fixedWeight_->setBackgroundFocused( angledFieldForcedFocused, true );
    fixedWeight_->setBackgroundReadOnly( angledFieldForcedReadOnly, true );
    fixedWeight_->setDropShadow( angledFieldShadow );
    fixedWeight_->setContentsMargins( margins );
    fixedWeight_->setObjectName( "fixedWeight" );
    fixedWeight_->setLabelObjectName( "fixedWeight_label" );

    fixedWeight_->setInputTruncation( true );
    fixedWeight_->setInputWrapping( true );
    fixedWeight_->setValidator( fixedWeightValidator_ = new QIntValidator( 0, 9999, this ) );

    editingFinishedSignal_->setMapping( fixedWeight_, fixedWeight_ );
    connect( fixedWeight_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( fixedWeight_, fixedWeight_ );
    connect( fixedWeight_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( fixedWeight_, SIGNAL( focused(bool,QRect) ), SLOT( onFixedWeightFocused(bool) ) );

    widgets_.push_back( fixedWeight_ );

    // volume
    volume_ = new hobart::labeledNumericLineEdit( this, angledField );
    volume_->setBackgroundFocused( angledFieldFocused );
    volume_->setBackgroundReadOnly( angledFieldReadOnly );
    volume_->setBackground( angledFieldForced, true );
    volume_->setBackgroundFocused( angledFieldForcedFocused, true );
    volume_->setBackgroundReadOnly( angledFieldForcedReadOnly, true );
    volume_->setDropShadow( angledFieldShadow );
    volume_->setContentsMargins( margins );
    volume_->setObjectName( "volume" );
    volume_->setLabelObjectName( "volume_label" );

    volume_->setInputTruncation( true );
    volume_->setInputWrapping( true );
    volume_->setValidator( volumeValidator_ = new QIntValidator( 0, 9999, this ) );

    editingFinishedSignal_->setMapping( volume_, volume_ );
    connect( volume_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( volume_, volume_ );
    connect( volume_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( volume_, SIGNAL( focused(bool,QRect) ), SLOT( onVolumeFocused(bool) ) );

    widgets_.push_back( volume_ );

    // display weight
    displayWeight_ = new hobart::labeledPlainTextEdit( this, tabbedField );
    displayWeight_->setBackgroundFocused( tabbedFieldFocused  );
    displayWeight_->setBackgroundReadOnly( tabbedFieldReadOnly  );
    displayWeight_->setDropShadow( tabbedFieldShadow );
    displayWeight_->setContentsMargins( marginsLargeLeft );
    displayWeight_->setObjectName( "displayWeight" );
    displayWeight_->setLabelObjectName( "displayWeight_label" );

    widgets_.push_back( displayWeight_ );

    // by count
    byCount_ = new hobart::labeledNumericLineEdit( this, horizontalPixmapFlipper( tabbedField ) );
    byCount_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldFocused ) );
    byCount_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldReadOnly ) );
    byCount_->setBackground( horizontalPixmapFlipper( tabbedFieldForced ), true );
    byCount_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldForcedFocused ), true );
    byCount_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldForcedReadOnly ), true );
    byCount_->setDropShadow( horizontalPixmapFlipper( tabbedFieldShadow ) );
    byCount_->setContentsMargins( marginsLargeRight );
    byCount_->setObjectName( "byCount" );
    byCount_->setLabelObjectName( "byCount_label" );

    byCount_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    byCount_->setInputTruncation( true );
    byCount_->setInputWrapping( true );
    byCount_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    byCount_->setValidator( byCountValidator_ = new QIntValidator( 0, 99, this ) );

    // by count half field
    byCountHalfie_ = new hobart::labeledNumericLineEdit( this, horizontalPixmapFlipper( squareField ) );
    byCountHalfie_->setBackgroundFocused( horizontalPixmapFlipper( squareFieldFocused ) );
    byCountHalfie_->setBackgroundReadOnly( horizontalPixmapFlipper( squareFieldReadOnly ) );
    byCountHalfie_->setBackground( horizontalPixmapFlipper( squareFieldForced ), true );
    byCountHalfie_->setBackgroundFocused( horizontalPixmapFlipper( squareFieldForcedFocused ), true );
    byCountHalfie_->setBackgroundReadOnly( horizontalPixmapFlipper( squareFieldForcedReadOnly ), true );
    byCountHalfie_->setDropShadow( horizontalPixmapFlipper( squareFieldShadow ) );
    byCountHalfie_->setContentsMargins( marginsLargeRight );
    byCountHalfie_->setObjectName( "byCountHalfie" );
    byCountHalfie_->setLabelObjectName( "byCountHalfie_label" );

    byCountHalfie_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    byCountHalfie_->setInputTruncation( true );
    byCountHalfie_->setInputWrapping( true );
    byCountHalfie_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    byCountHalfie_->setValidator( byCountValidator_ = new QIntValidator( 0, 99, this ) );

    editingFinishedSignal_->setMapping( byCount_, byCount_ );
    connect( byCount_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( byCount_, byCount_ );
    connect( byCount_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( byCount_, SIGNAL( focused(bool,QRect) ), SLOT( onByCountFocused(bool) ) );

    widgets_.push_back( byCount_ );

    editingFinishedSignal_->setMapping( byCountHalfie_, byCountHalfie_ );
    connect( byCountHalfie_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( byCountHalfie_, byCountHalfie_ );
    connect( byCountHalfie_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    connect( byCountHalfie_, SIGNAL( focused(bool,QRect) ), SLOT( onByCountFocused(bool) ) );

    widgets_.push_back( byCountHalfie_ );

    // exception price
    exceptionPrice_ = new hobart::labeledNumericLineEdit( this, angledField );
    exceptionPrice_->setBackgroundFocused( angledFieldFocused );
    exceptionPrice_->setBackgroundReadOnly( angledFieldReadOnly );
    exceptionPrice_->setDropShadow( angledFieldShadow );
    exceptionPrice_->setContentsMargins( margins );
    exceptionPrice_->setObjectName( "exceptionPrice" );
    exceptionPrice_->setLabelObjectName( "exceptionPrice_label" );

    widgets_.push_back( exceptionPrice_ );

    // upc code
    upc_ = new hobart::labeledUpcEdit( this, angledField );
    upc_->setBackgroundFocused( angledFieldFocused );
    upc_->setBackgroundReadOnly( angledFieldReadOnly );
    upc_->setDropShadow( angledFieldShadow );
    upc_->setContentsMargins( margins );
    upc_->setObjectName( "upc" );
    upc_->setLabelObjectName( "upc_label" );

    connect( upc_, SIGNAL( prefixChanged(int) ), SLOT( onUpcPrefixChanged(int) ) );

    widgets_.push_back( upc_ );

    // discount unit price
    discountUnitPrice_ = new hobart::labeledNumericLineEdit( this, tabbedField );
    discountUnitPrice_->setBackgroundFocused( tabbedFieldFocused  );
    discountUnitPrice_->setBackgroundReadOnly( tabbedFieldReadOnly  );
    discountUnitPrice_->setDropShadow( tabbedFieldShadow );
    discountUnitPrice_->setContentsMargins( marginsLargeLeft );
    discountUnitPrice_->setObjectName( "discountUnitPrice" );
    discountUnitPrice_->setLabelObjectName( "discountUnitPrice_label" );

    discountUnitPrice_->setValidator( discountUnitPriceValidator_ = new QIntValidator( 0, 99999, this ) );

    editingFinishedSignal_->setMapping( discountUnitPrice_, discountUnitPrice_ );
    connect( discountUnitPrice_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( discountUnitPrice_, discountUnitPrice_ );
    connect( discountUnitPrice_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    widgets_.push_back( discountUnitPrice_ );

    // discount price
    discountPrice_ = new hobart::labeledNumericLineEdit( this, horizontalPixmapFlipper( tabbedField ) );
    discountPrice_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldFocused ) );
    discountPrice_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldReadOnly ) );
    discountPrice_->setDropShadow( horizontalPixmapFlipper( tabbedFieldShadow ) );
    discountPrice_->setContentsMargins( marginsLargeRight2 );
    discountPrice_->setObjectName( "discountPrice" );
    discountPrice_->setLabelObjectName( "discountPrice_label" );

    discountPrice_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    discountPrice_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    discountPrice_->setValidator( discountPriceValidator_ = new QIntValidator( 0, 999999, this ) );

    editingFinishedSignal_->setMapping( discountPrice_, discountPrice_ );
    connect( discountPrice_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( discountPrice_, discountPrice_ );
    connect( discountPrice_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    widgets_.push_back( discountPrice_ );

    // savings
    savings_ = new hobart::labeledNumericLineEdit( this, horizontalPixmapFlipper( angledField ) );
    savings_->setBackgroundFocused( horizontalPixmapFlipper( angledFieldFocused ) );
    savings_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
    savings_->setDropShadow( horizontalPixmapFlipper( angledFieldShadow ) );
    savings_->setContentsMargins( margins );
    savings_->setObjectName( "savings" );
    savings_->setLabelObjectName( "savings_label" );

    savings_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    savings_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    savings_->setValidator( savingsValidator_ = new QIntValidator( 0, 99999, this ) );

    editingFinishedSignal_->setMapping( savings_, savings_ );
    connect( savings_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    invalidAccessLevelSignal_->setMapping( savings_, savings_ );
    connect( savings_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    widgets_.push_back( savings_ );

    // weight empty
    weightEmpty_ = new hobart::labeledLineEdit( this, angledField );
    weightEmpty_->setBackgroundFocused( angledFieldFocused );
    weightEmpty_->setBackgroundReadOnly( angledFieldReadOnly );
    weightEmpty_->setDropShadow( angledFieldShadow );
    weightEmpty_->setContentsMargins( margins );
    weightEmpty_->setObjectName( "weightEmpty" );
    weightEmpty_->setLabelObjectName( "weightEmpty_label" );

    widgets_.push_back( weightEmpty_ );

    // tare empty
    tareEmpty_ = new hobart::labeledLineEdit( this, tabbedField );
    tareEmpty_->setBackgroundFocused( tabbedFieldFocused  );
    tareEmpty_->setBackgroundReadOnly( tabbedFieldReadOnly  );
    tareEmpty_->setDropShadow( tabbedFieldShadow );
    tareEmpty_->setContentsMargins( marginsLargeLeft );
    tareEmpty_->setObjectName( "tareEmpty" );
    tareEmpty_->setLabelObjectName( "tareEmpty_label" );

    widgets_.push_back( tareEmpty_ );

    // unit price empty
    unitPriceEmpty_ = new hobart::labeledLineEdit( this, horizontalPixmapFlipper( tabbedField ) );
    unitPriceEmpty_->setBackgroundFocused( horizontalPixmapFlipper( tabbedFieldFocused ) );
    unitPriceEmpty_->setBackgroundReadOnly( horizontalPixmapFlipper( tabbedFieldReadOnly ) );
    unitPriceEmpty_->setDropShadow( horizontalPixmapFlipper( tabbedFieldShadow ) );

    unitPriceEmpty_->setContentsMargins( marginsLargeRight );
    unitPriceEmpty_->setObjectName( "unitPriceEmpty" );
    unitPriceEmpty_->setLabelObjectName( "unitPriceEmpty_label" );

    unitPriceEmpty_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    unitPriceEmpty_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );

    widgets_.push_back( unitPriceEmpty_ );

    // price empty
    priceEmpty_ = new hobart::labeledLineEdit( this, horizontalPixmapFlipper( angledField ) );
    priceEmpty_->setBackgroundFocused( horizontalPixmapFlipper( angledFieldFocused ) );
    priceEmpty_->setBackgroundReadOnly( horizontalPixmapFlipper( angledFieldReadOnly ) );
    priceEmpty_->setDropShadow( horizontalPixmapFlipper( angledFieldShadow ) );
    priceEmpty_->setContentsMargins( margins );
    priceEmpty_->setObjectName( "priceEmpty" );
    priceEmpty_->setLabelObjectName( "priceEmpty_label" );

    priceEmpty_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    priceEmpty_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );

    widgets_.push_back( priceEmpty_ );

    // exception empty
    exceptionEmpty_ = new hobart::labeledLineEdit( this, angledField );
    exceptionEmpty_->setBackgroundFocused( angledFieldFocused );
    exceptionEmpty_->setBackgroundReadOnly( angledFieldReadOnly );
    exceptionEmpty_->setDropShadow( angledFieldShadow  );
    exceptionEmpty_->setContentsMargins( margins );
    exceptionEmpty_->setObjectName( "exceptionEmpty" );
    exceptionEmpty_->setLabelObjectName( "exceptionEmpty_label" );

    widgets_.push_back( exceptionEmpty_ );

    // bycount empty
    byCountUnitPrice_ = new hobart::labeledLineEdit( this, horizontalPixmapFlipper( byCountUPField ) );
    byCountUnitPrice_->setBackgroundReadOnly( horizontalPixmapFlipper( byCountUPFieldReadOnly ) );
    byCountUnitPrice_->setDropShadow( horizontalPixmapFlipper( byCountUPFieldShadow ) );

    byCountUnitPrice_->setContentsMargins( marginsBcUpTop );
    byCountUnitPrice_->setObjectName( "byCountUnitPrice" );
    byCountUnitPrice_->setLabelObjectName( "byCountUnitPrice_label" );
    byCountUnitPrice_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    byCountUnitPrice_->setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter  );

    widgets_.push_back( byCountUnitPrice_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::createLayout()
{
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    // position widgets
    const int offset( (BottomWeightBarArea == area() ? 0 : 5) );

    weight_->move( 0, offset );
    tare_->move( (width() / 2) - tare_->width(), offset );

    propTare_->move( ( (width() / 2) - propTare_->width() ) + 1, offset );

    unitPrice_->move( width() / 2, offset );
    price_->move( width() - price_->width(), offset );
    fixedWeight_->move( 0, offset );
    volume_->move( 0, offset );
    displayWeight_->move( (width() / 2) - displayWeight_->width(), offset );

    byCount_->move( width() / 2, offset );

    exceptionPrice_->move( 0, offset );

    weightEmpty_->move( 0, offset );
    tareEmpty_->move( (width() / 2) - tareEmpty_->width(), offset );
    unitPriceEmpty_->move( width() / 2, offset );
    priceEmpty_->move( width() - priceEmpty_->width(), offset );
    exceptionEmpty_->move( 0, offset );
    byCountUnitPrice_->move( width() / 2, offset );
    byCountHalfie_->move( ((width() / 2) + byCountUnitPrice_->width()) - 7, offset );

    upc_->move( 0, offset );
    discountUnitPrice_->move( (width() / 2) - discountUnitPrice_->width(), offset );
    discountPrice_->move( width() / 2, offset );
    savings_->move( width() - savings_->width(), offset );

    // set font
    foreach ( QWidget *w, widgets_ )
    {
        hobart::labeledLineEdit *l;
        hobart::labeledNumericLineEdit *n;
        hobart::labeledPlainTextEdit *p;
        hobart::labeledUpcEdit *u;

        if ( (l = dynamic_cast<hobart::labeledLineEdit*>( w )) )
        {
            setFontPointSize32( l );

            if ( BottomWeightBarArea == area() )
                l->flip();
        }
        else if ( (n = dynamic_cast<hobart::labeledNumericLineEdit*>( w )) )
        {
            setFontPointSize32( n );

            if ( BottomWeightBarArea == area() )
                n->flip();
        }
        else if ( (p = dynamic_cast<hobart::labeledPlainTextEdit*>( w )) )
        {
            setFontPointSize32( p );

            if ( BottomWeightBarArea == area() )
                p->flip();
        }
        else if ( (u = dynamic_cast<hobart::labeledUpcEdit*>( w )) )
        {
            setFontPointSize32( u );

            if ( BottomWeightBarArea == area() )
                u->flip();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::clearFields()
{
    clearingFields_ = true;

    // clear widgets
    foreach ( QWidget *w, widgets_ )
    {
        hobart::labeledLineEdit *l;
        hobart::labeledNumericLineEdit *n;
        hobart::labeledPlainTextEdit *p;
        hobart::labeledUpcEdit *u;

        if ( (l = dynamic_cast<hobart::labeledLineEdit*>( w )) )
        {
            l->clear();
            l->setForced( false );
            l->setReadOnly( true );
        }
        else if ( (n = dynamic_cast<hobart::labeledNumericLineEdit*>( w )) )
        {
            if (( weight_ != n ) && ( price_ != n ))	//	HA-167: The total price field will stay blank if we clear it here until a new weight occurs.
                n->clear();

            n->setForced( false );
            n->setReadOnly( true );
        }
        else if ( (p = dynamic_cast<hobart::labeledPlainTextEdit*>( w )) )
        {
            p->clear();
            p->setForced( false );
            p->setReadOnly( true );
        }
        else if ( (u = dynamic_cast<hobart::labeledUpcEdit*>( w )) )
        {
            u->clear();
            u->setForced( false );
            u->setReadOnly( true );
        }
    }

    clearingFields_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::clearProductTotals()
{
    price_->fromSessionField( "" );
    setWeightText( "", "" );

    // Change Background COLOR
    if( pick5PriceFieldEnabled_ )
    {
        // pixmaps
        const QSize angledFieldSize( scaleSizeForScreen( QSize( 270, 90 ) ) );

        const hobart::backgroundWidgetRenderer renderAngledFieldForced(
            new hobart::leftSlantedWeightBarBackgroundWidget(),
            new hobart::forcedOperatorBackgroundWidgetRendererAppearance<1>() );
        const hobart::backgroundWidgetRenderer renderAngledField(
            new hobart::leftSlantedWeightBarBackgroundWidget( ),
            new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

        const QPixmap angledFieldReadOnly( renderAngledField.drawControl( angledFieldSize, hobart::ReadOnly ) );
        const QPixmap angledField( renderAngledField.drawControl( angledFieldSize, hobart::Normal ) );

        flipPixmap pixmapFlipper( true, (BottomWeightBarArea == area()) );

        // change
        price_->setBackgroundReadOnly( pixmapFlipper( angledFieldReadOnly ), false );
        price_->setBackground( pixmapFlipper( angledField ) );
        pick5PriceFieldEnabled_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateProgrammableFields()
{
    DomCurrentPLU *product = currentProduct();

    // keyboard net weight
    isKeyboardNetWeightItem_ = product ? product->acceptKbdNetWeight() : false;

    // weight label text
    QString weightLabelText = product ? tr( "Net Weight" ) : tr( "Weight" );

    // FT-881: manual weight item
    if (( product ) && ( isKeyboardNetWeightItem_ ))
        weightLabelText = tr( "Manual Weight" );

    if ( convertUppercase_ )
        weightLabelText = weightLabelText.toUpper();

    weight_->setLabelText( weightLabelText );

    MadpScaleConfig *scaleConfig( &config_->scaleConfig );

    // hide widgets
    foreach ( QWidget *w, widgets_ )
        w->hide();

    if ( FreqShopperDisplayMode == displayMode() )
    {
        MadpOperatorDisplayFields *config = &session_->scaleConfig->operatorDisplayFields;

        if ( config->enableUpcNumber() )
            upc_->show();
        else
            weightEmpty_->show();

        discountUnitPrice_->show();
        discountPrice_->show();
        savings_->show();
    }
    else if ( NetWeightOnlyMode == displayMode() )
    {
        int productType = (product ? product->productType() : 0);

        // handle product type
        switch ( productType )
        {
        // random weight
        case 1:
            if ( isKeyboardNetWeightItem_ )
                weight_->show();
            else if ( scaleConfig->weigherAttached().toInt() )
                weight_->show();
            else
            {
                weightEmpty_->show();
                setWeightText( "", "" ); // if there isn't a weigher, then we can clear this
            }
            break;

        // by count
        case 3:
            if ( !scaleConfig->weigherAttached().toInt() )
            {
                weightEmpty_->show();
                setWeightText( "", "" ); // if there isn't a weigher, then we can clear this
            }
            else if (( scaleConfig->weightOnByCount() ) && ( product ) && ( 1 == product->byCount() ))
                weight_->show();
            else
                weightEmpty_->show();
            break;

        // fixed weight
        // volume
        case 2:
        case 4:
            weightEmpty_->show();
            break;

        // weight (no item)
        default:
            if ( scaleConfig->weigherAttached().toInt() )
                weight_->show();
            else
            {
                weightEmpty_->show();
                setWeightText( "", "" );		// if there isn't a weigher, then we can clear this
            }
            break;
        }

        tareEmpty_->show();
        unitPriceEmpty_->show();
        priceEmpty_->show();
    }
    else if ( WeightDisplayMode == displayMode() )
    {
        int productType = (product ? product->productType() : 0);
        double propTareVal = ( product ? product->proportionalTare() : "0.0" ).toDouble();

        int tmpFont = unitPrice_->font().pointSize();
        modifyFontPointSize setFontPointSize32( tmpFont );
        setFontPointSize32( tare_ );

        // handle product type
        switch ( productType )
        {
        // random weight
        case 1:
            if ( isKeyboardNetWeightItem_ )
            {
                weight_->show();
                tareEmpty_->show();
            }
            else if ( scaleConfig->weigherAttached().toInt() )
            {
                weight_->show();
                tare_->show();
                if( propTareVal > 0 )
                {
					// SOF-1886
					modifyFontPointSize setFontPointSize18( (window()->height() * 18) / 600 );
                    setFontPointSize18( tare_);
                    setFontPointSize18( propTare_);
                    propTare_->show();
                }
                else
                {
                    propTare_->hide();
                }
            }
            else
            {
                weightEmpty_->show();
                tareEmpty_->show();
                setWeightText( "", "" ); // if there isn't a weigher, then we can clear this
            }
            unitPrice_->show();
            price_->show();
            break;

        // fixed weight
        case 2:
            fixedWeight_->show();
            displayWeight_->show();
            byCount_->show();
            price_->show();
            break;

        // by count
        case 3:
        {
            byCount_->show();

            updateWeightTareFields();
            price_->show();
        }
            break;

        // volume
        case 4:
            volume_->show();
            displayWeight_->show();
            byCount_->show();
            price_->show();
            break;

        // weight (no item)
        default:
            if ( scaleConfig->weigherAttached().toInt() )
            {
                weight_->show();
                tare_->show();
                propTare_->hide();
            }
            else
            {
                weightEmpty_->show();
                tareEmpty_->show();
                setWeightText( "", "" );		// if there isn't a weigher, then we can clear this
            }
            unitPrice_->show();
            price_->show();
            break;
        }
    }

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::netWeightString, weight_->labelText() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateWeightOutOfTilt(bool hideIt)
{
    // This is only true for valuemax enabled scales.
    // If we are not a valuemax scale, exit
    // if we are +3 degrees out of tilt from level, hide the weight
    // otherwise display it

    if( hideIt )
    {
        weight_->hide();
        weightEmpty_->show();
    }
    else
    {
        weightEmpty_->hide();
        weight_->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateWeightTareFields()
{
    if ( WeightDisplayMode != displayMode() )
        return;

    DomCurrentPLU *product = currentProduct();
    int productType = (product ? product->productType() : 0);

    // by count
    if ( 3 != productType )
        return;

    MadpScaleConfig *scaleConfig( &config_->scaleConfig );

    // HA-122
    // Interesting scenario for this bug where we have a forced tare item... we would set focus to
    // tare due to forced tare... but then an update message would come in and call this function.
    // We would then hide tare then show it again... this behavior broke focus in the tare field.
    // So now we create a list of fields to show and hide and process them at the end in one big
    // batch update.

    QWidgetList widgetsToHide;
    QWidgetList widgetsToShow;

    // hide everything
    widgetsToHide.append( weight_ );
    widgetsToHide.append( tare_ );
    widgetsToHide.append( propTare_ );    // jhc proptare
    //widgetsToHide.append( propBlue_ );
    widgetsToHide.append( exceptionPrice_ );
    widgetsToHide.append( weightEmpty_ );
    widgetsToHide.append( tareEmpty_ );
    widgetsToHide.append( exceptionEmpty_ );

    QString productByCount( product->byCountRaw().trimmed() );

    // by count = (null) (i.e. forced and not filled in yet)
    if ( productByCount.isEmpty() )
    {
        widgetsToShow.append( weightEmpty_ );
        widgetsToShow.append( tareEmpty_ );
    }
    // by count = 1
    else if ( 1 == productByCount.toInt() )
    {
        if ( scaleConfig->weigherAttached().toInt() && scaleConfig->weightOnByCount() )
        {
            widgetsToShow.append( weight_ );
            widgetsToShow.append( tare_ );
        }
        else
        {
            widgetsToShow.append( weightEmpty_ );
            widgetsToShow.append( tareEmpty_ );

            setWeightText( "", "" );		// if there isn't a weigher, then we can clear this
        }
    }
    // by count > 1
    else
    {
        // SOF-2393 Exception price should not be displayed when the by count item is forced by count
        // This is because if the unit price is $1.00 and a forced value was entered, the total price
        // is going to be the bycount*unitprice no matter what the exception price for the item is
        if( product->forcedByCount() )
            widgetsToShow.append( exceptionEmpty_ );
        else
            widgetsToShow.append( exceptionPrice_ );

        widgetsToShow.append( tareEmpty_ );
    }

    // hide!
    foreach ( QWidget *w, widgetsToHide )
        if ( !widgetsToShow.contains( w ) )
            w->hide();

    // show!
    foreach ( QWidget *w, widgetsToShow )
        w->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateForcedFields()
{
    DomCurrentPLU *product( currentProduct() );
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    if ( !product )
        return;

    int productType = product->productType();

    // HT-136 Use correct tare
    QString productTare( (scaleConfig->useSecondTare() ? product->tareB().trimmed() : product->tare().trimmed() ) );
    QString productByCount( product->byCountRaw().trimmed() );
    QString productUnitPrice( product->unitPrice().trimmed() );

    tare_->setForced( product->forcedTare() && productTare.isEmpty() );
    byCount_->setForced( product->forcedByCount() && productByCount.isEmpty() );

    if( byCountHalfie_->isVisible() )
        byCountHalfie_->setForced( product->forcedByCount() && productByCount.isEmpty() );

    // for non-random weight items the total price is unit price
    if (( productType ) && ( 1 != productType ))
        price_->setForced( product->forcedUnitPrice() && productUnitPrice.isEmpty() );
    else
        unitPrice_->setForced( product->forcedUnitPrice() && productUnitPrice.isEmpty() );

    // refresh field permissions
    updateFieldPermissions();

    // process forced fields
    if ( operateMode_->isForcedFields() )
        operateMode_->processForcedFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void WeightBarWidget::setPermissions( T *field, bool canModify, int accessLevel )
{
    LOG_CLASS_INFO( "WeightBarWidget", "setPermissions" );

    LOG_DEBUG( "%s %d %d (%d)",
        qPrintable( field->objectName() ),
        canModify,
        accessLevel,
        field->isForced() );

    // allow access to forced field always
    if ( field->isForced() )
    {
        canModify = true;
        accessLevel = 1;
    }

    // set permissions
    field->setAccessLevel( accessLevel );
    field->setReadOnly( !canModify );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateFieldPermissions()
{
    DomCurrentPLU *product( currentProduct() );

    // no product
    if ( !product )
        return;

    ScaleConfiguration *config( session_->scaleConfig );
    MadpOperatorCanModify *operatorCanModify( &config->operatorCanModify );
    DomWeighModeTotalType *weighModeTotalType( &config->weighModeTotalType );

    int productType( product->productType() );
    int curIntPrefix( product->numberSystem().toInt() );
    bool allowFieldAccess( false );             // ofiginally this was true but I changed it to false.  WHY?

    // FT-1463
    //
    // for items of type:
    // fixed weight
    // fluid ounces
    //
    // we must block fields when upc type is not 2 or 4
    //

    if ( ( ProductType::PRODUCTTYPE_RANDOMWEIGHT == productType ) || ( ProductType::PRODUCTTYPE_BYCOUNT == productType ) )
    {
        // For Random Weight and By Counts, their number system is ALWAYS 2 or 4, which means the fields are editable always
        allowFieldAccess = true;
    }
    else if ( ( ProductType::PRODUCTTYPE_FIXEDWEIGHT == productType ) || ( ProductType::PRODUCTTYPE_FLUIDOUNCE == productType ) )
    {
        // UPCA barcode type
        // UPCD3 barcode type
        // UPCE barcode type
        if (( 0 <= product->barcodeType() ) && ( product->barcodeType() <= 2 ))
        {
            if (( 2 == curIntPrefix ) || ( 4 == curIntPrefix ))
                allowFieldAccess = true;
        }

        else if (( product->gs1DatabarGTIN().contains( "P" ) ) ||
            ( product->gs1DatabarAI().contains( "P" ) ) ||
            ( product->eanFormatString().contains( "P" ) ) ||
            ( product->ean128CFormatString().contains( "P" ) ) ||
            ( product->qrCodeFormatString().contains( "P" ) ) ||
            ( product->dataMatrixFormatString().contains( "P" ) ))
        {
            allowFieldAccess = true;
        }

        // TODO - bacrodes: Need to talk it over with Larry and System Test about what these rules should be.  Right now
        // for a given label type and if it doesnt have a P in ANY format string, then we do not allow access to
        // weight, tare, unit price, bycount, bycount halfie, volume, fixed_weight, and when productType IS NOT RW, Price

        // labels no longer mutually exclusive so this doesnt work.  Almost need to test each circumstance
        //switch ( product->barcodeType() )
        //{
            //case 0:  // UPCA barcode type
            //case 1:  // UPCD3 barcode type
            //case 2:  // UPCE barcode type
            //    if ( ( 2 != curIntPrefix ) && ( 4 != curIntPrefix ) )
            //        allowFieldAccess = false;                // IF RW AND prefix not 2 or 4 - cant really happen but ok - well it can in COMM if App doesn't throw it out, which Im going to assume it will
            //    break;
            //case 3:  // EAN barcode type
            //    // check if format string contains any 'price digits'
            //    if ( !product->eanFormatString().contains("P") )
            //        allowFieldAccess = false;
            //    break;
            //case 4:  // EAN128 barcode type
            //    // check if format string contains any 'price digits'
            //    if ( !product->ean128CFormatString().contains("P") )
            //        allowFieldAccess = false;
            //    break;
            //case 5:  // RSS14E barcode type (GS1)
            //    // check if format string contains any 'price digits'
            //    if ( !product->gs1DatabarAI().contains("P") )
            //        allowFieldAccess = false;
            //    break;
            //case 6:  // RSS14E_GTIN_13 barcode type (GS1)
            //case 7:  // RSS14E_GTIN_12 barcode type (GS1)
            //case 8:  // RSS14E_GTIN_8 barcode type (GS1)
            //    // check if format string contains any 'price digits'
            //    if ( !product->gs1DatabarGTIN().contains("P") )
            //        allowFieldAccess = false;
            //    break;
            //default:
            //    break;
        //}
    }

    // WEH-14
    // Fields are locked when meal builder mode
    if ( OpModeMealBuilder == weighModeTotalType->weighMode() )
        allowFieldAccess = false;

    // weight
    setPermissions( weight_, allowFieldAccess && isKeyboardNetWeightItem_, 1 );

    // tare
    setPermissions( tare_, allowFieldAccess && operatorCanModify->tare(), operatorCanModify->tareAccessLevel() );

    // unit price
    setPermissions( unitPrice_, allowFieldAccess && operatorCanModify->price(), operatorCanModify->priceAccessLevel() );

    // price
    // for non-random weight items the total price is unit price
    if (( productType ) && ( 1 != productType ))
        setPermissions( price_, allowFieldAccess && operatorCanModify->price(), operatorCanModify->priceAccessLevel() );

    // fixed weight
    setPermissions( fixedWeight_, allowFieldAccess && operatorCanModify->fixedWeight(), operatorCanModify->fixedWeightAccessLevel() );

    // volume
    setPermissions( volume_, allowFieldAccess && operatorCanModify->fixedWeight(), operatorCanModify->fixedWeightAccessLevel() );

    // by count
    setPermissions( byCount_,       allowFieldAccess && operatorCanModify->byCount(), operatorCanModify->byCountAccessLevel() );
    setPermissions( byCountHalfie_, allowFieldAccess && operatorCanModify->byCount(), operatorCanModify->byCountAccessLevel() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *WeightBarWidget::currentProduct() const
{
    DomCurrentPLU *product = (operateMode_ ? operateMode_->currentProduct() : NULL);

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MadpUpdateCurrentPLUTotals *WeightBarWidget::currentProductTotals() const
{
    if ( !currentProduct() )
        return NULL;

    return (operateMode_ ? operateMode_->currentProductTotals() : NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateCurrencySettings()
{
    MadpCurrencySettings *currencySettings( &config_->currencySettings );

    int currencyPrecision = currencySettings->primaryPrecision();
    QString currencySeparator( currencySettings->unitSeparator() );
    if( currencySeparator.length() > 0 )
        curSeparator_ = currencySeparator;      // bc default is . and dont want to blow it away if blank

    QString currencyUnit( currencySettings->priCurrSymbol() );

    int unitPriceDigits = currencySettings->unitPriceDigits();
    int priceDigits = currencySettings->totalPriceDigits();

    // ---- //

    // update seperator for all fields!
    foreach ( QWidget *w, widgets_ )
    {
        hobart::labeledNumericLineEdit *n;

        if ( (n = dynamic_cast<hobart::labeledNumericLineEdit*>( w )) )
            n->setDecimal( currencySeparator );
    }

    // unit price
    unitPriceValidator_->setTop( makeValidatorTop( unitPriceDigits ) );
    unitPrice_->setDecimalPlaces( currencyPrecision );
    unitPrice_->setPrefix( currencyUnit );

    // price
    priceValidator_->setTop( makeValidatorTop( priceDigits ) );
    price_->setDecimalPlaces( currencyPrecision );
    price_->setPrefix( currencyUnit );

    // exception price
    exceptionPrice_->setDecimalPlaces( currencyPrecision );
    exceptionPrice_->setPrefix( currencyUnit );

    // discount unit price
    discountUnitPriceValidator_->setTop( makeValidatorTop( unitPriceDigits ) );
    discountUnitPrice_->setDecimalPlaces( currencyPrecision );
    discountUnitPrice_->setPrefix( currencyUnit );

    // discount price
    discountPriceValidator_->setTop( makeValidatorTop( priceDigits ) );
    discountPrice_->setDecimalPlaces( currencyPrecision );
    discountPrice_->setPrefix( currencyUnit );

    // savings
    savingsValidator_->setTop( makeValidatorTop( priceDigits ) );
    savings_->setDecimalPlaces( currencyPrecision );
    savings_->setPrefix( currencyUnit );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateScaleConfig()
{
    LOG_CLASS_INFO( "WeightBarWidget", "updateScaleConfig" );

    MadpScaleConfig *scaleConfig( &config_->scaleConfig );

    QString scaleUnits( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    QString fixedWeightUnits( scaleConfig->isScaleMetric() ? " g" : " oz" );
    QString volumeUnits( " fl oz" );

    int tarePrecision = scaleConfig->tarePrecision();

    // ensure that ( 2 <= precision <= 3 )
    if ( 2 > tarePrecision )
        tarePrecision = 2;
    else if ( 3 < tarePrecision )
        tarePrecision = 3;

    // ---- //

    // weight
    weight_->setSuffix( scaleUnits );

    // tare
    tare_->setDecimalPlaces( tarePrecision );
    tare_->setSuffix( scaleUnits );
    tare_->setInputPrecision( tarePrecision );

    // jhc proptare
    // proportional tare - eventually remove
    propTare_->setDecimalPlaces( tarePrecision );
    propTare_->setSuffix( scaleUnits );
    propTare_->setInputPrecision( tarePrecision );

    LOG_DEBUG( "maximumTare_ = %s", qPrintable(maximumTare_) );
    if ( maximumTare_.length() )
    {
        // here we are using the maximum tare from normal scale operations
        QString tareMaximumWt( maximumTare_ );

		//	HA-73:  We have issues with the service counter scale always relying on a precision of 3 digits, but for Prepack,
		//			it can be 2 digits and that causes problems.
		int maxLength = tareMaximumWt.length();
        int decimalPos = tareMaximumWt.indexOf('.');
        if (decimalPos == 0)
        {
            tareMaximumWt.append(".000");
        }
        else
        {
             while ((maxLength - decimalPos) < 4)
             {
                 tareMaximumWt.append("0");
                 maxLength = tareMaximumWt.length();
             }
        }
		//	HA-73:  Now that we have 3 decimal places to the right, we can take out the decimal point
        tareMaximumWt.replace( ".", "" );

        if ( 3 == tarePrecision )
            tareValidator_->setTop( tareMaximumWt.toInt() );
        else if ( 2 == tarePrecision )
            tareValidator_->setTop( tareMaximumWt.toInt() / 10 );
    }
    else
    {
        // here we are default maximum tare
        int top = (scaleConfig->isScaleMetric() ? 15000 : 30000);

        if ( 3 == tarePrecision )
            tareValidator_->setTop( top );
        else if ( 2 == tarePrecision )
            tareValidator_->setTop( top / 10 );
    }

    // SoftwareSOF-2017 Unit price is displaying in price / LB when scale is in metric mode
    // The following code was lost fixing another issue, I'm guessing lost in the merge
    // I'm not really sure.  Can't think of a reason why this code was removed at all.

    //
    // fixed weight
   fixedWeight_->setSuffix( fixedWeightUnits );

   if ( scaleConfig->isScaleMetric() )
       fixedWeight_->setDecimalPlaces( 0 );
   else if ( 0 == scaleConfig->fixedWtUnits() ) // whole ounces
       fixedWeight_->setDecimalPlaces( 0 );
   else if ( 1 == scaleConfig->fixedWtUnits() ) // tenths of an ounce
       fixedWeight_->setDecimalPlaces( 1 );

   // volume
   volume_->setSuffix( volumeUnits );

   // unit price
   unitPrice_->setMetric( scaleConfig->isScaleMetric() );
   unitPrice_->setFractionalEnabled( scaleConfig->fractionalPricingOn() );
   unitPrice_->setWeightForEnabled( scaleConfig->poundsForPricingOn() );

   // update hooks
   sessionHooks::instance()->updateFieldValue( sessionHooks::metricScale, scaleConfig->isScaleMetric() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateTareDigits()
{
    // compute number of decimals
    MadpCurrencySettings *currencySettings( &config_->currencySettings );
    QString currencySeparator( currencySettings->unitSeparator() );

    int numDecs( 0 );

    QString netWt = weight_->text();
    if( netWt.contains( " " ) )
    {
        // need to get rid of the units lb/kg - everything past a space
        netWt = netWt.mid(0, netWt.indexOf(" "));
    }

    if( netWt.length() == 0 )
        return;

    QStringList tp( netWt.split( currencySeparator ) );

    if ( 2 == tp.count() )
        numDecs = tp[1].length();
    else
        return;

    // ensure that ( 2 <= precision <= 3 )
    int tarePrecision( numDecs );
    if ( 2 > tarePrecision )
        tarePrecision = 2;
    else if ( 3 < tarePrecision )
        tarePrecision = 3;

    // tare
    MadpScaleConfig *scaleConfig( &config_->scaleConfig );
    QString scaleUnits( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    tare_->setDecimalPlaces( tarePrecision );
    tare_->setSuffix( scaleUnits );
    tare_->setInputPrecision( tarePrecision );

    if ( maximumTare_.length() )
    {
        // here we are using the maximum tare from normal scale operations

        QString tareMaximumWt( maximumTare_ );

        //	HA-73:  We have issues with the service counter scale always relying on a precision of 3 digits, but for Prepack,
        //			it can be 2 digits. or if over 12lbs and single range, and that causes problems.
        int maxLength = tareMaximumWt.length();
        int decimalPos = tareMaximumWt.indexOf('.');

        if (decimalPos == 0)
        {
            tareMaximumWt.append(".000");
        }
        else
        {
             while ((maxLength - decimalPos) < 4)
             {
                 tareMaximumWt.append("0");
                 maxLength = tareMaximumWt.length();
             }
        }
        //	HA-73:  Now that we have 2 or 3 decimal places to the right, we can take out the decimal point
        tareMaximumWt.replace( ".", "" );

        if ( 3 == tarePrecision )
            tareValidator_->setTop( tareMaximumWt.toInt() );
        else if ( 2 == tarePrecision )
            tareValidator_->setTop( tareMaximumWt.toInt() / 10 );
    }
    else
    {
        // here we are default maximum tare
        int top = (scaleConfig->isScaleMetric() ? 15000 : 30000);

        if ( 3 == tarePrecision )
            tareValidator_->setTop( top );
        else if ( 2 == tarePrecision )
            tareValidator_->setTop( top / 10 );
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QString WeightBarWidget::updateUpcNumberField()
{
    // This is called IF (1) Trans Type Changes (2) Single Field Update (3) New PLU

    DomCurrentPLU *product( currentProduct() );
    QString result = "";

    //QString upcText = tr( "Barcode Number" );

    if ( product )
    {
        // REPLACE THIS
        // UseNoFields(x0),UseProductNumber(x01),UseVendorNumber(x02),UseProductAndVendorNumber(implied=3),UseEanCode(x04)
        // int barcodeType = product->barcodeType();
        //if ( 1 == barcodeFields )
        //    result = product->productNumber();
        //else if ( 2 == barcodeFields )
        //    result = product->vendorNumber();
        //else if ( 3 == barcodeFields )
        //    result = product->vendorNumber() + product->productNumber();
        //else if ( 4 == barcodeFields )
        //    result = product->eanCode();

        // TODO - TALK THIS OVER WITH LARRY/RANDY
        // WITH THIS - currently we dont display anything NOT ean or UPC.
        // The NEW CODE will display it.  If we have it, we display it, if possible

        QString bc_ean  = product->eanCode();
        QString bc_prod = product->productNumber();
        QString bc_vend = product->vendorNumber();

        // WHAT result should I display IF I have BOTH UPC fields and EAN fields? Does one have precedence? Does now.
        if( ( bc_vend.length() > 0 ) && ( bc_prod.length() > 0 ) )
            result = bc_vend + bc_prod;     // 3 == vendor# + product# displayed
        else if( bc_prod.length() > 0 )
            result = bc_prod;               // 1 == product# displayed
        else if( bc_vend.length() > 0 )
            result = bc_vend;               // 2 == vendor# displayed
        else if( bc_ean.length() > 0 )
            result = bc_ean;                // 4 == ean# displayed

        // UPCA Barcode
        // NEW ISSUES: IF the EAN has N and V in its format string, the code will go into the IF, but for EAN we dont
        //             want to test if we have manual UPC checkdigit turned on.  Will a customer have UPC labels with
        //             manual checkdigit turned on, and co-existing WITH EAN labels too?  Unlikely but.....
        //             This could also happen if UPC label > 99.99 so pushed to EAN label type, and then manualcheckdigit enabled
        //             TODO - jhc barcodes - must settle this out with Larry if the barcodeType is available, or NOT.
        // NEW RULE PROPOSAL: If you want checkdigit, you cant have data in your EAN number field
        if ( ( bc_ean.length() > 0 ) && ( bc_ean.toInt() > 0))
        {
            //upc_->setLabelText( upcText );

            // ORIGINAL TEST - Dont have this field to test for sure for sure
            // if ( 3 == barcodeFields )          - IF we have vendor#+product# in EAN format string
            if( ( bc_prod.length() > 0 ) || ( bc_vend.length() > 0 ) )
            {
                // which comes first, vendor or product number?
                // lets check the format string
                QString formatString( product->eanFormatString() );

                // only problem with this though, it only displays EAN13, not EAN128
                int vendorAt = formatString.indexOf( "V" );
                int productAt = formatString.indexOf( "N" );

                if ( vendorAt < productAt )
                    result = product->vendorNumber() + " " + product->productNumber();
                //else          // already have this from up above as default
                //    result = product->productNumber() + " " + product->vendorNumber();
            }
        }
        else if( ( ( bc_prod.length() > 0 ) || ( bc_vend.length() > 0 ) ) && ( ( bc_ean.length() == 0 ) || ( 0 == bc_ean.toInt() ) ) )
        //if ( 0 == barcodeType )           // ORIGINAL TEST - but dont really know IF I have this field available to me
        {
            DomWeighModeTotalType *weighModeTotalType = &session_->scaleConfig->weighModeTotalType;

            bool manualUPCDigitChange = weighModeTotalType->manualUPCDigitChange();

            // set up the values
            if (( manualUPCDigitChange ) && ( 1 < result.length() ))
            {
                int digit = result.left( 1 ).toInt();

                ignoreSignals_ = true;

                upc_->setPrefixVisible( true );
                upc_->setPrefix( digit );
                //upc_->setLabelText( upcText );

                ignoreSignals_ = false;

                result = result.right( result.length() - 1 );

                if ( result.length() > 6 )
                {
                    //	HA-100: When the Combo box is up for the UPC digit, we need to make the rest of the UPC number smaller so it will fit.
                    modifyFontPointSize setFontPointSize24( scaleFontPointSizeForScreen( 24 ) );
                    setFontPointSize24( upc_ );
                }
                else
                {
                    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );
                    setFontPointSize32( upc_ );
                }
            }
        }

        //	EAN barcode
        //
        // TODO - jhc barcode: need to see if I really have these fields available to me from Larry and if I dont what do I do?  Lots of DA logic if not.
        // ORIGINAL TEST = IF EAN barcodeType - but I cant really dont know I have this field to test
        // ORIGINAL TEST else if ( 3 == barcodeType )
        // MOVED UP TOP
        //else if ( ( bc_ean.length() > 0 ) && ( 0 == bc_prod.length() ) && ( 0 == bc_vend.length() ) )
        //{
        //    upc_->setLabelText( eanText );
        //
        //    // ORIGINAL TEST - Dont have this field to test for sure for sure
        //    // if ( 3 == barcodeFields )          - IF we have vendor#+product# in EAN format string
        //    if( ( bc_prod.length() > 0 ) || ( bc_vend.length() > 0 ) )
        //    {
        //        // which comes first, vendor or product number?
        //        // lets check the format string
        //        QString formatString( product->eanFormatString() );
        //
        //        // only problem with this though, it only displays EAN13, not EAN128
        //        int vendorAt = formatString.indexOf( "V" );
        //        int productAt = formatString.indexOf( "N" );
        //
        //         if ( vendorAt < productAt )
        //            result = product->vendorNumber() + " " + product->productNumber();
        //        //else          // already have this from up above as default
        //        //    result = product->productNumber() + " " + product->vendorNumber();
        //    }

            // ALL this was done up above
            //else if ( 4 == barcodeFields )
            //    result = product->eanCode();
            //else if ( 2 == barcodeFields )
            //    result = product->vendorNumber();
            //else if ( 1 == barcodeFields )
            //    result = product->productNumber();
            //else
            //    result = "";
        //}
        //else
        //{
        //    // not sure what else I need to add here. Some other value that falls through to the UPC field
        //    upc_->setLabelText( upcText );
        //}
    }

    if( result.length() > 10 )
    {
        // SOF-3339
        modifyFontPointSize setFontPointSize20( scaleFontPointSizeForScreen( 20 ) );
        setFontPointSize20( upc_ );
    }
    else if ( result.length() > 6 )
    {
        //	HA-100: When the Combo box is up for the UPC digit, we need to make the rest of the UPC number smaller so it will fit.
        modifyFontPointSize setFontPointSize24( scaleFontPointSizeForScreen( 24 ) );
        setFontPointSize24( upc_ );
    }
    else
    {
        modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );
        setFontPointSize32( upc_ );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateDisplayWeightField( const QString& newValue )
{
    QString value( newValue );

    modifyFontPointSize setFontPointSize18( scaleFontPointSizeForScreen( 18 ) );
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    int indexOfSeperator = value.indexOf( "!" );

    // strip unit value
    if ( 0 < indexOfSeperator )
        value = value.right( value.length() - (indexOfSeperator + 1) );

    // FT-477 adjust font size for text that is too large
    if (( 10 < value.length() ) && ( !value.contains( "\n" ) ))
        value.prepend( "\n" );

    // check for really large text that wont fit and adjust font size
    if ( value.contains( "\n" ) )
        setFontPointSize18( displayWeight_ );
    else
        setFontPointSize32( displayWeight_ );

    displayWeight_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FT-472 (stolen from updateDisplayWeightField)
void WeightBarWidget::updateDisplayUnitPriceField( const QString& newValue )
{
    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );
    DomPrintSpecialFields *config = &session_->scaleConfig->printSpecialFields;
    DomCurrentPLU *product( currentProduct() );

    QString tmpPr = productTotals->unitPrice();

    QString value( newValue );
    MadpScaleConfig *scaleConfig( &config_->scaleConfig );

    modifyFontPointSize setFontPointSize18( scaleFontPointSizeForScreen( 18 ) );
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    int indexOfSeparator = value.indexOf( "!" );

    // strip unit value
    if ( 0 < indexOfSeparator )
        value = value.right( value.length() - (indexOfSeparator + 1) );

    // FT-472 adjust font size for text that is too large
    if (( 6 < value.length() ) && ( !value.contains( "\n" ) ))
        value.prepend( "\n" );

    // check for really large text that wont fit and adjust font size
    // SOF-1887
    if ( value.contains( "\n" ) || scaleConfig->fractionalPricingOn() || scaleConfig->poundsForPricingOn() )
        setFontPointSize18( unitPrice_ );
    else
        setFontPointSize32( unitPrice_ );

    // FT-840 Comma separator not showing on the Unit Price on Operator Display
    // To get a comma on any field, just call fromSessionField() instead of setText()
    //unitPrice_->setText( value );
    unitPrice_->fromSessionField( value );

    // update hooks
    if( ( config->unitPriceFormat().toInt() > 1 ) && ( 3 == product->productType() ) )
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, tmpPr );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemUnitPrice, unitPrice_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::setScalePrecision( bool lowRange )
{
    Q_UNUSED( lowRange );

    // Note:
    // The bottom validator range here should match the tare top validator range, but since the
    // operator will never touch this field why bother...

    // Note:
    // The weight field is display only, no operator entry. We don't need to set any validators.
    // Commenting the code out for now...

    //weightValidator_->setBottom( lowRange ? -dualRangeWeightTop_ : -weightTop_ );
    //weightValidator_->setTop( lowRange ? dualRangeWeightTop_ : weightTop_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::hideOperatorMessage()
{
    PluMainScreen *screen = dynamic_cast<PluMainScreen*>( operateMode_->screen() );

    if ( screen )
        screen->hideOperatorMessage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::showOperatorMessage( int type, const QString& message )
{
    PluMainScreen *screen = dynamic_cast<PluMainScreen*>( operateMode_->screen() );

    if ( screen )
        screen->showOperatorMessage( type, message );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeightBarWidget::updateFreshProductionPlanningWeight(const QString& weight)
{
    PluMainScreen *screen = dynamic_cast<PluMainScreen*>( operateMode_->screen() );

    if ( screen )
        screen->updateProductionPlanningMessage(weight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WeightBarWidget::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    return top;
}
