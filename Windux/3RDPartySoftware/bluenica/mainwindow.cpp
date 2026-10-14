/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "configuration.h"
#include "mainwindow.h"
#include "profileeditor.h"
#include "profileselection.h"
#include "sensorpointeditor.h"
#include "sensorpointselection.h"
#include "sensorselection.h"
#include "temperatureviewer.h"

#include <QDateTime>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( QWidget *parent ) :
    _Mybase( parent ),
    state_( STATE_UNKNOWN )
{
    // init
    initialize();
    createLayout();

    // proceed to dashboard
    setCurrentState( STATE_DASHBOARD );

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
    if ( back_ )
        back_->setText( tr( "&Back" ) );

    if ( config_ )
        config_->setText( tr( "&Configuration" ) );

    if ( delete_ )
        delete_->setText( tr( "&Delete" ) );

    if ( edit_ )
        edit_->setText( tr( "&Edit" ) );

    if ( exit_ )
        exit_->setText( tr( "E&xit" ) );

    if ( new_ )
        new_->setText( tr( "&New" ) );

    if ( refresh_ )
        refresh_->setText( tr( "&Refresh" ) );

    if ( save_ )
        save_->setText( tr( "&Save" ) );

    if ( show1Day_ )
        show1Day_->setText( tr( "Today" ) );

    if ( show1Week_ )
        show1Week_->setText( tr( "1 Week" ) );

    if ( show2Weeks_ )
        show2Weeks_->setText( tr( "2 Weeks" ) );

    if ( show3Days_ )
        show3Days_->setText( tr( "3 Days" ) );

    if ( showPrev12h_ )
        showPrev12h_->setText( tr( "Last 12h" ) );

    if ( showPrev24h_ )
        showPrev24h_->setText( tr( "Last 24h" ) );

    if ( view_ )
        view_->setText( tr( "&View" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int mainWindow::selectedSensorPoint() const
{
    QList<unsigned int> items( sensorPointSelection_->selectedItems() );

    if ( !items.size() )
        return 0;

    return items.first();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int mainWindow::selectedSensor() const
{
    QList<unsigned int> items( sensorSelection_->selectedItems() );

    if ( !items.size() )
        return 0;

    return items.first();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int mainWindow::selectedTempProfile() const
{
    QList<unsigned int> items( tempProfileSelection_->selectedItems() );

    if ( !items.size() )
        return 0;

    return items.first();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::setCurrentState( state value )
{
    QPalette p;

    // nothing to do
    if ( value == state_ )
        return;

    // remove all buttons
    removeAllButtons();

    // handle state change
    switch ( (state_ = value) )
    {
    case STATE_DASHBOARD:
        // update buttons
        buttons_->addButton( (refresh_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (view_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (edit_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (new_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (config_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (exit_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        sensorPointSelection_->clearSelection();
        sensorSelection_->clearSelection();
        tempProfileSelection_->clearSelection();

        refresh_->setEnabled( tempProfileSelection_ != dashboard_->currentWidget() );
        view_->setEnabled( false );
        edit_->setEnabled( false );
        new_->setEnabled( sensorSelection_ != dashboard_->currentWidget() );

        sensorPointSelection_->refresh();
        sensorSelection_->refresh();
        tempProfileSelection_->refresh();

        // switch to widget
        stackedWidget_->setCurrentWidget( dashboard_ );
        break;

    case STATE_SENSOR_POINT_CONFIGURATION:

        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (delete_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        sensorPointConfiguration_->load( selectedSensorPoint() );

        p = delete_->palette();
        p.setColor( delete_->foregroundRole(), RED );
        delete_->setPalette( p );

        // switch to widget
        stackedWidget_->setCurrentWidget( sensorPointConfiguration_ );
        break;

    case STATE_SENSOR_POINT_NEW:

        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        sensorPointConfiguration_->insert();

        // switch to widget
        stackedWidget_->setCurrentWidget( sensorPointConfiguration_ );
        break;

    case STATE_SENSOR_POINT_TEMP_GRAPH:

        // update buttons
        buttons_->addButton( (showPrev12h_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (showPrev24h_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show1Day_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show3Days_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show1Week_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show2Weeks_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // update widget
        tempGraph_->setSensorPointID( selectedSensorPoint() );

        // switch to widget
        stackedWidget_->setCurrentWidget( tempGraph_ );
        break;

    case STATE_SENSOR_TEMP_GRAPH:

        // update buttons
        buttons_->addButton( (showPrev12h_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (showPrev24h_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show1Day_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show3Days_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show1Week_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (show2Weeks_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // update widget
        tempGraph_->setSensorID( selectedSensor() );

        // switch to widget
        stackedWidget_->setCurrentWidget( tempGraph_ );
        break;

    case STATE_TEMP_PROFILE_CONFIGURATION:

        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (delete_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        tempProfileConfiguration_->load( selectedTempProfile() );

        p = delete_->palette();
        p.setColor( delete_->foregroundRole(), RED );
        delete_->setPalette( p );

        // switch to widget
        stackedWidget_->setCurrentWidget( tempProfileConfiguration_ );
        break;

    case STATE_TEMP_PROFILE_NEW:

        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        tempProfileConfiguration_->insert();

        // switch to widget
        stackedWidget_->setCurrentWidget( tempProfileConfiguration_ );
        break;

    case STATE_CONFIGURATION:

        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        configuration_->load();

        // switch to widget
        stackedWidget_->setCurrentWidget( configuration_ );
        break;

    default:
        break;
    }

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::removeAllButtons()
{
    // remove all buttons
    foreach ( QAbstractButton *b, buttons_->buttons() )
    {
        buttons_->removeButton( b );
        b->deleteLater();
    }

    // reset internal pointers
    back_ = config_ = delete_ = edit_ = exit_ = new_ =
        refresh_ = save_ = show1Day_ = show1Week_ = show2Weeks_ = show3Days_ =
        showPrev12h_ = showPrev24h_ = view_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked( QAbstractButton *button )
{
    static const unsigned int period_12h( 12 * 60 * 60 );
    static const unsigned int period_24h( 24 * 60 * 60 );
    static const unsigned int period_3d( 3 * 24 * 60 * 60 );
    static const unsigned int period_1w( 7 * 24 * 60 * 60 );
    static const unsigned int period_2w( 14 * 24 * 60 * 60 );

    unsigned int top( QDateTime::currentMSecsSinceEpoch() / 1000 );

    if ( back_ == button )
        setCurrentState( STATE_DASHBOARD );

    else if ( config_ == button )
        setCurrentState( STATE_CONFIGURATION );

    else if (( delete_ == button ) && ( STATE_SENSOR_POINT_CONFIGURATION == state_ ))
    {
        if ( sensorPointConfiguration_->remove() )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( delete_ == button ) && ( STATE_TEMP_PROFILE_CONFIGURATION == state_ ))
    {
        if ( tempProfileConfiguration_->remove() )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( edit_ == button ) && ( sensorPointSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_SENSOR_POINT_CONFIGURATION );

    else if (( edit_ == button ) && ( tempProfileSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_TEMP_PROFILE_CONFIGURATION );

    else if ( exit_ == button )
        close();

    else if (( new_ == button ) && ( sensorPointSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_SENSOR_POINT_NEW );

    else if (( new_ == button ) && ( tempProfileSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_TEMP_PROFILE_NEW );

    else if (( refresh_ == button ) && ( sensorPointSelection_ == dashboard_->currentWidget() ))
        sensorPointSelection_->refresh();

    else if (( refresh_ == button ) && ( sensorSelection_ == dashboard_->currentWidget() ))
        sensorSelection_->refresh();

    else if (( save_ == button ) && ( STATE_CONFIGURATION == state_ ))
    {
        if ( configuration_->save() )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( save_ == button ) && ( STATE_SENSOR_POINT_CONFIGURATION == state_ ))
    {
        if ( sensorPointConfiguration_->save() )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( save_ == button ) && ( STATE_SENSOR_POINT_NEW == state_ ))
    {
        if ( sensorPointConfiguration_->save( true ) )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( save_ == button ) && ( STATE_TEMP_PROFILE_CONFIGURATION == state_ ))
    {
        if ( tempProfileConfiguration_->save() )
            setCurrentState( STATE_DASHBOARD );
    }

    else if (( save_ == button ) && ( STATE_TEMP_PROFILE_NEW == state_ ))
    {
        if ( tempProfileConfiguration_->save( true ) )
            setCurrentState( STATE_DASHBOARD );
    }

    else if ( showPrev12h_ == button )
        tempGraph_->setSensorFilter( top - period_12h, top );

    else if ( showPrev24h_ == button )
        tempGraph_->setSensorFilter( top - period_24h, top );

    else if ( show1Day_ == button )
    {
        top += period_24h - (top % period_24h);
        top -= utcOffset();

        tempGraph_->setSensorFilter( top - period_24h, top );
    }

    else if ( show1Week_ == button )
    {
        top += period_24h - (top % period_24h);
        top -= utcOffset();

        tempGraph_->setSensorFilter( top - period_1w, top );
    }

    else if ( show2Weeks_ == button )
    {
        top += period_24h - (top % period_24h);
        top -= utcOffset();

        tempGraph_->setSensorFilter( top - period_2w, top );
    }

    else if ( show3Days_ == button )
    {
        top += period_24h - (top % period_24h);
        top -= utcOffset();

        tempGraph_->setSensorFilter( top - period_3d, top );
    }

    else if (( view_ == button ) && ( sensorPointSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_SENSOR_POINT_TEMP_GRAPH );

    else if (( view_ == button ) && ( sensorSelection_ == dashboard_->currentWidget() ))
        setCurrentState( STATE_SENSOR_TEMP_GRAPH );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onCurrentChanged( int id )
{
    Q_UNUSED( id );

    // reset widget
    sensorPointSelection_->clearSelection();
    sensorSelection_->clearSelection();
    tempProfileSelection_->clearSelection();

    if ( refresh_ )
        refresh_->setEnabled( tempProfileSelection_ != dashboard_->currentWidget() );

    if ( view_ )
        view_->setEnabled( false );

    if ( edit_ )
        edit_->setEnabled( false );

    if ( new_ )
        new_->setEnabled( sensorSelection_ != dashboard_->currentWidget() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onItemDoubleClicked( unsigned int id )
{
    Q_UNUSED( id );

    if ( sensorPointSelection_ == dashboard_->currentWidget() )
        setCurrentState( STATE_SENSOR_POINT_TEMP_GRAPH );
    else if ( sensorSelection_ == dashboard_->currentWidget() )
        setCurrentState( STATE_SENSOR_TEMP_GRAPH );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onItemSelectionChanged()
{
    QList<unsigned int> items;

    if ( sensorPointSelection_ == dashboard_->currentWidget() )
        items = sensorPointSelection_->selectedItems();
    else if ( sensorSelection_ == dashboard_->currentWidget() )
        items = sensorSelection_->selectedItems();
    else if ( tempProfileSelection_ == dashboard_->currentWidget() )
        items = tempProfileSelection_->selectedItems();

    view_->setEnabled( !items.isEmpty() && tempProfileSelection_ != dashboard_->currentWidget() );
    edit_->setEnabled( !items.isEmpty() && sensorSelection_ != dashboard_->currentWidget() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    // increase font size
    QFont f( font() );
    f.setPointSize( 14 );

    setFont( f );

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    setCentralWidget( centralWidget_ );

    stackedWidget_ = new QStackedWidget( centralWidget_ );
    stackedWidget_->setObjectName( "stackedWidget" );

    dashboard_ = new QTabWidget( stackedWidget_ );
    dashboard_->setObjectName( "dashboard" );

    connect( dashboard_, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)) );

    sensorPointConfiguration_ = new sensorPointEditorWidget( stackedWidget_ );
    sensorPointConfiguration_->setObjectName( "sensorPointConfiguration" );

    tempProfileConfiguration_ = new tempProfileEditorWidget( stackedWidget_ );
    tempProfileConfiguration_->setObjectName( "tempProfileConfiguration_" );

    configuration_ = new configurationWidget( stackedWidget_ );
    configuration_->setObjectName( "configuration" );

    tempGraph_ = new temperatureViewerWidget( stackedWidget_ );
    tempGraph_->setObjectName( "tempGraph" );

    stackedWidget_->addWidget( dashboard_ );
    stackedWidget_->addWidget( sensorPointConfiguration_ );
    stackedWidget_->addWidget( tempProfileConfiguration_ );
    stackedWidget_->addWidget( configuration_ );
    stackedWidget_->addWidget( tempGraph_ );

    sensorPointSelection_ = new sensorPointSelectionWidget( dashboard_ );
    sensorPointSelection_->setObjectName( "sensorPointSelection" );

    connect( sensorPointSelection_, SIGNAL(itemDoubleClicked(uint)), SLOT(onItemDoubleClicked(uint)) );
    connect( sensorPointSelection_, SIGNAL(itemSelectionChanged()), SLOT(onItemSelectionChanged()) );

    sensorSelection_ = new sensorSelectionWidget( dashboard_ );
    sensorSelection_->setObjectName( "sensorSelection" );

    connect( sensorSelection_, SIGNAL(itemDoubleClicked(uint)), SLOT(onItemDoubleClicked(uint)) );
    connect( sensorSelection_, SIGNAL(itemSelectionChanged()), SLOT(onItemSelectionChanged()) );

    tempProfileSelection_ = new tempProfileSelectionWidget( dashboard_ );
    tempProfileSelection_->setObjectName( "tempProfileSelection" );

    connect( tempProfileSelection_, SIGNAL(itemDoubleClicked(uint)), SLOT(onItemDoubleClicked(uint)) );
    connect( tempProfileSelection_, SIGNAL(itemSelectionChanged()), SLOT(onItemSelectionChanged()) );

    dashboard_->blockSignals( true );
    dashboard_->addTab( sensorPointSelection_, tr( "Data Points" ) );
    dashboard_->addTab( sensorSelection_, tr( "Sensors" ) );
    dashboard_->addTab( tempProfileSelection_, tr( "Profiles" ) );
    dashboard_->blockSignals( false );

    buttons_ = new QDialogButtonBox( Qt::Horizontal, centralWidget_ );
    buttons_->setCenterButtons( true );
    buttons_->setObjectName( "buttons" );

    connect( buttons_, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClicked(QAbstractButton*)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->setContentsMargins( QMargins( 0, 0, 0, 6 ) );
    layout->addWidget( stackedWidget_, 1 );
    layout->addWidget( buttons_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int mainWindow::utcOffset()
{
    const QDateTime dateTime1( QDateTime::currentDateTime() );
    const QDateTime dateTime2( QDateTime( dateTime1.date(), dateTime1.time(), Qt::UTC ) );

    return dateTime1.secsTo( dateTime2 );
}
