/**
 * @file wrapperservicemodetestspanel.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrapperservicemodetestspanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../modes/servicemode.h"

#include "../screens/serviceNewWrapperBspCamera.h"
#include "../screens/serviceNewWrapperTestAirCompressor.h"
#include "../screens/serviceNewWrapperTestCamera.h"
#include "../screens/serviceNewWrapperTestElevator.h"
#include "../screens/serviceNewWrapperTestFilmSupply.h"
#include "../screens/serviceNewWrapperTestGripper.h"
#include "../screens/serviceNewWrapperTestIntake.h"
#include "../screens/serviceNewWrapperTestKnife.h"
#include "../screens/serviceNewWrapperTestLabelApplier.h"
#include "../screens/serviceNewWrapperTestSealerBelt.h"
#include "../screens/serviceNewWrapperTestSideClamps.h"
#include "../screens/serviceNewWrapperTestUpperPusher.h"
#include "../screens/serviceNewWrapperTestUnderfolders.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <pushbutton.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

/// Renderer traits type.
typedef hobart::operateModePanelBackgroundWidget rendererTraitsType;

const int WrapperServiceModeTestsPanel::buttonsPerRow_ = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeTestsPanel::WrapperServiceModeTestsPanel( int mode, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    mode_( (TestSuite) mode )
{
    setFixedSize( sizeHint() );
    setObjectName( "WrapperServiceModeTestsPanel" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeTestsPanel::~WrapperServiceModeTestsPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperServiceModeTestsPanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *WrapperServiceModeTestsPanel::rendererTraits() const
{
    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *WrapperServiceModeTestsPanel::createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel, bool selected )
{
    QPushButton *pushButton( createButton( buttons, buttonText, requiredAccessLevel, selected ) );

    // give object name and map signal
    if ( pushButton )
    {
        pushButton->setObjectName( buttonObjectName );

        connect( pushButton, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
        signalMapper_->setMapping( pushButton, pushButton );
    }

    return pushButton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeTestsPanel::onBackClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeTestsPanel::onPushButtonPressed( QWidget *widget )
{
    if ( !widget )
        return;

    QString screen( widget->objectName() );

    if ( screen.isEmpty() )
        return;

    // wrapper
    if ( "wrapperBspCamera" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperBspCamera>();
    else if ( "wrapperTestCamera" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestCamera>();
    else if ( "wrapperTestIntake" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestIntake>();
    else if ( "wrapperTestElevator" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestElevator>();
    else if ( "wrapperTestFilmSupply" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestFilmSupply>();
    else if (  "wrapperTestGripper" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestGripper>();
    else if ( "wrapperTestSideClamps" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestSideClamps>();
    else if ( "wrapperTestUnderfolders" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestUnderfolders>();
    else if ( "wrapperTestKnife" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestKnife>();
    else if ( "wrapperTestUpperPusher" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestUpperPusher>();
    else if ( "wrapperTestAirCompressor" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestAirCompressor>();
    else if ( "wrapperTestSealerBelt" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestSealerBelt>();
    else if ( "wrapperTestLabelApplier" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperTestLabelApplier>();
  
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeTestsPanel::initialize()
{
    // screen layout
    QHBoxLayout *screenLayout = new QHBoxLayout( this );

    screenLayout->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    screenLayout->addStretch();

    // create button columns
    QVBoxLayout *column;

    ///////////////////////////////////////////////////////////////////////////
    // back button

    const QSize backButtonSize(
        (mainWindow_->width() * 180) / SessionMainWindow::scalingDividerY_,
        (mainWindow_->height() * 45) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderBackButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::transparentBackgroundWidgetRendererAppearance() );

    const QPixmap backButtonPixmap( renderBackButton.drawControl( backButtonSize, hobart::ReadOnly ) );

    modifyFontBold setFontBold( true );
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::blue ) );

    // create expand button
    hobart::pushButton *backButton = new hobart::pushButton(
        (RightDialogLocation == parentDialog_->location() ? tr( "BACK >" ) : tr( "< BACK" )),
        this,
        backButtonPixmap );

    backButton->setObjectName( "backButton" );
    setFontBold( backButton );
    setForeground( backButton );

    connect( backButton, SIGNAL( clicked() ), SLOT( onBackClicked() ) );

    ///////////////////////////////////////////////////////////////////////////
    // test suite buttons

    // create our buttons
    initializeButtons();

    // column 0
    column = new QVBoxLayout();
    column->addStretch();
    column->addLayout( createButtonLayout( tests_, buttonsPerRow_ ) );
    column->addStretch();
    //column->addWidget( backButton );

    screenLayout->addLayout( column );
    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    column = createMenuButtonLayout( ServiceButtonSelected );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( column );
    else
        screenLayout->insertLayout( 0, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeTestsPanel::initializeButtons()
{
    // create signal mapper for buttons
    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onPushButtonPressed(QWidget*) ) );

    //
    // WRAPPER
    //

    if ( WrapperTests == mode_ )
    {
        createNamedButton( tests_, tr( "Intake\nCamera\nView" ), "wrapperBspCamera" );
        createNamedButton( tests_, tr( "Camera" ), "wrapperTestCamera" );
        createNamedButton( tests_, tr( "Intake" ), "wrapperTestIntake" );
        createNamedButton( tests_, tr( "Elevator" ), "wrapperTestElevator" );
        createNamedButton( tests_, tr( "Film Supply" ), "wrapperTestFilmSupply" );
        createNamedButton( tests_, tr( "Gripper" ), "wrapperTestGripper" );
        createNamedButton( tests_, tr( "Side Clamps" ), "wrapperTestSideClamps" );
        createNamedButton( tests_, tr( "Underfolders" ), "wrapperTestUnderfolders" );
        createNamedButton( tests_, tr( "Knife" ), "wrapperTestKnife" );
        createNamedButton( tests_, tr( "Upper\nPusher" ), "wrapperTestUpperPusher" );
        createNamedButton( tests_, tr( "Air\nCompressor" ), "wrapperTestAirCompressor" );
        createNamedButton( tests_, tr( "Sealer Belt" ), "wrapperTestSealerBelt" );

        // PJR-145:  Wrap-only
        if( !( session_->uiUtilities->isWrapOnly() ) )
            createNamedButton( tests_, tr( "Label\nApplier" ), "wrapperTestLabelApplier" );
    }
}

