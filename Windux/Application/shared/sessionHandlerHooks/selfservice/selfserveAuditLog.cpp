/**
 * @file selfserveWeightsMeasures.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Job E. Crew
 */

#include "selfserveAuditLog.h"
#include "selfservicewmnumberpad.h"
#include "../sessionHandler/sessionmainwindow.h"
#include "../sessionHandler/overlaywidget.h"
#include "../sessionHandler/dialogs/filedialog.h"

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
#include "TokenParser.h"
static const int DEFAULT_COLS = 7;
static const int DEFAULT_ROWS = 8;


/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveAuditLog::selfserveAuditLog( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h ),
    scaleConfig_( NULL ),
    unlocked_( false )
{
    setObjectName( "selfserveAuditLog" );

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

selfserveAuditLog::~selfserveAuditLog()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::refresh()
{
    // update numberpad style
    numberPad_->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );

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
void selfserveAuditLog::translate()
{
    rezero_->setText( tr( "Rezero" ) );
    quit_->setText( tr( "Exit Self-\nService Mode" ) );

    title_->setText( tr( "Enter ID to exit self-service mode,\nthen press Enter." ) );

    savebut_->setText( tr("Save to\nDisk") );
    printbut_->setText( tr("Print") );

    exitLabel_->setText( tr( "Close" ) );
    exit_->setText( "X" );

    userinfo_->setText( tr("Audit trail information...") );

    numberPad_->translate();
    enter_->setText( tr( "Enter" ) );

    QStringList header_list;
    //header_list << tr("Record") << tr("Audit Type") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("Old Value");
    header_list << tr("Record") << tr("") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("");

    audittable_->setHorizontalHeaderLabels( header_list );

    calcItemColumnWidths();
}

//////////////////////////////0/////////////////////////////////////////////////////////////////////
void selfserveAuditLog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // generate pixmap for weights and measures area
    QPixmap pixmap( auditLog_->size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush( QColor( 255, 255, 255, 127 ) );
    painter.setPen( QPen( Qt::black, 2 ) );

    // create button
    painter.drawRoundedRect( auditLog_->rect().adjusted( 1, 1, -1, -1 ), 30, 30 );

    // set palette
    QPalette p( auditLog_->palette() );
    p.setBrush( backgroundRole(), pixmap );
    auditLog_->setPalette( p );

    auditLog_->setAutoFillBackground( true );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::calcItemColumnWidths()
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

    //audittable_->setColumnWidth( 1, (width * 10) / 100 ); // audit type
    //audittable_->setColumnWidth( 3, (width * 22) / 100 ); // parameter
    //audittable_->setColumnWidth( 5, (width * 12) / 100 ); // new value
    //audittable_->setColumnWidth( 6, (width * 8) / 100 );  // old value
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::showEvent( QShowEvent *e )
{
    numberPad_->blockSignals( true );
    numberPad_->clear();
    numberPad_->blockSignals( false );

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
bool selfserveAuditLog::eventFilter( QObject *obj, QEvent *e )
{
    // exit self service mode when exit pressed
    if ( exitLabel_ == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
        {
            setVisible(false);
            hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );      // close window
            hook_->setPreviousState( selfServiceHook::itemEntryKeyed );
        }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::onButtonClicked( QWidget *w )
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
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );      // close window
        hook_->setPreviousState( selfServiceHook::itemEntryKeyed );
    }
    else if( printbut_ == w )
    {
        hook_->sendSurrogateMsg( Weigher, WgPrintAuditTrail );
    }
    else if( savebut_ == w )
    {
        onSaveButtonClicked();
    }
    else if ( enter_ == w )
    {
        hook_->emitChangeFieldValue( sessionHooks::selfServicePassword, numberPad_->text() );
        numberPad_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::onReloadImages()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::initialize()
{
    QWidgetList buttons;

    const hobart::backgroundWidgetRenderer renderDarkButtonPixmap(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderGreenButtonPixmap(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::greenButtonBackgroundWidgetRendererAppearance() );

    const QPixmap darkButtonPixmap( renderDarkButtonPixmap.drawControl( QSize(150, 64), hobart::ReadOnly ) );
    const QPixmap greenButtonPixmap( renderGreenButtonPixmap.drawControl( QSize(150, 64), hobart::ReadOnly ) );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // rezero
    rezero_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );
    buttons.append( rezero_ );

    // exit self-service mode
    quit_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( quit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( quit_, quit_ );
    buttons.append( quit_ );

    // title
    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );

    QFont titleFont( title_->font() );
    titleFont.setPointSize( 18 );
    title_->setFont( titleFont );

    // close label
    exitLabel_ = new QLabel( this );
    exitLabel_->setAlignment( Qt::AlignCenter );

    QPalette exitLabelPalette( exitLabel_->palette() );
    exitLabelPalette.setColor( exitLabel_->foregroundRole(), hobart::colorPalette::white );
    exitLabel_->setPalette( exitLabelPalette );

    QFont exitLabelFont( exitLabel_->font() );
    exitLabelFont.setPointSize( 12 );
    exitLabel_->setFont( exitLabelFont );

    exitLabel_->installEventFilter( this );

    // close button
    exit_ = new hobart::pushButton( this, exitButtonPixmap() );
    connect( exit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( exit_, exit_ );
    buttons.append( exit_ );

    // weights and measures audit log - this widget
    auditLog_ = new QWidget( this );

    // number pad
    numberPad_ = new selfServiceWmNumberPad( this );
    numberPad_->setValidator( new QIntValidator( 0, 99999999, this ) );
    connect( numberPad_, SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );

    // enter button
    enter_ = new hobart::pushButton( this, greenButtonPixmap );
    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );
    buttons.append( enter_ );

    QFont auditLogFont( auditLog_->font() );
    auditLogFont.setPointSize( 10 );
    auditLog_->setFont( auditLogFont );

    // audit table
    //audittable_ = new QTableWidget( this );
    audittable_ = new QTableWidget( auditLog_ );
    audittable_->setObjectName(QStringLiteral("tblAuditLog"));
    audittable_->setGeometry(QRect(30, 10, 891, 431));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(audittable_->sizePolicy().hasHeightForWidth());
    audittable_->setSizePolicy(sizePolicy1);
    audittable_->setMinimumSize(QSize(691, 371));
    audittable_->setMaximumSize(QSize(2891, 2571));
    audittable_->setStyleSheet(QLatin1String("QTableWidget{\n"
    "color: black;\n"
    "}\n"
    "\n"
    "QTableWidget::item{ \n"
    "selection-background-color: #55ffff;\n"
    " }"));
    audittable_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    audittable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    audittable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    audittable_->setSelectionMode(QAbstractItemView::SingleSelection);
    audittable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    audittable_->horizontalHeader()->setStretchLastSection(true);
    audittable_->verticalHeader()->setVisible(false);
    audittable_->verticalHeader()->setCascadingSectionResizes(true);
    audittable_->verticalHeader()->setDefaultSectionSize(50);
    audittable_->verticalHeader()->setHighlightSections(false);
    audittable_->verticalHeader()->setMinimumSectionSize(50);
    audittable_->verticalHeader()->setStretchLastSection(true);

    QFont audittable_Font;
    audittable_Font.setPointSize(12);
    audittable_->setFont( audittable_Font );

    // information label
    userinfo_   = new QLabel( auditLog_ );
    QFont userFont;
    userFont.setPointSize(14);
    userinfo_->setFont( userFont );

    // save button
    savebut_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( savebut_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( savebut_, savebut_ );
    buttons.append( savebut_ );

    // print button
    printbut_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( printbut_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printbut_, printbut_ );
    buttons.append( printbut_ );

    spacer1_Label_ = new QLabel( auditLog_ );
    spacer1_ = new QLabel( auditLog_ );

    printbut_->setVisible( false );
    savebut_->setVisible( false );

    // ----- //

    foreach ( QWidget *w, buttons )
    {
        QPalette palette( w->palette() );
        palette.setColor( QPalette::ButtonText, hobart::colorPalette::white );
        w->setPalette( palette );

        QFont font( w->font() );
        font.setPointSize( 16 );
        w->setFont( font );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::createLayout()
{
    // ----- Top Row of buttons with Close ----- //
    QHBoxLayout *closeButtonLayout( new QHBoxLayout() );
    closeButtonLayout->setContentsMargins( QMargins() );

    closeButtonLayout->addWidget( exitLabel_, 1 );
    closeButtonLayout->addWidget( exit_ );

    QVBoxLayout *closeButtonAreaLayout( new QVBoxLayout() );
    closeButtonAreaLayout->setContentsMargins( QMargins() );

    closeButtonAreaLayout->addLayout( closeButtonLayout );
    closeButtonAreaLayout->addStretch();

    QHBoxLayout *titleLayout( new QHBoxLayout() );
    titleLayout->setContentsMargins( QMargins() );

    titleLayout->addWidget( rezero_ );
    titleLayout->addWidget( quit_ );
    titleLayout->addStretch( 1 );
    titleLayout->addWidget( title_ );
    titleLayout->addStretch( 1 );
    titleLayout->addLayout( closeButtonAreaLayout );

    // ----- Keypad Area -----//
    QHBoxLayout *enterButtonLayout( new QHBoxLayout() );
    enterButtonLayout->setContentsMargins( QMargins() );

    enterButtonLayout->addStretch();
    enterButtonLayout->addWidget( enter_ );
    enterButtonLayout->addStretch();

    QVBoxLayout *numberPadLayout( new QVBoxLayout() );
    numberPadLayout->setContentsMargins( QMargins() );

    numberPadLayout->addStretch();
    numberPadLayout->addWidget( numberPad_ );
    numberPadLayout->addLayout( enterButtonLayout );
    numberPadLayout->addStretch();

    // ----- Audit Log Layout ----- //
    QHBoxLayout *auditLogLayout( new QHBoxLayout() );
    auditLogLayout->setContentsMargins( QMargins( 6, 0, 0, 6 ) );
    auditLogLayout->setSpacing( 6 );

    auditLogLayout->addWidget( auditLog_, 1 );
    auditLogLayout->addLayout( numberPadLayout );

    // ----- Screen Layout ----- //
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->addLayout( titleLayout );
    screenLayout->addLayout( auditLogLayout, 1 );


    QHBoxLayout *actionButtonLayout( new QHBoxLayout() );
    actionButtonLayout->setContentsMargins( QMargins( 20, 20, 20, 20) );
    actionButtonLayout->addWidget( savebut_);
    actionButtonLayout->addWidget( spacer1_Label_);
    actionButtonLayout->addWidget( printbut_);

    QVBoxLayout *auditLogFormLayout( new QVBoxLayout() );
    auditLogFormLayout->addWidget( audittable_ );
    //auditLogFormLayout->addWidget( spacer1_Label_ );
    auditLogFormLayout->addWidget( userinfo_);
    //auditLogFormLayout->addWidget( spacer1_Label_ );
    auditLogFormLayout->addLayout( actionButtonLayout );

    QVBoxLayout *auditLogFormLayout_V( new QVBoxLayout() );
    auditLogFormLayout_V->addStretch();
    auditLogFormLayout_V->addLayout( auditLogFormLayout );
    auditLogFormLayout_V->addStretch();

    QHBoxLayout *auditLogFormLayout_H( new QHBoxLayout( auditLog_ ) );
    auditLogFormLayout_H->addStretch();
    auditLogFormLayout_H->addLayout( auditLogFormLayout_V );
    auditLogFormLayout_H->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap selfserveAuditLog::exitButtonPixmap()
{
    QPixmap pixmap( 32, 32 );
    pixmap.fill( Qt::transparent );

    QPainter p( &pixmap );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.setBrush( hobart::colorPalette::red );
    p.setPen( hobart::colorPalette::red );

    p.drawEllipse( QPointF( 15.5, 15.5 ), 15, 15 );

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::onSaveButtonClicked()
{
    // Send a message to the application to save the log

    // the user clicked the Save button

    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    FileDialog filepicker( this );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save to USB" ) );

    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            QString args = fileName;
            userinfo_->setText( tr("Saving log, please wait ...") );

            hook_->sendSurrogateMsg( Weigher, WgSaveAuditTrailToDisc, args );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveAuditLog::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                //else if( myfile.indexOf("C:") > 0 )     // cannot contain C:
                //    errStr = tr("The file cannot be saved to the root file system");
                //else if( isLinux() && myfile.indexOf("media") < 0 )  // has to contain media (a path to a thumb)
                //    errStr = tr("The file cannot be saved to the root file system");
                else
                    validFile = true;
            }
        }
    }

    if ( validFile )
        userinfo_->setText( tr("Valid file name") );
    else
        userinfo_->setText( errStr );

    return validFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfserveAuditLog::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if( myfile.indexOf(".csv") > 0 )
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .csv
            strippedFileName = myfile;                              // then we are good to go
        }
        else if(myfile.indexOf(".") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path, just the file.
            // An extension was added to the file, but we don't recogonize it, but include it.
            int getDot = myfile.indexOf(".") + 1;
            if (myfile.length() == getDot + 3)
                strippedFileName = myfile;
        }
        else
        {
            // we don't have a file, so create one. Do not include the entire path of the file.
            // No extension was given for the file, so default to the file to an csv file ext.
            strippedFileName = myfile + ".csv";
        }

        retFile = strippedFileName;
    }
    return retFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveAuditLog::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::onEmitUpdateSaveMessage()
{
    userinfo_->setText( tr("File saved") );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveAuditLog::onEmitUpdateAuditLogList()
{
    // could not get it to work. Would not called SLOT - revisit later
    //void selfserveAuditLog::onEmitUpdateAuditLogList(DomAuditLogInformation *m )

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
    audittable_->setColumnCount( DEFAULT_COLS );

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
           //if (curEnum >= unknownType )
           //   auditTypeItem->setText( QString::number(curEnum) );
           //else
           //{
           //    curStr = "???" + audittype_list[curEnum] + "???";
           //    auditTypeItem->setText( curStr );
           //}
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
           //curStr = audit_oldval[curIndex];
           //auditOldItem->setText( curStr );
           auditOldItem->setText( curStr );
           audittable_->setItem( j, curCol++, auditOldItem );

           curIndex++;
       }

       printbut_->setVisible( true );
       //savebut_->setVisible( true );
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
