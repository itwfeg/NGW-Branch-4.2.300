#include <logmanager.h>

#include "superLabelPlacementView.h"
#include "superChangeProductAllFields.h"
#include "superProductDefaults.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledpluedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <QComboBox>
#include <QDial>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QListView>

///////////////////////////////////////////////////////////////////////////////////////////////////
superLabelPlacementView::superLabelPlacementView( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ignoreSignals_( false ),
    registered_( false ),
    labelPlacementFromProduct_( false ),
    doneButtonClicked_( false ),
    printerCount_( 0 ),
    prevPeripheralList_( new MadpPeripheralList() ),

    // not the actual physical size of label but how it is displayed in UI
    primaryLabelSize_(80, 125),
    secondaryLabelSize_(80, 80)
{
    // Ways to skip saving data
    // 1: Leave by hitting the Home button - Make them click DONE to leave
    // 2: LEave by clicking another screen location - Make them click DONE to leave

    evolWrapperConfig_ = &session_->scaleConfig->evolWrapperConfig;
    evolWrapperSystemInformation_ = &session_->scaleConfig->evolWrapperSystemInformation;
    peripheralList_ = &session_->scaleConfig->peripheralList;
    scaleConfig_ = &session_->scaleConfig->scaleConfig;
    product_ = &session_->scaleConfig->currentPLU;

    supervisorMode_ = dynamic_cast<SupervisorMode*>( parent );

    // reserve 3 printers even though we really only use up to 2...
    printers_.resize( 3 );

    for ( int i( 0 ); i < printers_.size(); ++i )
    {
        printers_[i].installed = false;
        printers_[i].orientation = 0;
        printers_[i].placement = 0;
        printers_[i].rotation = 0;
    }

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    // initialize
    initialize();
    createLayout( Qt::RightDockWidgetArea );

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superLabelPlacementView::~superLabelPlacementView()
{
    if ( prevPeripheralList_ )
    {
        delete prevPeripheralList_;
        prevPeripheralList_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superLabelPlacementView::translate()
{
    static const bool convertUpperCase( false );

    QString printerText( tr( "Printer" ) );
    QString labelPlacementText( tr( "Label Position" ) );
    QString labelRotationText( tr( "Label Rotation" ) );

    if ( convertUpperCase )
    {
        QList<QString*> strings;

        strings.append( &printerText );
        strings.append( &labelPlacementText );
        strings.append( &labelRotationText );

        foreach ( QString *s, strings )
            (*s) = s->toUpper();
    }

    printerLabel_->setStyleSheet("QLabel { color: white; }");
    printerLabel_->setText( printerText );

    printer_->setItemText( 0, tr( "Primary" ) );
    printer_->setItemText( 1, tr( "Secondary" ) );

    labelPlacementLabel_->setStyleSheet("QLabel { color: white; }");
    labelPlacementLabel_->setText( labelPlacementText );

    labelPlacement_->setItemText( 0, tr( "Upper Right" ) );
    labelPlacement_->setItemText( 1, tr( "Upper Left" ) );
    labelPlacement_->setItemText( 3, tr( "Lower Right" ) );
    labelPlacement_->setItemText( 4, tr( "Lower Left" ) );
    labelPlacement_->setItemText( 5, tr( "Right of Primary" ) );
    labelPlacement_->setItemText( 6, tr( "Left of Primary" ) );
    labelPlacement_->setItemText( 7, tr( "Above Primary" ) );
    labelPlacement_->setItemText( 8, tr( "Below Primary" ) );

    labelPlacement_->setItemText( 2, tr( "System Default" ) );

    labelRotationLabel_->setStyleSheet("QLabel { color: white; }");
    labelRotationLabel_->setText( labelRotationText );

    _Mybase::translate();
}

corner superLabelPlacementView::calculateLabelPlacement( int printer ) const
{
    if (( 0 <= printer ) && ( printer < printerCount_ ))
    {
        return ((0 == printer) ? primaryLabelPos_ : secondaryLabelPos_);
    }

    return none;
}

bool superLabelPlacementView::eventFilter( QObject *obj, QEvent *e )
{
    // check for click in preview area
    if (( preview_ == obj ) && ( QEvent::MouseButtonRelease == e->type() ))
    {
        QMouseEvent *mouseEvent;

        int edgeOffset = 20;
        bool nothingClicked = false;
        if ( (mouseEvent = dynamic_cast<QMouseEvent*>( e )) )
        {
            corner loc( none );

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

            // two cases to handle:
            // a) 2 printers, in which case we are selecting a printer
            // b) 1 printer, in which case we are picking a corner for the label
            if (( 2 <= printerCount_ ) && ( printer_->isEnabled() ))
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

                            break;
                        }
                    }

                    if(!printerClicked)
                    {
                        if(printer_->currentIndex() == first)
                        {
                            if( loc < none )
                            {
                                primaryLabelMoved_ = true;
                                previousPrimaryPosition_ = primaryLabelPos_;
                                primaryLabelPos_ = loc;

                                if( secondaryLabelPos_ >= rightOfPrimary)
                                {
                                    updateSecondaryOnPrimaryMove();
                                }
                            }
                            else
                            {
                                primaryLabelMoved_ = false;
                                //loc = previousPrimaryPosition_;
                            }
                        }
                        else if(printer_->currentIndex() == second)
                        {
                            secondaryLabelMoved_ = true;
                            previousSecondaryPosition_ = secondaryLabelPos_;
                            secondaryLabelPos_ = loc;
                        }

                        updateInterfaceInternal( true );
                    }
                }
            }
        }
    }

    return _Mybase::eventFilter( obj, e );
}

