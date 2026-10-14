/**
 * @file labeleddateedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeleddateedit.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include "../dialogs/calendardialog.h"

#include "Messages.h"
#include "ScaleConfiguration.h"

#include <QTimer>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDateEdit::labeledDateEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    dialogOpen_( false ),
    basisDate_( QDate::currentDate() ),
    doNotPrint_( false ),
    doNotPrintEnabled_( false ),
    hoursModifier_( 0 ),
    daysMax_( 0 ),
    backward_( false ),
    timer_( NULL ),
    mainWindow_( NULL )
{
    setObjectName( "hobartLabeledDateEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDateEdit::labeledDateEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image ),
    dialogOpen_( false ),
    basisDate_( QDate::currentDate() ),
    doNotPrint_( false ),
    doNotPrintEnabled_( false ),
    hoursModifier_( 0 ),
    daysMax_( 0 ),
    backward_( false ),
    timer_( NULL ),
    mainWindow_( NULL )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDateEdit::~labeledDateEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::translate()
{
    QString value( text() );

    if ( !value.isEmpty() )
    {
        setText( toString( date_ ) );
        update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::clear()
{
    date_ = QDate();
    doNotPrint_ = false;

    _Mybase::clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::setDate( const QDate& value )
{
    if ( date_ == value )
        return;

    date_ = value;
    setText( toString( date_ ) );

    Q_EMIT dateChanged();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::setDoNotPrint( bool value )
{
    if ( doNotPrint_ == value )
        return;

    doNotPrint_ = value;

    Q_EMIT doNotPrintChanged();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::setDoNotPrintEnabled( bool value )
{
    doNotPrintEnabled_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::promptForInput()
{
    // check access level
    if ( !checkOperatorAccessLevel() )
    {
        Q_EMIT invalidAccessLevel();
        return;
    }

    // pop the dialog
    timer_->setSingleShot( true );
    timer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledDateEdit::eventFilter( QObject *obj, QEvent *event )
{
    // for focus in we handle the event ourselves
    if ( child_ == obj )
        if ( QEvent::FocusIn == event->type() )
        {
            // prompt
            promptForInput();

            // eat the event
            return true;
        }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::mousePressEvent( QMouseEvent *e )
{
    // when widget is read only and do not print is enabled show do not print
    if ( Qt::LeftButton == e->button() )
        if (( isReadOnly() ) && ( doNotPrintEnabled_ ))
        {
            // prompt
            promptForInput();
        }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::onSelectionChanged()
{
    // do not allow text selection in this widget
    if ( child_->hasSelectedText() )
        child_->deselect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::onTimeout()
{
    CalendarDialog dialog( mainWindow_ );
    int rc;

    // set enabled (not read only when forced)
    dialog.setCalendarEnabled( (isReadOnly() && !isForced()) ? false : true );

    // set required
    dialog.setRequired( isForced() );

    // set do not print
    dialog.setDoNotPrintVisible( doNotPrintEnabled_ );
    dialog.setDoNotPrint( doNotPrint_ );

    if ( backward_ )
        dialog.setDaysDirection( CalendarDialog::Backward );

    if ( daysMax_ )
        dialog.setDaysMax( daysMax_ );

    if ( hoursModifier_ )
        dialog.setHoursModifier( hoursModifier_ );

    // set date
    dialog.setBasisDate( basisDate_ );
    dialog.setDate( date() );

    // set dialog title
    dialog.setTitle( labelText() );

    // open the dialog
    dialogOpen_ = true;

    if ( QDialog::Accepted == (rc = dialog.exec()) )
    {
        setDate( dialog.date() );

        if ( doNotPrintEnabled_ )
            setDoNotPrint( dialog.doNotPrint() );
    }

    // done
    dialogOpen_ = false;
    updateBackgroundPalette();

    Q_EMIT dialogActionComplete( rc );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDateEdit::initialize()
{
    // create timer
    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    // retrieve main window
    sessionInfo mySession( this );
    mainWindow_ = mySession();

    // connect signals
    connect( child_, SIGNAL( selectionChanged() ), SLOT( onSelectionChanged() ) );

    // install event filter
    child_->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledDateEdit::convertDateToString( const QDate &date, int dateStyle )
{
    bool fixShortMonth = false;
    bool fixShortMonthToCaps = false;

    QString result;

    // retrieve locale
    QLocale locale;

    // handle date format
    switch ( dateStyle )
    {
    // "FE 28, 13"
    case 0:
        fixShortMonth = true;
        // drop into next case

    // "Feb 28, 13"
    case 1:
        result = locale.toString( date, "MMM dd, yy" );
        break;

    // "13/02/28"
    case 2:
        result = locale.toString( date, "yy/MM/dd" );
        break;

    // "02/28/13"
    case 3:
        result = locale.toString( date, "MM/dd/yy" );
        break;

    // "28/Feb/13"
    case 5:
        result = locale.toString( date, "dd/MMM/yy" );
        break;

    // "28/02/13"
    case 6:
        result = locale.toString( date, "dd/MM/yy" );
        break;

    // "13 FE 28"
    case 7:
        fixShortMonth = true;
        result = locale.toString( date, "yy MMM dd" );
        break;

    // "28 FEB 13"
    case 8:
        fixShortMonthToCaps = true;
        result = locale.toString( date, "dd MMM yy" );
        break;

    default:
        result = date.toString( "MMM dd, yy" );
        break;
    }

    // replace month name with abbreviated month name
    if ( fixShortMonth )
    {
        QString oldName( locale.monthName( date.month(), QLocale::ShortFormat ) );

        if ( result.contains( oldName ) )
        {
            QString newName;

            // the abbreviated months are only in english locale
            switch ( date.month() )
            {
            case 1: newName = QApplication::tr( "JA" ); break;
            case 2: newName = QApplication::tr( "FE" ); break;
            case 3: newName = QApplication::tr( "MR" ); break;
            case 4: newName = QApplication::tr( "AL" ); break;
            case 5: newName = QApplication::tr( "MA" ); break;
            case 6: newName = QApplication::tr( "JN" ); break;
            case 7: newName = QApplication::tr( "JL" ); break;
            case 8: newName = QApplication::tr( "AU" ); break;
            case 9: newName = QApplication::tr( "SE" ); break;
            case 10: newName = QApplication::tr( "OC" ); break;
            case 11: newName = QApplication::tr( "NO" ); break;
            case 12: newName = QApplication::tr( "DE" ); break;
            default:
                break;
            }

            if ( newName.length() )
                result.replace( oldName, newName );
        }
    }

    if ( fixShortMonthToCaps )
    {
        // I suppose I could have Capitalized these using .toTitleCase() and QLatin1String(),
        // but I see above we are only doing this for English anyways
        QString oldName( locale.monthName( date.month(), QLocale::ShortFormat ) );

        if ( result.contains( oldName ) )
        {
            QString newName;

            // the abbreviated months are only in english locale
            switch ( date.month() )
            {
            case 1: newName = QApplication::tr( "JAN" ); break;
            case 2: newName = QApplication::tr( "FEB" ); break;
            case 3: newName = QApplication::tr( "MAR" ); break;
            case 4: newName = QApplication::tr( "APR" ); break;
            case 5: newName = QApplication::tr( "MAY" ); break;
            case 6: newName = QApplication::tr( "JUN" ); break;
            case 7: newName = QApplication::tr( "JUL" ); break;
            case 8: newName = QApplication::tr( "AUG" ); break;
            case 9: newName = QApplication::tr( "SEP" ); break;
            case 10: newName = QApplication::tr( "OCT" ); break;
            case 11: newName = QApplication::tr( "NOV" ); break;
            case 12: newName = QApplication::tr( "DEC" ); break;
            default:
                break;
            }

            if ( newName.length() )
                result.replace( oldName, newName );
        }
    }

    return result;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledDateEdit::convertTimeToString( const QTime &time, int timeStyle )
{
    QString result;

    switch ( timeStyle )
    {
    // military time
    case 1:
        result = time.toString( "hh:mm:ss" );
        break;

    // normal (am/pm) time
    case 0:
    default:
        result = time.toString( "hh:mm:ss AP" );
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledDateEdit::convertDateTimeToString( const QDateTime &datetime, int dateStyle, int timeStyle )
{
    return convertDateToString( datetime.date(), dateStyle ) + " " +
        convertTimeToString( datetime.time(), timeStyle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledDateEdit::toString( const QDate &date )
{
    // retrieve style
    int dateStyle = (mainWindow_ ? mainWindow_->session()->scaleConfig->scaleConfig.dateStyle() : 1);

    // convert!
    return convertDateToString( date, dateStyle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
