/**
 * @file marqueetexteditorwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueetexteditorwidget.h"
#include "marqueesyntaxhighlighter.h"

#include <marqueetextconstants.h>

#include <QTextCursor>
#include <QTextDocumentFragment>

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeTextEditorWidget::MarqueeTextEditorWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // make sure our images are ready
    Q_INIT_RESOURCE( marqueetexteditorwidget );

    if ( objectName().isEmpty() )
        setObjectName( "MarqueeTextEditorWidget" );

    // list of all known tags
    tags_.push_back( SCROLL );
    tags_.push_back( FRAME );
    tags_.push_back( ROLL );
    tags_.push_back( CLEAR );
    tags_.push_back( HOLD );
    tags_.push_back( SLOW );
    tags_.push_back( NORMAL );
    tags_.push_back( QUICK );
    tags_.push_back( EXTRA_QUICK );
    tags_.push_back( BLINK_PREFIX );
    tags_.push_back( BLINK_SUFFIX );

    tagLengthMaximum_ = 0;

    // figure out largest tag length
    foreach ( const QString& tag, tags_ )
        if ( tag.length() > tagLengthMaximum_ )
            tagLengthMaximum_ = tag.length();

    ///////////////////////////////////////////////////////////////////////////
    // setup layout

    QVBoxLayout *layout = new QVBoxLayout( this );

    layout->setMargin( 0 );

    ///////////////////////////////////////////////////////////////////////////
    // setup buttons at top

    QHBoxLayout *buttons = new QHBoxLayout();

    scrollButton_ = new QPushButton( QIcon( ":/images/Scroll2.gif" ), tr( "&Scroll" ), this );
    frameButton_ = new QPushButton( QIcon( ":/images/Frame2.gif" ), tr( "&Frame" ), this );
    blinkButton_ = new QPushButton( QIcon( ":/images/Blink2.gif" ), tr( "&Blink" ), this );
    rollButton_ = new QPushButton( QIcon( ":/images/Roll2.gif" ), tr( "&Roll" ), this );
    holdButton_ = new QPushButton( QIcon( ":/images/Hold2.gif" ), tr( "&Hold" ), this );
    clearButton_ = new QPushButton( QIcon( ":/images/Clear2.gif" ), tr( "&Clear" ), this );

    slowButton_ = new QPushButton( QIcon( ":/images/Slow2.gif" ), tr( "S&low" ), this );
    normalButton_ = new QPushButton( QIcon( ":/images/Norm2.gif" ), tr( "&Normal" ), this );
    quickButton_ = new QPushButton( QIcon( ":/images/Quick2.gif" ), tr( "&Quick" ), this );
    extraQuickButton_ = new QPushButton( QIcon( ":/images/XQuick2.gif" ), tr( "&X-Quick" ), this );

    // button names
    scrollButton_->setObjectName( "scrollButton" );
    frameButton_->setObjectName( "frameButton" );
    blinkButton_->setObjectName( "blinkButton" );
    rollButton_->setObjectName( "rollButton" );
    holdButton_->setObjectName( "holdButton" );
    clearButton_->setObjectName( "clearButton" );

    slowButton_->setObjectName( "slowButton" );
    normalButton_->setObjectName( "normalButton" );
    quickButton_->setObjectName( "quickButton" );
    extraQuickButton_->setObjectName( "extraQuickButton" );

    // buttons cannot receive focus
    scrollButton_->setFocusPolicy( Qt::NoFocus );
    frameButton_->setFocusPolicy( Qt::NoFocus );
    blinkButton_->setFocusPolicy( Qt::NoFocus );
    rollButton_->setFocusPolicy( Qt::NoFocus );
    holdButton_->setFocusPolicy( Qt::NoFocus );
    clearButton_->setFocusPolicy( Qt::NoFocus );

    slowButton_->setFocusPolicy( Qt::NoFocus );
    normalButton_->setFocusPolicy( Qt::NoFocus );
    quickButton_->setFocusPolicy( Qt::NoFocus );
    extraQuickButton_->setFocusPolicy( Qt::NoFocus );

    // these buttons are disabled by default
    blinkButton_->setEnabled( false );

    // set button icon size
    QSize buttonIconSize( 32, 32 );

    scrollButton_->setIconSize( buttonIconSize );
    frameButton_->setIconSize( buttonIconSize );
    blinkButton_->setIconSize( buttonIconSize );
    rollButton_->setIconSize( buttonIconSize );
    holdButton_->setIconSize( buttonIconSize );
    clearButton_->setIconSize( buttonIconSize );

    slowButton_->setIconSize( buttonIconSize );
    normalButton_->setIconSize( buttonIconSize );
    quickButton_->setIconSize( buttonIconSize );
    extraQuickButton_->setIconSize( buttonIconSize );

    // add them to the layout
    buttons->addWidget( scrollButton_ );
    buttons->addWidget( frameButton_ );
    buttons->addWidget( blinkButton_ );
    buttons->addWidget( rollButton_ );
    buttons->addWidget( holdButton_ );
    buttons->addWidget( clearButton_ );

    buttons->addWidget( slowButton_ );
    buttons->addWidget( normalButton_ );
    buttons->addWidget( quickButton_ );
    buttons->addWidget( extraQuickButton_ );

    layout->addLayout( buttons );

    // connect up the signals and slots
    connect( scrollButton_, SIGNAL( clicked() ), this, SLOT( onScrollButtonClicked() ) );
    connect( frameButton_, SIGNAL( clicked() ), this, SLOT( onFrameButtonClicked() ) );
    connect( blinkButton_, SIGNAL( clicked() ), this, SLOT( onBlinkButtonClicked() ) );
    connect( rollButton_, SIGNAL( clicked() ), this, SLOT( onRollButtonClicked() ) );
    connect( holdButton_, SIGNAL( clicked() ), this, SLOT( onHoldButtonClicked() ) );
    connect( clearButton_, SIGNAL( clicked() ), this, SLOT( onClearButtonClicked() ) );

    connect( slowButton_, SIGNAL( clicked() ), this, SLOT( onSlowButtonClicked() ) );
    connect( normalButton_, SIGNAL( clicked() ), this, SLOT( onNormalButtonClicked() ) );
    connect( quickButton_, SIGNAL( clicked() ), this, SLOT( onQuickButtonClicked() ) );
    connect( extraQuickButton_, SIGNAL( clicked() ), this, SLOT( onExtraQuickButtonClicked() ) );

    ///////////////////////////////////////////////////////////////////////////
    // setup text editor

    text_ = new QPlainTextEdit( this );
    text_->setObjectName( "text" );

    text_->setAcceptDrops( false );
    text_->setLineWrapMode( QPlainTextEdit::WidgetWidth );

    text_->installEventFilter( this );

    // install highlighter for tags
    marqueeSyntaxHighlighter *highlighter( new marqueeSyntaxHighlighter( text_->document() ) );

    // Note:
    // This variable is used by Qt even though it is not referenced in this method. This macro will
    // get rid of the compiler warning.
    Q_UNUSED( highlighter );

    // setup font
    QFont textFont( text_->font() );

    textFont.setPointSize( 15.0 );
    text_->setFont( textFont );

    layout->addWidget( text_ );

    // connect up the signals and slots
    connect( text_, SIGNAL( cursorPositionChanged() ), this, SLOT( onCursorPositionChanged() ) );
    connect( text_, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
    connect( text_, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeTextEditorWidget::~MarqueeTextEditorWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::translate()
{
    scrollButton_->setText( tr( "&Scroll") );
    frameButton_->setText( tr("&Frame") );
    blinkButton_->setText( tr( "&Blink") );
    rollButton_->setText( tr( "&Roll" ) );
    holdButton_->setText( tr( "&Hold" ) );
    clearButton_->setText( tr( "&Clear" ) );
    slowButton_->setText( tr( "S&low" ) );
    normalButton_->setText( tr( "&Normal" ) );
    quickButton_->setText( tr( "&Quick" ) );
    extraQuickButton_->setText( tr( "&X-Quick" ) );

    update();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QString MarqueeTextEditorWidget::text() const
{
    QString converted( toPlainText() );

    // some scales use different tag formats, here we will convert...
    converted.replace( SCROLL, "\\S" );
    converted.replace( FRAME, "\\F" );
    converted.replace( ROLL, "\\R" );
    converted.replace( CLEAR, "\\C" );
    converted.replace( HOLD, "\\H" );

    converted.replace( SLOW, "\\L" );
    converted.replace( NORMAL, "\\N" );
    converted.replace( QUICK, "\\Q" );
    converted.replace( EXTRA_QUICK, "\\X" );

    converted.replace( BLINK_PREFIX, "\\B" );
    converted.replace( BLINK_SUFFIX, "\\B" );

    return converted;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::setText( const QString& value )
{
    QString converted( value );

    // some scales use different tag formats, here we will convert...
    converted.replace( "\\S", SCROLL );
    converted.replace( "\\F", FRAME );
    converted.replace( "\\R", ROLL );
    converted.replace( "\\C", CLEAR );
    converted.replace( "\\H", HOLD );

    converted.replace( "\\L", SLOW );
    converted.replace( "\\N", NORMAL );
    converted.replace( "\\Q", QUICK );
    converted.replace( "\\X", EXTRA_QUICK );

    int index;
    int count = 0;

    while ( -1 != (index = converted.indexOf( "\\B" )) )
    {
        if ( 0 == (count%2) )
            converted.replace( index, 2, BLINK_PREFIX );
        else
            converted.replace( index, 2, BLINK_SUFFIX );

        ++count;
    }

    setPlainText( converted );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeTextEditorWidget::eventFilter( QObject *obj, QEvent *e )
{
    if ( text_ == obj )
    {
        if ( QEvent::KeyPress == e->type() )
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>( e );

            bool removeTag = false;
            bool moveTagEnd = false;
            bool updateCursor = false;

            int pos;
            int length;

            // when deleting a tag we must remove all of it
            if ( keyEvent->matches( QKeySequence::Delete ) )
                removeTag = isTag( Forward, pos, length );

            // when backspacing a tag we must remove all of it
            else if ( keyEvent->matches( QKeySequence::Back ) )
                removeTag = isTag( Backward, pos, length );

            // when using the right arrow key do not stop within a tag
            else if ( keyEvent->matches( QKeySequence::MoveToNextChar ) )
                moveTagEnd = isTag( Forward, pos, length );

            // remove current tag
            if ( removeTag )
            {
                QString text( text_->toPlainText() );
                text_->setPlainText( text.remove( pos, length ) );
                updateCursor = true;
            }
            // set position to tag end
            else if ( moveTagEnd )
            {
                QString text( text_->toPlainText() );

                while ( ++pos < text.length() )
                    if ( '>' == text.at( pos ) )
                    {
                        ++pos;
                        break;
                    }

                updateCursor = true;
            }

            // update the cursor position
            if ( updateCursor )
            {
                QTextCursor cursor( text_->textCursor() );
                cursor.setPosition( pos );
                text_->setTextCursor( cursor );
                return true;
            }
        }
        else if ( QEvent::FocusIn == e->type() )
        {
            Q_EMIT haveFocus();
        }
        else if ( QEvent::FocusOut == e->type() )
        {
            Q_EMIT lostFocus();
        }
    }

    // default event filter
    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::insertTag( const QString& tag )
{
    text_->insertPlainText( tag );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onCursorPositionChanged()
{
    int pos;
    int length;

    // do not allow cursor within tag
    if ( isTag( Within, pos, length ) )
    {
        QTextCursor cursor( text_->textCursor() );
        cursor.setPosition( pos );
        text_->setTextCursor( cursor );
    }

    Q_EMIT cursorPositionChanged();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onSelectionChanged()
{
    // allow/disallow markup depending upon the selected text
    blinkButton_->setEnabled( isSelectionValid() );

    Q_EMIT selectionChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onTextChanged()
{
    Q_EMIT textChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onScrollButtonClicked()
{
    insertTag( SCROLL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onFrameButtonClicked()
{
    insertTag( FRAME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onBlinkButtonClicked()
{
    // only mark for blinking the selected text
    if ( isSelectionValid() )
    {
        QTextCursor cursor = text_->textCursor();
        QString text = cursor.selection().toPlainText();

        insertTag( BLINK_PREFIX );
        cursor.insertText( text );
        insertTag( BLINK_SUFFIX );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onRollButtonClicked()
{
    insertTag( ROLL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onHoldButtonClicked()
{
    insertTag( HOLD );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onClearButtonClicked()
{
    insertTag( CLEAR );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onSlowButtonClicked()
{
    insertTag( SLOW );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onNormalButtonClicked()
{
    insertTag( NORMAL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onQuickButtonClicked()
{
    insertTag( QUICK );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTextEditorWidget::onExtraQuickButtonClicked()
{
    insertTag( EXTRA_QUICK );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeTextEditorWidget::isSelectionValid() const
{
    bool valid = false;

    QTextCursor cursor = text_->textCursor();

    // only allow blink button when text is selected
    if ( cursor.hasSelection() )
    {
        int count = 0;

        // make sure no tags in selection
        QString selected( cursor.selection().toPlainText() );

        foreach ( const QChar& c, selected )
        {
            if ( '<' == c )
                ++count;
            else if ( '>' == c )
                --count;
        }

        valid = (0 == count);
    }

    return valid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeTextEditorWidget::isTag( Direction d, int& pos, int& length ) const
{
    QString text( text_->toPlainText() );
    bool checkTags = false;

    pos = text_->textCursor().position();

    // check at cursor position looking forward
    if ( Forward == d )
    {
        if ( pos < text.length() )
            if ( '<' == text.at( pos ) )
                checkTags = true;
    }

    // check at cursor position looking backward
    else if ( Backward == d )
    {
        if ( 0 <= --pos )
            if ( '>' == text.at( pos ) )
            {
                // find tag start
                d = Within;
            }
    }

    // check at cursor position all around
    if ( Within == d )
    {
        int posEnd = pos - tagLengthMaximum_;

        if ( posEnd < 0 )
            posEnd = 0;

        while ( posEnd <= --pos )
        {
            if ( '>' == text.at( pos ) )
                break;
            else if ( '<' == text.at( pos ) )
            {
                checkTags = true;
                break;
            }
        }
    }

    // check the position against known tags
    if ( checkTags )
    {
        foreach ( const QString& tag, tags_ )
            if ( tag == text.mid( pos, (length = tag.length()) ) )
                 return true;
    }

    return false;
}

