/**
 * @file selfservicenumberpad.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicenumberpad.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <numericlineedit.h>
#include <pushbutton.h>
#include <selfservicehook.h>
#include <ScaleClasses.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QTextEdit>
#include <QPainter>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QSize>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>

static const QString CLEAR_KEY( "clear" );
static const QString ENTER_KEY( "enter" );          // added enter key

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceNumberPad::selfServiceNumberPad( QWidget *parent ) :
    _Mybase( parent ),
    enterAction_( 0 ),
    autoLookupProduct_( false ),
    autoLookupNumOfDigits_( 0 ),
    autoLookupOverrideDigit_( 0 ),
    stackIndex_(0),
    displayBarcode_(0),
    operatorID_( "0"),
    validator_( NULL ),
    selfServicePreventClicks_( false )
{
    setObjectName( "selfServiceNumberPad" );

    // initialize
    initialize();

    // create layouts
    createLayout();

    displayStackedNumWidget(0);         // 0 = keypad, 1=Print&Scan

    // translate
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceNumberPad::~selfServiceNumberPad()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::clear()
{
    lineEdit_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setAutoLookupNumOfDigits( int value )
{
    autoLookupNumOfDigits_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setAutoLookupOverrideDigit( int value )
{
    autoLookupOverrideDigit_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setAutoLookupProduct( bool value )
{
    autoLookupProduct_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setStyle( const QString& value )
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
void selfServiceNumberPad::setText( const QString& value )
{
    lineEdit_->setText( value );

    // force repaint!
    lineEdit_->repaint();

    bool hasInstructions = !value.trimmed().isEmpty();

    if ( hasInstructions )
        displayStackedWidget(0);
    else
        displayStackedWidget(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfServiceNumberPad::text() const
{
    return lineEdit_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setInstructions( const QString& value )
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
void selfServiceNumberPad::setBarcodeFileName(const QString &value)
{
    barcodeFile_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setEnterResponse( int value )
{
    enterAction_ = value;       // 0 = Enter, 1 = Print
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setEnterText( int value )
{
    // 0 = Enter, 1 = Print

    if( 1 == value )
        enter_->setText( tr("Print") );
    else
        enter_->setText( tr("Enter") );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setOperatorID(const QString &value )
{
    operatorID_ = value;       // 0 = Enter, 1 = Print
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::setDisplayBarcodeOnUserInterface( int value )
{
    displayBarcode_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::displayStackedWidget( int value )
{
    stacked_->setCurrentIndex( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceNumberPad::numberPadStackState()
{
    return stackIndex_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::displayStackedNumWidget( int value )
{
    if( 0 == value )
    {
        stacked_->setVisible( true );
        selfServicePreventClicks_ = false;
        lineEdit_->setText( "" );
    }
    else
    {
        stacked_->setVisible( false );
        printLabel_->setText( tr("Print Label") );
        lblScanBarcode_->setText( tr("SCAN BARCODE") );
    }

    stackedNums_->setCurrentIndex( value );
    stackIndex_ = value;        // CALL numberPadStackState()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::translate()
{
    clear_->setText( tr( "Clear" ) );
    enter_->setText( tr( "Enter" ) );       // added enter key
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "selfServiceNumberPad", "resizeEvent" );

    //_Mybase::resizeEvent( e );

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

    if( displayBarcode_ )
    {
        int bgSize = 318;                       // 14 inch
        int npSize = 480;                       // 14 inch

        int screensize = checkScreenSize();

        //int labelHeight( 304 );
        int printHeight( 178 );

        if( 10 == screensize )
        {
            bgSize = 268;
            npSize = 416;
            //labelHeight = 180;
        }

        //stackedNums_->setFixedSize( bgSize - 10, npSize + 70);
        stackedNums_->setFixedSize( bgSize, npSize + 70);
        barcodeOnly_->setFixedSize( (bgSize - 12), printHeight - 2 );
        bigPrintKey_->setFixedSize( (bgSize - 12), printHeight - 3 );
        lblScanBarcode_->setFixedSize( bgSize - 12, 35 );

        LOG_DEBUG( "CALL renderLabelImage(false) from resize()", NULL );
        renderLabelImage();
    }
	_Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfServiceNumberPad", "renderBarcodeOnlyLabel" );

    LOG_DEBUG( "renderBarcodeOnlyLabel() call renderLabelImage()", NULL );

    renderLabelImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::renderLabelImage()
{
    LOG_CLASS_INFO( "selfServiceNumberPad", "renderLabelImage" );

    LOG_DEBUG( "selfServiceNumberPad::renderLabelImage() START", NULL );

    // New Scaling
    int bgSize = 318;                       // 14 inch
    int npSize = 480;                       // 14 inch

    int screensize = checkScreenSize();
    int screenheight = checkScreenHeight();

    int labelHeight( 304 );
    int printHeight( 178);
    int spacing( (screenheight - ((printHeight * 2) + 35) ) / 2 );

    if( 10 == screensize )
    {
        bgSize = 268;
        npSize = 416;

        labelHeight = 180;
        spacing = 20;
    }

    stackedNums_->setFixedSize( bgSize, npSize + 70);                                   // ORIG = bgsize-10
    barcodeOnly_->setAutoFillBackground( true );
    barcodeOnly_->setFixedSize( (bgSize - 12), printHeight - 2 );
    bigPrintKey_->setFixedSize( (bgSize - 12), printHeight - 3 );
    lblScanBarcode_->setFixedSize( bgSize - 12, 35 );
    topSpacer_->setGeometry( QRect( 1, 1, 20, spacing) );
    bottomSpacer_->setGeometry( QRect( 1, 1, 20, spacing) );

    // had to pass it in bc we dont have access to the hook to get the filename
    //QString label = "/etc/Hobart/LabelImagePluBarcode.bmp";
     QString label = barcodeFile_;

    LOG_WARN( "renderLabelImage() assigned label: %s", qPrintable( label ) );

    bool labelValid( false );

    QPalette p( barcodeOnly_->palette() );

    if ( !label.isEmpty() )
    {
        LOG_DEBUG( "renderLabelImage() label.isEmpty() is false so check if label is null", NULL );

        QPixmap image( label );

        if ( !image.isNull() )
        {
            LOG_DEBUG( "renderLabelImage() label is not null so render it", NULL );

            // create pixmap
            QPixmap dest( (bgSize - 17), (printHeight - 7) );

            if( dest.isNull() )
                LOG_DEBUG( "renderLabelImage() dest.isNull() is TRUE so this update() WILL FAIL", NULL );

            dest.fill( Qt::transparent );
            {
                // create painter
                QPainter painter( &dest );

                painter.setRenderHint( QPainter::Antialiasing, true );

                // Got rid of this
                //painter.setBackgroundMode( Qt::TransparentMode );

                // draw pixmap
                bool beginit = painter.begin( &dest );
                LOG_DEBUG( "renderLabelImage() begin it: %d",  beginit);
                painter.drawPixmap( 0, 0, dest.width(), (printHeight - 7), image );
                bool endit = painter.end();
                LOG_DEBUG( "renderLabelImage() end it: %d",  endit);
            }

            p.setBrush( backgroundRole(), dest );
            labelValid = true;
        }
        else
            LOG_DEBUG( "renderLabelImage() image.isNull = true so do not do anything", NULL );
    }
    else
        LOG_DEBUG( "renderLabelImage() label.isEmpty() label is empty so do not display it", NULL );

    // no nutrifacts
    if ( !labelValid )
    {
        LOG_DEBUG( "!labelValid - Label is not valid so EXIT", NULL );
        //p.setColor( backgroundRole(), Qt::white );
        return;
    }

    // set new palette
    barcodeOnly_->setPalette( p );

    barcodeOnly_->hide();
    barcodeOnly_->show();

    barcodeOnly_->update();

    LOG_DEBUG( "selfServiceNumberPad::renderLabelImage() END\n", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::onButtonClicked( QWidget *w )
{
    // clear button just clears
    if ( clear_ == w )
    {
        lineEdit_->clear();
        displayStackedWidget(0);        // show instructions
        return;
    }
    else if(  enter_ == w )
    {
        // force repaint!
        lineEdit_->repaint();

        if( enterAction_ == SelfServicePrintOption::Action_Print )
        {
            if( displayBarcode_ )
            {
                QString value( lineEdit_->text() );
                if( value.toInt() > 0)
                {
                    displayStackedNumWidget(1);         // 0 = keypad, 1=Print&Scan
                    Q_EMIT showBackButton();
                }
                else
                    Q_EMIT printError(ProductDevError::ProductErrForcedFieldNotEntered);
            }
            else
                Q_EMIT print();
        }
        else if( enterAction_ == SelfServicePrintOption::Action_Enter )
        {
            Q_EMIT lookup();                // added enter key
        }
        else if( enterAction_ == SelfServicePrintOption::Action_LookupOperator )
        {
            Q_EMIT selfServiceLogin( operatorID_ );     // Look up an operator
        }

        return;
    }
    else if ( printLabel_ == w || bigPrintKey_ == w )
    {
        // have to handle clicks and double clicks even if we are ready or not. But I dont want to handle them again
        // if I havent drawn the print screen in the first place. so only allow again once Ive displayed the print screen

        if ( selfServicePreventClicks_ )
            return;

        selfServicePreventClicks_ = true;

        bigPrintKey_->setText( "" );        // because we are getting ghost Print Label Commands
        printLabel_->setText( "" );
        Q_EMIT print();
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
        {
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
}

hobart::pushButton *selfServiceNumberPad::enter() const
{
    return enter_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::initialize()
{
    buttonSignalMapper_ = new QSignalMapper( this );

    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    int spSize = 10;
    int bgSize = 318;
    int npSize = 480;
    int screensize = checkScreenSize();

    if( screensize == 10 )
    {
        spSize = 6;
        bgSize = 268;   // 273
        npSize = 416;
    }

    // ----- //
    lineEdit_ = new hobart::numericLineEdit( this );
    lineEdit_->setAutoFillBackground( true );
    lineEdit_->setContentsMargins( QMargins( spSize, 0, spSize, 0 ) );          // L,R,T,B
    // lineEdit_->setContentsMargins( QMargins( 0, 0, 0, 0 ) );                 // L,T,R,B - use for initial wiggle for keypads on BC screen
    lineEdit_->setFixedSize( bgSize, 90 );
    lineEdit_->setFocusPolicy( Qt::NoFocus );
    lineEdit_->setFrame( false );
    lineEdit_->setReadOnly( true );

    instructions_ = new QTextEdit();
    instructions_->setAutoFillBackground( true );
    instructions_->setContentsMargins( QMargins( spSize, 0, spSize, 0 ) );      // L,R,T,B
    // instructions_->setContentsMargins( QMargins( 0, 0, 0, 0 ) );             // L,T,R,B - use for initial wiggle for keypads on BC screen
    instructions_->setFixedSize( bgSize, 90 );
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
    instructionEditFont.setPointSize( 24 );
    instructions_->setFont( instructionEditFont );

    connect( lineEdit_, SIGNAL(textChanged(QString)), SIGNAL(textChanged(QString)));

    // ----- //

    buttonsArea_ = new QWidget( this );
    buttonsArea_->setAutoFillBackground( true );
    // buttonsArea_->setContentsMargins( QMargins( 0, 0, 0, 0 ) );              // L,T,R,B - use for initial wiggle for keypads on BC screen

    QPalette buttonsAreaPalette( buttonsArea_->palette() );
    buttonsAreaPalette.setColor( buttonsArea_->backgroundRole(), QColor( 32, 32, 32 ) );
    buttonsArea_->setPalette( buttonsAreaPalette );

    // ----- //

    QPixmap button_1_1;
    QPixmap button_2_1;
    QPixmap button_3_1;

    if ( screensize == 14 )
    {
        const hobart::backgroundWidgetRenderer renderKeyPixmap(
            new hobart::panelButtonBackgroundWidget( 5 ),
            new hobart::darkButtonBackgroundWidgetRendererAppearance );

        const hobart::backgroundWidgetRenderer renderGreenKeyPixmap(
            new hobart::panelButtonBackgroundWidget( 5 ),
            new hobart::greenButtonBackgroundWidgetRendererAppearance );

        const QSize renderKeyPixmapSize( 90, 90 );
        const QSize renderKeyHorzPixmapSize( 192, 90 );
        const QSize renderKey3XHorzPixmapSize( 294, 90 );

        button_1_1 = renderKeyPixmap.drawControl( renderKeyPixmapSize, hobart::ReadOnly );
        button_2_1 = renderKeyPixmap.drawControl( renderKeyHorzPixmapSize, hobart::ReadOnly );
        button_3_1 = renderGreenKeyPixmap.drawControl( renderKey3XHorzPixmapSize, hobart::ReadOnly );
    }
    else
    {
        const hobart::backgroundWidgetRenderer renderKeyPixmap(
            new hobart::panelButtonBackgroundWidget( 4 ),
            new hobart::darkButtonBackgroundWidgetRendererAppearance );

        const hobart::backgroundWidgetRenderer renderGreenKeyPixmap(
            new hobart::panelButtonBackgroundWidget( 4 ),
            new hobart::greenButtonBackgroundWidgetRendererAppearance );

        const QSize renderKeyPixmapSize( 75, 75 );
        const QSize renderKeyHorzPixmapSize( 155, 75 );
        const QSize renderKey3XHorzPixmapSize( 239, 75 );

        button_1_1 = renderKeyPixmap.drawControl( renderKeyPixmapSize, hobart::ReadOnly );
        button_2_1 = renderKeyPixmap.drawControl( renderKeyHorzPixmapSize, hobart::ReadOnly );
        button_3_1 = renderGreenKeyPixmap.drawControl( renderKey3XHorzPixmapSize, hobart::ReadOnly );
    }

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
    buttonInfoList.append( buttonInfo( ENTER_KEY, button_3_1 ) );       // added enter key

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

    enter_ = buttons_[ENTER_KEY];           // added enter key
    enter_->setFont( clearButtonFont );     // added enter key

    // ------------------------------------------------------------- //

    // dont have this value initialized yet
    // if( uiRefresh_ )
    // {
    int printHeight( 178 );
    int screenheight = checkScreenHeight();
    int spacing( (screenheight - ((printHeight * 2) + 35) ) / 2 );

    ////////////// START WIDGET SETUP /////////////////
    scanAndPrint_ = new QWidget();
    scanAndPrint_->setObjectName(QStringLiteral("scanAndPrint"));
    // scanAndPrint_->setStyleSheet(QLatin1String("border: 3px solid rgb(255,204,204);"));
    // scanAndPrint_->setContentsMargins( QMargins( 0, 0, 0, 0 ) );             // L,T,R,B - use for initial wiggle for keypads on BC screen

    layoutWidget1_ = new QWidget(scanAndPrint_);
    layoutWidget1_->setObjectName(QStringLiteral("layoutWidget1"));
    // layoutWidget1_->setContentsMargins( QMargins( 0, 0, 0, 0 ) );            // L,T,R,B - use for initial wiggle for keypads on BC screen

    if( screensize == 10 )
        layoutWidget1_->setGeometry(QRect(0, 0, 241, 561));
    else
         layoutWidget1_->setGeometry(QRect(0, 0, bgSize, npSize + 70));                        // bgSize - 10

    layPrintAndScan_ = new QVBoxLayout(layoutWidget1_);
    layPrintAndScan_->setSpacing(0);
    layPrintAndScan_->setObjectName(QStringLiteral("layPrintAndScan"));
    layPrintAndScan_->setContentsMargins(2, 2, 2, 2);
    // layPrintAndScan_->setContentsMargins(0, 0, 0, 0);                        // L,T,R,B - use for initial wiggle for keypads on BC screen

    topSpacer_ = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layPrintAndScan_->addItem(topSpacer_);
    topSpacer_->setGeometry( QRect( 1, 1, 20, spacing) );

    bigPrintKey_ = new hobart::pushButton(layoutWidget1_);
    bigPrintKey_->setObjectName(QStringLiteral("bigPrintKey"));
    bigPrintKey_->setFixedSize( bgSize - 12, printHeight - 3 );

    QFont font3;
    font3.setPointSize(18);
    bigPrintKey_->setFont(font3);
    bigPrintKey_->setStyleSheet(QLatin1String("color: white;\n"));
    // bigPrintKey_->setStyleSheet(QLatin1String("color: white;\n" "border: 3px solid rgb(255,255,255);"));

    layPrintAndScan_->addWidget(bigPrintKey_);

    lblScanBarcode_ = new QLabel(layoutWidget1_);
    lblScanBarcode_->setObjectName(QStringLiteral("lblScanBarcode"));
    lblScanBarcode_->setFixedSize( bgSize - 12, 35 );

    lblScanBarcode_->setFont(font3);
    lblScanBarcode_->setStyleSheet(QLatin1String("color: rgb(0, 238, 0);\n" "background: rgb(0, 0, 0);"));
    lblScanBarcode_->setAlignment(Qt::AlignCenter);

    layPrintAndScan_->addWidget(lblScanBarcode_);

    barcodeOnly_ = new QWidget(layoutWidget1_);
    barcodeOnly_->setObjectName(QStringLiteral("barcodeOnly"));
    barcodeOnly_->setFixedSize( bgSize - 12, printHeight - 2 );

    barcodeOnly_->setAutoFillBackground(false);
    barcodeOnly_->setStyleSheet(QLatin1String("border: 3px solid rgb(0,0,0);"));

    layBarcodeOnly_ = new QHBoxLayout(barcodeOnly_);
    layBarcodeOnly_->setSpacing(0);
    layBarcodeOnly_->setObjectName(QStringLiteral("layBarcodeOnly"));
    layBarcodeOnly_->setContentsMargins(0, 0, 0, 0);

    layPrintAndScan_->addWidget(barcodeOnly_);

    bottomSpacer_ = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layPrintAndScan_->addItem(bottomSpacer_);
    bottomSpacer_->setGeometry( QRect( 1, 1, 20, spacing) );

    bigPrintKey_->setText( tr("Print Label") );
    lblScanBarcode_->setText( tr("SCAN BARCODE") );

    ////////////// START PROPERTIES /////////////////

    const hobart::backgroundWidgetRenderer renderGreenButton( new hobart::panelButtonBackgroundWidget( 4 ), new hobart::greenButtonBackgroundWidgetRendererAppearance() );
    const QPixmap greenButtonPixmap( renderGreenButton.drawControl( QSize( bgSize - 12, printHeight ), hobart::ReadOnly ) );
    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    printLabel_ = new hobart::pushButton( bigPrintKey_, greenButtonPixmap );

    QFont font( printLabel_->font() );
    font.setPointSize( 18 );
    printLabel_->setFont( font );

    connect( printLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printLabel_, printLabel_ );

    bigPrintKey_ = printLabel_;
    barcodeOnly_->setFixedSize( bgSize - 12, printHeight - 2 );
    bigPrintKey_->setFixedSize( bgSize - 12, printHeight - 3 );
    printLabel_->setFixedSize(  bgSize - 2, printHeight - 2 );

    ////////////// END PROPERTIES /////////////////

    //}     // end if( uiRefresh_ )

    // ------------------------------------------------------------- //

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceNumberPad::checkScreenSize()
{
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceNumberPad::checkScreenHeight()
{
    int screen_height = 600;        // 10 inch

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        screen_height = resolution_.width();
    }

    return screen_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceNumberPad::createLayout()
{
    LOG_CLASS_INFO( "selfServiceNumberPad", "createLayout" );

    int spSize = 10;
    int bgSize = 318;                       // 14 inch
    int npSize = 480;                       // 14 inch
    int screensize = checkScreenSize();

    if( screensize == 10 )
    {
        LOG_INFO( "Screen Size: 10", NULL );

        spSize = 6;
        bgSize = 268;
        npSize = 416;

        // scale down the keypad so it fits on 10 inch screens
        buttonsArea_->setFixedHeight( 396 );
    }

    QVBoxLayout *widgetLayout( new QVBoxLayout( this ) );
    widgetLayout->setContentsMargins( QMargins() );
    // widgetLayout->setContentsMargins( QMargins(0, 0, 0, 0) );                // L,T,R,B - use for initial wiggle for keypads on BC screen

    widgetLayout->addStretch();
    stacked_ = new QStackedWidget( this );
    stacked_->setFixedSize( bgSize, 90 );
    stacked_->addWidget( instructions_  );
    stacked_->addWidget( lineEdit_  );
    // stacked_->setContentsMargins( QMargins(0, 0, 0, 0) );                    // L,T,R,B - use for initial wiggle for keypads on BC screen

    // dont have this value set yet
    //if( uiRefresh_ )
    //{
    stackedNums_ = new QStackedWidget( this );
    stackedNums_->setObjectName(QStringLiteral("stackedNums"));
    stackedNums_->setFixedSize( bgSize, npSize + 70);

    // always double stacking at this point
    stackedNums_->addWidget( buttonsArea_  );
    stackedNums_->addWidget( scanAndPrint_  );
    //stackedNums_->setStyleSheet(QLatin1String("border: 3px solid rgb(204,204,255);"));      // light purple
    //}     // end if( uiRefresh_ )

    widgetLayout->addWidget( stacked_ );

    // not available - can I make it available?
    //if( uiRefresh_ )
    widgetLayout->addWidget( stackedNums_ );
    //else
    //    widgetLayout->addWidget( buttonsArea_ );

    widgetLayout->addStretch();

    QString sizemsg;
    sizemsg = "\nwidgetLayout.width: " + QString::number(widgetLayout->geometry().width() );
    sizemsg = sizemsg + "\nstacked_.width: " + QString::number( stacked_->width() );
    sizemsg = sizemsg + "\nbuttonsArea_.width: " + QString::number( buttonsArea_->width() );
    sizemsg = sizemsg + "\nstackedNums_.width: " + QString::number( stackedNums_->width() );
    //QMessageBox::information(this, "Resolution", sizemsg, QMessageBox::Ok);

    displayStackedNumWidget(0);      // 0 = keypad, 1=Print&Scan

    // ----- //

    QVBoxLayout *buttonsAreaLayout( new QVBoxLayout( buttonsArea_ ) );
    buttonsAreaLayout->setContentsMargins( QMargins( spSize, spSize, spSize, spSize ) );
    buttonsAreaLayout->setSpacing( spSize );
    // buttonsAreaLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );         // L,T,R,B - use for initial wiggle for keypads on BC screen

    QHBoxLayout *row_1( new QHBoxLayout() );
    row_1->setContentsMargins( QMargins() );
    row_1->setSpacing( spSize );

    QHBoxLayout *row_2( new QHBoxLayout() );
    row_2->setContentsMargins( QMargins() );
    row_2->setSpacing( spSize );

    QHBoxLayout *row_3( new QHBoxLayout() );
    row_3->setContentsMargins( QMargins() );
    row_3->setSpacing( spSize );

    QHBoxLayout *row_4( new QHBoxLayout() );
    row_4->setContentsMargins( QMargins() );
    row_4->setSpacing( spSize );

    QHBoxLayout *row_5( new QHBoxLayout() );
    row_5->setContentsMargins( QMargins() );
    row_5->setSpacing( spSize );

    buttonsAreaLayout->addLayout( row_1 );
    buttonsAreaLayout->addLayout( row_2 );
    buttonsAreaLayout->addLayout( row_3 );
    buttonsAreaLayout->addLayout( row_4 );
    buttonsAreaLayout->addLayout( row_5 );

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

    row_5->addStretch();
    row_5->addWidget( buttons_[ENTER_KEY] );    // added enter key
    row_5->addStretch();
}

