/**
 * @file superDepartments.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "superDepartments.h"

#include "../dialogs/messageboxdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgetstyles.h>
#include <font.h>
#include <logmanager.h>
#include <numericlineedit.h>
#include <palette.h>

#include <ScaleConfiguration.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

const bool DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
SuperDepartmentsMaint::SuperDepartmentsMaint( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    currentDept_( -1 ),
    currentActiveDept_( -1),
    sentReadCurrentDept_( false ),
    handleSignals_( 0 )
{
    // initialize
    initialize();
    createLayout();
    translate();

    // request all (READALL) departments
    session_->send(DeptDev, DeptDevDtReadAll);

    // request active (CURRENT_ACTIVE) department
    sentReadCurrentDept_ = true;
    session_->send(DeptDev, DeptDevDtReadCurrentDept);

    // DEBUG ONLY
    if( DEBUG_MODE )
    {
        // DEBUG ONLY
        addDept( 0, "Default", "Default2" );
        addDept( 2001, "Bakery", "Bakery2" );
        addDept( 1000, "Meat", "Meat2" );
        addDept( 2000, "Deli", "Deli2" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SuperDepartmentsMaint::~SuperDepartmentsMaint()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::translate()
{
    numberLabel_->setText( tr( "Department Number" ) );
    namePrimaryLabel_->setText( tr( "Primary Description" ) );
    nameSecondaryLabel_->setText( tr( "Secondary Description" ) );

    deleteDept_->setText( tr( "Delete" ) );

    translateTable();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::resizeEvent( QResizeEvent *e )
{
    translateTable();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperDepartmentsMaint::onScreenChanging()
{
    saveCurrentDept();

    return _Mybase::onScreenChanging();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::translateTable()
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "translateTable" );

    LOG_TRACE( "entry", NULL );

    QStringList header;
    header.append( tr( "Dept Number" ) );
    header.append( tr( "Description" ) );

    depts_->setHeaderLabels( header );
    depts_->setColumnCount( 2 );

    // ---- //

    int tableWidth( depts_->contentsRect().width() - 32 );

    depts_->setColumnWidth( 0, (tableWidth * 25) / 100 ); // dept number
    depts_->setColumnWidth( 1, (tableWidth * 75) / 100 ); // dept name

    // ---- //

    int index( 0 );

    foreach ( const deptInfo &d, departments_ )
    {
        QTreeWidgetItem *item( depts_->topLevelItem( index++ ) );

        if( !item )
            break;

        item->setText( 0, QString::number( d.number ) );

        if ( 2 == session_->scaleConfig->currentLanguage )
            item->setText( 1, d.nameSec );
        else
            item->setText( 1, d.namePrim );
    }

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::onClicked()
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "onClicked" );

    if ( !handleSignals() )
        return;

    LOG_TRACE( "entry", NULL );

    //
    // DELETE DEPT
    //
    if ( deleteDept_ == sender() )
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "DELETE DEPARTMENT" ) );

        if ( currentDept_ <= 0 )
        {
            dialog.setText( tr( "Cannot delete default department." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();
        }
        else
        {
            QString deptName( QString::number( currentDept_ ) );

            if( currentActiveDept_ == currentDept_ )
            {
                dialog.setText( tr( "You are about to delete the Current Active Department.\n\nAre you sure you want to delete this department?" ) );
                dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );
                //session_->send( Error, ErrDepartments, "173103,1" );
            }
            else
            {
                deptName += " - ";

                if ( 2 == session_->scaleConfig->currentLanguage )
                    deptName += departments_[currentDept_].nameSec;
                else
                    deptName += departments_[currentDept_].namePrim;

                dialog.setText( tr( "You are about to delete department:" ) + "\n" + deptName + "\n\n" + tr( "Are you sure you want to delete this department?" ) );
                dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );
            }

            // make sure keyboard is hidden
            mainWindow_->hideInputWidgets();

            LOG_DEBUG( "prompting to delete department...", NULL );

            if ( QDialog::Accepted == dialog.exec() )
            {
                if( currentActiveDept_ == currentDept_ )
                    session_->send(DeptDev, DeptDevDtSetCurrentDept, "0" );

                // send delete (DELETE) message to backend
                session_->send( DeptDev, DeptDevDtDelete, deptName );

                QTreeWidgetItem *item( depts_->currentItem() );

                if ( item )
                {
                    setHandleSignals( false );
                    delete item;
                    setHandleSignals( true );
                }

                departments_.remove( currentDept_ );
                translateTable();

                resetForm();
            }
        }
    }

    //
    // SHOW SELECTED DEPT
    //
    else if ( depts_ == sender() )
    {
        QTreeWidgetItem *item( depts_->currentItem() );

        // no items selected
        if ( !item )
            resetForm();

        // one item selected
        else
        {
            // need to do a (READ) to set the currentDept in the Application
            // but do a Save real quick first
            QString clickedDeptStr = item->text( 0 );
            int clickedDept( clickedDeptStr.toInt() );

            if( 0 <= clickedDept )
            {
                if( 0 == clickedDept )
                    enableDepts(false);   //session_->send(Error, DeptDev, "175001,5");
                else
                    enableDepts(true);

                saveCurrentDept();
                session_->send( DeptDev, DeptDevDtRead, clickedDeptStr );
                showDept( clickedDept );
            }
        }

        // TODO MAYBE
        // Should I do a READ one right here on the selected row?
    }

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::onEditingFinished()
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "onEditingFinished" );

    if ( !handleSignals() )
        return;

    LOG_TRACE( "entry", NULL );

    if ( number_ == sender() )
    {
        const int newDept( number_->toInt() );

        // nothing changed
        if ( newDept == currentDept_ )
            return;

        // existing department
        else if ( departments_.contains( newDept ) )
            showDept( newDept );

        // new department
        else
        {
            setHandleSignals( false );

            MessageBoxDialog dialog( mainWindow_ );
            dialog.setTitle( tr( "NEW DEPARTMENT" ) );
            dialog.setText( tr( "This department does not exist in the database:" ) + "\n" + QString::number( newDept ) + "\n\n" + tr( "Would you like to add it?" ) );
            dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

            // make sure keyboard is hidden
            mainWindow_->hideInputWidgets();

            LOG_DEBUG( "prompting for new department...", NULL );

            if ( QDialog::Accepted == dialog.exec() )
            {
                // send create new (CREATE) dept message to backend
                QString deptNameStr( QString::number( newDept ) );
                session_->send( DeptDev, DeptDevDtCreateNew, deptNameStr );
                enableDepts(true);
                addDept( newDept, QString(), QString(), true );
                showDept( newDept );
            }
            else
            {
                if ( 0 <= currentDept_ )
                    number_->fromInt( currentDept_ );
                else
                    number_->clear();
            }

            setHandleSignals( true );
        }
    }

    else if ( namePrimary_ == sender() )
    {
        if ( 0 <= currentDept_ )
        {
            departments_[currentDept_].namePrim = namePrimary_->text();
            translateTable();
        }
    }

    else if ( nameSecondary_ == sender() )
    {
        if ( 0 <= currentDept_ )
        {
            departments_[currentDept_].nameSec = nameSecondary_->text();
            translateTable();
        }
    }

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::initialize()
{
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize20( (mainWindow_->height() * 20) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize20( this );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    // ---- //

    buttonLayoutWidget_ = new QWidget( this );

    // ---- //

    numberLabel_ = new QLabel( this );
    numberLabel_->setObjectName( "numberLabel" );

    number_ = new hobart::numericLineEdit( buttonLayoutWidget_ );
    number_->setDecimalPlaces( 0 );
    number_->setFixedHeight( 40 );
    number_->setInputWrapping( true );
    number_->setInputTruncation(true);
    number_->setObjectName( "number" );
    number_->setValidator( new QIntValidator( 0, 999999, this ) );
    setFontPointSize18( number_ );

    connect( number_, SIGNAL(editingFinished()), SLOT(onEditingFinished()) );

    namePrimaryLabel_ = new QLabel( this );
    namePrimaryLabel_->setObjectName( "namePrimaryLabel" );

    namePrimary_ = new QLineEdit( this );
    namePrimary_->setEnabled( false );
    namePrimary_->setFixedHeight( 40 );
    namePrimary_->setObjectName( "namePrimary" );
    setFontPointSize18( namePrimary_ );

    connect( namePrimary_, SIGNAL(editingFinished()), SLOT(onEditingFinished()) );

    nameSecondaryLabel_ = new QLabel( this );
    nameSecondaryLabel_->setObjectName( "nameSecondaryLabel" );

    nameSecondary_ = new QLineEdit( this );
    nameSecondary_->setEnabled( false );
    nameSecondary_->setFixedHeight( 40 );
    nameSecondary_->setObjectName( "nameSecondary" );
    setFontPointSize18( nameSecondary_ );

    connect( nameSecondary_, SIGNAL(editingFinished()), SLOT(onEditingFinished()) );

    deleteDept_ = new QPushButton( buttonLayoutWidget_ );
    deleteDept_->setEnabled( false );
    deleteDept_->setFixedSize( 200, 40 );
    deleteDept_->setObjectName( "deleteDept" );
    setFontPointSize18( deleteDept_ );

    connect( deleteDept_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    depts_ = new QTreeWidget( this );
    depts_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    depts_->setObjectName( "depts" );
    depts_->setRootIsDecorated( false );
    depts_->setSelectionMode( QAbstractItemView::SingleSelection );
    depts_->setUniformRowHeights( true );
    setFontPointSize18( depts_ );
    setHighlight( depts_ );
    setHighlightText( depts_ );

    connect( depts_, SIGNAL(itemSelectionChanged()), SLOT(onClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::createLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout( buttonLayoutWidget_ );
    buttonLayout->setContentsMargins( QMargins() );

    buttonLayout->addWidget( number_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( deleteDept_ );

    QFormLayout *editBoxesLayout = new QFormLayout();
    editBoxesLayout->addRow( numberLabel_, buttonLayoutWidget_ );
    editBoxesLayout->addRow( namePrimaryLabel_, namePrimary_ );
    editBoxesLayout->addRow( nameSecondaryLabel_, nameSecondary_ );

    QVBoxLayout *formLayout = new QVBoxLayout( this );
    formLayout->setContentsMargins( 48, 24, 48, 8 );

    formLayout->addLayout( editBoxesLayout );
    formLayout->addSpacing( 16 );
    formLayout->addWidget( depts_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::resetForm()
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "resetForm" );

    LOG_TRACE( "entry", NULL );

    currentDept_ = -1;

    setHandleSignals( false );

    number_->clear();
    namePrimary_->clear();
    nameSecondary_->clear();
    enableDepts(false);

    depts_->clearSelection();

    deleteDept_->setEnabled( false );

    setHandleSignals( true );

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::enableDepts( bool isEnabled )
{
    namePrimary_->setEnabled( isEnabled );
    nameSecondary_->setEnabled( isEnabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::showDept( int number )
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "showDept" );

    if ( !departments_.contains( number ) )
        return;

    LOG_TRACE( "entry", NULL );

    // save current department
    saveCurrentDept();

    currentDept_ = number;

    // setup form with this department
    setHandleSignals( false );

    number_->fromInt( number );
    namePrimary_->setText( departments_[number].namePrim );
    nameSecondary_->setText( departments_[number].nameSec );

    // find and highlight dept
    depts_->clearSelection();

    for ( int index( departments_.size() ); index--; )
    {
        QTreeWidgetItem *item( depts_->topLevelItem( index ) );

        if (( item ) && ( item->text( 0 ) == QString::number( number ) ))
        {
            item->setSelected( true );
            break;
        }
    }

    // enable delete button (if applicable)
    deleteDept_->setEnabled( 0 < number );

    setHandleSignals( true );

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::saveCurrentDept()
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "saveCurrentDept" );

    // nothing to do
    if ( currentDept_ < 0 )
        return;
    // This else was always called and true so always exited because when the text changes
    // in the lineedit onEditingFinished()is called and make these equal so it always returns
    // // TODO - maybe revist when all working and see if we can optimize. Or remove onEdit
    //else if (( departments_[currentDept_].namePrim == namePrimary_->text() ) && ( departments_[currentDept_].nameSec == nameSecondary_->text() ))
    //    return;

    LOG_TRACE( "entry", NULL );

    departments_[currentDept_].namePrim = namePrimary_->text();
    departments_[currentDept_].nameSec = nameSecondary_->text();

    // send message to backend (WRITE and SAVE) with current dept
    QString deptNameStr( QString::number( currentDept_ ) );
    session_->send( DeptDev, DeptDevDtWrite, deptNameStr + "," + departments_[currentDept_].namePrim + "," + departments_[currentDept_].nameSec);
    session_->send( DeptDev, DeptDevDtSave,  deptNameStr );

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::addDept( int number, const QString &namePrim, const QString &nameSec, bool trans )
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "addDept" );

    const bool exists( departments_.contains( number ) );

    LOG_TRACE( "entry", NULL );

    deptInfo d;
    d.number = number;
    d.namePrim = namePrim;
    d.nameSec = nameSec;

    // add to table
    departments_[number] = d;

    // create widget item
    if ( !exists )
    {
        QStringList rows;
        rows.append( "" );
        rows.append( "" );

        QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
        item->setSizeHint( 0, QSize( 40, 40 ) );

        // add to widget
        setHandleSignals( false );
        depts_->addTopLevelItem( item );
        setHandleSignals( true );
    }

    // translate if needed
    if ( trans )
        translateTable();

    LOG_TRACE( "exit", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperDepartmentsMaint::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SuperDepartmentsMaint", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if( DeptDev == device && DeptDevDtWriteAll == event )
    {
        // This will stifle write alls if this screen is being shown and we pop up the operate mode dept panel
        MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );
        if( internalConfig->stifleWriteAll )
            return;

        DomDepartments m( msg );
        setHandleSignals( false );
        depts_->clear();
        setHandleSignals( true );

        int numOfDepts( m.numDepts() );

        if( numOfDepts <= 0 )
            return;

        const QStringList deptsArray( m.deptsList().split(",") );
        const QStringList name1Array( m.names1().split(",") );
        const QStringList name2Array( m.names2().split(",") );

        if( ( numOfDepts != name1Array.count() ) ||
            ( numOfDepts != name2Array.count() ) ||
            ( numOfDepts != deptsArray.count() ) )
        {
            LOG_WARN( "mismatch in array length!", NULL );
            return;
        }

        int index( 0 );

        foreach( const QString depts_, deptsArray )
        {
            addDept( depts_.toInt(), name1Array.at(index), name2Array.at(index++) );
        }
    }
    else if( DeptDev == device && DeptDevDtWrite == event )
    {
        MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );

        if( sentReadCurrentDept_ || internalConfig->allowWriteOnes )
        {
            // set current department
            sentReadCurrentDept_ = false;
            internalConfig->allowWriteOnes = false;
            DomCurrentDept m( msg );
            currentActiveDept_ = m.deptNum();
        }
    }
    else if ( Error == device )
    {
        returnedErrDepartments m( msg );
    }

    LOG_TRACE( "exit", NULL );

    _Mybase::onSessionMessage( msg );
}
