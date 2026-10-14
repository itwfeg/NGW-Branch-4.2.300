/**
 * @file labeledlabelviewer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledlabelviewer.h"

#include "backgroundwidgettypes.h"
#include "pushbutton.h"

#include <QDir>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLabelViewer::labeledLabelViewer( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    file_( QDir::tempPath() + "/" + "XXXXXX.png" )
{
    setObjectName( "hobartLabeledLabelViewer" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLabelViewer::~labeledLabelViewer()
{
    if ( renderer_ )
    {
        renderer_->deleteLater();
        renderer_ = NULL;
    }

    if ( rendererZoomed_ )
    {
        rendererZoomed_->deleteLater();
        rendererZoomed_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelPointer labeledLabelViewer::label() const
{
    return renderer_->label();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledLabelViewer::printButtonVisible() const
{
    return printButton_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledLabelViewer::readOnly() const
{
    return renderer_->readOnly();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm *labeledLabelViewer::selected() const
{
    return renderer_->selected();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setContentsMargins( const QMargins& value )
{
    // move margins into the top and bottom layouts
    topLayout_->setContentsMargins( QMargins( value.left(), value.top(), value.right(), 0 ) );
    bottomLayout_->setContentsMargins( QMargins( 6, 0, 6, 6 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setLabel( const ::label& value )
{
    if ( renderer_ )
        renderer_->setLabel( value );

    child_->render();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setLabelZoomed( const ::label& value )
{
    if ( rendererZoomed_ )
        rendererZoomed_->setLabel( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setNutrifactsId( int value )
{
    if ( renderer_ )
        renderer_->setNutrifactsId( value );

    if ( rendererZoomed_ )
        rendererZoomed_->setNutrifactsId( value );

    child_->render();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setPrintButtonVisible( bool value )
{
    printButton_->setVisible( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setReadOnly( bool value )
{
    if ( renderer_ )
        renderer_->setReadOnly( value );

    if ( rendererZoomed_ )
        rendererZoomed_->setReadOnly( value );

    child_->render();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::setStore( const dataStoreBase *value )
{
    if ( renderer_ )
        renderer_->setStore( value );

    if ( rendererZoomed_ )
        rendererZoomed_->setStore( value );

    child_->render();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::resetLabel()
{
    if ( renderer_ )
        renderer_->resetLabel();

    if ( rendererZoomed_ )
        rendererZoomed_->resetLabel();

    child_->render();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledLabelViewer::saveRenderedImage()
{
    QImage image( renderer_->toImage() );
    QString fileName;

    // retrieve rendered image
    if ( !image.isNull() )
    {
        // save to file
        if (( file_.open() ) && ( image.save( &file_, "png" ) ))
            fileName = file_.fileName();

        file_.close();
    }

    return fileName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledLabelViewer::saveZoomedRenderedImage()
{
    QImage image( rendererZoomed_->toImage() );
    QString fileName;

    // retrieve rendered image
    if ( !image.isNull() )
    {
        // save to file
        if (( file_.open() ) && ( image.save( &file_, "png" ) ))
            fileName = file_.fileName();

        file_.close();
    }

    return fileName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledLabelViewer::eventFilter( QObject *obj, QEvent *event )
{
    // do not allow renderers to become visible
    if ( renderer_ == obj )
    {
        if ( QEvent::Show == event->type() )
            return true;
    }
    else if ( rendererZoomed_ == obj )
    {
        if ( QEvent::Show == event->type() )
            return true;
    }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::initialize()
{
    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 2 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    renderer_ = new LabelRenderer( true );
    renderer_->installEventFilter( this );
    renderer_->hide();

    // connect slots
    connect( renderer_, SIGNAL( labelChanged() ), SIGNAL( labelChanged() ) );
    connect( renderer_, SIGNAL( labelFormSelected() ), SIGNAL( labelFormSelected() ) );

    rendererZoomed_ = new LabelRenderer( true );
    rendererZoomed_->setPreviewMode( true );
    rendererZoomed_->installEventFilter( this );
    rendererZoomed_->hide();

    // setup child widget
    child_->setRenderer( renderer_ );

    connect( child_, SIGNAL( clicked() ), SIGNAL( zoomButtonClicked() ) );

    // print button
    QPixmap printButtonIcon( ":/images/numberPadPrintIcon.png" );

    printButton_ = new pushButton( printButtonIcon, QString(), this, renderButton.drawControl( QSize( (height() / 4) - 20, (height() / 4) - 20 ), hobart::ReadOnly ) );
    printButton_->setVisible( false );

    printButton_->setFocusPolicy( Qt::NoFocus );
    printButton_->setIconSize( QSize( 48, 48 ) );

    connect( printButton_, SIGNAL( clicked() ), SIGNAL( printButtonClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLabelViewer::createLayout()
{
    // create layout for button
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setContentsMargins( QMargins() );

    buttonLayout->addWidget( printButton_ );
    buttonLayout->addStretch();

    // add widgets to layout
    topLayout_->addSpacing( 5 );
    topLayout_->addLayout( buttonLayout );

    // fix the stretching and spacing
    // default layout has a stretch between widgets, lets remove it and put up top
    QLayoutItem *stretch = widgetsLayout_->takeAt( 1 );
    topLayoutInner_->addItem( stretch );

    // need to add a spacer now that the stretch is gone
    widgetsLayout_->insertSpacing( 1, 3 );

    // the preview should stretch the entire widget
    widgetsLayout_->setStretchFactor( bottomLayout_, INT_MAX );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