void superLabelPlacementView::requestConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superLabelPlacementView::onComboBoxCurrentIndexChanged( QWidget *widget )
{
    LOG_CLASS_INFO( "superLabelPlacementView", "onComboBoxCurrentIndexChanged" );

    // nothing to do
    if ( ignoreSignals_ )
        return;
    else if ( !product_ )
        return;


    // process label placement
    if ( labelPlacement_ == widget )
    {
        LOG_INFO("LabelPlacement == widget", NULL);

        // primary printer
        if ( printers_[first].installed )
        {
            if(printer_->currentIndex() == first)
            {
                LOG_INFO("Updating first printer placement", NULL);
                QString newValue( QString::number( primaryLabelPos_ ) );
                //QString oldValue( QString::number( product_->primaryLabelPlacement() ) );

                primaryLabelPos_ = (corner)labelPlacement_->currentIndex();
                printers_[first].placement = primaryLabelPos_;

                if( secondaryLabelPos_ > rightOfPrimary )
                {
                    updateSecondaryOnPrimaryMove();
                }
            }

        }

        // second printer
        if ( printers_[second].installed )
        {
            // need to check if secondary label moved since it could have moved with the primay label
            if(printer_->currentIndex() == second)
            {
                LOG_INFO("Updating second printer placement", NULL);
                QString newValue( QString::number( secondaryLabelPos_ ) );
                //QString oldValue( QString::number( product_->secondLabelPlacement() ) );

                secondaryLabelPos_ = (corner)labelPlacement_->currentIndex();
                printers_[second].placement = secondaryLabelPos_;
            }
        }
    }

    LOG_INFO("onComboBoxCurrentIndexChanged ending", NULL);

    // update interface
    updateInterfaceInternal( true );
}

void superLabelPlacementView::onLabelRotationChanged( int value )
{
    LOG_CLASS_INFO( "superLabelPlacementView", "onLabelRotationChanged" );

    // nothing to do
    if ( ignoreSignals_ )
        return;
    else if ( !product_ )
        return;

    int angle( (((value * 360) / labelRotation_->maximum()) + 180) % 360 );
    int selected( printer_->currentIndex() );

    LOG_DEBUG( "rotation of printer %d changed to %d", selected, angle );
    printers_[selected].rotation = angle;

    if( selected == first )
    {
        primaryRotation_ = angle;
    }
    else if( selected == second )
    {
        secondaryRotation_ = angle;
    }

    // update interface
    updateInterfaceInternal( true );
}

void superLabelPlacementView::updateInterface()
{
    // no product
    if ( !product_ )
    {
        return;
    }

    if( supervisorMode_->cameFromProductDefaults() )
    {
        //int primary = session_->scaleConfig->primaryDefaultLabelType;
        if( session_->scaleConfig->primaryLabelPlacementDefault == none )
        {
            primaryLabelPos_ = ( corner )session_->scaleConfig->primaryPlacementSystem;
        }
        else
        {
            primaryLabelPos_ = ( corner )session_->scaleConfig->primaryLabelPlacementDefault;
        }

        //int secondary = session_->scaleConfig->secondaryDefaultLabelType;
        if( session_->scaleConfig->secondaryLabelPlacementDefault == none ) // system default
        {
            secondaryLabelPos_ = ( corner )session_->scaleConfig->secondaryPlacementSystem;
        }
        else
        {
            secondaryLabelPos_ = ( corner )session_->scaleConfig->secondaryLabelPlacementDefault;
        }

        primaryRotation_ = session_->scaleConfig->primaryLabelRotationDefault;
        secondaryRotation_ = session_->scaleConfig->secondaryLabelRotationDefault;
        printers_[0].rotation = session_->scaleConfig->primaryLabelRotationDefault;
        printers_[1].rotation = session_->scaleConfig->secondaryLabelRotationDefault;
    }
    else
    {

        if( product_->primaryLabelPlacement() == none ) // system default
        {
            primaryLabelPos_ = (corner)session_->scaleConfig->primaryPlacementSystem;
        }
        else
        {
            primaryLabelPos_ = ( corner )product_->primaryLabelPlacement();
        }

        if( product_->secondLabelType() > 0 || product_->secondDefaultLabelType() > 0 )
        {
            if( product_->secondLabelPlacement() == none ) // system default
            {
                secondaryLabelPos_ = ( corner )session_->scaleConfig->secondaryPlacementSystem;
            }
            else
            {
                secondaryLabelPos_ = ( corner )product_->secondLabelPlacement();
            }
        }

        printers_[0].rotation = product_->primaryLabelRotation();
        printers_[1].rotation = product_->secondLabelRotation();

        printers_[0].labelStockUpsideDown = (0 != product_->labelStockRotation1());
        printers_[1].labelStockUpsideDown = (0 != product_->labelStockRotation2());

        primaryRotation_   = product_->primaryLabelRotation();
        secondaryRotation_ = product_->secondLabelRotation();
    }

    // update interface
    updateInterfaceInternal( true );
}

