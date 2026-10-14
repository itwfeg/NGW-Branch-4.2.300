/**
 * @file calendardialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "calendardialog.h"

#include "../widgets/labeleddateedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <ScaleConfiguration.h>

#include <QCheckBox>
#include <QDate>
#include <QEvent>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

const bool CalendarDialog::convertUppercase_ = true;

const int CalendarDialog::dateMaxDays_ = 999;
const int CalendarDialog::doNotPrintMagic_ = 999;

///////////////////////////////////////////////////////////////////////////////////////////////////
CalendarDialog::CalendarDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    now_( session_ ? session_->scaleConfig->currentDate() : QDate::currentDate() ),
    calendarEnabled_( true ),
    required_( false ),
    daysDirection_( Forward ),
    ignoreSignals_( true )
{
    setObjectName( "CalendarDialog" );

    // initialize interface
    initialize();

    // set date
    setDate( now_ );

    // setup focus proxy
    setFocusProxy( days_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CalendarDialog::CalendarDialog( const QDate& value, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    now_( session_ ? session_->scaleConfig->currentDate() : QDate::currentDate() ),
    calendarEnabled_( true ),
    required_( false ),
    daysDirection_( Forward )
{
    // initialize interface
    initialize();

    // set date
    QDate date( value );

    // validate date
    if (( date.isNull() ) || ( !date.isValid() ))
        date = now_;

    setDate( date );

    // setup focus proxy
    setFocusProxy( days_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CalendarDialog::CalendarDialog( int value, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    now_( session_ ? session_->scaleConfig->currentDate() : QDate::currentDate() ),
    required_( false ),
    daysDirection_( Forward )
{
    // initialize interface
    initialize();

    // set date
    selectedDate_ = now_;
    setDays( value );

    // setup focus proxy
    setFocusProxy( days_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CalendarDialog::~CalendarDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CalendarDialog::days() const
{
    int value = 0;

    if ( Backward == daysDirection() )
        value = (now_.toJulianDay() - selectedDate_.toJulianDay());
    else if ( Forward == daysDirection() )
        value = (selectedDate_.toJulianDay() - now_.toJulianDay());

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CalendarDialog::daysMax() const
{
    return daysValidator_->top();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CalendarDialog::daysMin() const
{
    return daysValidator_->bottom();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CalendarDialog::doNotPrint() const
{
    return doNotPrint_->isChecked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CalendarDialog::doNotPrintVisible() const
{
    return doNotPrint_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDate CalendarDialog::rangeMax() const
{
    if ( Backward == daysDirection() )
        return now_;

    return now_.addDays( daysValidator_->top() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDate CalendarDialog::rangeMin() const
{
    if ( Forward == daysDirection() )
        return now_;

    return now_.addDays( -daysValidator_->top() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CalendarDialog::isValidDate( const QDate& value ) const
{
    return (( !value.isNull() ) && ( value.isValid() ) && ( rangeMin() <= value ) && ( value <= rangeMax() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDate( const QDate& value )
{
    // validate date
    if (( value.isNull() ) || ( !value.isValid() ) || ( selectedDate_ == value ))
        return;

    // set date!
    selectedDate_ = value;

    updateCalendar();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDays( int value )
{
    setDate( now_.addDays( (Backward == daysDirection() ? -1 : 1) * value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDaysDirection( Direction value )
{
    if ( daysDirection_ == value )
        return;

    int selectedDays = days();

    daysDirection_ = value;
    setDays( selectedDays );

    updateCalendar();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDaysMax( int value )
{
    if (( value < 0 ) || ( value < daysMin() ) || ( daysMax() == value ))
        return;

    int selectedDays = days();

    daysValidator_->setTop( value );

    if ( value < selectedDays )
        setDays( value );

    updateCalendar();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDaysMin( int value )
{
    if (( value < 0 ) || ( daysMax() < value ) || ( daysMin() == value ))
        return;

    int selectedDays = days();

    daysValidator_->setBottom( value );

    if ( selectedDays < value )
        setDays( value );

    updateCalendar();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDoNotPrint( bool value )
{
    doNotPrint_->setChecked( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setDoNotPrintVisible( bool value )
{
    doNotPrint_->setVisible( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setHoursModifier( int value )
{
    hoursModifier_->setText( QString::number( value ) + "h" );
    hoursModifier_->setVisible( 0 < value );

    QDateTime temp( now_, session_->scaleConfig->currentTime() );
    temp = temp.addSecs( 3600 * value );

    now_ = temp.date();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setRangeMax( const QDate& value )
{
    if (( value.isNull() ) || ( !value.isValid() ))
        return;

    if ( Backward == daysDirection() )
        setDaysMin( now_.toJulianDay() - value.toJulianDay() );
    else if ( Forward == daysDirection() )
        setDaysMax( value.toJulianDay() - now_.toJulianDay() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setRangeMin( const QDate& value )
{
    if (( value.isNull() ) || ( !value.isValid() ))
        return;

    if ( Backward == daysDirection() )
        setDaysMax( now_.toJulianDay() - value.toJulianDay() );
    else if ( Forward == daysDirection() )
        setDaysMin( value.toJulianDay() - now_.toJulianDay() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::setTitle( const QString& value )
{
    title_->setText( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::title() const
{
    return title_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CalendarDialog::eventFilter( QObject *obj, QEvent *event )
{
    if ( days_ == obj )
        if ( QEvent::FocusOut == event->type() )
        {
            QString text( days_->text() );

            // handle special case when enter button pressed and no text entered
            if ( text.isEmpty() )
            {
                reject();
                return true;
            }
        }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::showEvent( QShowEvent *event )
{
    _Mybase::showEvent( event );

    // save off original date
    original_ = selectedDate_;

    // FT-502
    updateCalendar();

    // focus to days line edit and highlight selection
    days_->setFocus();

    if ( calendarEnabled_ )
        days_->selectAll();

    ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *CalendarDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CalendarDialog::rendererStyle() const
{
    // show focused border
    return hobart::Focused;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onDateClicked( QWidget *widget )
{
    if ( ignoreSignals_ )
        return;

    QPushButton *button = dynamic_cast<QPushButton*>( widget );

    // sanity check
    if ( !button )
        return;

    QString buttonText( button->text() );

    // check for valid day
    if ( buttonText.isEmpty() )
        return;

    QDate date;

    // check for valid date
    if ( date.setDate( selectedDate_.year(), selectedDate_.month(), buttonText.toInt() ) )
    {
        setDate( date );

        if ( isValidDate() )
            accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onDaysTextEdited( const QString& value )
{
    if ( ignoreSignals_ )
        return;

    setDays( value.toInt() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onDaysTextEditingFinished()
{
    if ( ignoreSignals_ )
        return;

    // when operator enters the magic do not print days number he (or she) really means do not
    // print this field... as per access scale logic
    if (( doNotPrintMagic_ == days() ) && ( doNotPrintVisible() ))
    {
        selectedDate_ = original_;
        setDoNotPrint( true );
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onDoNotPrintToggled( bool value )
{
    Q_UNUSED( value );

    if ( ignoreSignals_ )
        return;

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onPrevMonthClicked()
{
    if ( ignoreSignals_ )
        return;

    QDate newDate( date().addMonths( -1 ) );

    if ( newDate < rangeMin() )
        newDate = rangeMin();

    setDate( newDate );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::onNextMonthClicked()
{
    if ( ignoreSignals_ )
        return;

    QDate newDate( date().addMonths( 1 ) );

    if ( rangeMax() < newDate )
        newDate = rangeMax();

    setDate( newDate );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::initialize()
{
    QVBoxLayout *verticalLayout;

    // setup layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins( QMargins() );
    topLayout->setSpacing( 0 );
    screenLayout->addLayout( topLayout );

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins( QMargins() );
    bottomLayout->setSpacing( 0 );
    screenLayout->addLayout( bottomLayout );

    modifyPaletteBackground setTransparentBackground( Qt::transparent );
    modifyPaletteBackground setWhiteBackground( hobart::colorPalette::white );

    modifyPaletteColorRole<QPalette::Text> setCheckBoxForegroundColor( hobart::colorPalette::black );
    modifyPaletteColorRole<QPalette::Window> setCheckBoxMouseOverColor( Qt::transparent );

    modifyPaletteHighlight setTransparentHighlight( Qt::transparent );

    modifyFontPointSize setDaysEntryFontPointSize( (mainWindow_->height() * 48) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize( (mainWindow_->height() * 9) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    setForegroundPalette( this );

    ///////////////////////////////////////////////////////////////////////////
    // create signal mapper

    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onDateClicked(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // top row

    // previous month button
    prevMonth_ = new QPushButton( this );
    prevMonth_->setFixedSize( 32, 32 );
    prevMonth_->setFlat( true );
    prevMonth_->setFocusPolicy( Qt::NoFocus );
    prevMonth_->setIcon( QIcon( ":/images/dialogs/leftArrowIcon.png" ) );
    prevMonth_->setObjectName( "prevMonthButton" );
    setTransparentBackground( prevMonth_ );
    topLayout->addWidget( prevMonth_ );

    connect( prevMonth_, SIGNAL( clicked() ), SLOT( onPrevMonthClicked() ) );

    // current month name label
    month_ = new QLabel( this );
    month_->setAlignment( Qt::AlignCenter );
    month_->setFixedWidth( 120 );
    month_->setObjectName( "month" );
    topLayout->addWidget( month_ );

    // next month button
    nextMonth_ = new QPushButton( this );
    nextMonth_->setFixedSize( 32, 32 );
    nextMonth_->setFlat( true );
    nextMonth_->setFocusPolicy( Qt::NoFocus );
    nextMonth_->setIcon( QIcon( ":/images/dialogs/rightArrowIcon.png" ) );
    nextMonth_->setObjectName( "nextMonthButton" );
    setTransparentBackground( nextMonth_ );
    topLayout->addWidget( nextMonth_ );

    connect( nextMonth_, SIGNAL( clicked() ), SLOT( onNextMonthClicked() ) );

    topLayout->addStretch();

    // dialog title
    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );
    setFontPointSize( title_ );
    topLayout->addWidget( title_ );

    topLayout->addStretch();

    // selected date label
    date_ = new QLabel( this );
    date_->setAlignment( Qt::AlignCenter );
    date_->setObjectName( "date" );
    setFontPointSize( date_ );
    topLayout->addWidget( date_ );

    ///////////////////////////////////////////////////////////////////////////
    // options

    verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins( QMargins() );
    verticalLayout->setSpacing( 0 );

    // text values
    QString daysText( tr( "Number of Days:" ) );
    QString doNotPrintText( tr( "Do Not Print" ) );

    if ( convertUppercase_ )
    {
        daysText = daysText.toUpper();
        doNotPrintText = doNotPrintText.toUpper();
    }

    verticalLayout->addStretch();

    // number of days label
    QLabel *daysLabel = new QLabel( daysText, this );
    daysLabel->setObjectName( "daysLabel" );
    setFontPointSize( daysLabel );
    verticalLayout->addWidget( daysLabel );

    // number of days
    days_ = new QLineEdit( this );
    setDaysEntryFontPointSize( days_ );
    setTransparentBackground( days_ );
    setTransparentHighlight( days_ );
    days_->setAutoFillBackground( true );
    days_->setFrame( false );
    days_->setObjectName( "days" );
    days_->setValidator( daysValidator_ = new QIntValidator( this ) );
    verticalLayout->addWidget( days_ );

    connect( days_, SIGNAL( textEdited(QString) ), SLOT( onDaysTextEdited(QString) ) );
    connect( days_, SIGNAL( editingFinished() ), SLOT( onDaysTextEditingFinished() ) );

    daysValidator_->setBottom( 0 );
    daysValidator_->setTop( dateMaxDays_ );

    // hours modifier
    hoursModifier_ = new QLabel( this );
    hoursModifier_->setObjectName( "hoursModifierLabel" );
    setFontPointSize( hoursModifier_ );
    verticalLayout->addWidget( hoursModifier_ );

    verticalLayout->addStretch();

    // do not print
    doNotPrint_ = new QCheckBox( doNotPrintText, this );
    setCheckBoxForegroundColor( doNotPrint_ );
    setCheckBoxMouseOverColor( doNotPrint_ );
    setFontPointSize( doNotPrint_ );
    setTransparentBackground( doNotPrint_ );
    doNotPrint_->setAutoFillBackground( true );
    doNotPrint_->setFocusPolicy( Qt::NoFocus );
    doNotPrint_->setObjectName( "doNotPrint" );
    doNotPrint_->setVisible( false ); // not visible by default
    verticalLayout->addWidget( doNotPrint_ );

    connect( doNotPrint_, SIGNAL( toggled(bool) ), SLOT( onDoNotPrintToggled(bool) ) );

    bottomLayout->addLayout( verticalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // calendar

    for ( int day = 0; day < 7; ++day )
    {
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        // vertical line
        if ( 0 < day )
        {
            QWidget *line = new QWidget( this );
            line->setFixedWidth( 2 );
            line->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
            setWhiteBackground( line );
            line->setAutoFillBackground( true );

            bottomLayout->addWidget( line );
        }

        // day of week text
        QString labelText( shortDayName( ((day + 6) % 7) + 1 ) );

        if ( convertUppercase_ )
            labelText = labelText.toUpper();

        // day of week
        QString labelObjectName( "dayOfWeek_%1" );

        QLabel *label = new QLabel( labelText, this );
        label->setAlignment( Qt::AlignCenter );
        label->setFixedHeight( 32 );
        label->setObjectName( labelObjectName.arg( day ) );
        setFontPointSize( label );
        verticalLayout->addWidget( label );

        labels_[ QString::number( day ) ] = label;

        // button for day
        for ( int dayButton = 0; dayButton < 6; ++dayButton )
        {
            QString buttonObjectName( "day_%1_%2" );

            QPushButton *button = new QPushButton( this );
            button->setFixedSize( 60, 32 );
            button->setFlat( true );
            button->setFocusPolicy( Qt::NoFocus );
            button->setObjectName( buttonObjectName.arg( day ).arg( dayButton ) );
            setTransparentBackground( button );
            verticalLayout->addWidget( button );

            buttons_[ QString::number( day ) + QString::number( dayButton ) ] = button;

            signalMapper_->setMapping( button, button );
            connect( button, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
        }

        bottomLayout->addLayout( verticalLayout );
    }

    ///////////////////////////////////////////////////////////////////////////
    // filters

    // install event filter
    days_->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CalendarDialog::updateCalendar()
{
    // retrieve style
    int dateStyle = (mainWindow_ ? mainWindow_->session()->scaleConfig->scaleConfig.dateStyle() : 1);

    // get strings
    QString month = toString( selectedDate_, "MMMM" );
    QString date = hobart::labeledDateEdit::convertDateToString( selectedDate_, dateStyle );

    // convert if needed
    if ( convertUppercase_ )
        month = month.toUpper();

    // set fields
    month_->setText( month );
    date_->setText( date );

    days_->setText( QString::number( days() ) );

    modifyPaletteForeground setBlueForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::blue, hobart::colorPalette::blue ) );
    modifyPaletteForeground setWhiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    QDate firstDayOfMonth( selectedDate_.year(), selectedDate_.month(), 1 );
    int dayNumber = 1 - (firstDayOfMonth.dayOfWeek() % 7);

    int dayButton;
    int day;

    // set week day labels to white
    for ( day = 0; day < 7; ++day )
        setWhiteForeground( labels_[ QString::number( day ) ] );

    // set day buttons
    for ( dayButton = 0; dayButton < 6; ++dayButton )
        for ( day = 0; day < 7; ++day )
        {
            QString labelText = "";

            if (( 0 < dayNumber ) && ( dayNumber <= selectedDate_.daysInMonth() ))
            {
                labelText = QString::number( dayNumber );

                if ( 1 == labelText.length() )
                    labelText = "0" + labelText;
            }

            QPushButton *button = buttons_[ QString::number( day ) + QString::number( dayButton ) ];

            // button text
            button->setText( labelText );

            // button color
            if ( labelText.isEmpty() )
                button->setEnabled( false );
            else if ( !isValidDate( QDate( selectedDate_.year(), selectedDate_.month(), dayNumber ) ) )
            {
                button->setEnabled( false );
                setWhiteForeground( button );
            }
            else
            {
                button->setEnabled( true );

                if ( dayNumber != selectedDate_.day() )
                    setWhiteForeground( button );
                else
                {
                    setBlueForeground( labels_[ QString::number( day ) ] );
                    setBlueForeground( button );
                }
            }

            ++dayNumber;
        }

    // check for disabled calendar
    if ( !calendarEnabled_ )
    {
        prevMonth_->setEnabled( false );
        nextMonth_->setEnabled( false );
        days_->setEnabled( false );

        foreach( QPushButton *button, buttons_ )
            button->setEnabled( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::shortDayName( int value )
{
    // retrieve locale
    QLocale locale;

    // make string
    return locale.standaloneDayName( value, QLocale::ShortFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::longDayName( int value )
{
    // retrieve locale
    QLocale locale;

    // make string
    return locale.standaloneDayName( value, QLocale::LongFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::shortMonthName( int value )
{
    // retrieve locale
    QLocale locale;

    // make string
    return locale.standaloneMonthName( value, QLocale::ShortFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::longMonthName( int value )
{
    // retrieve locale
    QLocale locale;

    // make string
    return locale.standaloneMonthName( value, QLocale::LongFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CalendarDialog::toString( const QDate &date, const QString& format )
{
    // retrieve locale
    QLocale locale;

    // make string
    return locale.toString( date, format );
}


