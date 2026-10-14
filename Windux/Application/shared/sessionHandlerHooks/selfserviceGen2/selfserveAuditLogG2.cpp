/**
 * @file selfserveAuditLogG2.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Job E. Crew
 */

#include "selfserveAuditLogG2.h"
#include "../sessionHandler/sessionmainwindow.h"
#include "../sessionHandler/overlaywidget.h"
#include "../sessionHandler/dialogs/filedialog.h"

#include "ui_selfserveAuditLogG2.h"
#include <backgroundwidgettypes.h>
#include <pushbutton.h>
#include <selfservicehook.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <Session.h>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPainter>
#include <QSignalMapper>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMouseEvent>
#include <QDesktopWidget>
#include "TokenParser.h"

static const int DEFAULT_ROWS = 8;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveAuditLogG2::selfserveAuditLogG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveAuditLogG2),
    hook_( h ),
    scaleConfig_( NULL ),
    unlocked_( false ),
    activeKey_( NULL ),
    cycleNumpadTimer_( NULL )
{
	ui->setupUi(this);
    setObjectName( "selfserveAuditLogG2" );

    // initialize
    initialize();

    // create layout
    createLayout();

    // translate
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );

    connect( hook_, SIGNAL( emitUpdateAuditLogList() ), SLOT( onEmitUpdateAuditLogList() ) );
    connect( hook_, SIGNAL( emitUpdateSaveMessage() ), SLOT( onEmitUpdateSaveMessage() ) );
}

