/**
 * @file deptpanel.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */


#include "deptpanel.h"

#include "../sessionmainwindow.h"

#include <font.h>
#include <logmanager.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

const bool DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
DepartmentPanel::DepartmentPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "DepartmentPanel" );

    // initialize
    initialize();
    createLayout();

    // request all (READALL) departments
    session_->send(DeptDev, DeptDevDtReadAll);

    // request current department
    // will this always come back b4 the READ, or should we do this
    // when we get a read back? Possible timing issue?
    session_->send(DeptDev, DeptDevDtReadCurrentDept);

    // DEBUG ONLY
    if( DEBUG_MODE )
    {
        addDept( 0, "No Department", "Nodept2" );
        addDept( 1000, "Meat", "Meat2" );
        addDept( 2000, "Deli", "Deli2" );
        addDept( 2001, "Bakery", "Bakery2" );
        setCurrentDept( 2001 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DepartmentPanel::~DepartmentPanel()
{
    // used to prevent a writeAll from occuring in Super is SuperDepartments are up
    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );
    internalConfig->stifleWriteAll = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "DepartmentPanel", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // departments
    if( DeptDev == device && DeptDevDtWriteAll == event )
    {
        DomDepartments m( msg );
        selectDeptCombo_->blockSignals( true );
        selectDeptCombo_->clear();
        selectDeptCombo_->blockSignals( false );

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

        selectDeptCombo_->setEnabled( true );
    }
    else if( DeptDev == device && DeptDevDtWrite == event )
    {
        // set current department
        DomCurrentDept m( msg );

        setCurrentDept( m.deptNum() );
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::onClicked()
{
    const int index( selectDeptCombo_->currentIndex() );

    if ( index < 0 )
        return;
    else if ( index < departments_.size() )
    {
        const int newDeptNumber( departments_[index].number );

        // update session with new department
        session_->send(DeptDev, DeptDevDtSetCurrentDept, QString::number(newDeptNumber) );

        // reset the screen of the current PLU
        session_->send( ProductDev, ProductRead, "0" );

        if( mainWindow_->isSupervisorMode() )
        {
            MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );
            internalConfig->allowWriteOnes = true;
            session_->send(DeptDev, DeptDevDtReadCurrentDept); // in case I am on the superDeptsPanel
        }

        //resetFlashkeys();

        // TODO
        // anything else to refresh?
        // ???

        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::onCurrentIndexChanged( int index )
{
    // update button enabled
    selectDept_->setEnabled( index != currentDeptIndex_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::initialize()
{
    const int fontPointSize20( (mainWindow_->height() * 20) / SessionMainWindow::scalingDividerY_ );

    // set panel title
    setTitle( tr( "Select Department" ) );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize20( fontPointSize20 );

    setFontPointSize12( this );

    // current department
    currentDeptLabel_ = new QLabel( tr( "Current Department" ), this );
    currentDeptLabel_->setAlignment( Qt::AlignLeft );
    currentDeptLabel_->setObjectName( "currentDeptLabel" );

    currentDept_ = new QLabel( this );
    currentDept_->setAlignment( Qt::AlignCenter );
    currentDept_->setObjectName( "currentDept" );

    setFontPointSize20( currentDept_ );

    // choose new department
    selectDeptGroupBox_ = new QGroupBox( tr( "Choose New Department" ), this );
    selectDeptGroupBox_->setObjectName( "selectDeptGroupBox" );

    selectDeptCombo_ = new QComboBox( selectDeptGroupBox_ );
    selectDeptCombo_->setEnabled( false );
    selectDeptCombo_->setFocusPolicy( Qt::NoFocus );
    selectDeptCombo_->setObjectName( "selectDeptCombo" );
    selectDeptCombo_->setPalette( QApplication::palette() );
    selectDeptCombo_->setStyleSheet( QString( "font: %1pt;" ).arg( fontPointSize20 ) );

    connect( selectDeptCombo_, SIGNAL( currentIndexChanged(int) ), SLOT( onCurrentIndexChanged(int) ) );

    selectDept_ = new QPushButton( tr( "Apply " ), selectDeptGroupBox_ );
    selectDept_->setEnabled( false );
    selectDept_->setFocusPolicy( Qt::NoFocus );
    selectDept_->setObjectName( "selectDept" );
    selectDept_->setPalette( QApplication::palette() );

    setFontPointSize20( selectDept_ );

    connect( selectDept_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );
    internalConfig->stifleWriteAll = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::createLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget( selectDept_ );
    buttonLayout->addStretch();

    QVBoxLayout *groupBoxLayout = new QVBoxLayout( selectDeptGroupBox_ );
    groupBoxLayout->addWidget( selectDeptCombo_ );
    groupBoxLayout->addLayout( buttonLayout );

    // add widgets
    widgetsLayout_->insertSpacing( 1, 32 );
    widgetsLayout_->insertWidget( 2, currentDeptLabel_ );
    widgetsLayout_->insertWidget( 3, currentDept_ );
    widgetsLayout_->insertSpacing( 4, 32 );
    widgetsLayout_->insertWidget( 5, selectDeptGroupBox_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::addDept( int number, const QString &namePrim, const QString &nameSec )
{
    deptInfo d;
    d.number = number;
    d.namePrim = namePrim;
    d.nameSec = nameSec;

    // add to table
    departments_.append( d );

    // add to combo
    selectDeptCombo_->blockSignals( true );

    if ( 2 == session_->scaleConfig->currentLanguage )
        selectDeptCombo_->addItem( nameSec, number );
    else
        selectDeptCombo_->addItem( namePrim, number );

    selectDeptCombo_->blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::setCurrentDept( int number )
{
    // find index
    for ( int i( departments_.size() ); i--; )
        if ( departments_[i].number == number )
        {
            // set label and combo
            if ( 2 == session_->scaleConfig->currentLanguage )
                currentDept_->setText( departments_[i].nameSec );
            else
                currentDept_->setText( departments_[i].namePrim );

            selectDeptCombo_->blockSignals( true );
            selectDeptCombo_->setCurrentIndex( i );
            selectDeptCombo_->blockSignals( false );

            // enable combo
            selectDeptCombo_->setEnabled( true );

            // save off index
            currentDeptIndex_ = i;
            break;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DepartmentPanel::resetFlashkeys( bool force )
{
    dcaPrimary_->resetFlashkeys( force );
    dcaSecondary_->resetFlashkeys( force );
}