void superLabelPlacementView::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    updateInterface();
}

void superLabelPlacementView::initialize()
{
    //LOG_CLASS_INFO( "LabelPlacementScreen", "initialize" );

    static const QMargins margins( 10, 10, 10, 6 );

    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );

    const hobart::backgroundWidgetRenderer frameRenderer(
        new hobart::rectangleBackgroundWidget(),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance()
    );

    comboBoxCurrentIndexChanged_ = new QSignalMapper( this );

    connect( comboBoxCurrentIndexChanged_, SIGNAL( mapped(QWidget*) ), SLOT( onComboBoxCurrentIndexChanged(QWidget*) ) );

    //
    // messages
    //

    const QSize messageFrameSize( mainWindow_->width() - 100, 50 );

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

    labelPlacement_->addItem( "", 0 );
    labelPlacement_->addItem( "", 1 );
    labelPlacement_->addItem( "", 3 );
    labelPlacement_->addItem( "", 4 );
    labelPlacement_->addItem( "", 5 );
    labelPlacement_->addItem( "", 6 );
    labelPlacement_->addItem( "", 7 );
    labelPlacement_->addItem( "", 8 );

    //labelPlacement_->addItem( "", 2 );

    connect( labelPlacement_, SIGNAL( currentIndexChanged(int) ), comboBoxCurrentIndexChanged_, SLOT( map() ) );
    comboBoxCurrentIndexChanged_->setMapping( labelPlacement_, labelPlacement_ );

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

    doneButton = new QPushButton( labelFrame_ );
    doneButton->setFixedSize( QSize( 250, 40 ) );
    doneButton->setFocusPolicy( Qt::NoFocus );
    doneButton->setObjectName( "doneButton" );
    doneButton->setText("Done");

    connect( doneButton, SIGNAL( clicked() ), SLOT( doneButtonClicked() ) );

    QList<QComboBox*> comboBoxes;
    comboBoxes.append( printer_ );
    comboBoxes.append( labelPlacement_ );

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
        comboBox->setEnabled( true );
    }

    // enable everything
    // this screen is only on two printer systems
    labelRotation_->setEnabled( true );
    doneButton->setEnabled( true );
    labelPlacement_->setEnabled( true );
    printer_->setEnabled( true );
    labelRotation_->setEnabled( true );

    //plu_->setTotalTypeVisible( scaleConfig_->showTotalTypeOnRunScreen() ); // HA-176

    // initial label positions
    if(supervisorMode_->cameFromProductDefaults())
    {
        primaryLabelPos_ = upperLeft;
        secondaryLabelPos_ = upperRight;
    }
    else
    {
        primaryLabelPos_ = (corner)product_->primaryLabelPlacement();
        secondaryLabelPos_ = (corner)product_->secondLabelPlacement();
    }

    primaryLabelMoved_ = false;
    secondaryLabelMoved_ = false;

    primaryRotation_ = 0;
    secondaryRotation_ = 0;

    for ( int i( 0 ); i < maxLabels; ++i )
    {
        // this screen only updates the product record, and is on two printer systems only
        // so we can go ahead and tell it both printers are installed
        printers_[i].installed = true;
        printers_[i].orientation = 0;
        printers_[i].placement = 0;
        printers_[i].rotation = 0;
    }
    printerCount_ = 2;

    //defineCornerLocations();
}

