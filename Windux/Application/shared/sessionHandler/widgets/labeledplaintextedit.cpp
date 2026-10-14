/**
 * @file labeledplaintextedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledplaintextedit.h"

#include "backgroundwidgettypes.h"
#include "pushbutton.h"

#include "../sessioninfo.h"

#include "../dialogs/expandedtextdialog.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// expandedTextIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPlainTextEdit::expandedTextIcon::expandedTextIcon( QWidget *parent ) :
    _Mybase( parent )
{
    setPixmap( QPixmap( ":/images/screens/downArrowIcon.png" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPlainTextEdit::expandedTextIcon::~expandedTextIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// labeledPlainTextEdit
//
///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPlainTextEdit::labeledPlainTextEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    expandable_( true ),
    expandedTextIconVisible_( WhenToBigToFit ),
    sizeToFit_( false )
{
    setObjectName( "hobartLabeledPlainTextEdit" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPlainTextEdit::labeledPlainTextEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image ),
    expandable_( true ),
    expandedTextIconVisible_( WhenToBigToFit ),
    sizeToFit_( false )
{
    setObjectName( "hobartLabeledPlainTextEdit" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPlainTextEdit::~labeledPlainTextEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledPlainTextEdit::printButtonVisible() const
{
    return printButton_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::setExpandedTextIconVisible( IconVisiblityMode value )
{
    expandedTextIconVisible_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::setPrintButtonVisible( bool value )
{
    printButton_->setVisible( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::setText( const QString& value )
{
    // icon not visible by default
    expandedTextIcon_->setVisible( false );

    // set text
    child_->setPlainText( value );

    // set fixed size
    if ( sizeToFit_ )
    {
        static const int textMargin = 4;

        // compute bounding rect
        QRect rect( 0, 0, INT_MAX, INT_MAX );

        QFontMetrics metrics( child_->font() );
        QRect boundingRect( metrics.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, value ) );

        // set new height
        child_->setFixedHeight( boundingRect.height() + (2 * textMargin) );
    }

    // set icon visibility
    else if ( Always == expandedTextIconVisible_ )
    {
        // FT-1307: always allow the text to be enlarged
        expandedTextIcon_->setVisible( value.length() ? true : false );
    }

    // set icon visibility
    else if ( WhenToBigToFit == expandedTextIconVisible_ )
    {
        // compute bounding rect
        QRect rect( child_->rect() );
        rect.setHeight( INT_MAX );

        QFontMetrics metrics( child_->font() );
        QRect boundingRect( metrics.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, value ) );

        // allow the widget a little extra room for 2/3 visible lines
        int widgetHeight = child_->height() + (metrics.lineSpacing() / 3);

        if ( widgetHeight < boundingRect.height() )
            expandedTextIcon_->setVisible( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledPlainTextEdit::eventFilter( QObject *obj, QEvent *event )
{
    bool result = _Mybase::eventFilter( obj, event );

    if ( child_ == obj )
        if (( QEvent::FocusIn == event->type() ) || ( QEvent::FocusOut == event->type() ))
        {
            // check access level
            if ( QEvent::FocusIn == event->type() )
                if ( !checkOperatorAccessLevel() )
                {
                    Q_EMIT invalidAccessLevel();

                    // eat the event
                    return true;
                }

            updateBackgroundPalette();

            Q_EMIT focused( (QEvent::FocusIn == event->type()), geometry() );
        }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::mousePressEvent( QMouseEvent *e )
{
    // when widget is read only clicking anywhere is same as clicking expand icon
    if ( Qt::LeftButton == e->button() )
        if (( isReadOnly() ) && ( expandable_ ) && ( expandedTextIcon_->isVisible() ))
        {
            // pop the expanded text dialog
            timer_->setSingleShot( true );
            timer_->start( 0 );
        }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::onExpandedTextIconClicked()
{
    // get session info
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();

    // create dialog
    if ( mainWindow )
    {
        ExpandedTextDialog dialog( mainWindow );

        QPoint pos( mapTo( mainWindow, QPoint( 0, 0 ) ) );

        // FT-1307: make font of popup bigger
        QFont f( font() );
        f.setPointSize( 16 );

        int leftOffset = pos.x();
        int topOffset = pos.y();

        dialog.setDialogAlignment( Qt::AlignLeft );
        dialog.setDialogAlignmentOffset( leftOffset );

        // pop dialog so it looks like field is expanding down
        if ( (topOffset + dialog.height()) < mainWindow->height() )
            dialog.setDialogOffset( topOffset );

        // align dialog to bottom edge
        else
        {
            dialog.setDialogOffset( mainWindow->height() - dialog.height() );
        }

        dialog.setText( text() );
        dialog.setTextFont( f ); // FT-1307

        dialog.exec();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::initialize()
{
    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 2 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    modifyPaletteBase setBackground( Qt::transparent );
    setBackground( child_ );

    child_->setBackgroundVisible( false );
    child_->setFrameShape( QFrame::NoFrame );
    child_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    child_->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // connect slots
    connect( child_, SIGNAL( blockCountChanged(int) ), SIGNAL( blockCountChanged(int) ) );
    connect( child_, SIGNAL( copyAvailable(bool) ), SIGNAL( copyAvailable(bool) ) );
    connect( child_, SIGNAL( cursorPositionChanged() ), SIGNAL( cursorPositionChanged() ) );
    connect( child_, SIGNAL( modificationChanged(bool) ), SIGNAL( modificationChanged(bool) ) );
    connect( child_, SIGNAL( redoAvailable(bool) ), SIGNAL( redoAvailable(bool) ) );
    connect( child_, SIGNAL( selectionChanged() ), SIGNAL( selectionChanged() ) );
    connect( child_, SIGNAL( textChanged() ), SIGNAL( textChanged() ) );
    connect( child_, SIGNAL( undoAvailable(bool) ), SIGNAL( undoAvailable(bool) ) );
    connect( child_, SIGNAL( updateRequest(QRect,int) ), SIGNAL( updateRequest(QRect,int) ) );

    // install event filter
    child_->installEventFilter( this );

    // create timer
    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onExpandedTextIconClicked() ) );

    // drop down icon
    expandedTextIcon_ = new expandedTextIcon( this );
    expandedTextIcon_->setObjectName( "expandedTextIcon" );
    expandedTextIcon_->setVisible( false );

    connect( expandedTextIcon_, SIGNAL( clicked() ), SLOT( onExpandedTextIconClicked() ) );

    // print button
    QPixmap printButtonIcon( ":/images/numberPadPrintIcon.png" );

    printButton_ = new pushButton( printButtonIcon, QString(), this, renderButton.drawControl( QSize( height() - 20, height() - 20 ), hobart::ReadOnly ) );
    printButton_->setObjectName( "printButton" );
    printButton_->setVisible( false );

    printButton_->setFocusPolicy( Qt::NoFocus );
    printButton_->setIconSize( QSize( 48, 48 ) );

    connect( printButton_, SIGNAL( clicked() ), SIGNAL( printButtonClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPlainTextEdit::createLayout()
{
    // create layout for icon
    QVBoxLayout *iconLayout = new QVBoxLayout();
    iconLayout->setContentsMargins( QMargins() );

    iconLayout->addStretch();
    iconLayout->addWidget( expandedTextIcon_ );
    iconLayout->addSpacing( 10 );

    // create layout for button
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setContentsMargins( QMargins() );

    buttonLayout->addWidget( printButton_ );
    buttonLayout->addStretch();

    // add widgets to layout
    screenLayoutInner_->addSpacing( 5 );
    screenLayoutInner_->addLayout( iconLayout );
    screenLayoutInner_->addSpacing( 5 );
    screenLayoutInner_->addLayout( buttonLayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
