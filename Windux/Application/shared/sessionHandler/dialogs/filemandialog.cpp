/**
 * @file filemandialog.cpp
 *
 * @copyright Copyright (c) 2022, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "filemandialog.h"
#include "ui_filemandialog.h"

#include "../sessionmainwindow.h"
#include <logmanager.h>
#include "dialogs/filedialog.h"
#include "messageboxdialog.h"

#include <QDir>

// NEW
// filemandialog::filemandialog(QWidget *parent , QString dirpath, QString heading) :
//ORIG
    filemandialog::filemandialog( QString dirpath, QString heading, QWidget *parent ) :

    SessionDialog( TopDialogLocation, parent ),
    ui( new Ui::filemandialog ),
    dirPath_( dirpath ),
    dialogHeading_( heading )
{
    setObjectName( "filemandialog" );

    ui->setupUi( this );

    const QObjectList children( this->children() );
    if(mainWindow_->width() > mainWindow_->scalingDividerX_)
    {
        foreach ( QObject *child, children )
            if ( child->isWidgetType() )
                SessionDialog::scaleWidget( qobject_cast<QWidget*>( child ) );
    }

    initialize();
}

filemandialog::~filemandialog()
{
    delete ui;
}

void filemandialog::translate()
{
    ui->retranslateUi( this );

    update();
}

void filemandialog::initialize()
{
    LOG_CLASS_INFO( "filemandialog", "initialize" );

    ui->butDeleteButton->setEnabled( false );
    connect( ui->butOK, SIGNAL( clicked() ), this, SLOT( exitFileMan() ) );
    connect( ui->butDeleteButton, SIGNAL( clicked() ), this, SLOT( deleteFiles() ) );

    ui->lblHeading->setText( dialogHeading_ );

    int rowCount = 0;

    // Get the list of files in the designated directory
    QDir dir( dirPath_ );
    LOG_DEBUG( "Looking for files here: %s", qPrintable( dirPath_ ) );

    QStringList filters;
    filters << "*";

    dir.setFilter( QDir::Files );
    dir.setNameFilters( filters );

    QStringList files( dir.entryList() );

    ui->fileTable->clear();

    if( dir.count() )
    {
        // Populate the table with the files in the directory

        ui->fileTable->setRowCount( dir.count() );
        ui->fileTable->setColumnCount( 2 );

        for( QStringList::ConstIterator file = files.begin(); file != files.end(); ++file )
        {
            QString filename( *file );

            LOG_DEBUG( "initialize(): Adding file %s", qPrintable( filename ) );

            QTableWidgetItem *checkBoxItem = new QTableWidgetItem;
            QTableWidgetItem *fileNameItem = new QTableWidgetItem;

            ui->fileTable->setRowHeight( rowCount, 50 );

            checkBoxItem->setCheckState( Qt::Unchecked );
            checkBoxItem->setFlags( checkBoxItem->flags() ^ Qt::ItemIsEditable );

            fileNameItem->setText( filename );
            fileNameItem->setFlags( fileNameItem->flags() ^ Qt::ItemIsEditable );

            ui->fileTable->setItem( rowCount, 0, checkBoxItem );
            ui->fileTable->item( rowCount, 0 )->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

            ui->fileTable->setItem( rowCount, 1, fileNameItem );
            ui->fileTable->item( rowCount, 1)->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

            ui->fileTable->setColumnWidth( 0, 40 );
            ui->fileTable->setColumnWidth( 1, 500 );

            ++rowCount;
        }

        connect( ui->fileTable, SIGNAL( itemChanged( QTableWidgetItem* ) ), this, SLOT( fileCheckboxChanged( QTableWidgetItem* ) ) );
    }
}


void filemandialog::exitFileMan()
{
    accept();
}

// Toggle the Delete button: Only enable if a checkbox is checked
void filemandialog::fileCheckboxChanged( QTableWidgetItem* item )
{
    LOG_CLASS_INFO( "filemandialog", "fileCheckboxChanged" );

    ui->butDeleteButton->setEnabled( false );

    for( int currRow = 0; currRow < ui->fileTable->rowCount(); ++currRow )
    {
        LOG_DEBUG( "fileCheckboxChanged(): checking row %d ", currRow );

        if( ( ui->fileTable->item( currRow, 0 )->checkState() ) )
        {
            ui->butDeleteButton->setEnabled( true );
        }
    }
}

void filemandialog::deleteFiles()
{
    LOG_CLASS_INFO( "filemandialog", "deleteFiles" );

    MessageBoxDialog dialog( real_time_priority, mainWindow_ );
    dialog.setAllowOverlayClickDismiss( false );
    dialog.setTitle( tr( "DELETE FILES" ) );
    dialog.setText( tr( "\nDo you want to delete the selected file(s)?" ) );

    dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

    int result = dialog.exec();

    if ( QDialog::Accepted == result )
    {
        QDir dir( dirPath_ );
        QString filename;

        LOG_DEBUG( "Deleting files here: %s", qPrintable( dirPath_ ) );

        for( int currRow = 0; currRow < ui->fileTable->rowCount(); ++currRow )
        {
            if( ( ui->fileTable->item( currRow, 0 )->checkState() ) )
            {
                filename = ui->fileTable->item( currRow, 1 )->text();
                dir.remove( filename );

                LOG_DEBUG( "Deleted: %s ", qPrintable( filename ) );
            }
        }
    }
}
