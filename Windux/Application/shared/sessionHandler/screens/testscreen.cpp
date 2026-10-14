/**
 * @file testscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "testscreen.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QScrollBar>
#include <QSlider>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QProcess>

///////////////////////////////////////////////////////////////////////////////////////////////////
TestScreen::TestScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    // initialize the screen
    if ( PrimarySessionType == mainWindow_->sessionType() )
        initializeForDemo();
    else
        initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
TestScreen::~TestScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onSetCapacityBarAreaTop()
{
    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    if ( operateMode )
        operateMode->setCapacityBarArea( TopCapacityBarArea );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onSetCapacityBarAreaBottom()
{
    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    if ( operateMode )
        operateMode->setCapacityBarArea( BottomCapacityBarArea );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onHideOperateWeightBar()
{
    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    if ( operateMode )
        operateMode->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onShowOperateWeightBar()
{
    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    if ( operateMode )
        operateMode->showWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onPlayOperatorVideo1()
{
    QProcess *p = new QProcess( this );

    p->start( "videoPlayer /var/www/videos/load_labelstock.mp4" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::onPlayOperatorVideo2()
{
    QProcess *p = new QProcess( this );

    p->start( "videoPlayer /var/www/videos/print_plu.mp4" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::initialize()
{
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;

    QLabel *label;
    QPushButton *button;
    QLineEdit *lineEdit;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QGroupBox *groupBox;
    QRadioButton *radioButton;
    QCheckBox *checkBox;
    QComboBox *comboBox;
    QListWidget *listWidget;

    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    ///////////////////////////////////////////////////////////////////////////
    // layout the screen

    screenLayout->addStretch();
    screenLayout->addLayout( layout1 );
    screenLayout->addWidget( new QScrollBar( Qt::Vertical, this ) );
    screenLayout->addWidget( new QSlider( Qt::Vertical, this ) );
    screenLayout->addLayout( layout2 );
    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // create some buttons

    horizontalLayout = new QHBoxLayout();

    button = new QPushButton( "Button 1", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onSetCapacityBarAreaTop() ) );

    button = new QPushButton( "Button 2", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onSetCapacityBarAreaBottom() ) );

    button = new QPushButton( "Button 3", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onHideOperateWeightBar() ) );

    button = new QPushButton( "Button 4", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onShowOperateWeightBar() ) );

    button = new QPushButton( "Video 1", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onPlayOperatorVideo1() ) );

    button = new QPushButton( "Video 2", this );
    horizontalLayout->addWidget( button );

    connect( button, SIGNAL( clicked() ), this, SLOT( onPlayOperatorVideo2() ) );

    layout1->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // create some edit boxes

    // text input
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Alphanumeric", this );
    horizontalLayout->addWidget( label );

    lineEdit = new QLineEdit( this );
    horizontalLayout->addWidget( lineEdit );

    layout1->addLayout( horizontalLayout );

    // integer input
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Numeric Only", this );
    horizontalLayout->addWidget( label );

    lineEdit = new QLineEdit( this );
    lineEdit->setValidator( new QIntValidator( this ) );
    horizontalLayout->addWidget( lineEdit );

    layout1->addLayout( horizontalLayout );

    // double input
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Double Only", this );
    horizontalLayout->addWidget( label );

    lineEdit = new QLineEdit( this );
    lineEdit->setValidator( new QDoubleValidator( this ) );
    horizontalLayout->addWidget( lineEdit );

    layout1->addLayout( horizontalLayout );

    // integer spin box
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Numeric Spin", this );
    horizontalLayout->addWidget( label );

    spinBox = new QSpinBox( this );
    horizontalLayout->addWidget( spinBox );

    layout1->addLayout( horizontalLayout );

    // double spin
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Double Spin", this );
    horizontalLayout->addWidget( label );

    doubleSpinBox = new QDoubleSpinBox( this );
    horizontalLayout->addWidget( doubleSpinBox );

    layout1->addLayout( horizontalLayout );

    // integer spin box no buttons
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Spin No Buttons", this );
    horizontalLayout->addWidget( label );

    spinBox = new QSpinBox( this );
    spinBox->setButtonSymbols( QSpinBox::NoButtons );
    horizontalLayout->addWidget( spinBox );

    layout1->addLayout( horizontalLayout );

    // integer spin box plus/minus buttons
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Spin Plus/Minus", this );
    horizontalLayout->addWidget( label );

    spinBox = new QSpinBox( this );
    spinBox->setButtonSymbols( QSpinBox::PlusMinus );
    horizontalLayout->addWidget( spinBox );

    layout1->addLayout( horizontalLayout );

    layout1->addStretch();

    // text input
    horizontalLayout = new QHBoxLayout();

    label = new QLabel( "Alphanumeric", this );
    horizontalLayout->addWidget( label );

    lineEdit = new QLineEdit( this );
    horizontalLayout->addWidget( lineEdit );

    layout1->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // scroll bar

    layout1->addWidget( new QScrollBar( Qt::Horizontal, this ) );
    layout1->addWidget( new QSlider( Qt::Horizontal, this ) );

    ///////////////////////////////////////////////////////////////////////////
    // combo dropdown

    comboBox = new QComboBox( this );
    comboBox->addItem( "Item 0" );
    comboBox->addItem( "Item 1" );
    comboBox->addItem( "Item 2" );
    comboBox->addItem( "Item 3" );
    comboBox->addItem( "Item 4" );
    comboBox->addItem( "Item 5" );
    comboBox->addItem( "Item 6" );
    comboBox->addItem( "Item 7" );
    comboBox->addItem( "Item 8" );
    comboBox->addItem( "Item 9" );
    comboBox->addItem( "Item A" );
    comboBox->addItem( "Item B" );
    comboBox->addItem( "Item C" );
    comboBox->addItem( "Item D" );
    comboBox->addItem( "Item E" );
    comboBox->addItem( "Item F" );
    layout2->addWidget( comboBox );

    ///////////////////////////////////////////////////////////////////////////
    // editable combo

    comboBox = new QComboBox( this );
    comboBox->setEditable( true );
    comboBox->addItem( "Item 0" );
    comboBox->addItem( "Item 1" );
    comboBox->addItem( "Item 2" );
    comboBox->addItem( "Item 3" );
    comboBox->addItem( "Item 4" );
    comboBox->addItem( "Item 5" );
    comboBox->addItem( "Item 6" );
    comboBox->addItem( "Item 7" );
    comboBox->addItem( "Item 8" );
    comboBox->addItem( "Item 9" );
    comboBox->addItem( "Item A" );
    comboBox->addItem( "Item B" );
    comboBox->addItem( "Item C" );
    comboBox->addItem( "Item D" );
    comboBox->addItem( "Item E" );
    comboBox->addItem( "Item F" );
    layout2->addWidget( comboBox );

    ///////////////////////////////////////////////////////////////////////////
    // group box

    groupBox = new QGroupBox( "Group Box", this );
    verticalLayout = new QVBoxLayout( groupBox );

    radioButton = new QRadioButton( "Option 1", groupBox );
    verticalLayout->addWidget( radioButton );

    radioButton = new QRadioButton( "Option 2", groupBox );
    verticalLayout->addWidget( radioButton );

    radioButton = new QRadioButton( "Option 3", groupBox );
    verticalLayout->addWidget( radioButton );

    checkBox = new QCheckBox( "Check Box 1", groupBox );
    verticalLayout->addWidget( checkBox );

    checkBox = new QCheckBox( "Check Box 2", groupBox );
    verticalLayout->addWidget( checkBox );

    layout2->addWidget( groupBox );

    ///////////////////////////////////////////////////////////////////////////
    // list

    listWidget = new QListWidget( this );
    listWidget->setMinimumHeight( 200 );
    listWidget->addItem( "Item 0" );
    listWidget->addItem( "Item 1" );
    listWidget->addItem( "Item 2" );
    listWidget->addItem( "Item 3" );
    listWidget->addItem( "Item 4" );
    listWidget->addItem( "Item 5" );
    listWidget->addItem( "Item 6" );
    listWidget->addItem( "Item 7" );
    listWidget->addItem( "Item 8" );
    listWidget->addItem( "Item 9" );
    listWidget->addItem( "Item A" );
    listWidget->addItem( "Item B" );
    listWidget->addItem( "Item C" );
    listWidget->addItem( "Item D" );
    listWidget->addItem( "Item E" );
    listWidget->addItem( "Item F" );
    layout2->addWidget( listWidget );

    layout2->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen::initializeForDemo()
{
    QHBoxLayout *horizontalLayout;

    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    QVBoxLayout *layout = new QVBoxLayout();

    ///////////////////////////////////////////////////////////////////////////
    // layout the screen

    screenLayout->addStretch();
    screenLayout->addLayout( layout );
    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // create welcome label

    QLabel *welcomeLabel = new QLabel( "Welcome to the Fastrack demo unit!", this );
    layout->addWidget( welcomeLabel );

    welcomeLabel->setAlignment( Qt::AlignHCenter );

    ///////////////////////////////////////////////////////////////////////////
    // create some buttons

    horizontalLayout = new QHBoxLayout();

    QPushButton *video1 = new QPushButton( "Video 1", this );
    horizontalLayout->addWidget( video1 );

    connect( video1, SIGNAL( clicked() ), this, SLOT( onPlayOperatorVideo1() ) );

    QPushButton *video2 = new QPushButton( "Video 2", this );
    horizontalLayout->addWidget( video2 );

    connect( video2, SIGNAL( clicked() ), this, SLOT( onPlayOperatorVideo2() ) );

    layout->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // create some edit boxes

    // text input
    horizontalLayout = new QHBoxLayout();

    QLabel *alphaLabel = new QLabel( "Please type something", this );
    horizontalLayout->addWidget( alphaLabel );

    QLineEdit *alphaLineEdit = new QLineEdit( this );
    horizontalLayout->addWidget( alphaLineEdit );

    layout->addLayout( horizontalLayout );

    // numeric input
    horizontalLayout = new QHBoxLayout();

    QLabel *numberLabel = new QLabel( "Please enter a number", this );
    horizontalLayout->addWidget( numberLabel );

    QSpinBox *numberSpinBox = new QSpinBox( this );
    horizontalLayout->addWidget( numberSpinBox );

    numberSpinBox->setMinimum( 0 );
    numberSpinBox->setMaximum( 999999999 );
    numberSpinBox->setButtonSymbols( QSpinBox::NoButtons );

    layout->addLayout( horizontalLayout );


    layout->insertStretch( 0, 1 );
    layout->addStretch( 3 );
}