selfserveAuditLogG2::~selfserveAuditLogG2()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::refresh()
{
    // check for unlocked screen
    if ( !unlocked_ )
    {
        if (( "1" == hook_->fieldValue( sessionHooks::selfServiceKey ) ) ||
            ( "1" == hook_->fieldValue( sessionHooks::serviceKey ) ) ||
            ( "1" == hook_->fieldValue( sessionHooks::supervisorKey ) ))
        {
            unlocked_ = true;
        }
    }

    quit_->setVisible( unlocked_ );
    title_->setVisible( !unlocked_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::translate()
{
    rezero_->setText( tr( "Rezero" ) );
    quit_->setText( tr( "Exit Self-\nService Mode" ) );

    title_->setText( tr( "Enter ID to exit self-service mode,\nthen press Enter." ) );

    printbut_->setText( tr("Print") );

    exitLabel_->setText( tr( "Close" ) );
    exit_->setText( "X" );

    userinfo_->setText( tr("Audit trail information...") );

    enter_->setText( tr( "Enter" ) );

    QStringList header_list;
    header_list << tr("Record") << tr("") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("");

    audittable_->setHorizontalHeaderLabels( header_list );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}


//////////////////////////////0/////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::calcItemColumnWidths()
{
    int width = audittable_->width();

    audittable_->setColumnWidth( 0, (width *  6) / 100 ); // audit item number
    audittable_->setColumnWidth( 1, 0 );                  // audit type
    audittable_->setColumnWidth( 2, (width * 15) / 100 ); // audit event
    audittable_->setColumnWidth( 3, (width * 34) / 100 ); // parameter
    audittable_->setColumnWidth( 4, (width * 21) / 100 ); // date/time
    audittable_->setColumnWidth( 5, (width * 18) / 100 ); // new value
    audittable_->setColumnWidth( 6, 0 );                  // old value
    //                                   -------------------
    //                                           94%             because of scrollbar width
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::showEvent( QShowEvent *e )
{
    opID_->blockSignals( true );
    opID_->clear();
    opID_->blockSignals( false );

    //------------------------------------------
    // Request the Weigher Audit Log
    //------------------------------------------
    hook_->sendSurrogateMsg( Weigher, WgReadAuditTrail );

    // force refresh
    unlocked_ = false;
    refresh();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveAuditLogG2::eventFilter( QObject *obj, QEvent *e )
{
    // exit self service mode when exit pressed
    if ( exitLabel_ == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
        {
            setVisible(false);
            hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );      // close window
            hook_->setPreviousState( selfServiceHook::itemEntryKeyedGen2 );
        }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::onButtonClicked( QWidget *w )
{
    if ( rezero_ == w )
    {
        hook_->emitZeroScale();
    }
    else if ( quit_ == w )
    {
        hook_->leave();                                                        // exit self serve
    }
    else if ( exit_ == w )
    {
        setVisible(false);
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );      // close window
        hook_->setPreviousState( selfServiceHook::itemEntryKeyedGen2 );
    }
    else if( printbut_ == w )
    {
        hook_->sendSurrogateMsg( Weigher, WgPrintAuditTrail );
    }
    else if( backspace_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one
        QString value( opID_->text() );

        if( value.length() > 1 )
            value.chop(1);
        else if( value.length() == 1 )
            value = "";

        opID_->setText( value );
    }
    else if ( enter_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one

        hook_->emitChangeFieldValue( sessionHooks::selfServicePassword, opID_->text() );
        opID_->clear();

        opID_->repaint();
    }
    else
    {
        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        // append button text to line edit
        if ( button )
        {
            // in case there is a key already pressed and it hasn't timed out yet
            if( activeKey_ )
            {
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );

                if( cycleNumpadTimer_->isActive() )
                    cycleNumpadTimer_->stop();
            }

            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(247,248,250);\n" "color: rgb(128,128,128);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
            activeKey_ = button;

            cycleNumpadTimer_->start(500);
            cycleNumpadTimer_->setSingleShot(true);

            QString id( opID_->text() );
            QString value( "" );
            if( ( "0" == id ) || ( "" == id ) )
            {
                value = button->text();
            }
            else
            {
                value = id.append( button->text() );
                if( value.length() > 6 )
                    value.remove(0, 1);
            }

            value.remove( QRegExp("^[0]*") );

            opID_->setText( value );


        }       // if a button
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::onReloadImages()
{
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveAuditLogG2::makeValidatorTop( int digits )
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
void selfserveAuditLogG2::initialize()
{
    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    rezero_     = ui->butRezero;
    quit_       = ui->butQuit;
    exit_       = ui->butExit;
    enter_      = ui->butEnter;
    printbut_   = ui->butPrint;
    backspace_  = ui->butBackspace;

    screen_     = ui->screenContainer;
    exitLabel_  = ui->lblExitLabel;
    title_      = ui->lblTitle;
    userinfo_   = ui->lblUserInfo;

    opID_       = ui->txtOpID;
    
    audittable_ = ui->tblAuditLog;

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

	connect( quit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( quit_, quit_ );

    connect( printbut_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printbut_, printbut_ );

    exitLabel_->installEventFilter( this );

    // close button
    connect( exit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( exit_, exit_ );

    // weights and measures audit log - this widget
    auditLog_ = ui->wgtAuditLog;

    // number pad
    opID_->setValidator( new QIntValidator( 0, 99999999, this ) );
    connect( opID_, SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );

    // enter button
    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    // backspace button
    connect( backspace_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspace_, backspace_ );

    connect( ui->butZero, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butZero, ui->butZero );
    connect( ui->butOne, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butOne, ui->butOne );
    connect( ui->butTwo, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butTwo, ui->butTwo );
    connect( ui->butThree, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butThree, ui->butThree );
    connect( ui->butFour, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butFour, ui->butFour );
    connect( ui->butFive, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butFive, ui->butFive );
    connect( ui->butSix, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butSix, ui->butSix );
    connect( ui->butSeven, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butSeven, ui->butSeven );
    connect( ui->butEight, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butEight, ui->butEight );
    connect( ui->butNine, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butNine, ui->butNine );

    // audit table
    audittable_ = ui->tblAuditLog;

    printbut_->setVisible( false );

    cycleNumpadTimer_ = new QTimer(this);
    cycleNumpadTimer_->setInterval(500);         // half sec timeout

    connect( cycleNumpadTimer_, SIGNAL( timeout() ), this, SLOT( onKeypadTimeout() ) );

    opID_ = ui->txtOpID;
    opID_->setValidator(numValidator_ = new QIntValidator(0, INT_MAX, this));
    opID_->setDecimalPlaces(0);
    opID_->setInputWrapping(true);
    numValidator_->setTop(makeValidatorTop(6));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::createLayout()
{
    return;
}

bool selfserveAuditLogG2::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::onEmitUpdateSaveMessage()
{
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLogG2::onEmitUpdateAuditLogList()
{
    // could not get it to work. Would not called SLOT - revisit later
    //void selfserveAuditLogG2::onEmitUpdateAuditLogList(DomAuditLogInformation *m )

    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    DomAuditLogInformation *m( &scaleConfig_->auditLogInformation );

    int numEvents  = m->numOfEvents();
    int numTypes   = m->numOfTypes();
    int numParams  = m->numOfParams();
    int numDates   = m->numOfDates();
    int numNewVals = m->numOfNewVals();
    int numOldVals = m->numOfOldVals();

    QString auditEventStr  = m->auditEventList();
    QString auditTypeStr   = m->auditTypeList();
    QString parameterIdStr = m->parameterIdList();
    QString dateTimeStr    = m->dateTimeList();
    QString newValueStr    = m->newValueList();
    QString previousValStr = m->previousValueList();

    QStringList audit_events = auditEventStr.split(",");
    QStringList audit_types  = auditTypeStr.split(",");
    QStringList audit_params = parameterIdStr.split(",");
    QStringList audit_dates  = dateTimeStr.split(",");
    
    QString tmp1 = m->newValueList();
    QVector<QString>  vecNewNames;
    vecNewNames = TokenParser::splitStringTokens( tmp1 );
    //QStringList audit_newval = newValueStr.split(",");

    QString tmp2 = m->previousValueList();
    QVector<QString>  vecOldNames;
    vecOldNames = TokenParser::splitStringTokens( tmp2 );
    //QStringList audit_oldval = previousValStr.split(",");

    // all of these numbers should always match.  But in case they dont take the lowest numbered value.
    int count = numEvents;
    count = qMin( count, numTypes );
    count = qMin( count, numParams );
    count = qMin( count, numDates );
    count = qMin( count, numNewVals );
    count = qMin( count, numOldVals );

    audittable_->clear();
    if( count < DEFAULT_ROWS )
        audittable_->setRowCount( DEFAULT_ROWS );
    else
        audittable_->setRowCount( count );

    calcItemColumnWidths();

    QStringList header_list;;
    //header_list << tr("Record") << tr("Category") << tr("Old Value") << tr("New Value") << tr("Date/Time") << tr("Event") << tr("Audit Type");
    //header_list << tr("Record") << tr("Audit Type") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("Old Value");
    header_list << tr("Record") << tr("") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("");

    audittable_->setHorizontalHeaderLabels( header_list );

    /// Received enums to text

    /// For the Audit Type column
    QStringList audittype_list;
    audittype_list << tr("Locally") << tr("Remote") << tr("Unknown");
    //int unknownType = 2;

    /// For the Audit Event column
    QStringList auditevent_list;
    auditevent_list << tr("Calibration") << tr("Configuration") << tr("Upgrade") << tr("Unknown");
    int unknownEvent = 3;

    /// For the Audit Parameter column
    QStringList auditparam_list;
    auditparam_list << tr("Center of maintenance zone") << tr("Scale factor") << tr("Max weight") << tr("Gain factor") << tr("Zero reference") << tr("Cal gain coefficient") << tr("Cal offset coefficient") << tr("Filter speed") << tr("Min weight to print") << tr("Weigher model") << tr("Division size") << tr("Software update") << tr("Gravity constant") << tr("Fractional pricing") << tr("Pounds for multiplier") << tr("Service switch pressed") << tr("Weigh Mode") \
    << tr("Decimal Separator") << tr("Currency Symbol Placement") << tr("Total Price Digits") << tr("Unit Price Digits") << tr("Rounding Factor") \
    << tr("Monetary Rounding Method") << tr("Primary Monetary Symbol") << tr("Primary Precision Digits") << tr("Secondary Monetary Symbol") \
    << tr("Secondary Monetary Decimal")<< tr("Unknown");

    int unknownParameter = 27;


   if ( count )
   {
       QString curStr    = "";
       int     curCol    = 0;
       int     curIndex  = 0;
       int     curEnum   = 0;
       int     numrows   = count - 1;

       for( int j = numrows; j >= 0; j-- )
       {
           curCol  = 0;                      // always reset to 0 each time around
           curStr  = "";                     // always blank the string each time around
           curEnum = 0;

           // Record number
           QTableWidgetItem *auditNumItem = new QTableWidgetItem;
           curStr = QString::number(curIndex);
           auditNumItem->setText( curStr );
           audittable_->setItem( j, curCol++, auditNumItem );

           // Audit Type
           QTableWidgetItem *auditTypeItem = new QTableWidgetItem;
           curEnum = audit_types[curIndex].toInt();

           auditTypeItem->setText( "" );
           audittable_->setItem( j, curCol++, auditTypeItem );

           // Audit Event
           QTableWidgetItem *auditEventItem = new QTableWidgetItem;
           curEnum = audit_events[curIndex].toInt();
           if (curEnum >= unknownEvent )
              auditEventItem->setText( "???" + QString::number(curEnum) + "???" );
           else
           {
               curStr = auditevent_list[curEnum];
               auditEventItem->setText( curStr );
           }
           audittable_->setItem( j, curCol++, auditEventItem );

           // Audit Parameter/Category
           QTableWidgetItem *auditParamItem = new QTableWidgetItem;
           curEnum = audit_params[curIndex].toInt();
           if (curEnum >= unknownParameter )
              auditParamItem->setText( "???" + QString::number(curEnum) + "???" );
           else
           {
               curStr = auditparam_list[curEnum];
               auditParamItem->setText( curStr );
           }
           audittable_->setItem( j, curCol++, auditParamItem );

           // Date/time value
           QTableWidgetItem *auditDateItem = new QTableWidgetItem;
           curStr = audit_dates[curIndex];
           auditDateItem->setText( curStr );
           audittable_->setItem( j, curCol++, auditDateItem );

           // New value
           QTableWidgetItem *auditNewItem = new QTableWidgetItem;
           curStr = vecNewNames.at(curIndex);      // to access vector items
           //curStr = audit_newval[curIndex];
           auditNewItem->setText( curStr );
           audittable_->setItem( j, curCol++, auditNewItem );

           // Old value
           QTableWidgetItem *auditOldItem = new QTableWidgetItem;
           curStr = " ";
           auditOldItem->setText( curStr );
           audittable_->setItem( j, curCol++, auditOldItem );

           curIndex++;
       }

       printbut_->setVisible( true );
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
