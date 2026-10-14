/**
 * @file selfserveByCountQtyG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#include "selfserveByCountQtyG2.h"
#include "ui_selfserveByCountQtyG2.h"

#include <logmanager.h>

#include <Session.h>
#include <sessionhooks.h>
#include <ScaleClasses.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <QSignalMapper>
#include <QGraphicsOpacityEffect>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCountQtyG2::selfserveByCountQtyG2(const QString &qty, QWidget *parent ) :
    _Mybase( parent ),
    ui( new Ui::selfserveByCountQtyG2 ),
    bycount_( qty ),
    activeKey_( NULL ),
    validator_( NULL )
{
    //LOG_CLASS_INFO( "selfserveByCountQtyG2", "selfserveByCountQtyG2" );

    ui->setupUi( this );

    // initialize
    initialize();
    translate();

    // incoming quantity
    if( ( bycount_ != "0" ) )
        quantity_->setText( bycount_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCountQtyG2::~selfserveByCountQtyG2()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::showEvent( QShowEvent *e )
{
    Q_UNUSED( e );

    quantity_->setText( "" );                           // always clear opening or closing. One of them. Maybe on Enter
    ui->txtQuantity->clear();

    // didnt work - also made the children have partial opacity like the numpad.  I did not want that
    //QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    //opacityEffect->setOpacity(.30);
    //ui->wgtNumpadBack->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::onButtonClicked( QWidget *w )
{
    if( backspace_ == w )
    {
        QString value( quantity_->text() );
        if( value.length() > 1 )
            value.chop(1);
        else if( value.length() == 1 )
            value = "";

        if ( validator_ )
        {
            if ( value.toInt() < validator_->bottom() )
                return;
            else if ( validator_->top() < value.toInt() )
                return;
        }

        quantity_->setText( value );
    }
    else if ( enter_ == w )
    {
        quantity_->repaint();

        QString byCount( quantity_->text() );

        if ( byCount.isEmpty() )
            byCount = "0";

        // update by count
        emit enterClicked( byCount );                       // moved update of bycount to SLOT in selfservebyCountG2

        quantity_->setText( "" );                           // always clear opening or closing. One of them. Maybe on Enter
        ui->txtQuantity->clear();

        if( byCount.toInt() > 0 )
            hide();
        else
        {
            emit printError( ProductDevError::ProductErrPrintBadByCount );
        }

        return;
    }
    else if ( cancel_ == w )
    {
        quantity_->repaint();

        QString byCount( "0" );

        // update by count
        //emit enterClicked( byCount );                       // moved update of bycount to SLOT in selfservebyCountG2
        emit cancelClicked();

        quantity_->setText( "0" );                           // always clear opening or closing. One of them. Maybe on Enter

        //emit printError( ProductDevError::ProductErrPrintBadByCount );        // just return to lookup now - no error

        quantity_->setText( "" );                           // always clear opening or closing. One of them. Maybe on Enter
        ui->txtQuantity->clear();

        hide();

        return;
    }
    else
    {
        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        // append button text to line edit
        if ( button )
        {
            // in case there is a key already pressed and it hasn't timed out yet
            if( activeKey_ )
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );

            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(247,248,250);\n" "color: rgb(128,128,128);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
            activeKey_ = button;

            cycleNumpadTimer_->start(500);
            cycleNumpadTimer_->setSingleShot(true);

            QString bc( quantity_->text() );
            QString value = "";
            if( ( "0" == bc ) || ( "" == bc ) )
            {
                value = button->text();
            }
            else
            {
                value = bc.append( button->text() );
                if( value.length() > 2 )
                    value.remove(0, 1);
            }

            value.remove( QRegExp("^[0]*") );

            quantity_->setText( value );

            //if ( validator_ )
            //{
            //    if ( value.toInt() < validator_->bottom() )
            //        return;
            //    else if ( validator_->top() < value.toInt() )
            //        return;
            //}

        }       // if a button
    }

    emit resetIdleInactive();                           // emits hook to reset idle timer
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::resetKeypadDisplay()
{
    quantity_->clear();                         // always clear opening or closing. One of them. Maybe on Enter
    ui->txtQuantity->clear();
}

void selfserveByCountQtyG2::resetEntry()
{
    quantity_->clear();                          // always clear opening or closing. One of them. Maybe on Enter
    ui->txtQuantity->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::onNumberPadTextChanged( const QString& text )
{
    QString byCount( text );

    if ( byCount.isEmpty() )
        byCount = "0";

    // update by count
    emit enterClicked( byCount );                       // moved update of bycount to SLOT in selfservebyCountG2

    emit resetIdleInactive();                           // emits hook to reset idle timer
}

void selfserveByCountQtyG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveByCountQtyG2::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountQtyG2::initialize()
{
    quantity_           = ui->txtQuantity;
    digitZero_          = ui->butZero;
    digitOne_           = ui->butOne;
    digitTwo_           = ui->butTwo;
    digitThree_         = ui->butThree;
    digitFour_          = ui->butFour;
    digitFive_          = ui->butFive;
    digitSix_           = ui->butSix;
    digitSeven_         = ui->butSeven;
    digitEight_         = ui->butEight;
    digitNine_          = ui->butNine;

    enter_              = ui->butEnter;
    cancel_             = ui->butExit;
    backspace_          = ui->butBackspace;

    //quantity_->setValidator( new QIntValidator( 0, 99, this ) );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    connect( cancel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( cancel_, cancel_ );

    connect( backspace_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspace_, backspace_ );

    connect( digitZero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitZero_, digitZero_ );
    connect( digitOne_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitOne_, digitOne_ );
    connect( digitTwo_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitTwo_, digitTwo_ );
    connect( digitThree_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitThree_, digitThree_ );
    connect( digitFour_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitFour_, digitFour_ );
    connect( digitFive_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitFive_, digitFive_ );
    connect( digitSix_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitSix_, digitSix_ );
    connect( digitSeven_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitSeven_, digitSeven_ );
    connect( digitEight_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitEight_, digitEight_ );
    connect( digitNine_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitNine_, digitNine_ );

    cycleNumpadTimer_ = new QTimer(this);
    cycleNumpadTimer_->setInterval(500);         // half sec timeout

    connect( cycleNumpadTimer_, SIGNAL( timeout() ), this, SLOT( onKeypadTimeout() ) );

    quantity_->setValidator(numValidator_ = new QIntValidator(0, INT_MAX, this));
    quantity_->setDecimalPlaces(0);
    quantity_->setInputWrapping(true);
    numValidator_->setTop(makeValidatorTop(2));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart namespace
