/**
 * @file webserviceslogviewerwidget.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "webserviceslogviewerwidget.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextStream>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
WebServicesLogViewerWidget::WebServicesLogViewerWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // setup user readable column headers
    columnHeaders_["TIMESTAMP"] = tr("Time");
    columnHeaders_["PLU"] = tr("PLU");
    columnHeaders_["UUID"] = tr("UUID");
    columnHeaders_["RETURNCODE"] = tr("Code");
    columnHeaders_["RETURNBYTES"] = tr("Bytes");
    columnHeaders_["RETRIES"] = tr("Att");
    columnHeaders_["TIMESPENT"] = tr("Time");
    columnHeaders_["COMMENTS"] = tr("Return Code Details");

    // setup column widths
    columnWidths_["TIMESTAMP"] = 18;
    columnWidths_["PLU"] = 0;
    columnWidths_["UUID"] = 42;
    columnWidths_["RETURNCODE"] = 0;
    columnWidths_["RETURNBYTES"] = 8;
    columnWidths_["RETRIES"] = 4;
    columnWidths_["TIMESPENT"] = 8;
    columnWidths_["COMMENTS"] = 20;

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString WebServicesLogViewerWidget::filename()
{
#if defined( Q_OS_LINUX )
    return "/var/log/EdgifyRequests.log";
#else
    return "EdgifyRequests.log";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebServicesLogViewerWidget::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebServicesLogViewerWidget::onButtonClicked()
{
    if ( saveLogs_ == sender() )
    {
        QStringList folders;

#if defined( Q_OS_LINUX )
        folders.append( "/media/thumb/" );
        folders.append( "/media/thumb1/" );
        folders.append( "/media/thumb2/" );
        folders.append( "/media/thumb3/" );
        folders.append( "/media/" );
        folders.append( "/home/" );
#elif defined( Q_OS_WIN )
        folders.append( "D:\\" );
        folders.push_back( "E:\\" );
        folders.append( "C:\\" );
#endif

        // find existing folder
        QString startDirStr;

        foreach ( const QString& f, folders )
            if ( QDir( startDirStr = f ).exists() )
                break;

        const QString dir( QFileDialog::getExistingDirectory( this, tr( "Choose Export Location" ), startDirStr, QFileDialog::ShowDirsOnly ) );

        if ( !dir.length() )
            return;

        // ---- //

        const QDateTime now( QDateTime::currentDateTime() );
        const QString nowStr( now.toString( "yyyyMMdd_hhmmsszzz" ) );

#if defined( Q_OS_LINUX )
        QString sep( "/" );

        if ( dir.endsWith( "/" ) )
            sep.clear();

        const QString filename( dir + sep + "webservices_log_" + nowStr );

        // make list of files to tar
        QList<QDirIterator*> iters;
        iters.append( new QDirIterator( "/var/log", QStringList( "edgify*" ), QDir::Files | QDir::NoSymLinks ) );
        iters.append( new QDirIterator( "/usr/share/edgify-apphook", QDir::Files | QDir::NoSymLinks ) );

        if ( includeScaleLogs_->isChecked() )
        {
            iters.append( new QDirIterator( "/etc/hobart", QStringList( "*.log" ), QDir::Files | QDir::NoSymLinks ) );
            iters.append( new QDirIterator( "/usr/local/hobart", QStringList( "*.log" ), QDir::Files | QDir::NoSymLinks ) );
            iters.append( new QDirIterator( "/tmp", QStringList( "scale*.txt" ), QDir::Files | QDir::NoSymLinks ) );
        }

        // create tar command arguments
        QStringList args;
        args.append( "-cvzf" );
        args.append( filename );
        args.append( "/var/log/WebServiceRequests.log" );

        foreach ( QDirIterator *it, iters )
        {
            while ( it->hasNext() )
                args.append( it->next() );

            delete it;
        }

        // tar!
        QProcess p;
        p.setWorkingDirectory( "/" );
        p.start( "tar", args );

        if (( p.waitForStarted() ) && ( p.waitForFinished() ))
        {
            QMessageBox::information( this, tr( "Done" ), tr( "Logs saved as:" ) + "\n" + filename );
        }
#endif
    }
    else if ( exit_ == sender() )
    {
        emit exitScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebServicesLogViewerWidget::initialize()
{
    // table
    if ( !loadAllQueries() )
        table_ = NULL;
    else
    {
        const QStringList rawHeaders( queries_.at( 0 ).split( "," ) );

        table_ = new QTreeWidget( this );
        table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        table_->setObjectName( "table" );
        table_->setRootIsDecorated( false );
        table_->setSelectionMode( QAbstractItemView::NoSelection );
        table_->setUniformRowHeights( true );

        table_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        table_->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

        // ---- //

        // reduce font size
        QFont f( table_->font() );
        f.setPointSize( (f.pointSize() * 80) / 100 );
        table_->setFont( f );

        // ---- //

        QStringList headers;

        foreach ( const QString& rawHeader, rawHeaders )
        {
            if ( columnHeaders_.contains( rawHeader ) )
                headers.append( columnHeaders_[rawHeader] );
            else
                headers.append( rawHeader );
        }

        table_->setHeaderLabels( headers );
        table_->setColumnCount( headers.size() );

        // ---- //

        // add data to table (reverse order; newest at top)
        for ( int i( queries_.size() ); 1 < i--; )
        {
            QString rawRowData( queries_.at( i ) );
            rawRowData.replace( " AM ", " AM\n" );
            rawRowData.replace( " PM ", " PM\n" );
            rawRowData.replace( " am ", " am\n" );
            rawRowData.replace( " pm ", " pm\n" );

            QTreeWidgetItem *item( new QTreeWidgetItem( rawRowData.split( "," ) ) );
            item->setSizeHint( 1, QSize( 64, 64 ) );

            // update item
            for ( int j( 0 ); j < table_->columnCount(); ++j )
            {
                item->setBackground( j, Qt::white );
                item->setFlags( Qt::NoItemFlags );
                item->setForeground( j, Qt::black );
            }

            // add item!
            table_->addTopLevelItem( item );
        }
    }

    // include scale logs
    includeScaleLogs_ = new QCheckBox( this );
    includeScaleLogs_->setFocusPolicy( Qt::NoFocus );
    includeScaleLogs_->setObjectName( "includeScaleLogs" );
    includeScaleLogs_->setText( tr( "Include Scale Logs" ) );

    // save logs
    saveLogs_ = new QPushButton( this );
    saveLogs_->setMinimumHeight( 64 );
    saveLogs_->setFocusPolicy( Qt::NoFocus );
    saveLogs_->setObjectName( "saveLogs" );
    saveLogs_->setText( tr( "Save to USB" ) );
    connect( saveLogs_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    // exit
    exit_ = new QPushButton( this );
    exit_->setMinimumHeight( 64 );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Back" ) );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebServicesLogViewerWidget::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( saveLogs_ );
    buttons->addWidget( includeScaleLogs_ );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );

    if ( table_ )
        layout->addWidget( table_, 1 );
    else
        layout->addStretch();

    layout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebServicesLogViewerWidget::loadAllQueries()
{
    QFile f( filename() );

    if ( f.open( QFile::ReadOnly | QFile::Text ) )
    {
        QTextStream s( &f );

        while ( !s.atEnd() )
            queries_.append( s.readLine() );

        f.close();
    }

    return (2 <= queries_.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebServicesLogViewerWidget::calcColumnWidths()
{
    const QStringList rawHeaders( queries_.at( 0 ).split( "," ) );

    if ( !table_ )
        return;

    const int tableWidth( table_->width() - 34 );

    int i( 0 );

    foreach ( const QString& rawHeader, rawHeaders )
    {
        if ( columnWidths_.contains( rawHeader ) )
        {
            const int width( columnWidths_[rawHeader] );

            if ( !width )
                table_->setColumnHidden( i, true );
            else
            {
                table_->setColumnHidden( i, false );
                table_->setColumnWidth( i, (tableWidth * width)  / 100 );
            }
        }

        ++i;
    }
}
