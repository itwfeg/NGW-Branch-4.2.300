/**
 * @file keyboardwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "keyboardwidget.h"

#include <backgroundwidgettypes.h>
#include <pushbutton.h>

#include <QHBoxLayout>
#include <QLocale>
#include <QPlainTextEdit>
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

const int KeyboardWidget::specialCharsModeCount_ = 2;

const QSize KeyboardWidget::buttonIconSize_( 40, 40 );
const int KeyboardWidget::buttonFontPointSize_ = 24;

const int KeyboardWidget::layoutSpacing_ = 6;

///////////////////////////////////////////////////////////////////////////////////////////////////
KeyboardWidget::KeyboardWidget( QWidget *parent ) :
    _Mybase( parent ),
    signalMapper_( NULL ),
    keyLayoutStyle_( (KeyLayoutStyle) -1 ),
    capsActive_( false ),
    specialCharsMode_( 0 )
{
    setObjectName( "KeyboardWidget" );

    // set font
    QFont f( font() );
    f.setBold( true );
    f.setPointSize( buttonFontPointSize_ );
    setFont( f );

    // initialize
    initialize();
    createLayout();

    // set keys
    updateKeys();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
KeyboardWidget::~KeyboardWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::setKeyLayoutStyle( KeyLayoutStyle value )
{
    if ( value == keyLayoutStyle_ )
        return;

    keyLayoutStyle_ = value;

    // create layout
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::setLocale( const QLocale& locale )
{
    _Mybase::setLocale( locale );

    // update keyboard keys
    updateKeys();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::onKeyClicked( QWidget *widget )
{
    LOG_CLASS_INFO( "KeyboardWidget", "onKeyClicked" );

    if ( widget == keys_[KEY_BACKSPACE] )
    {
        LOG_TRACE( "backspace", NULL );

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
        bool isMultiLineInput = false;

        // look for focus widget that is multi-line input
        if ( (w = focusWidget()) )
        {
            if (( dynamic_cast<QPlainTextEdit*>( w ) ) ||
                ( dynamic_cast<QTextEdit*>( w ) ))
            {
                isMultiLineInput = true;
            }
        }

        // accept
        acceptFocusWidget();

        // dismiss keyboard when not multi-line
        if ( !isMultiLineInput )
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
        QPushButton *pushButton = dynamic_cast<QPushButton*>( widget );

        // emit what key was pressed
        if (( pushButton ) && ( 1 <= pushButton->text().length() ))
        {
            LOG_TRACE( "'%s' key", qPrintable( pushButton->text() ) );

            QChar c = pushButton->text().at( 0 );
            Q_EMIT characterGenerated( c.unicode(), Qt::NoModifier, QString( c ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::resizeEvent( QResizeEvent *e )
{
    // set background
    const hobart::backgroundWidgetRenderer renderer(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::inputBackgroundWidgetRendererAppearance() );

    const QPixmap background( renderer.drawControl( size(), hobart::Normal ) );

    modifyPaletteBackground setBackground( background );
    setBackground( this );

    setAutoFillBackground( true );

    // set button backgrounds
    const int keyWidth( (width() * 64) / 940 );
    const int keyHeight( (height() * 48) / 230 );

    const hobart::backgroundWidgetRenderer buttonRenderer(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap keyPixmap( buttonRenderer.drawControl( QSize( keyWidth, keyHeight ), hobart::ReadOnly ) );
    const QPixmap keyWidePixmap( buttonRenderer.drawControl( QSize( ((3 * keyWidth) + layoutSpacing_) / 2, keyHeight ), hobart::ReadOnly ) );
    const QPixmap keySpacebarPixmap( buttonRenderer.drawControl( QSize( (5 * keyWidth) + (4 * layoutSpacing_), keyHeight ), hobart::ReadOnly ) );
    const QPixmap keyZeroPixmap( buttonRenderer.drawControl( QSize( (2 * keyWidth) + layoutSpacing_, keyHeight ), hobart::ReadOnly ) );

    keys_["1"]->setBackground( keyPixmap );
    keys_["2"]->setBackground( keyPixmap );
    keys_["3"]->setBackground( keyPixmap );
    keys_["4"]->setBackground( keyPixmap );
    keys_["5"]->setBackground( keyPixmap );
    keys_["6"]->setBackground( keyPixmap );
    keys_["7"]->setBackground( keyPixmap );
    keys_["8"]->setBackground( keyPixmap );
    keys_["9"]->setBackground( keyPixmap );
    keys_["0"]->setBackground( keyZeroPixmap );
    keys_["."]->setBackground( keyPixmap );

    keys_["a"]->setBackground( keyPixmap );
    keys_["b"]->setBackground( keyPixmap );
    keys_["c"]->setBackground( keyPixmap );
    keys_["d"]->setBackground( keyPixmap );
    keys_["e"]->setBackground( keyPixmap );
    keys_["f"]->setBackground( keyPixmap );
    keys_["g"]->setBackground( keyPixmap );
    keys_["h"]->setBackground( keyPixmap );
    keys_["i"]->setBackground( keyPixmap );
    keys_["j"]->setBackground( keyPixmap );
    keys_["k"]->setBackground( keyPixmap );
    keys_["l"]->setBackground( keyPixmap );
    keys_["m"]->setBackground( keyPixmap );
    keys_["n"]->setBackground( keyPixmap );
    keys_["o"]->setBackground( keyPixmap );
    keys_["p"]->setBackground( keyPixmap );
    keys_["q"]->setBackground( keyPixmap );
    keys_["r"]->setBackground( keyPixmap );
    keys_["s"]->setBackground( keyPixmap );
    keys_["t"]->setBackground( keyPixmap );
    keys_["u"]->setBackground( keyPixmap );
    keys_["v"]->setBackground( keyPixmap );
    keys_["w"]->setBackground( keyPixmap );
    keys_["x"]->setBackground( keyPixmap );
    keys_["y"]->setBackground( keyPixmap );
    keys_["z"]->setBackground( keyPixmap );

    keys_[KEY_BACKSPACE]->setBackground( keyWidePixmap );
    keys_[KEY_CAPS]->setBackground( keyWidePixmap );
    keys_[KEY_CLEAR]->setBackground( keyWidePixmap );
    keys_[KEY_DISMISS]->setBackground( keyPixmap );
    keys_[KEY_RETURN]->setBackground( keyWidePixmap );
    keys_[KEY_SPACE]->setBackground( keySpacebarPixmap );
    keys_[KEY_SPC_CHARS]->setBackground( keyPixmap );

    // set pressed button backgrounds
    invertPixmap pixmapInverter;

    const QPixmap invKeyPixmap( pixmapInverter( keyPixmap ) );
    const QPixmap invKeyWidePixmap( pixmapInverter( keyWidePixmap ) );
    const QPixmap invKeySpacebarPixmap( pixmapInverter( keySpacebarPixmap ) );
    const QPixmap invKeyZeroPixmap( pixmapInverter( keyZeroPixmap ) );

    keys_["1"]->setBackgroundPressed( invKeyPixmap );
    keys_["2"]->setBackgroundPressed( invKeyPixmap );
    keys_["3"]->setBackgroundPressed( invKeyPixmap );
    keys_["4"]->setBackgroundPressed( invKeyPixmap );
    keys_["5"]->setBackgroundPressed( invKeyPixmap );
    keys_["6"]->setBackgroundPressed( invKeyPixmap );
    keys_["7"]->setBackgroundPressed( invKeyPixmap );
    keys_["8"]->setBackgroundPressed( invKeyPixmap );
    keys_["9"]->setBackgroundPressed( invKeyPixmap );
    keys_["0"]->setBackgroundPressed( invKeyZeroPixmap );
    keys_["."]->setBackgroundPressed( invKeyPixmap );

    keys_["a"]->setBackgroundPressed( invKeyPixmap );
    keys_["b"]->setBackgroundPressed( invKeyPixmap );
    keys_["c"]->setBackgroundPressed( invKeyPixmap );
    keys_["d"]->setBackgroundPressed( invKeyPixmap );
    keys_["e"]->setBackgroundPressed( invKeyPixmap );
    keys_["f"]->setBackgroundPressed( invKeyPixmap );
    keys_["g"]->setBackgroundPressed( invKeyPixmap );
    keys_["h"]->setBackgroundPressed( invKeyPixmap );
    keys_["i"]->setBackgroundPressed( invKeyPixmap );
    keys_["j"]->setBackgroundPressed( invKeyPixmap );
    keys_["k"]->setBackgroundPressed( invKeyPixmap );
    keys_["l"]->setBackgroundPressed( invKeyPixmap );
    keys_["m"]->setBackgroundPressed( invKeyPixmap );
    keys_["n"]->setBackgroundPressed( invKeyPixmap );
    keys_["o"]->setBackgroundPressed( invKeyPixmap );
    keys_["p"]->setBackgroundPressed( invKeyPixmap );
    keys_["q"]->setBackgroundPressed( invKeyPixmap );
    keys_["r"]->setBackgroundPressed( invKeyPixmap );
    keys_["s"]->setBackgroundPressed( invKeyPixmap );
    keys_["t"]->setBackgroundPressed( invKeyPixmap );
    keys_["u"]->setBackgroundPressed( invKeyPixmap );
    keys_["v"]->setBackgroundPressed( invKeyPixmap );
    keys_["w"]->setBackgroundPressed( invKeyPixmap );
    keys_["x"]->setBackgroundPressed( invKeyPixmap );
    keys_["y"]->setBackgroundPressed( invKeyPixmap );
    keys_["z"]->setBackgroundPressed( invKeyPixmap );

    keys_[KEY_BACKSPACE]->setBackgroundPressed( invKeyWidePixmap );
    keys_[KEY_CAPS]->setBackgroundPressed( invKeyWidePixmap );
    keys_[KEY_CLEAR]->setBackgroundPressed( invKeyWidePixmap );
    keys_[KEY_DISMISS]->setBackgroundPressed( invKeyPixmap );
    keys_[KEY_RETURN]->setBackgroundPressed( invKeyWidePixmap );
    keys_[KEY_SPACE]->setBackgroundPressed( invKeySpacebarPixmap );
    keys_[KEY_SPC_CHARS]->setBackgroundPressed( invKeyPixmap );

    // adjust spacing
    leftSpacer_->changeSize( (keyWidth + layoutSpacing_) / 2, 10 );
    rightSpacer_->changeSize( (keyWidth + layoutSpacing_) / 2, 10 );

    // invoke base
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::initialize()
{
    QPixmap keyBackspaceIcon( ":/images/keyboardBackspaceIcon.png" );
    QPixmap keyCapsLockIcon( ":/images/keyboardCapsLockIcon.png" );
    QPixmap keyClearIcon( ":/images/keyboardClearIcon.png" );
    QPixmap keyDismissIcon( ":/images/keyboardDismissIcon.png" );
    QPixmap keyReturnIcon( ":/images/keyboardReturnIcon.png" );

    ///////////////////////////////////////////////////////////////////////////
    // create keyboard keys

    keys_["1"] = new hobart::pushButton( this );
    keys_["2"] = new hobart::pushButton( this );
    keys_["3"] = new hobart::pushButton( this );
    keys_["4"] = new hobart::pushButton( this );
    keys_["5"] = new hobart::pushButton( this );
    keys_["6"] = new hobart::pushButton( this );
    keys_["7"] = new hobart::pushButton( this );
    keys_["8"] = new hobart::pushButton( this );
    keys_["9"] = new hobart::pushButton( this );
    keys_["0"] = new hobart::pushButton( this );
    keys_["."] = new hobart::pushButton( this );

    keys_["a"] = new hobart::pushButton( this );
    keys_["b"] = new hobart::pushButton( this );
    keys_["c"] = new hobart::pushButton( this );
    keys_["d"] = new hobart::pushButton( this );
    keys_["e"] = new hobart::pushButton( this );
    keys_["f"] = new hobart::pushButton( this );
    keys_["g"] = new hobart::pushButton( this );
    keys_["h"] = new hobart::pushButton( this );
    keys_["i"] = new hobart::pushButton( this );
    keys_["j"] = new hobart::pushButton( this );
    keys_["k"] = new hobart::pushButton( this );
    keys_["l"] = new hobart::pushButton( this );
    keys_["m"] = new hobart::pushButton( this );
    keys_["n"] = new hobart::pushButton( this );
    keys_["o"] = new hobart::pushButton( this );
    keys_["p"] = new hobart::pushButton( this );
    keys_["q"] = new hobart::pushButton( this );
    keys_["r"] = new hobart::pushButton( this );
    keys_["s"] = new hobart::pushButton( this );
    keys_["t"] = new hobart::pushButton( this );
    keys_["u"] = new hobart::pushButton( this );
    keys_["v"] = new hobart::pushButton( this );
    keys_["w"] = new hobart::pushButton( this );
    keys_["x"] = new hobart::pushButton( this );
    keys_["y"] = new hobart::pushButton( this );
    keys_["z"] = new hobart::pushButton( this );

    keys_[KEY_BACKSPACE] = new hobart::pushButton( keyBackspaceIcon, QString(), this );
    keys_[KEY_CAPS] = new hobart::pushButton( keyCapsLockIcon, QString(), this );
    keys_[KEY_CLEAR] = new hobart::pushButton( keyClearIcon, QString(), this );
    keys_[KEY_DISMISS] = new hobart::pushButton( keyDismissIcon, QString(), this );
    keys_[KEY_RETURN] = new hobart::pushButton( keyReturnIcon, QString(), this );
    keys_[KEY_SPACE] = new hobart::pushButton( " ", this );
    keys_[KEY_SPC_CHARS] = new hobart::pushButton( "#+=", this );

    // backspace key repeats
    keys_[KEY_BACKSPACE]->setAutoRepeat( true );
    keys_[KEY_BACKSPACE]->setAutoRepeatDelay( keyAutoRepeatDelay_ );
    keys_[KEY_BACKSPACE]->setAutoRepeatInterval( keyAutoRepeatInterval_ );

    // caps lock key is checkable
    keys_[KEY_CAPS]->setCheckable( true );

    ///////////////////////////////////////////////////////////////////////////
    // create signal mapper

    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), this, SLOT( onKeyClicked(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // setup keyboard keys

    // set layout
    setKeyLayoutStyle( CalculatorKeyLayoutStyle );

    // groom buttons
    for ( pushButtonMap::iterator i = keys_.begin(); i != keys_.end(); ++i )
    {
        hobart::pushButton *button = dynamic_cast<hobart::pushButton*>( i.value() );

        button->setFocusPolicy( Qt::NoFocus );
        button->setIconSize( buttonIconSize_ );
        button->setObjectName( i.key() );

        signalMapper_->setMapping( button, button );
        connect( button, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardWidget::updateKeys()
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
                keys_["0"]->setText( QStringLiteral("À") );
                keys_["1"]->setText( QStringLiteral("Â") );
                keys_["2"]->setText( QStringLiteral("Æ") );
                keys_["3"]->setText( QStringLiteral("Ç") );
                keys_["4"]->setText( QStringLiteral("É") );
                keys_["5"]->setText( QStringLiteral("È") );
                keys_["6"]->setText( QStringLiteral("Ê") );
                keys_["7"]->setText( QStringLiteral("Ë") );
                keys_["8"]->setText( QStringLiteral("Î") );
                keys_["9"]->setText( QStringLiteral("Ï") );
                keys_["."]->setText( "." );

                keys_["q"]->setText( QStringLiteral("Ô") );
                keys_["w"]->setText( QStringLiteral("Œ") );
                keys_["e"]->setText( QStringLiteral("Ù") );
                keys_["r"]->setText( QStringLiteral("Û") );
                keys_["t"]->setText( QStringLiteral("Ü") );
                keys_["y"]->setText( QStringLiteral("Ÿ") );
            }

            // lowercase
            else
            {
                keys_["0"]->setText( QStringLiteral("à") );
                keys_["1"]->setText( QStringLiteral("â") );
                keys_["2"]->setText( QStringLiteral("æ") );
                keys_["3"]->setText( QStringLiteral("ç") );
                keys_["4"]->setText( QStringLiteral("é") );
                keys_["5"]->setText( QStringLiteral("è") );
                keys_["6"]->setText( QStringLiteral("ê") );
                keys_["7"]->setText( QStringLiteral("ë") );
                keys_["8"]->setText( QStringLiteral("î") );
                keys_["9"]->setText( QStringLiteral("ï") );
                keys_["."]->setText( "." );

                keys_["q"]->setText( QStringLiteral("ô") );
                keys_["w"]->setText( QStringLiteral("œ") );
                keys_["e"]->setText( QStringLiteral("ù") );
                keys_["r"]->setText( QStringLiteral("û") );
                keys_["t"]->setText( QStringLiteral("ü") );
                keys_["y"]->setText( QStringLiteral("ÿ") );
            }
        }
        else if ( QLocale::Spanish == lang.language() )
        {
            // uppercase
            if ( capsActive_ )
            {
                keys_["1"]->setText( QStringLiteral("Á") );
                keys_["2"]->setText( QStringLiteral("É") );
                keys_["3"]->setText( QStringLiteral("Í") );
                keys_["4"]->setText( QStringLiteral("Ó") );
                keys_["5"]->setText( QStringLiteral("Ú") );
                keys_["6"]->setText( QStringLiteral("Ñ") );
                keys_["7"]->setText( QStringLiteral("Ü") );
                keys_["8"]->setText( QStringLiteral("¿") );
                keys_["9"]->setText( QStringLiteral("¡") );
                keys_["0"]->setText( "" );
                keys_["."]->setText( "." );
            }

            // lowercase
            else
            {
                keys_["1"]->setText( QStringLiteral("á") );
                keys_["2"]->setText( QStringLiteral("é") );
                keys_["3"]->setText( QStringLiteral("í") );
                keys_["4"]->setText( QStringLiteral("ó") );
                keys_["5"]->setText( QStringLiteral("ú") );
                keys_["6"]->setText( QStringLiteral("ñ") );
                keys_["7"]->setText( QStringLiteral("ü") );
                keys_["8"]->setText( QStringLiteral("¿") );
                keys_["9"]->setText( QStringLiteral("¡") );
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
void KeyboardWidget::createLayout()
{
    // delete old layout
    QLayout *oldScreenLayout( layout() );

    if ( oldScreenLayout )
        delete oldScreenLayout;

    // create layouts
    QHBoxLayout *horizontalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setSpacing( layoutSpacing_ );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
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
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 10 ) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["7"] : keys_["1"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["8"] : keys_["2"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["9"] : keys_["3"]) );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addSpacerItem( leftSpacer_ = new QSpacerItem( 35, 10 ) );
    horizontalLayout->addWidget( keys_["a"] );
    horizontalLayout->addWidget( keys_["s"] );
    horizontalLayout->addWidget( keys_["d"] );
    horizontalLayout->addWidget( keys_["f"] );
    horizontalLayout->addWidget( keys_["g"] );
    horizontalLayout->addWidget( keys_["h"] );
    horizontalLayout->addWidget( keys_["j"] );
    horizontalLayout->addWidget( keys_["k"] );
    horizontalLayout->addWidget( keys_["l"] );
    horizontalLayout->addSpacerItem( rightSpacer_ = new QSpacerItem( 35, 10 ) );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 10 ) );
    horizontalLayout->addWidget( keys_["4"] );
    horizontalLayout->addWidget( keys_["5"] );
    horizontalLayout->addWidget( keys_["6"] );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
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
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 10 ) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["1"] : keys_["7"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["2"] : keys_["8"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["3"] : keys_["9"]) );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_[KEY_SPC_CHARS] );
    horizontalLayout->addWidget( keys_[KEY_CLEAR] );
    horizontalLayout->addWidget( keys_[KEY_SPACE] );
    horizontalLayout->addWidget( keys_[KEY_RETURN] );
    horizontalLayout->addWidget( keys_[KEY_DISMISS] );
    horizontalLayout->addSpacerItem( new QSpacerItem( 10, 10 ) );
    horizontalLayout->addWidget( keys_["0"] );
    horizontalLayout->addWidget( keys_["."] );
    horizontalLayout->addStretch();
    screenLayout->addLayout( horizontalLayout );

    screenLayout->insertStretch( 0 );
    screenLayout->addStretch();
}

