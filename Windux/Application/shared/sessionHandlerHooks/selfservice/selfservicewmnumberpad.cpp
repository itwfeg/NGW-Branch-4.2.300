/**
 * @file selfservicewmnumberpad.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Joe Crew
 */

#include "selfservicewmnumberpad.h"

#include <backgroundwidgettypes.h>
#include <numericlineedit.h>
#include <pushbutton.h>
#include <selfservicehook.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QTextEdit>
#include <QPainter>
#include <QSignalMapper>
#include <QVBoxLayout>

static const QString CLEAR_KEY( "clear" );

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWmNumberPad::selfServiceWmNumberPad( QWidget *parent ) :
    _Mybase( parent ),
    autoLookupProduct_( false ),
    autoLookupNumOfDigits_( 0 ),
    autoLookupOverrideDigit_( 0 ),
    validator_( NULL )
{
    setObjectName( "selfServiceWmNumberPad" );

    // initialize
    initialize();

    // create layouts
    createLayout();

    // translate
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWmNumberPad::~selfServiceWmNumberPad()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::clear()
{
    lineEdit_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setAutoLookupNumOfDigits( int value )
{
    autoLookupNumOfDigits_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setAutoLookupOverrideDigit( int value )
{
    autoLookupOverrideDigit_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setAutoLookupProduct( bool value )
{
    autoLookupProduct_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setStyle( const QString& value )
{
    // nothing to do
    if ( style_ == value )
        return;

    style_ = value;

    // destroy existing layouts
    if ( layout() )
        delete layout();

    if ( buttonsArea_->layout() )
        delete buttonsArea_->layout();

    // create layouts
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setText( const QString& value )
{
    lineEdit_->setText( value );

    bool hasInstructions = !value.trimmed().isEmpty();

    if ( hasInstructions )
        displayStackedWidget(0);
    else
        displayStackedWidget(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfServiceWmNumberPad::text() const
{
    return lineEdit_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::setInstructions( const QString& value )
{
    instructions_->setText( value );
    instructions_->setAlignment(Qt::AlignCenter);

    bool hasInstructions = !value.trimmed().isEmpty();

    if ( hasInstructions )
        displayStackedWidget(0);
    else
        displayStackedWidget(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::displayStackedWidget( int value )
{
    stacked_->setCurrentIndex( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::translate()
{
    clear_->setText( tr( "Clear" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // draw new background for numeric input widget
    QSize size( lineEdit_->size() );

    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.setBrush( QBrush( QColor( 32, 32, 32 ) ) );
    painter.setPen( QPen( hobart::colorPalette::green, 4 ) );

    int a = painter.pen().width() / 2;
    painter.drawRoundedRect( QRect( QPoint( 0, 0 ), size ).adjusted( a, a, -a, -a ), 5, 5 );

    // set background
    QPalette palette( lineEdit_->palette() );
    palette.setBrush( lineEdit_->backgroundRole(), pixmap );
    lineEdit_->setPalette( palette );
    instructions_->setPalette( palette );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::onButtonClicked( QWidget *w )
{
    // clear button just clears
    if ( clear_ == w )
    {
        lineEdit_->clear();
        displayStackedWidget(0);        // show instructions
        return;
    }

    hobart::pushButton *button( dynamic_cast<hobart::pushButton*>( w ) );

    // append button text to line edit
    if ( button )
    {
        QString value( lineEdit_->text() );
        value.append( button->text() );
        displayStackedWidget(1);

        if ( validator_ )
        {
            if ( value.toInt() < validator_->bottom() )
                return;
            else if ( validator_->top() < value.toInt() )
                return;
        }

        lineEdit_->setText( value );

        // check for auto lookup
        if ( autoLookupProduct_ )
            if (( 0 < autoLookupNumOfDigits_ ) && ( autoLookupNumOfDigits_ <= value.length() ))
            {
                if (( 0 < autoLookupOverrideDigit_ ) &&
                    ( autoLookupNumOfDigits_ == value.length() ) &&
                    ( QString::number( autoLookupOverrideDigit_ ) == QString( value[0] ) ))
                {
                    // nothing to do, first digit is override digit
                }
                else
                {
                    // force repaint!
                    lineEdit_->repaint();

                    Q_EMIT lookup();
                }
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::initialize()
{
    buttonSignalMapper_ = new QSignalMapper( this );

    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // ----- //
    lineEdit_ = new hobart::numericLineEdit( this );
    lineEdit_->setAutoFillBackground( true );
    lineEdit_->setContentsMargins( QMargins( 10, 0, 10, 0 ) );  // L,R,T,B
    lineEdit_->setFixedSize( 265, 75 );
    lineEdit_->setFocusPolicy( Qt::NoFocus );
    lineEdit_->setFrame( false );
    lineEdit_->setReadOnly( true );

    instructions_ = new QTextEdit();
    instructions_->setAutoFillBackground( true );
    instructions_->setContentsMargins( QMargins( 10, 0, 10, 0 ) );  // L,R,T,B
    instructions_->setFixedSize( 265, 75 );
    instructions_->setFocusPolicy( Qt::NoFocus );
    instructions_->setReadOnly( true );
    instructions_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    instructions_->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    instructions_->setAlignment(Qt::AlignCenter);

    QPalette lineEditPalette( lineEdit_->palette() );
    lineEditPalette.setColor( lineEdit_->backgroundRole(), QColor( 32, 32, 32 ) );
    lineEditPalette.setColor( lineEdit_->foregroundRole(), hobart::colorPalette::white );
    lineEdit_->setPalette( lineEditPalette );

    QFont lineEditFont( lineEdit_->font() );
    lineEditFont.setPointSize( 36 );
    lineEdit_->setFont( lineEditFont );

    QPalette instructionEditPalette( instructions_->palette() );
    instructionEditPalette.setColor( instructions_->backgroundRole(), QColor( 32, 32, 32 ) );
    instructionEditPalette.setColor( instructions_->foregroundRole(), hobart::colorPalette::white );
    instructions_->setPalette( instructionEditPalette );

    QFont instructionEditFont( instructions_->font() );
    instructionEditFont.setPointSize( 18 );
    instructions_->setFont( instructionEditFont );

    connect( lineEdit_, SIGNAL(textChanged(QString)), SIGNAL(textChanged(QString)));

    // ----- //

    buttonsArea_ = new QWidget( this );
    buttonsArea_->setAutoFillBackground( true );

    QPalette buttonsAreaPalette( buttonsArea_->palette() );
    buttonsAreaPalette.setColor( buttonsArea_->backgroundRole(), QColor( 32, 32, 32 ) );
    buttonsArea_->setPalette( buttonsAreaPalette );

    // ----- //

    const hobart::backgroundWidgetRenderer renderKey(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderHorzKey(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap button_1_1( renderKey.drawControl( QSize( 75, 75 ), hobart::ReadOnly ) );
    const QPixmap button_2_1( renderHorzKey.drawControl( QSize( 160, 75 ), hobart::ReadOnly ) );

    typedef QPair<QString, QPixmap> buttonInfo;
    QList<buttonInfo> buttonInfoList;

    // list of buttons to create
    buttonInfoList.append( buttonInfo( "0", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "1", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "2", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "3", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "4", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "5", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "6", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "7", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "8", button_1_1 ) );
    buttonInfoList.append( buttonInfo( "9", button_1_1 ) );
    buttonInfoList.append( buttonInfo( CLEAR_KEY, button_2_1 ) );

    foreach ( const buttonInfo& bi, buttonInfoList )
    {
        hobart::pushButton *button( new hobart::pushButton( buttonsArea_, bi.second ) );
        button->setFocusPolicy( Qt::NoFocus );
        button->setText( bi.first );

        QPalette buttonPalette( button->palette() );
        buttonPalette.setColor( button->foregroundRole(), hobart::colorPalette::white );
        button->setPalette( buttonPalette );

        QFont buttonFont( button->font() );
        buttonFont.setPointSize( 48 );
        button->setFont( buttonFont );

        connect( button, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
        buttonSignalMapper_->setMapping( button, button );

        buttons_[bi.first] = button;
    }

    clear_ = buttons_[CLEAR_KEY];

    QFont clearButtonFont( clear_->font() );
    clearButtonFont.setPointSize( 36 );
    clear_->setFont( clearButtonFont );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWmNumberPad::createLayout()
{
    QVBoxLayout *widgetLayout( new QVBoxLayout( this ) );
    widgetLayout->setContentsMargins( QMargins() );

    widgetLayout->addStretch();
    stacked_ = new QStackedWidget( this );
    stacked_->setFixedSize( 265, 75 );
    stacked_->addWidget( instructions_  );
    stacked_->addWidget( lineEdit_  );

    widgetLayout->addWidget( stacked_ );
    widgetLayout->addWidget( buttonsArea_ );
    widgetLayout->addStretch();

    // ----- //

    QVBoxLayout *buttonsAreaLayout( new QVBoxLayout( buttonsArea_ ) );
    buttonsAreaLayout->setContentsMargins( QMargins( 10, 10, 10, 10 ) );
    buttonsAreaLayout->setSpacing( 10 );

    QHBoxLayout *row_1( new QHBoxLayout() );
    row_1->setContentsMargins( QMargins() );
    row_1->setSpacing( 10 );

    QHBoxLayout *row_2( new QHBoxLayout() );
    row_2->setContentsMargins( QMargins() );
    row_2->setSpacing( 10 );

    QHBoxLayout *row_3( new QHBoxLayout() );
    row_3->setContentsMargins( QMargins() );
    row_3->setSpacing( 10 );

    QHBoxLayout *row_4( new QHBoxLayout() );
    row_4->setContentsMargins( QMargins() );
    row_4->setSpacing( 10 );

    buttonsAreaLayout->addLayout( row_1 );
    buttonsAreaLayout->addLayout( row_2 );
    buttonsAreaLayout->addLayout( row_3 );
    buttonsAreaLayout->addLayout( row_4 );

    // telephone style - 1,2,3 keys on top
    if ( "123" == style() )
    {
        row_1->addStretch();
        row_1->addWidget( buttons_["1"] );
        row_1->addWidget( buttons_["2"] );
        row_1->addWidget( buttons_["3"] );
        row_1->addStretch();
    }
    // keyboard style - 7,8,9 keys on top
    else
    {
        row_1->addStretch();
        row_1->addWidget( buttons_["7"] );
        row_1->addWidget( buttons_["8"] );
        row_1->addWidget( buttons_["9"] );
        row_1->addStretch();
    }

    row_2->addStretch();
    row_2->addWidget( buttons_["4"] );
    row_2->addWidget( buttons_["5"] );
    row_2->addWidget( buttons_["6"] );
    row_2->addStretch();

    // telephone style - 1,2,3 keys on top
    if ( "123" == style() )
    {
        row_3->addStretch();
        row_3->addWidget( buttons_["7"] );
        row_3->addWidget( buttons_["8"] );
        row_3->addWidget( buttons_["9"] );
        row_3->addStretch();
    }
    // keyboard style - 7,8,9 keys on top
    else
    {
        row_3->addStretch();
        row_3->addWidget( buttons_["1"] );
        row_3->addWidget( buttons_["2"] );
        row_3->addWidget( buttons_["3"] );
        row_3->addStretch();
    }

    row_4->addStretch();
    row_4->addWidget( buttons_["0"] );
    row_4->addWidget( buttons_[CLEAR_KEY] );
    row_4->addStretch();
}

