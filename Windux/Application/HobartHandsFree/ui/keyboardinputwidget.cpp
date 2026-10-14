/**
 * @file keyboardinputwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "keyboardinputwidget.h"

#include <QtDebug>

#include <QBitmap>
#include <QHBoxLayout>
#include <QLocale>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalMapper>
#include <QTextEdit>
#include <QVBoxLayout>

static const QString KEY_BACKSPACE( "backspace" );
static const QString KEY_CAPS( "capslock" );
static const QString KEY_CLEAR( "clear" );
static const QString KEY_DISMISS( "dismiss" );
static const QString KEY_RETURN( "enter" );
static const QString KEY_SPACE( "space" );
static const QString KEY_SPC_CHARS( "special" );

const int keyboardInputWidget::specialCharsModeCount_ = 2;

const QSize keyboardInputWidget::fixedSize_( 940, 230 );
const int keyboardInputWidget::fontPointSize_ = 24;

const QSize keyboardInputWidget::buttonIconSize_( 40, 40 );

///////////////////////////////////////////////////////////////////////////////////////////////////
keyboardInputWidget::keyboardInputWidget( QWidget *parent ) :
    _Mybase( parent ),
    signalMapper_( NULL ),
    layoutStyle_( CalculatorLayoutStyle ),
    capsActive_( false ),
    specialCharsMode_( 0 )
{
    setObjectName( "keyboardInputWidget" );

    // set fixed size
    setFixedSize( fixedSize_ );

    // set font
    QFont f( font() );
    f.setBold( true );
    f.setPointSize( fontPointSize_ );
    setFont( f );

    // set palette
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::darkGray );
    setPalette( p );

    // init
    initialize();
    createLayout();
    groom();

    // set keys
    updateKeys();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
keyboardInputWidget::~keyboardInputWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::setLayoutStyle( LayoutStyle value )
{
    if ( value == layoutStyle_ )
        return;

    layoutStyle_ = value;
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::setLocale( const QLocale &locale )
{
    _Mybase::setLocale( locale );

    // update keyboard keys
    updateKeys();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::activate( QWidget *widget )
{
    QWidget *screen;

    // adjust keyboard geometry above or below widget
    if (( widget ) && ( (screen = widget->parentWidget()) ))
    {
        QPlainTextEdit *plainTextEdit;
        QTextEdit *textEdit;

        // find top level widget
        while ( screen->parentWidget() )
            screen = screen->parentWidget();

        // calculate top left corner of input widget
        QPoint topLeft( 0, 0 );

        // for multi line edit use cursor position instead of widget location
        if ( (plainTextEdit = dynamic_cast<QPlainTextEdit*>( widget )) )
            topLeft = plainTextEdit->cursorRect().topLeft();
        else if ( (textEdit = dynamic_cast<QTextEdit*>( widget )) )
            topLeft = textEdit->cursorRect().topLeft();

        // compute vertical offset
        const QPoint widgetPos( widget->mapTo( screen, topLeft ) );

        int verticalOffset( 0 );

        // check if widget position is on top half of the screen
        if ( widgetPos.y() < (screen->height() / 2) )
            verticalOffset = screen->height() - height();

        // move keyboard!
        move( (screen->width() - width()) / 2, verticalOffset );
    }

    // invoke base
    _Mybase::activate( widget );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::onKeyClicked( QWidget *widget )
{
    if ( widget == keys_[KEY_BACKSPACE] )
    {
        qDebug() << "backspace";

        // emit backspace key
        Q_EMIT characterGenerated( Qt::Key_Backspace, Qt::NoModifier );
    }
    else if ( widget == keys_[KEY_CAPS] )
    {
        // toggle caps
        capsActive_ = !capsActive_;

        // update
        updateKeys();
    }
    else if ( widget == keys_[KEY_CLEAR] )
    {
        // clear the focused input widget
        clearFocusWidget();
    }
    else if ( widget == keys_[KEY_DISMISS] )
    {
        // dismiss
        dismiss();
    }
    else if ( widget == keys_[KEY_RETURN] )
    {
        QWidget *w;
        bool multiLineInput( false );

        // look for focus widget that is multi-line input
        if ( (w = focusWidget()) )
        {
            if (( dynamic_cast<QPlainTextEdit*>( w ) ) ||
                ( dynamic_cast<QTextEdit*>( w ) ))
            {
                multiLineInput = true;
            }
        }

        // accept
        acceptFocusWidget();

        // dismiss keyboard when not multi-line
        if ( !multiLineInput )
            dismiss();
    }
    else if ( widget == keys_[KEY_SPC_CHARS] )
    {
        // next mode
        if ( ++specialCharsMode_ == specialCharsModeCount_ )
            specialCharsMode_ = 0;

        // update
        updateKeys();
    }
    else
    {
        QPushButton *pushButton( dynamic_cast<QPushButton*>( widget ) );

        // emit what key was pressed
        if ( pushButton )
            if ( pushButton->text().length() )
            {
                const QChar c( pushButton->text().at( 0 ) );

                qDebug() << "key pressed: " << pushButton->text();
                Q_EMIT characterGenerated( c.unicode(), Qt::NoModifier, QString( c ) );
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::initialize()
{
    QPixmap backspaceIcon( resourceName( "res/backspaceIcon.png" ) );
    QPixmap capsLockIcon( resourceName( "res/capsLockIcon.png" ) );
    QPixmap clearIcon( resourceName( "res/clearIcon.png" ) );
    QPixmap dismissIcon( resourceName( "res/dismissIcon.png" ) );
    QPixmap returnIcon( resourceName( "res/returnIcon.png" ) );

    //
    // convert pixmaps to proper color
    //

    QList<QPixmap*> pixmaps;
    pixmaps.append( &backspaceIcon );
    pixmaps.append( &capsLockIcon );
    pixmaps.append( &clearIcon );
    pixmaps.append( &dismissIcon );
    pixmaps.append( &returnIcon );

    foreach ( QPixmap *p, pixmaps )
        if ( !p->isNull() )
        {
            QPixmap converted( p->size() );
            converted.fill( palette().color( foregroundRole() ) );
            converted.setMask( p->createMaskFromColor( Qt::transparent ) );

            p->swap( converted );
        }

    //
    // create keyboard keys
    //

    keys_["1"] = new QPushButton( this );
    keys_["2"] = new QPushButton( this );
    keys_["3"] = new QPushButton( this );
    keys_["4"] = new QPushButton( this );
    keys_["5"] = new QPushButton( this );
    keys_["6"] = new QPushButton( this );
    keys_["7"] = new QPushButton( this );
    keys_["8"] = new QPushButton( this );
    keys_["9"] = new QPushButton( this );
    keys_["0"] = new QPushButton( this );
    keys_["."] = new QPushButton( this );

    keys_["a"] = new QPushButton( this );
    keys_["b"] = new QPushButton( this );
    keys_["c"] = new QPushButton( this );
    keys_["d"] = new QPushButton( this );
    keys_["e"] = new QPushButton( this );
    keys_["f"] = new QPushButton( this );
    keys_["g"] = new QPushButton( this );
    keys_["h"] = new QPushButton( this );
    keys_["i"] = new QPushButton( this );
    keys_["j"] = new QPushButton( this );
    keys_["k"] = new QPushButton( this );
    keys_["l"] = new QPushButton( this );
    keys_["m"] = new QPushButton( this );
    keys_["n"] = new QPushButton( this );
    keys_["o"] = new QPushButton( this );
    keys_["p"] = new QPushButton( this );
    keys_["q"] = new QPushButton( this );
    keys_["r"] = new QPushButton( this );
    keys_["s"] = new QPushButton( this );
    keys_["t"] = new QPushButton( this );
    keys_["u"] = new QPushButton( this );
    keys_["v"] = new QPushButton( this );
    keys_["w"] = new QPushButton( this );
    keys_["x"] = new QPushButton( this );
    keys_["y"] = new QPushButton( this );
    keys_["z"] = new QPushButton( this );

    keys_[KEY_BACKSPACE] = new QPushButton( backspaceIcon, QString(), this );
    keys_[KEY_CAPS] = new QPushButton( capsLockIcon, QString(), this );
    keys_[KEY_CLEAR] = new QPushButton( clearIcon, QString(), this );
    keys_[KEY_DISMISS] = new QPushButton( dismissIcon, QString(), this );
    keys_[KEY_RETURN] = new QPushButton( returnIcon, QString(), this );
    keys_[KEY_SPACE] = new QPushButton( " ", this );
    keys_[KEY_SPC_CHARS] = new QPushButton( "#+=", this );

    // backspace key repeats
    keys_[KEY_BACKSPACE]->setAutoRepeat( true );
    keys_[KEY_BACKSPACE]->setAutoRepeatDelay( autoRepeatDelay_ );
    keys_[KEY_BACKSPACE]->setAutoRepeatInterval( autoRepeatInterval_ );

    // caps lock key is checkable
    keys_[KEY_CAPS]->setCheckable( true );

    //
    // create signal mapper
    //

    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL(mapped(QWidget*)), SLOT(onKeyClicked(QWidget*)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::createLayout()
{
    // delete old layout
    if ( layout() )
        delete layout();

    QHBoxLayout *horizontalLayout;

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setSpacing( 6 );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( 6 );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_["q"] );
    horizontalLayout->addWidget( keys_["w"] );
    horizontalLayout->addWidget( keys_["e"] );
    horizontalLayout->addWidget( keys_["r"] );
    horizontalLayout->addWidget( keys_["t"] );
    horizontalLayout->addWidget( keys_["y"] );
    horizontalLayout->addWidget( keys_["u"] );
    horizontalLayout->addWidget( keys_["i"] );
    horizontalLayout->addWidget( keys_["o"] );
    horizontalLayout->addWidget( keys_["p"] );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 1 ) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["7"] : keys_["1"]) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["8"] : keys_["2"]) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["9"] : keys_["3"]) );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( 6 );
    horizontalLayout->addStretch();
    horizontalLayout->addSpacerItem( new QSpacerItem( 35, 1 ) );
    horizontalLayout->addWidget( keys_["a"] );
    horizontalLayout->addWidget( keys_["s"] );
    horizontalLayout->addWidget( keys_["d"] );
    horizontalLayout->addWidget( keys_["f"] );
    horizontalLayout->addWidget( keys_["g"] );
    horizontalLayout->addWidget( keys_["h"] );
    horizontalLayout->addWidget( keys_["j"] );
    horizontalLayout->addWidget( keys_["k"] );
    horizontalLayout->addWidget( keys_["l"] );
    horizontalLayout->addSpacerItem( new QSpacerItem( 35, 1 ) );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 1 ) );
    horizontalLayout->addWidget( keys_["4"] );
    horizontalLayout->addWidget( keys_["5"] );
    horizontalLayout->addWidget( keys_["6"] );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( 6 );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_[KEY_CAPS] );
    horizontalLayout->addWidget( keys_["z"] );
    horizontalLayout->addWidget( keys_["x"] );
    horizontalLayout->addWidget( keys_["c"] );
    horizontalLayout->addWidget( keys_["v"] );
    horizontalLayout->addWidget( keys_["b"] );
    horizontalLayout->addWidget( keys_["n"] );
    horizontalLayout->addWidget( keys_["m"] );
    horizontalLayout->addWidget( keys_[KEY_BACKSPACE] );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 1 ) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["1"] : keys_["7"]) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["2"] : keys_["8"]) );
    horizontalLayout->addWidget( (CalculatorLayoutStyle == layoutStyle() ? keys_["3"] : keys_["9"]) );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( 6 );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_[KEY_SPC_CHARS] );
    horizontalLayout->addWidget( keys_[KEY_CLEAR] );
    horizontalLayout->addWidget( keys_[KEY_SPACE] );
    horizontalLayout->addWidget( keys_[KEY_RETURN] );
    horizontalLayout->addWidget( keys_[KEY_DISMISS] );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 1 ) );
    horizontalLayout->addWidget( keys_["0"] );
    horizontalLayout->addWidget( keys_["."] );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    screenLayout->insertStretch( 0 );
    screenLayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::groom()
{
    static const int KEY_HEIGHT( 48 );

    static const QSize keySize( 64, KEY_HEIGHT );
    static const QSize wideKeySize( 99, KEY_HEIGHT );
    static const QSize spacebarSize( 344, KEY_HEIGHT );
    static const QSize zeroSize( 134, KEY_HEIGHT );

    // iterate all keys
    for ( pushButtonMap::iterator i( keys_.begin() ); i != keys_.end(); ++i )
    {
        QPushButton *button( i.value() );

        // groom
        button->setFocusPolicy( Qt::NoFocus );
        button->setIconSize( buttonIconSize_ );
        button->setObjectName( i.key() );

        if ( "0" == i.key() )
            button->setFixedSize( zeroSize );
        else if ( KEY_SPACE == i.key() )
            button->setFixedSize( spacebarSize );
        else if (( KEY_BACKSPACE == i.key() ) || ( KEY_CAPS == i.key() ) || ( KEY_CLEAR == i.key() ) || ( KEY_RETURN == i.key() ))
            button->setFixedSize( wideKeySize );
        else
            button->setFixedSize( keySize );

        // map
        signalMapper_->setMapping( button, button );
        connect( button, SIGNAL(clicked()), signalMapper_, SLOT(map()) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardInputWidget::updateKeys()
{
    // set caps lock on/off
    keys_[KEY_CAPS]->setChecked( capsActive_ );

    // letters mode
    if ( 0 == specialCharsMode_ )
    {
        keys_[KEY_SPC_CHARS]->setText( "#+=" );

        // uppercase
        if ( capsActive_ )
        {
            keys_["1"]->setText( "!" );
            keys_["2"]->setText( "@" );
            keys_["3"]->setText( "#" );
            keys_["4"]->setText( "$" );
            keys_["5"]->setText( "%" );
            keys_["6"]->setText( "^" );
            keys_["7"]->setText( "&&" );
            keys_["8"]->setText( "*" );
            keys_["9"]->setText( "(" );
            keys_["0"]->setText( ")" );
            keys_["."]->setText( "." );

            keys_["a"]->setText( "A" );
            keys_["b"]->setText( "B" );
            keys_["c"]->setText( "C" );
            keys_["d"]->setText( "D" );
            keys_["e"]->setText( "E" );
            keys_["f"]->setText( "F" );
            keys_["g"]->setText( "G" );
            keys_["h"]->setText( "H" );
            keys_["i"]->setText( "I" );
            keys_["j"]->setText( "J" );
            keys_["k"]->setText( "K" );
            keys_["l"]->setText( "L" );
            keys_["m"]->setText( "M" );
            keys_["n"]->setText( "N" );
            keys_["o"]->setText( "O" );
            keys_["p"]->setText( "P" );
            keys_["q"]->setText( "Q" );
            keys_["r"]->setText( "R" );
            keys_["s"]->setText( "S" );
            keys_["t"]->setText( "T" );
            keys_["u"]->setText( "U" );
            keys_["v"]->setText( "V" );
            keys_["w"]->setText( "W" );
            keys_["x"]->setText( "X" );
            keys_["y"]->setText( "Y" );
            keys_["z"]->setText( "Z" );
        }

        // lowercase
        else
        {
            keys_["1"]->setText( "1" );
            keys_["2"]->setText( "2" );
            keys_["3"]->setText( "3" );
            keys_["4"]->setText( "4" );
            keys_["5"]->setText( "5" );
            keys_["6"]->setText( "6" );
            keys_["7"]->setText( "7" );
            keys_["8"]->setText( "8" );
            keys_["9"]->setText( "9" );
            keys_["0"]->setText( "0" );
            keys_["."]->setText( "." );

            keys_["a"]->setText( "a" );
            keys_["b"]->setText( "b" );
            keys_["c"]->setText( "c" );
            keys_["d"]->setText( "d" );
            keys_["e"]->setText( "e" );
            keys_["f"]->setText( "f" );
            keys_["g"]->setText( "g" );
            keys_["h"]->setText( "h" );
            keys_["i"]->setText( "i" );
            keys_["j"]->setText( "j" );
            keys_["k"]->setText( "k" );
            keys_["l"]->setText( "l" );
            keys_["m"]->setText( "m" );
            keys_["n"]->setText( "n" );
            keys_["o"]->setText( "o" );
            keys_["p"]->setText( "p" );
            keys_["q"]->setText( "q" );
            keys_["r"]->setText( "r" );
            keys_["s"]->setText( "s" );
            keys_["t"]->setText( "t" );
            keys_["u"]->setText( "u" );
            keys_["v"]->setText( "v" );
            keys_["w"]->setText( "w" );
            keys_["x"]->setText( "x" );
            keys_["y"]->setText( "y" );
            keys_["z"]->setText( "z" );
        }
    }

    // special characters mode
    else if ( 1 == specialCharsMode_ )
    {
        QLocale lang( locale() );

        keys_[KEY_SPC_CHARS]->setText( "abc" );

        keys_["q"]->setText( "!" );
        keys_["w"]->setText( "@" );
        keys_["e"]->setText( "#" );
        keys_["r"]->setText( "$" );
        keys_["t"]->setText( "%" );
        keys_["y"]->setText( "^" );
        keys_["u"]->setText( "&&" );
        keys_["i"]->setText( "*" );
        keys_["o"]->setText( "(" );
        keys_["p"]->setText( ")" );

        keys_["a"]->setText( "=" );
        keys_["s"]->setText( "'" );
        keys_["d"]->setText( "\"" );
        keys_["f"]->setText( ";" );
        keys_["g"]->setText( "," );
        keys_["h"]->setText( "+" );
        keys_["j"]->setText( "?" );
        keys_["k"]->setText( "[" );
        keys_["l"]->setText( "]" );

        keys_["z"]->setText( "\\" );
        keys_["x"]->setText( "/" );
        keys_["c"]->setText( "-" );
        keys_["v"]->setText( "_" );
        keys_["b"]->setText( ":" );
        keys_["n"]->setText( "<" );
        keys_["m"]->setText( ">" );

        // handle locale
        if ( QLocale::French == lang.language() )
        {
            // uppercase
            if ( capsActive_ )
            {
                keys_["0"]->setText( QStringLiteral("\xc1") );
                keys_["1"]->setText( QStringLiteral("\xc2") );
                keys_["2"]->setText( QStringLiteral("\xc6") );
                keys_["3"]->setText( QStringLiteral("\xc7") );
                keys_["4"]->setText( QStringLiteral("\xc9") );
                keys_["5"]->setText( QStringLiteral("\xc8") );
                keys_["6"]->setText( QStringLiteral("\xca") );
                keys_["7"]->setText( QStringLiteral("\xcb") );
                keys_["8"]->setText( QStringLiteral("\xce") );
                keys_["9"]->setText( QStringLiteral("\xcf") );
                keys_["."]->setText( "." );

                keys_["q"]->setText( QStringLiteral("\xd4") );
                keys_["w"]->setText( QStringLiteral("\x8c") );
                keys_["e"]->setText( QStringLiteral("\xd9") );
                keys_["r"]->setText( QStringLiteral("\xdb") );
                keys_["t"]->setText( QStringLiteral("\xdc") );
                keys_["y"]->setText( QStringLiteral("\x9f") );
            }

            // lowercase
            else
            {
                keys_["0"]->setText( QStringLiteral("\xe0") );
                keys_["1"]->setText( QStringLiteral("\xe2") );
                keys_["2"]->setText( QStringLiteral("\xe6") );
                keys_["3"]->setText( QStringLiteral("\xe7") );
                keys_["4"]->setText( QStringLiteral("\xe9") );
                keys_["5"]->setText( QStringLiteral("\xe8") );
                keys_["6"]->setText( QStringLiteral("\xea") );
                keys_["7"]->setText( QStringLiteral("\xeb") );
                keys_["8"]->setText( QStringLiteral("\xee") );
                keys_["9"]->setText( QStringLiteral("\xef") );
                keys_["."]->setText( "." );

                keys_["q"]->setText( QStringLiteral("\xf4") );
                keys_["w"]->setText( QStringLiteral("\x9c") );
                keys_["e"]->setText( QStringLiteral("\xf9") );
                keys_["r"]->setText( QStringLiteral("\xfb") );
                keys_["t"]->setText( QStringLiteral("\xfc") );
                keys_["y"]->setText( QStringLiteral("\xff") );
            }
        }
        else if ( QLocale::Spanish == lang.language() )
        {
            // uppercase
            if ( capsActive_ )
            {
                keys_["1"]->setText( QStringLiteral("\xc1") );
                keys_["2"]->setText( QStringLiteral("\xc9") );
                keys_["3"]->setText( QStringLiteral("\xcd") );
                keys_["4"]->setText( QStringLiteral("\xd3") );
                keys_["5"]->setText( QStringLiteral("\xda") );
                keys_["6"]->setText( QStringLiteral("\xd1") );
                keys_["7"]->setText( QStringLiteral("\xdc") );
                keys_["8"]->setText( QStringLiteral("\xbf") );
                keys_["9"]->setText( QStringLiteral("\xa1") );
                keys_["0"]->setText( "" );
                keys_["."]->setText( "." );
            }

            // lowercase
            else
            {
                keys_["1"]->setText( QStringLiteral("\xe1") );
                keys_["2"]->setText( QStringLiteral("\xe9") );
                keys_["3"]->setText( QStringLiteral("\xed") );
                keys_["4"]->setText( QStringLiteral("\xf3") );
                keys_["5"]->setText( QStringLiteral("\xfa") );
                keys_["6"]->setText( QStringLiteral("\xf1") );
                keys_["7"]->setText( QStringLiteral("\xfc") );
                keys_["8"]->setText( QStringLiteral("\xbf") );
                keys_["9"]->setText( QStringLiteral("\xa1") );
                keys_["0"]->setText( "" );
                keys_["."]->setText( "." );
            }
        }
        else
        {
            keys_["1"]->setText( "1" );
            keys_["2"]->setText( "2" );
            keys_["3"]->setText( "3" );
            keys_["4"]->setText( "4" );
            keys_["5"]->setText( "5" );
            keys_["6"]->setText( "6" );
            keys_["7"]->setText( "7" );
            keys_["8"]->setText( "8" );
            keys_["9"]->setText( "9" );
            keys_["0"]->setText( "0" );
            keys_["."]->setText( "." );
        }
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString keyboardInputWidget::resourceName( const QString& file )
{
    if ( QFile::exists( file ) )
        return file;

    return ":" + file;
}

