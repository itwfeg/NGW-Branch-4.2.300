/**
 * @file labelplacement.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labelplacement.h"
#include "plumainscreen.h"
#include "superChangeProductAllFields.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledpluedit.h"
#include "../widgets/labelplacementwidget.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QComboBox>
#include <QDial>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QListView>
#include <QCheckBox>
#include <QMessageBox>

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelPlacementScreen::LabelPlacementScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ignoreSignals_( false ),
    registered_( false ),
    labelPlacementFromProduct_( false ),
    direction_( leftToRight ),
    printerCount_( 0 ),
    prevPeripheralList_( new MadpPeripheralList() ),
    previewPackageClicked_( false ),
    primaryLabelSize_( 80, 125 ), // these are fixed size for now
    secondaryLabelSize_( 80, 80 ),
    precedence_( 0 ),
    trayName_( "" ),
    wrapBoxId_("0"),
    heightBoxIndex_("0"),
    wrapBoxIndex_("0")
{
    setObjectName( "LabelPlacementScreen" );

    evolWrapperConfig_ = &session_->scaleConfig->evolWrapperConfig;
    evolWrapperSystemInformation_ = &session_->scaleConfig->evolWrapperSystemInformation;
    peripheralList_ = &session_->scaleConfig->peripheralList;
    scaleConfig_ = &session_->scaleConfig->scaleConfig;

    // reserve 3 printers even though we really only use up to 2...
    printers_.resize( 3 );

    for ( int i( 0 ); i < printers_.size(); ++i )
    {
        printers_[i].installed = false;
        printers_[i].automaticRotation = false;
        printers_[i].orientation = 0;
        printers_[i].placement = 0;
        printers_[i].rotation = 0;
        printers_[i].labelStockUpsideDown = false;

        // SOF-4681 Add two new timings to control vertical label placement
        // Commented out for now
        //printers_[i].verticalOffset = 5;
    }

    // connect slots
    connect( operateMode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
    connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );
    connect( operateMode_, SIGNAL( currentProductUpdateField(DomUpdateRunModeField) ), SLOT( onCurrentProductUpdateField(DomUpdateRunModeField) ) );

    connect( mainWindow_, SIGNAL( numberPadMoved(Qt::DockWidgetArea) ), SLOT( onNumberPadMoved(Qt::DockWidgetArea) ) );

    // initialize
    initialize();
    createLayout( Qt::RightDockWidgetArea );

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelPlacementScreen::~LabelPlacementScreen()
{
    if ( prevPeripheralList_ )
    {
        delete prevPeripheralList_;
        prevPeripheralList_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::translate()
{
    static const bool convertUpperCase( true );

    QString printerText( tr( "Printer" ) );
    QString labelPlacementText( tr( "Label Position" ) );
    QString labelVerticalAdjText( tr( "Vertical Adjustment" ) );
    QString labelRotationText( tr( "Label Rotation" ) );
    QString rotationSyncText( tr( "Sync Rotation" ) );

    if ( convertUpperCase )
    {
        QList<QString*> strings;

        strings.append( &printerText );
        strings.append( &labelPlacementText );
        // SOF-4681 Add two new timings to control vertical label placement
        // Commented out for now
        //strings.append( &labelVerticalAdjText );
        strings.append( &labelRotationText );
        strings.append( &rotationSyncText );

        foreach ( QString *s, strings )
            (*s) = s->toUpper();
    }

    printerLabel_->setText( printerText );

    printer_->setItemText( 0, tr( "Primary" ) );
    printer_->setItemText( 1, tr( "Secondary" ) );

    labelPlacementLabel_->setText( labelPlacementText );

    ignoreSignals_ = true;
    labelPlacement_->clear();
    labelPlacement_->addItem( tr( "Upper Right" ), 0 );                 // Upper Right      1 of 2 primary options - valid for secondary
    labelPlacement_->addItem( tr( "Upper Left" ), 1 );                  // Upper Left       2 of 2 primary options - valid for secondary
    if( printer_->currentIndex() == second )
    {
        labelPlacement_->addItem( "", 2 );                              // None             dont display - just a blank spot
        labelPlacement_->addItem( tr( "Lower Right" ), 3 );             // Lower Right      valid for secondary - not a primary option
        labelPlacement_->addItem( tr( "Lower Left" ), 4  );             // Lower Left       valid for secondary - not a primary option
        labelPlacement_->addItem( tr( "Right of Primary" ), 5 );        // Right of Primary valid for secondary AT TOP LOCs - not a primary option
        labelPlacement_->addItem( tr( "Left of Primary" ), 6 );          // Left of Primary  valid for secondary AT TOP LOCs - not a primary option
        labelPlacement_->addItem( tr( "Above Primary" ), 7 );          // Above Primary    no longer a valid option since we cant put Primary at the bottom
        labelPlacement_->addItem( tr( "Below Primary" ), 8 );          // Below Primary
    }
    ignoreSignals_ = false;
    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    //labelVerticalAdjLabel_->setText( labelVerticalAdjText );
    // not translating numbers here

    labelRotationLabel_->setText( labelRotationText );

    syncRotation_->setText( rotationSyncText );

    // leverMessage_->setText( tr( "Set rotation lever to\nposition indicated above." ) );

    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief LabelPlacementScreen::calculateLabelPlacement
/// \param printer
/// \return Location of the label specified by the user. The primary will be in a corner while the
/// secondary label will either be a corner or a position relative to the primary
corner LabelPlacementScreen::calculateLabelPlacement( int printer ) const
{
    if (( 0 <= printer ) && ( printer < printerCount_ ))
    {
        if( printerCount_ <  2 )
        {
            // AWS with two printers
            // label placement is fixed
            if (( isAWS() ) && ( 2 == printerCount_ ))
            {
                // primary printer is always right hand side
                return ((0 == printer) ? primaryLabelPos_ : secondaryLabelPos_);
            }

            // evolution label applier
            // label placement is set directly
            else if ( labelPlacementFromProduct_ )
            {
                // test for placement in upper right corner
                if ( rightSide == printers_[printer].placement )
                    return (isCLA_EPSAC2() ? lowerRight : upperRight);

                return (isCLA_EPSAC2() ? lowerLeft : upperLeft);
            }

            // non-evolution applier
            // we must decipher leading edge versus trailing edge and belt direction
            else
            {
                // test for placement in upper right corner
                if ( (leadingEdge == printers_[printer].placement) ^ (rightToLeft == direction_) )
                    return (isCLA_EPSAC2() ? lowerRight : upperRight);

                return (isCLA_EPSAC2() ? lowerLeft : upperLeft);
            }
        }
        else
        {
            return ((0 == printer) ? primaryLabelPos_ : secondaryLabelPos_);
        }
    }

    return none;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *LabelPlacementScreen::swipeScreenRight() const
{
    // check what screen is the right screen...
    // 0 = PluInfoScreen
    // 1 = WrapperSettingsScreen
    // 2 = LabelPlacementScreen
    int rightScreen( session_->scaleConfig->scaleConfig.rightScreenType() );

    if (( session_->isPrepack() ) && ( 2 != rightScreen ))
        return NULL;

    ScreenWidget *screen = mode_->createScreen<PluMainScreen>();

    return screen;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelPlacementScreen::eventFilter( QObject *obj, QEvent *e )
{
    // check for click in preview area
    if (( preview_ == obj ) && ( QEvent::MouseButtonRelease == e->type() ))
    {
        QMouseEvent *mouseEvent;

        bool nothingClicked = false;
        if ( (mouseEvent = dynamic_cast<QMouseEvent*>( e )) )
        {
            corner loc( none );
            if( printerCount_ > 1 )
            {
                loc = handleTwoPrinterClickEvent(mouseEvent, nothingClicked);
            }
            else
            {
                loc = handleOnePrinterClickEvent(mouseEvent);
            }

            // two cases to handle:
            // a) 2 printers, in which case we are selecting a printer
            // b) 1 printer, in which case we are picking a corner for the label
            if ( ( 2 <= printerCount_ )  )
            {
                if(!nothingClicked)
                {
                    bool printerClicked = false;
                    // what printer did they click?
                    for ( int i( 0 ); i < printerCount_; ++i )
                    {
                        if ( loc == calculateLabelPlacement( i ) )
                        {
                            printerClicked = true;
                            printer_->setCurrentIndex( i );
                            labelPlacement_->setCurrentIndex( printer_->currentIndex() == first ? primaryLabelPos_ : secondaryLabelPos_ );

                            // show/hide combobox items
                            //updateLabelPlacementComboItems();

                            break;
                        }
                    }

                    if( ( printer_->currentIndex() == first ) && ( loc > upperLeft ) )
                        return 0;     // if we are a primary printer we dont mess with the bottom

                    if(!printerClicked)
                    {
                        if(printer_->currentIndex() == first)
                        {
                            if( loc < rightOfPrimary )
                            {
                                previewPackageClicked_ = true;

                                secondaryLabelMoved_ = false;
                                primaryLabelMoved_ = true;
                                previousPrimaryPosition_ = primaryLabelPos_;
                                primaryLabelPos_ = loc;

                                labelPlacement_->setCurrentIndex( primaryLabelPos_ );
                                writePlacementOffset( first );

                                if( secondaryLabelPos_ >= rightOfPrimary )
                                {
                                    updateSecondaryOnPrimaryMove();
                                    if( secondaryLabelMoved_ )
                                    {
                                        writePlacementOffset( second );
                                    }
                                }
                            }
                        }
                        else if(printer_->currentIndex() == second)
                        {
                            previewPackageClicked_ = true;

                            primaryLabelMoved_ = false;
                            secondaryLabelMoved_ = true;
                            previousSecondaryPosition_ = secondaryLabelPos_;
                            secondaryLabelPos_ = loc;

                            labelPlacement_->setCurrentIndex( secondaryLabelPos_ );
                            writePlacementOffset( second );
                        }

                        //labelPositionChanged();
                        //generatePreview();
                        updateInterfaceInternal( true );
                    }
                }
            }
            else if (( 1 == printerCount_ ) && ( labelPlacement_->isEnabled() ))
            {
                corner currentLoc( calculateLabelPlacement( 0 ) );

                // did they pick the opposite corner?
                if ( loc != currentLoc )
                {
                    QMap<int, int> validLocations;
                    //validLocations[lowerLeft] = lowerRight;       // no longer valid with non-movable primary printer
                    //validLocations[lowerRight] = lowerLeft;       // no longer valid with non-movable primary printer
                    validLocations[upperLeft] = upperRight;
                    validLocations[upperRight] = upperLeft;

                    if (( validLocations.contains( currentLoc ) ) && ( loc == validLocations[currentLoc] ))
                        labelPlacement_->setCurrentIndex( (0 == labelPlacement_->currentIndex()) ? 1 : 0 );
                }
            }
        }
    }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // register session
    if (( SessionDev == device ) && ( SnRegister == event ))
    {
        if ( !registered_ )
        {
            LOG_INFO( "session has been registered!", NULL );
            registered_ = true;
        }
    }

    // peripheral list
    else if (( System == device ) && ( SysPeripheralList == event ))
    {
        // Note:
        // For some reason or another backend gets into a loop where it keeps sending us peripheral
        // list data... so lets compare the new one versus the previous one and only take action if
        // different.
        if (( prevPeripheralList_->labelApplier() != peripheralList_->labelApplier() ) ||
            ( prevPeripheralList_->wrapper() != peripheralList_->wrapper() ))
        {
            LOG_INFO( "label applier type %d", peripheralList_->labelApplier() );
            LOG_INFO( "wrapper type %d", peripheralList_->wrapper() );

            requestConfig();

            (*prevPeripheralList_) = (*peripheralList_);
        }

        // wrapper was detached and is now attached
        if( !prevPeripheralList_->isWrapperAttached() && peripheralList_->isWrapperAttached() )
        {
            LOG_INFO( "Wrapper was detached and is now attached", NULL );
        }
    }
    // Note:
    //
    // In the HLX ASP implementation both the config and inquire messages are handled by the same
    // code... so I am not sure what to expect here.  I suppose lets handle both messages until
    // further testing says otherwise.

    // label applier config
    // label applier inquire
    else if (( LabelApplier == device ) && (( LaConfig == event ) || ( LaInquire == event )))
    {
        DomLabelApplierConfig m( msg );

        // when wrapper present package direction comes from wrapper config
        if ( !peripheralList_->wrapper() )
            direction_ = (packageDirection) m.packageDirection();

        printerCount_ = 0;

        if ( (printers_[0].installed = m.primaryStationInstalled()) )
            printerCount_ = 1;
        if ( (printers_[1].installed = m.secondaryStationInstalled()) )
            printerCount_ = 2; // 2

        // The UI shouldn't care about leading/trailing edge anymore, but leaving this here just in case
        if ( leadingEdge == (printers_[0].placement = m.primaryLabelPlacement()) )
            printers_[1].placement = trailingEdge;
        else
            printers_[1].placement = leadingEdge;

        labelPlacementFromProduct_ = false;

        updateConfig();
    }

    // label applier config (evolution)
    else if (( LabelApplier == device ) && ( EvolLaConfig == event ))
    {
        DomEvolLabelApplierConfig m( msg );

        // when wrapper present package direction comes from wrapper config
        if ( !peripheralList_->wrapper() )
            direction_ = (packageDirection) m.packageDirection();

        printerCount_ = 1; // 1

        printers_[0].installed = true;

        if ( (printers_[1].installed = m.hasSecondPrinter()) )
            printerCount_ = 2; // 2

        // this hardware uses product record to determine label placement
        labelPlacementFromProduct_ = true;

        updateConfig();
    }

    // wrapper config
    else if (( Wrapper == device ) && ( WrEvolInquire == event ))
    {
        // when wrapper present package direction comes from wrapper config
        if ( peripheralList_->wrapper() )
            direction_ = (packageDirection) evolWrapperConfig_->packageDirection();

        updateConfig();
    }

    // wrapper info
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        int awsModelNumber( evolWrapperSystemInformation_->awsModelNumber() );
        LOG_INFO( "wrapper model %d", awsModelNumber );

        switch ( awsModelNumber )
        {
        case DomEvolWrapperSystemInformation::AWS1RL:
        case DomEvolWrapperSystemInformation::AWS1RL_I:
        case DomEvolWrapperSystemInformation::AWS1BWRL:
        case DomEvolWrapperSystemInformation::AWS2RL:
        case DomEvolWrapperSystemInformation::AWS3RL:
        case DomEvolWrapperSystemInformation::AWS3BRL:
        case DomEvolWrapperSystemInformation::AWS4RL:
        case DomEvolWrapperSystemInformation::AWS4BRL:
        case DomEvolWrapperSystemInformation::NGS1BRL:
        case DomEvolWrapperSystemInformation::NGW2_C_RL:
        case DomEvolWrapperSystemInformation::NGW1_C_RL:
            direction_ = rightToLeft;
            break;

        case DomEvolWrapperSystemInformation::AWS1LR:
        case DomEvolWrapperSystemInformation::AWS1MLR:
        case DomEvolWrapperSystemInformation::AWS1LR_I:
        case DomEvolWrapperSystemInformation::AWS1BWLR:
        case DomEvolWrapperSystemInformation::AWS2LR:
        case DomEvolWrapperSystemInformation::AWS3LR:
        case DomEvolWrapperSystemInformation::AWS3BLR:
        case DomEvolWrapperSystemInformation::AWS4LR:
        case DomEvolWrapperSystemInformation::AWS4BLR:
        case DomEvolWrapperSystemInformation::NGS1BLR:
        case DomEvolWrapperSystemInformation::NGW2_C_LR:
        case DomEvolWrapperSystemInformation::NGW1_C_LR:
            direction_ = leftToRight;
            break;

        default:
            // direction from label applier config
            break;
        }

        updateConfig();
    }


        // SOF-4681 Add two new timings to control vertical label placement
        // Commented out for now
/*
    // wrapped package description(9,125)
    else if (( Wrapper == device ) && ( WrEvolPackageDesc == event ))
    {
        DomEvolWrapperPkgDesc m( msg );

        wrapBoxId_      = m.id();                   // remove when I can
        heightBoxIndex_ = m.heightBoxIndex();       // remove when I can
        wrapBoxIndex_   = m.wrapBoxIndex();

        // moved here bc we had to have wrapped one in order to change it
        // TODO: find out if this can allowed on 1 and/oir 2 printer systems
        labelVerticalAdjustment_->setEnabled( true );
        labelVerticalAdjustment_->setVisible( true );
        labelVerticalAdjLabel_->setVisible( true );

        //QMessageBox::information(this, "Variables", "wrapBoxIndex_: " + wrapBoxIndex_, QMessageBox::Ok);
    }

    // (9,237)
    else if (( Wrapper == device ) && ( WrEvolWriteAllTrayTypes == event ))
    {
        // (9,240) - once I get the list back, ask for last box wrapped

        // Get the wrapBoxIndex that is sent on a wrapped package with a 9,125 msg, then request it's attributes
        session_->send( Wrapper, WrEvolReadWrapBox, wrapBoxIndex_ );  // aiming at the last guy wrapped
    }

    // (9,241)
    else if (( Wrapper == device ) && ( WrEvolWriteWrapBox == event ))
    {
        // Get the current tray information so we can map back to it
        DomWrapBoxParams m( msg );

        precedence_ = m.precedence().toInt();
        trayName_ = m.trayName();
    }

    // wrapbox timing attributes (9,247)
    else if (( Wrapper == device ) && ( WrEvolWriteHeightBox == event ))
    {
        // update the current vertical offsets
        DomHeightBox m( msg );
        heightBox_ = new DomHeightBox(m);

        paramArray_.clear();
        paramArray_ = TokenParser::splitStringTokens( m.wrapAttributes() );

        printers_[0].verticalOffset = paramArray_[WrapParameter::PRIMARY_LABEL_VERT_OFFSET].toInt();
        printers_[1].verticalOffset = paramArray_[WrapParameter::SECONDARY_LABEL_VERT_OFFSET].toInt();

        ignoreSignals_ = true;
        labelVerticalAdjustment_->setCurrentIndex( printers_[printer_->currentIndex()].verticalOffset );
        ignoreSignals_ = false;
    }

    else
    {
        if ( Wrapper == device )
            LOG_INFO( "Else I received a wrapper message", NULL );
    }
*/

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::requestConfig()
{
    DomCurrentPLU *product( currentProduct() );

    // get label applier configuration
    session_->send( LabelApplier, LaReqInquire );

    // get wrapper configuration
    if ( peripheralList_->wrapper() )
        session_->send( Wrapper, WrReqInquire );

    if ( !product )
            return;
    
    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    // get wrapper configuration
    //if ( peripheralList_->wrapper() )
    //{
    //    session_->send(Wrapper, WrReqEvolSystemInfo);            // 9,143
    //    session_->send(Wrapper, WrEvolLoadPackageTimings,"1");   // 9,226, viewCurrentTimings_
    //}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onComboBoxCurrentIndexChanged( QWidget *widget )
{
    DomCurrentPLU *product( currentProduct() );

    // nothing to do
    if ( ignoreSignals_ )
        return;
    else if ( !product )
        return;

    // process label placement

    if ( labelPlacement_ == widget )
    {
        //int selected( printer_->currentIndex() );
        //int other( (0 == selected) ? 1 : 0 );

        if( printerCount_ < 2 )
        {
            int selected( printer_->currentIndex() );
            int other( (0 == selected) ? 1 : 0 );

            if ( rightSide == (printers_[selected].placement = labelPlacement_->currentIndex()) )
                printers_[other].placement = leftSide;
            else
                printers_[other].placement = rightSide;

            // primary printer
            if ( printers_[first].installed )
            {
                QString newValue( QString::number( printers_[first].placement ) );
                QString oldValue( QString::number( product->primaryLabelPlacement() ) );

                operateMode_->writeSingleField( OperateMode::OffsetPrimaryLabelPlacement, newValue, oldValue );
            }

            // second printer
            if ( printers_[second].installed )
            {
                QString newValue( QString::number( printers_[second].placement ) );
                QString oldValue( QString::number( product->secondLabelPlacement() ) );

                operateMode_->writeSingleField( OperateMode::OffsetSecondLabelPlacement, newValue, oldValue );
            }
        }
    }
    
    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    /*
    else if( printer_ == widget )
    {
        // update the vertical placement.  One combo displays the offset for both printers
        labelVerticalAdjustment_->setCurrentIndex( printers_[printer_->currentIndex()].verticalOffset );
    }
    else if( labelVerticalAdjustment_ == widget )
    {
        if( 0 == paramArray_.count() )
            return;

        int tmpIndex = labelVerticalAdjustment_->currentIndex();
        printers_[printer_->currentIndex()].verticalOffset = tmpIndex;

        if( 0 == printer_->currentIndex() )
            paramArray_[WrapParameter::PRIMARY_LABEL_VERT_OFFSET] = QString::number(printers_[0].verticalOffset);
        else if( 1 == printer_->currentIndex() )
            paramArray_[WrapParameter::SECONDARY_LABEL_VERT_OFFSET] = QString::number(printers_[1].verticalOffset);

        QString writeMsg = wrapBoxIndex_;
        writeMsg += "," + heightBox_->maxHeight();
        writeMsg += "," + heightBox_->numOfAttributes();
        writeMsg += ",\"";
        for (int j = 0; j < paramArray_.count(); j++)
        {
            if (j > 0)
                writeMsg += ",";

            writeMsg += paramArray_[j];
        }
        writeMsg += "\"";
        writeMsg += "," + heightBox_->canDelete();

        session_->send(Wrapper, WrEvolWriteHeightBox, writeMsg);              // 9,247,precedence,maxHeight,heightBoxes list

        session_->send(Wrapper, WrEvolDoneWrapBox, wrapBoxIndex_);
    }
    */

    // update interface
    updateInterfaceInternal( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onCurrentProductChanged( bool nof )
{
    Q_UNUSED( nof );

    // update interface
    updateInterface();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onCurrentProductUpdateField( const DomUpdateRunModeField& m )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    // handle update
    switch ( (OperateMode::ProductWriteSingleFieldOffset) m.offsetIndex() )
    {
    case OperateMode::OffsetPrimaryLabelPlacement:
    case OperateMode::OffsetPrimaryLabelRotation:
    case OperateMode::OffsetSecondLabelPlacement:
    case OperateMode::OffsetSecondLabelRotation:
    case OperateMode::OffsetThirdLabelPlacement:
    case OperateMode::OffsetThirdLabelRotation:
    {
        if( printerCount_ < 2 )
        {
            updateInterface();
        }
        break;
    }
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onLabelRotationChanged( int value )
{
    LOG_CLASS_INFO( "LabelPlacementScreen", "onLabelRotationChanged" );

    DomCurrentPLU *product( currentProduct() );

    // nothing to do
    if ( ignoreSignals_ )
        return;
    else if ( !product )
        return;

    int angle( (((value * 360) / labelRotation_->maximum()) + 180) % 360 );
    int selected( printer_->currentIndex() );

    LOG_DEBUG( "rotation of printer %d changed to %d", selected, angle );
    printers_[selected].rotation = angle;

    if( printerCount_ > 1 && syncRotation_->isChecked() )
    {
        int otherPrinter = ( selected == first ) ? second : first;
        printers_[otherPrinter].rotation = angle;

        if( printers_[first].installed )
        {
            operateMode_->writeSingleField( OperateMode::OffsetPrimaryLabelRotation, QString::number( angle ), product->primaryLabelRotationRaw() );
        }

        if( printers_[second].installed && product->secondDefaultLabelType() > 0 )
        {
            operateMode_->writeSingleField( OperateMode::OffsetSecondLabelRotation, QString::number( angle ), product->secondLabelRotationRaw() );
        }
    }
    else
    {
        // update session with new value
        int offset( OperateMode::OffsetPrimaryLabelRotation );
        QString newValue( QString::number( angle ) );
        QString oldValue( QString::number( product->primaryLabelRotation() ) );

        // second printer
        if ( 1 == selected )
        {
            offset = OperateMode::OffsetSecondLabelRotation;
            oldValue = QString::number( product->secondLabelRotation() );
        }
        // third printer
        else if ( 2 == selected )
        {
            offset = OperateMode::OffsetThirdLabelRotation;
            oldValue = QString::number( product->thirdLabelRotation() );
        }

        operateMode_->writeSingleField( ( OperateMode::ProductWriteSingleFieldOffset ) offset, newValue, oldValue );
    }

    // update interface
    updateInterfaceInternal( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::updateInterface()
{
    DomCurrentPLU *product ( currentProduct() );

    // no product
    if ( !(product = currentProduct()) )
    {
        updateInterfaceInternal( false );
        return;
    }

    if( printerCount_ < 2 )
    {
        // get label placement from product record
        if ( labelPlacementFromProduct_ )
        {
            int lp;

            if ( defaultSetting == (lp = product->primaryLabelPlacement()) )
                lp = scaleConfig_->defaultLabelPlacement();

            if ( rightSide == (printers_[0].placement = lp) )
                printers_[1].placement = leftSide;
            else
                printers_[1].placement = rightSide;
        }

        // get label rotation from product record
        printers_[0].rotation = adjustRotation(product->primaryLabelRotation());
        printers_[1].rotation = adjustRotation(product->secondLabelRotation());

        printers_[0].labelStockUpsideDown = (0 != product->labelStockRotation1());
        printers_[1].labelStockUpsideDown = (0 != product->labelStockRotation2());
    }
    else
    {
        if(printers_[0].installed)
        {
            printers_[0].rotation = adjustRotation(product->primaryLabelRotation());

            if( product->primaryLabelPlacement() == 2 )
            {
                //primaryLabelPos_ = (corner)session_->scaleConfig->primaryPlacementSystem;
                primaryLabelPos_ = (corner)session_->scaleConfig->normalScaleOperation.labelPlacement().toInt();
            }
            else
            {
                primaryLabelPos_ = (corner)product->primaryLabelPlacement();
            }

            //primaryLabelPos_ = (corner)product->primaryLabelPlacement();
            printers_[0].labelStockUpsideDown = (0 != product->labelStockRotation1());


            printers_[0].placement = primaryLabelPos_;
            printers_[0].rotation = product->primaryLabelRotation();
        }
        if(printers_[1].installed)
        {
            printers_[1].rotation = adjustRotation(product->secondLabelRotation());

            if( product->secondLabelType() > 0 || product->secondDefaultLabelType() > 0 )
            {
                if( product->secondLabelPlacement() == 2 )
                {
                    //secondaryLabelPos_ = (corner)session_->scaleConfig->secondaryPlacementSystem;
                    secondaryLabelPos_ = (corner)session_->scaleConfig->normalScaleOperation.secondaryLabelPlacement();
                }
                else
                {
                    secondaryLabelPos_ = (corner)product->secondLabelPlacement();
                }
            }
            else
            {
                secondaryLabelPos_ = none;
            }

            //secondaryLabelPos_ = (corner)product->secondLabelPlacement();
            printers_[1].labelStockUpsideDown = (0 != product->labelStockRotation2());


            printers_[1].placement = secondaryLabelPos_;
            printers_[1].rotation = product->secondLabelRotation();
        }

        if( printers_[0].labelStockUpsideDown || printers_[1].labelStockUpsideDown )
        {
            syncRotation_->setChecked( false );
        }
    }

    // TODO: find out if this can allowed on 1 and/oir 2 printer systems
    // TODO: find out how I can check how many labels have been printed,
    // Moved to the receiving 9,125 which I receive when every package is wrapped.  Need to have at least one
    //bool isEnabled = true;
    //labelVerticalAdjustment_->setEnabled( isEnabled );
    //labelVerticalAdjustment_->setVisible( isEnabled );
    //labelVerticalAdjLabel_->setVisible( isEnabled );

    // update interface
    updateInterfaceInternal( true );
}

bool LabelPlacementScreen::onScreenChanging()
{
    //savePlacementAndRotation();

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onNumberPadMoved( Qt::DockWidgetArea where )
{
    createLayout( where );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // SOF-2798: Make sure the correct weight bar is visible
    if ( operateMode_ )
        operateMode_->showWeightBar();

    plu_->setTotalTypeVisible( scaleConfig_->showTotalTypeOnRunScreen() ); // HA-176

    // get hardware info
    requestConfig();

    updateInterface();
    // generateLeverOrientation();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::initialize()
{
    //LOG_CLASS_INFO( "LabelPlacementScreen", "initialize" );

    static const QMargins margins( 10, 10, 10, 6 );

    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );

    const hobart::backgroundWidgetRenderer frameRenderer(
        new hobart::rectangleBackgroundWidget(),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

    comboBoxCurrentIndexChanged_ = new QSignalMapper( this );

    // not working as it should
    connect( comboBoxCurrentIndexChanged_, SIGNAL( mapped(QWidget*) ), SLOT( onComboBoxCurrentIndexChanged(QWidget*) ) );
    //connect(ui->cboSpAutoLookupDigit,           SIGNAL(currentIndexChanged(int)),    this, SLOT(onComboBoxCurrentIndexChanged(int)));
    //
    // messages
    //

    const QSize messageFrameSize( mainWindow_->width() - plu_->width(), plu_->height() );

    messageFrame_ = new frameWidget( frameRenderer.drawControl( messageFrameSize, hobart::ReadOnly ), this );
    messageFrame_->setDropShadow( frameRenderer.drawDropShadow( messageFrameSize ) );
    messageFrame_->setContentsMargins( margins );
    messageFrame_->setObjectName( "messageFrame" );

    //
    // label placement
    //

    const QSize labelFrameSize( mainWindow_->width() - 45, scaleHeightForScreen( 380 ) );

    labelFrame_ = new frameWidget( frameRenderer.drawControl( labelFrameSize, hobart::ReadOnly ), this );
    labelFrame_->setDropShadow( frameRenderer.drawDropShadow( labelFrameSize ) );
    labelFrame_->setContentsMargins( margins );
    labelFrame_->setObjectName( "labelFrame" );

    preview_ = new LabelPlacementWidget( labelFrame_ );
    preview_->setAutoFillBackground( true );
    preview_->setObjectName( "preview" );

    preview_->installEventFilter( this );

    printerLabel_ = new QLabel( labelFrame_ );
    printerLabel_->setObjectName( "printerLabel" );

    printer_ = new QComboBox( labelFrame_ );
    printer_->setFocusPolicy( Qt::NoFocus );
    printer_->setObjectName( "printer" );

    printer_->addItem( "", 0 );
    printer_->addItem( "", 1 );

    connect( printer_, SIGNAL( currentIndexChanged(int) ), comboBoxCurrentIndexChanged_, SLOT( map() ) );
    comboBoxCurrentIndexChanged_->setMapping( printer_, printer_ );

    labelPlacementLabel_ = new QLabel( labelFrame_ );
    labelPlacementLabel_->setObjectName( "labelPlacementLabel" );

    labelPlacement_ = new QComboBox( labelFrame_ );
    labelPlacement_->setFocusPolicy( Qt::NoFocus );
    labelPlacement_->setObjectName( "labelPlacement" );

    // Text, UserData
    labelPlacement_->addItem( "", 0 );          // Upper Right      1 of 2 primary options - valid for secondary
    labelPlacement_->addItem( "", 1 );          // Upper Left       2 of 2 primary options - valid for secondary

    labelPlacement_->addItem( "", 2 );          // None             dont display - just a blank spot
    labelPlacement_->addItem( "", 3 );          // Lower Right      valid for secondary - not a primary option
    labelPlacement_->addItem( "", 4 );          // Lower Left       valid for secondary - not a primary option
    labelPlacement_->addItem( "", 5 );          // Right of Primary valid for secondary AT TOP LOCs - not a primary option
    labelPlacement_->addItem( "", 6 );          // Left of Primary  valid for secondary AT TOP LOCs - not a primary option
    labelPlacement_->addItem( "", 7 );          // Above Primary    no longer a valid option since we cant put Primary at the bottom
    labelPlacement_->addItem( "", 8 );          // Below Primary

    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now

/*
    labelVerticalAdjLabel_ = new QLabel( labelFrame_ );
    labelVerticalAdjLabel_->setObjectName( "labelVerticalAdjLabel" );

    labelVerticalAdjustment_ = new QComboBox( labelFrame_ );
    labelVerticalAdjustment_->setFocusPolicy( Qt::NoFocus );
    labelVerticalAdjustment_->setObjectName( "labelVerticalAdjustment" );

    labelVerticalAdjustment_->addItem( "+5", 0 );
    labelVerticalAdjustment_->addItem( "+4", 1 );
    labelVerticalAdjustment_->addItem( "+3", 2 );
    labelVerticalAdjustment_->addItem( "+2", 3 );
    labelVerticalAdjustment_->addItem( "+1", 4 );
    labelVerticalAdjustment_->addItem( "None", 5 );
    labelVerticalAdjustment_->addItem( "-1", 6 );
    labelVerticalAdjustment_->addItem( "-2", 7 );
    labelVerticalAdjustment_->addItem( "-3", 8 );
    labelVerticalAdjustment_->addItem( "-4", 9 );
    labelVerticalAdjustment_->addItem( "-5", 10 );
*/
    connect( labelPlacement_, SIGNAL( currentIndexChanged(int) ), comboBoxCurrentIndexChanged_, SLOT( map() ) );
    comboBoxCurrentIndexChanged_->setMapping( labelPlacement_, labelPlacement_ );

    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    //connect( labelVerticalAdjustment_, SIGNAL( currentIndexChanged(int) ), comboBoxCurrentIndexChanged_, SLOT( map() ) );
    //comboBoxCurrentIndexChanged_->setMapping( labelVerticalAdjustment_, labelVerticalAdjustment_ );

    labelRotationLabel_ = new QLabel( labelFrame_ );
    labelRotationLabel_->setObjectName( "labelRotationLabel" );

    labelRotation_ = new QDial( labelFrame_ );
    labelRotation_->setFixedSize( QSize( labelFrameSize.height() / 2, labelFrameSize.height() / 2 ) );
    labelRotation_->setFocusPolicy( Qt::NoFocus );
    labelRotation_->setNotchesVisible( true );
    labelRotation_->setObjectName( "labelRotation" );
    labelRotation_->setWrapping( true );

    labelRotation_->setMaximum( 8 );
    labelRotation_->setMinimum( 0 );
    labelRotation_->setSingleStep( 1 );

    connect( labelRotation_, SIGNAL( valueChanged(int) ), SLOT( onLabelRotationChanged(int) ) );

    QList<QComboBox*> comboBoxes;
    comboBoxes.append( printer_ );
    comboBoxes.append( labelPlacement_ );
    
    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    //comboBoxes.append( labelVerticalAdjustment_ );

    foreach ( QComboBox *comboBox, comboBoxes )
    {
        setFontPointSize14( comboBox );

        QPalette p( comboBox->palette() );
        p.setBrush( QPalette::Active, comboBox->foregroundRole(), hobart::colorPalette::black );        // arrow
        p.setBrush( QPalette::Active, QPalette::ButtonText, hobart::colorPalette::black );              // scroll bar arrows
        p.setBrush( QPalette::Active, QPalette::Text, hobart::colorPalette::black );                    // item text
        p.setBrush( QPalette::Disabled, comboBox->foregroundRole(), hobart::colorPalette::lightGray );  // arrow
        p.setBrush( QPalette::Disabled, QPalette::ButtonText, hobart::colorPalette::lightGray );        // scroll bar arrows
        p.setBrush( QPalette::Disabled, QPalette::Text, hobart::colorPalette::lightGray );              // item text
        comboBox->setPalette( p );

        // everything disabled to start with
        comboBox->setEnabled( false );
    }

    syncRotation_ = new QCheckBox( labelFrame_ );
    syncRotation_->setFocusPolicy( Qt::NoFocus );
    syncRotation_->setObjectName( "syncRotation" );
    syncRotation_->setChecked( true );
    syncRotation_->setEnabled( false );

    syncRotation_->setStyleSheet( "QCheckBox::indicator { color: black }" );

    labelRotation_->setEnabled( false );

    plu_->setTotalTypeVisible( scaleConfig_->showTotalTypeOnRunScreen() ); // HA-176

    primaryLabelPos_ = none;
    secondaryLabelPos_ = none;

    //updateLabelPlacementComboItems();

    primaryLabelMoved_ = false;
    secondaryLabelMoved_ = false;

    primaryRotation_ = 0;

    heightBox_ = NULL;

    // grab config for system default
    session_->send(ConfigurationDev,CfgScaleOperationRead);

    //defineCornerLocations();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::createLayout( Qt::DockWidgetArea numPadLocation )
{
    // delete previous layout
    if ( labelFrame_->layout() )
        delete labelFrame_->layout();

    QVBoxLayout *center( new QVBoxLayout() );
    center->addWidget( printerLabel_ );
    center->addWidget( printer_ );
    center->addWidget( labelPlacementLabel_ );
    center->addWidget( labelPlacement_ );

    // SOF-4681 Add two new timings to control vertical label placement
    // Commented out for now
    //center->addWidget( labelVerticalAdjLabel_ );
    //center->addWidget( labelVerticalAdjustment_ );
    center->addStretch();
    center->addWidget( labelRotationLabel_ );
    center->addWidget( labelRotation_ );
    center->addWidget( syncRotation_ );
    //center->addWidget( doneButton_ );

    /*
    QHBoxLayout *rightBottom( new QHBoxLayout() );
    rightBottom->addStretch();
    rightBottom->addWidget( leverMessageIcon_ );
    rightBottom->addWidget( leverMessage_ );
    rightBottom->addStretch();

    QVBoxLayout *right( new QVBoxLayout() );
    right->addWidget( lever_, 1 );
    right->addLayout( rightBottom );
    */

    QHBoxLayout *labelFrameLayout( new QHBoxLayout( labelFrame_ ) );

    labelFrameLayout->addWidget( preview_, 1 );
    labelFrameLayout->addLayout( center );
    // labelFrameLayout->addLayout( right, 1 );

    // ---- //

    // delete previous layout
    if ( layout() )
        delete layout();

    QHBoxLayout *top( new QHBoxLayout() );
    top->setContentsMargins( QMargins() );
    top->setSpacing( 0 );

    top->addWidget( plu_ );
    top->addWidget( messageFrame_ );

    QHBoxLayout *bottom( new QHBoxLayout() );
    bottom->setContentsMargins( QMargins() );
    bottom->setSpacing( 0 );

    bottom->addWidget( labelFrame_ );

    QHBoxLayout *bottomLayout( new QHBoxLayout() );
    bottomLayout->setContentsMargins( QMargins() );
    bottomLayout->setSpacing( 0 );

    if ( Qt::LeftDockWidgetArea == numPadLocation )
    {
        bottomLayout->addLayout( bottom, 1 );
        bottomLayout->addSpacing( 45 );
    }
    else if ( Qt::RightDockWidgetArea == numPadLocation )
    {
        bottomLayout->addSpacing( 45 );
        bottomLayout->addLayout( bottom, 1 );
    }

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );

    screenLayout->addLayout( top );
    screenLayout->addLayout( bottomLayout, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Does this function even get called anymore?
void LabelPlacementScreen::updateConfig()
{
    LOG_CLASS_INFO( "LabelPlacementScreen", "updateConfig" );

    bool isLeftToRight( leftToRight == direction_ );

    // AWS has automatic rotation in primary printer
    printers_[0].automaticRotation = isAWS();

    // automatic rotation is value-added for the EWS
    if (( isEWS() ) && ( evolWrapperConfig_->autoRotationRaw().length() ))
        printers_[0].automaticRotation = evolWrapperConfig_->autoRotation();

    // Note:
    //
    // The printer faces different directions depending upon what model and
    // package direction we have.
    //
    // Below is a table showing direction of the label as it is printed. I have
    // confirmed these with Kraig Wooddell and Gary Cornish.
    //
    //                 Two printer systems   One printer systems
    //                 --------------------- --------------------------------
    //
    //                 AWS        AWS        AWS        EWS        CLA
    // Direction:      primary    secondary  primary    primary    primary
    // ------------- + ---------- ---------- ---------- ---------- ----------
    // Left To Right | 180        90         270        270        180
    // Right To Left | 270        180        270        270        180
    //               |
    // Rotation      | auto       manual     auto       either (*) manual
    // Placement     | fixed      fixed      adjustable adjustable fixed (^)
    //
    // * EWS printer may or may not have automatic rotation, this is an upgrade
    //   that may be purchased.
    // ^ Always placed on the trailing edge of package as it travels down belt.

    printers_[0].orientation = printers_[1].orientation = 180;

    if ( isAWS() )
    {
        LOG_DEBUG( "AWS, %d printer(s), belt direction %s", printerCount_, (isLeftToRight ? "L->R" : "R->L") );

        if ( 1 == printerCount_ )
            printers_[0].orientation = 270;
        else if ( 2 == printerCount_ )
        {
            printers_[0].orientation = ( isLeftToRight ) ? 180 : 270;
            printers_[1].orientation = ( isLeftToRight ) ? 90 : 180;
        }
    }
    else if ( isEWS() )
    {
        LOG_DEBUG( "EWS, belt direction %s", (isLeftToRight ? "L->R" : "R->L") );
        printers_[0].orientation = 270;
    }
    else if ( isCLA_EPSAC2() )
        LOG_DEBUG( "CLA, belt direction %s", (isLeftToRight ? "L->R" : "R->L") );
    else
        LOG_WARN( "unknown configuration (devices offline?)", NULL );

    updateInterface();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::updateInterfaceInternal( bool enabled )
{
    DomCurrentPLU *product( currentProduct() );

    ignoreSignals_ = true;

    if (( !enabled ) || ( !printerCount_ ))
    {
        // disable controls
        printer_->setCurrentIndex( 0 );
        printer_->setEnabled( false );

        labelPlacement_->setCurrentIndex( 0 );
        labelPlacement_->setEnabled( false );

        labelRotation_->setEnabled( false );
        labelRotation_->setMaximum( 8 );
        labelRotation_->setValue( 4 );

        syncRotation_->setEnabled( false );
        syncRotation_->setVisible( false );
    }
    else
    {
        int selected( printer_->currentIndex() );

        // enable controls
        //printer_->setEnabled( 1 < printerCount_ );

        // not used on two printer system
        labelPlacement_->setEnabled( printerCount_ < 2 );
        labelPlacement_->setVisible( printerCount_ <= 2 );
        labelPlacementLabel_->setVisible( printerCount_ <= 2 );

        // Sync rotation only on two printer system
        syncRotation_->setEnabled( printerCount_ >= 2 && product->secondDefaultLabelType() > 0 );
        syncRotation_->setVisible( printerCount_ >= 2 && product->secondDefaultLabelType() > 0 );

        if( printerCount_ < 2 )
        {
            labelPlacement_->setCurrentIndex( upperRight == calculateLabelPlacement( selected ) ? 0 : 1 );
            printer_->setEnabled( false );
        }
        else
        {
            labelPlacement_->setCurrentIndex( selected == first ? primaryLabelPos_ : secondaryLabelPos_ );

            // if PLU does not have a label type, we dont want to allow the assignment of a second label position
            printer_->setEnabled( product->secondLabelType() > 0 );
        }

        // for CLA label placement is always disabled
        if ( isCLA_EPSAC2() )
            labelPlacement_->setEnabled( false );

        labelRotation_->setEnabled( true );
        // we should only need support for 90 degree increments, if there is a need for the 45 degree increments again
        // just change the 4 to an 8
        labelRotation_->setMaximum( /* printers_[selected].automaticRotation ? 8 : 4 */ 4 );
        labelRotation_->setValue( (labelRotation_->maximum() * ((printers_[selected].rotation + 180) % 360)) / 360 );
    }

    generatePreview();
    // generateLeverOrientation();

    ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementScreen::generatePreview()
{
    if( labelRotation_->isEnabled() )
    {
        int primaryPos = none;

        // need this for single printer systems (might need anytime primary is selected)
        if( printer_->currentIndex() == first )
        {
            primaryPos = calculateLabelPlacement( first );
        }
        else
        {
            primaryPos = primaryLabelPos_;
        }

        preview_->generatePreview(
                printerCount_,
                printer_->currentIndex(),
                primaryPos,
                secondaryLabelPos_,
                primaryLabelSize_,
                secondaryLabelSize_,
                printers_
        );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void LabelPlacementScreen::generateLeverOrientation()
{
    QPalette p( lever_->palette() );

    QPixmap pixmap( lever_->size() );
    pixmap.fill( Qt::transparent );

    leverMessageIcon_->setVisible( false );
    leverMessage_->setVisible( false );

    // preview enabled
    if ( labelRotation_->isEnabled() )
    {
        int selected( printer_->currentIndex() );

        // selected printer is manual rotation
        if ( !printers_[selected].automaticRotation )
        {
            int numberOfTeeth( 44 );
            int width( qMin( lever_->width(), lever_->height() ) );

            leverMessageIcon_->setVisible( true );
            leverMessage_->setVisible( true );

            // Note:
            //
            // The current printer design for manual rotation involves a
            // lever which must be set properly.  This lever has four settings:
            //
            //                 |  rotate
            //     rotate   \  |  270 deg
            //     180 deg   \ |
            //                \|
            //  rotate   ------O        All rotation is
            //  90 deg        /         performed clockwise
            //               /
            //  no rotation /
            //
            // We also must consider how the printer is mounted... each wrapper
            // model mounts the printer differently.
            //
            // So in order to calculate lever position, we start with printer
            // orientation (how it is mounted) and then start adding in the
            // extra rotation from the lever position.  Once we find a match,
            // thats our lever position.
            //
            // The upper most position (straight up) is position zero (0).

            int leverPosition( 0 );
            int leverRotation( 270 );

            while ( printers_[selected].rotation != ((printers_[selected].orientation + leverRotation) % 360) )
            {
                ++leverPosition;
                leverRotation -= 90;

                // sanity check, prevent infinite looping
                if ( 3 < leverPosition )
                    break;
            }

            QPainter painter( &pixmap );
            painter.setRenderHint( QPainter::Antialiasing, true );

            QBrush brush( painter.brush() );
            brush.setColor( Qt::lightGray );

            QPen pen( painter.pen() );
            pen.setColor( Qt::lightGray );

            // translate to middle of widget
            painter.translate( lever_->width() / 2, lever_->height() / 2 );

            // draw lever
            for ( int pos( 0 ); pos < 4; ++pos )
            {
                QRect rect( QPoint( -(width / 40), -(width / 2) ), QPoint( width / 40, 0 ) );

                // lever position is solid
                if ( pos == leverPosition )
                {
                    brush.setStyle( Qt::SolidPattern );
                    pen.setStyle( Qt::SolidLine );
                }

                // non-lever position is dashed and no fill
                else
                {
                    brush.setStyle( Qt::NoBrush );
                    pen.setStyle( Qt::DashLine );
                }

                painter.setBrush( brush );
                painter.setPen( pen );

                painter.drawRect( rect );
                painter.rotate( -45 );
            }

            painter.rotate( 180 );

            // draw gear
            brush.setColor( Qt::white );
            brush.setStyle( Qt::SolidPattern );
            painter.setBrush( brush );

            pen.setColor( Qt::white );
            pen.setStyle( Qt::SolidLine );
            painter.setPen( pen );

            for ( int tooth( 0 ); tooth < numberOfTeeth; ++tooth )
            {
                QPainterPath path;
                path.lineTo( -(width / 12), width / 12 );
                path.lineTo( 0, width / 6 );
                path.lineTo( width / 12, width / 12 );
                path.closeSubpath();

                painter.drawPath( path );
                painter.rotate( 360.0 / numberOfTeeth );
            }
        }
    }

    // show pixmap
    p.setBrush( lever_->backgroundRole(), pixmap );
    lever_->setPalette( p );

    lever_->update();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelPlacementScreen::isAWS() const
{
    return (( 0 < peripheralList_->wrapper() ) &&
        ( DomEvolWrapperSystemInformation::AWS1BWRL != evolWrapperSystemInformation_->awsModelNumber() ) &&
        ( DomEvolWrapperSystemInformation::AWS1BWLR != evolWrapperSystemInformation_->awsModelNumber() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelPlacementScreen::isCLA_EPSAC2() const
{
    return (MadpPeripheralList::latCLA_EPSAC2 == peripheralList_->labelApplier());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelPlacementScreen::isEWS() const
{
    return (( 0 < peripheralList_->wrapper() ) &&
        (( DomEvolWrapperSystemInformation::AWS1BWRL == evolWrapperSystemInformation_->awsModelNumber() ) ||
         ( DomEvolWrapperSystemInformation::AWS1BWLR == evolWrapperSystemInformation_->awsModelNumber() )));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelPlacementScreen::isNGWWrapper() const
{
    return DomEvolWrapperSystemInformation::nextGenModels <= evolWrapperSystemInformation_->awsModelNumber();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int LabelPlacementScreen::adjustRotation(int rotation)		//	HTA-99
{
    LOG_CLASS_INFO( "LabelPlacementScreen", "adjustRotation" );

    int theRotation = rotation;
    //qDebug() << "adjustRotation():  Initial rotation = " << theRotation;

    if ( isCLA_EPSAC2() )
    {
        theRotation = ( rotation + 180 ) % 360;
    }
    //qDebug() << "adjustRotation():  theRotation = " << theRotation;
    LOG_DEBUG( "theRotation = %d", theRotation );

    return theRotation;

    return 0;
}

void LabelPlacementScreen::updateSecondaryOnPrimaryMove()
{
    //DomCurrentPLU *product( currentProduct() );

    if(secondaryLabelPos_ >= rightOfPrimary)
    {
        if( (primaryLabelPos_ == upperLeft || primaryLabelPos_ == upperRight) &&
            (secondaryLabelPos_ == abovePrimary)
        )
        {
            // just change the position to below
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = belowPrimary;
            secondaryLabelMoved_ = true;
        }
        // SOF-5447: Label placement change in Operate Mode
        //else if( (primaryLabelPos_ == lowerLeft || primaryLabelPos_ == lowerRight) &&
        //         (secondaryLabelPos_ == belowPrimary)
        //)
        //{
        //    // just change the position to above
        //    previousSecondaryPosition_ = secondaryLabelPos_;
        //    secondaryLabelPos_ = abovePrimary;
        //    secondaryLabelMoved_ = true;
        //}

        // SOF-5447: Label placement change in Operate Mode
        //else if( (primaryLabelPos_ == upperLeft || primaryLabelPos_ == lowerLeft) &&
        //         (secondaryLabelPos_ == leftOfPrimary)
        else if( (primaryLabelPos_ == upperLeft ) && (secondaryLabelPos_ == leftOfPrimary) )
        {
            // just change the position to right
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = rightOfPrimary;
            secondaryLabelMoved_ = true;
        }

        // SOF-5447: Label placement change in Operate Mode
        //else if( (primaryLabelPos_ == upperRight || primaryLabelPos_ == lowerRight) &&
        //         (secondaryLabelPos_ == rightOfPrimary)
        else if( ( primaryLabelPos_ == upperRight ) && ( secondaryLabelPos_ == rightOfPrimary ) )
        {
            // just change the position to left
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = leftOfPrimary;
            secondaryLabelMoved_ = true;
        }
    }
}

void LabelPlacementScreen::savePlacementAndRotation()
{
    // Need to add a configuration for this
    //session_->send(ProductDev, ProductSave);
}

// user click event on two printer system
corner LabelPlacementScreen::handleTwoPrinterClickEvent(QMouseEvent *mouseEvent, bool &nothingClicked)
{
    corner loc(none);
    int edgeOffset = 20;

    // click in top half of widget
    if ( mouseEvent->y() < ( preview_->y() + primaryLabelSize_.height() ) )
    {
        if ( mouseEvent->x() < ( preview_->x() + primaryLabelSize_.width() ) )
        {
            loc = upperLeft;
        }
        else if ( mouseEvent->x() > ( preview_->width() - primaryLabelSize_.width() - edgeOffset ) )
        {
            loc = upperRight;
        }
        else
        {
            // check if they want a position next to the primary label
            switch(primaryLabelPos_)
            {
                case upperLeft:
                {
                    if( (mouseEvent->y() < ( preview_->y() + primaryLabelSize_.height() )) &&
                        (mouseEvent->x() > ( primaryLabelSize_.width() ))
                    )
                    {
                        loc = rightOfPrimary;
                    }
                    else
                    {
                        nothingClicked = true;
                    }

                    break;
                }
                case upperRight:
                {
                    if( (mouseEvent->y() < ( preview_->y() + primaryLabelSize_.height() )) &&
                        (mouseEvent->x() < ( preview_->width() - primaryLabelSize_.width() - edgeOffset ))
                    )
                    {
                        loc = leftOfPrimary;
                    }
                    else
                    {
                        nothingClicked = true;
                    }

                    break;
                }
                default:
                {
                    nothingClicked = true;

                    break;
                }
            }
        }
    }
    // click in bottom half of widget
    else if( mouseEvent->y() > ( preview_->height() - primaryLabelSize_.height() - edgeOffset ) )
    {
        if ( mouseEvent->x() < ( preview_->x() + primaryLabelSize_.width() ) )
        {
            loc = lowerLeft;
        }
        else if( mouseEvent->x() > ( preview_->width() - primaryLabelSize_.width() - edgeOffset ) )
        {
            loc = lowerRight;
        }
        else
        {
            switch(primaryLabelPos_)
            {
                case lowerLeft:
                {
                    if( (mouseEvent->y() < preview_->height() && mouseEvent->y() > preview_->height() - primaryLabelSize_.height()) &&
                        (mouseEvent->x() > primaryLabelSize_.width())
                    )
                    {
                        loc = rightOfPrimary;
                    }
                    else
                    {
                        nothingClicked = true;
                    }

                    break;
                }
                case lowerRight:
                {
                    if( (mouseEvent->y() < preview_->height() && mouseEvent->y() > preview_->height() - primaryLabelSize_.height()) &&
                        (mouseEvent->x() < preview_->width() - primaryLabelSize_.width())
                    )
                    {
                        loc = leftOfPrimary;
                    }
                    else
                    {
                        nothingClicked = true;
                    }

                    break;
                }
                default:
                {
                    nothingClicked = true;

                    break;
                }
            }
        }
    }
    // this is the case where they click in a spot that doesnt fall into the top or bottom half geometry. This only happens when trying to click
    // a secondary position above or below the primary
    else
    {
        switch(primaryLabelPos_)
        {
            case upperLeft:
            {
                if( (mouseEvent->y() < ( primaryLabelSize_.height() + secondaryLabelSize_.height() + ( 2 * edgeOffset ) )) &&
                    (mouseEvent->x() < ( preview_->x() + secondaryLabelSize_.width() ))
                )
                {
                    loc = belowPrimary;
                }
                else
                {
                    nothingClicked = true;
                }

                break;
            }
            case upperRight:
            {
                if( (mouseEvent->y() < ( primaryLabelSize_.height() + secondaryLabelSize_.height() + ( 2 * edgeOffset ) )) &&
                    (mouseEvent->x() > ( preview_->width() - primaryLabelSize_.width() - edgeOffset ))
                )
                {
                    loc = belowPrimary;
                }
                else
                {
                    nothingClicked = true;
                }

                break;
            }
            case lowerLeft:
            {
                if( (mouseEvent->y() < preview_->height() - primaryLabelSize_.height()) &&
                    (mouseEvent->x() < ( preview_->x() + secondaryLabelSize_.width() ) )
                )
                {
                    loc = abovePrimary;
                }
                else
                {
                    nothingClicked = true;
                }

                break;
            }
            case lowerRight:
            {
                if( (mouseEvent->y() < preview_->height() - primaryLabelSize_.height()) &&
                    (mouseEvent->x() > ( preview_->width() - secondaryLabelSize_.width() - ( 2 * edgeOffset ) ) )
                )
                {
                    loc = abovePrimary;
                }
                else
                {
                    nothingClicked = true;
                }

                break;
            }
            default:
            {
                nothingClicked = true;

                break;
            }
        }
    }

    return loc;
}

// user click event on single printer systems
corner LabelPlacementScreen::handleOnePrinterClickEvent(QMouseEvent *mouseEvent)
{
    corner loc( none );

    // click in top half of widget
    if ( mouseEvent->y() < (preview_->height() / 2) )
    {
        if ( mouseEvent->x() < (preview_->width() / 2) )
            loc = upperLeft;
        else
            loc = upperRight;
    }
    // click in bottom half of widget
    else
    {
        if ( mouseEvent->x() < (preview_->width() / 2) )
            loc = lowerLeft;
        else
            loc = lowerRight;
    }

    return loc;
}

void LabelPlacementScreen::writePlacementOffset(int printer)
{
    DomCurrentPLU *product( currentProduct() );

    if( printer == first )
    {
        operateMode_->writeSingleField( OperateMode::OffsetPrimaryLabelPlacement, QString::number( primaryLabelPos_ ), product->primaryLabelPlacementRaw() );
    }
    else if( printer == second )
    {
        operateMode_->writeSingleField( OperateMode::OffsetSecondLabelPlacement, QString::number( secondaryLabelPos_ ), product->secondLabelPlacementRaw() );
    }
}