void superLabelPlacementView::createLayout( Qt::DockWidgetArea numPadLocation )
{
    // delete previous layout
    if ( labelFrame_->layout() )
        delete labelFrame_->layout();

    QVBoxLayout *center( new QVBoxLayout() );
    center->addWidget( printerLabel_ );
    center->addWidget( printer_ );
    center->addWidget( labelPlacementLabel_ );
    center->addWidget( labelPlacement_ );
    center->addStretch();
    center->addWidget( labelRotationLabel_ );
    center->addWidget( labelRotation_ );
    center->addWidget( doneButton );

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

    //top->addWidget( plu_ );

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

void superLabelPlacementView::updateInterfaceInternal( bool enabled )
{
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

        doneButton->setEnabled( false );
    }
    else
    {
        int selected( printer_->currentIndex() );

        // enable controls
        printer_->setEnabled( 1 < printerCount_ /*true*/ );

        labelPlacement_->setEnabled( false );
        labelPlacement_->setCurrentIndex( selected == first ? primaryLabelPos_ : secondaryLabelPos_ );

        labelRotation_->setEnabled( true );
        labelRotation_->setMaximum( 4 );
        labelRotation_->setValue( (labelRotation_->maximum() * ((printers_[selected].rotation + 180) % 360)) / 360 );

        doneButton->setEnabled( true );
    }

    generatePreview();


    ignoreSignals_ = false;
}

void superLabelPlacementView::generatePreview()
{
    preview_->generatePreview(printerCount_, printer_->currentIndex(), primaryLabelPos_, secondaryLabelPos_, primaryLabelSize_, secondaryLabelSize_, printers_);
}

void superLabelPlacementView::updateSecondaryOnPrimaryMove()
{
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
        else if( (primaryLabelPos_ == lowerLeft || primaryLabelPos_ == lowerRight) &&
                 (secondaryLabelPos_ == belowPrimary)
        )
        {
            // just change the position to above
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = abovePrimary;
            secondaryLabelMoved_ = true;
        }
        else if( (primaryLabelPos_ == upperLeft || primaryLabelPos_ == lowerLeft) &&
                 (secondaryLabelPos_ == leftOfPrimary)
        )
        {
            // just change the position to right
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = rightOfPrimary;
            secondaryLabelMoved_ = true;
        }
        else if( (primaryLabelPos_ == upperRight || primaryLabelPos_ == lowerRight) &&
                 (secondaryLabelPos_ == rightOfPrimary)
        )
        {
            // just change the position to left
            previousSecondaryPosition_ = secondaryLabelPos_;
            secondaryLabelPos_ = leftOfPrimary;
            secondaryLabelMoved_ = true;
        }
    }
    // otherwise we don't need to do anything
}

void superLabelPlacementView::doneButtonClicked()
{
    doneButton->setEnabled( false );
    doneButtonClicked_ = true;

    if( supervisorMode_->cameFromProductDefaults() )
    {
        session_->scaleConfig->primaryLabelPlacementDefault = primaryLabelPos_;
        session_->scaleConfig->secondaryLabelPlacementDefault = secondaryLabelPos_;
        session_->scaleConfig->primaryLabelRotationDefault = primaryRotation_;
        session_->scaleConfig->secondaryLabelRotationDefault = secondaryRotation_;

        supervisorMode_->setCameFromLabelPlacementView(true);
        supervisorMode_->setCameFromProductMarqueeEditor(false);
        supervisorMode_->setCameFromProductNutritionEditor(false);

        //mainWindow_->switchToModeScreen<SupervisorMode, SuperProductDefaults>();
    }
    else
    {
        session_->scaleConfig->primaryLabelPlacement = primaryLabelPos_;
        session_->scaleConfig->secondaryLabelPlacement = secondaryLabelPos_;
        session_->scaleConfig->primaryLabelRotation = primaryRotation_;
        session_->scaleConfig->secondaryLabelRotation = secondaryRotation_;

        supervisorMode_->setCameFromLabelPlacementView(true);
        supervisorMode_->setCameFromProductMarqueeEditor(false);
        supervisorMode_->setCameFromProductNutritionEditor(false);

        //mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
    }

    product_->primaryLabelPlacementRaw() = QString::number( session_->scaleConfig->primaryLabelPlacement );
    product_->secondLabelPlacementRaw()  = QString::number( session_->scaleConfig->secondaryLabelPlacement );
    product_->primaryLabelRotationRaw()  = QString::number( session_->scaleConfig->primaryLabelRotation );
    product_->secondLabelRotationRaw()   = QString::number( session_->scaleConfig->secondaryLabelRotation );

    if( supervisorMode_->cameFromProductDefaults() )
    {
        mainWindow_->switchToModeScreen<SupervisorMode, SuperProductDefaults>();
    }
    else
    {
        mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool superLabelPlacementView::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::onScreenChanging()";
#endif

    if ( !supervisorMode_->cameFromProductDefaults() &&  !doneButtonClicked_ )
    {

        QString errStr = QString::number(ProductDevError::ProductErrSaveWithDoneButtonOnly) + ",5";
        session_->send( Error, ErrProductDevice, errStr );      // MUST click DONE to leave and save - or nto save
        return false;
    }

    return _Mybase::onScreenChanging();
}
