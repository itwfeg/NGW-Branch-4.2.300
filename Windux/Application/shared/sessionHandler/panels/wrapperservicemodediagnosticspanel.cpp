/**
 * @file wrapperservicemodediagnosticspanel.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrapperservicemodediagnosticspanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/servicemode.h"

#include "../screens/serviceNewWrapperDiagActuators.h"
#include "screens/operateNewWrapperRunIn.h"

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

const int WrapperServiceModeDiagnosticsPanel::buttonsPerRow_ = 2;
const bool TURN_ON_WRAPPER_MENU = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeDiagnosticsPanel::WrapperServiceModeDiagnosticsPanel( int mode, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    mode_( (TestSuite) mode )
{
    setFixedSize( sizeHint() );
    setObjectName( "WrapperServiceModeDiagnosticsPanel" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeDiagnosticsPanel::~WrapperServiceModeDiagnosticsPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperServiceModeDiagnosticsPanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *WrapperServiceModeDiagnosticsPanel::rendererTraits() const
{
    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *WrapperServiceModeDiagnosticsPanel::createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel, bool selected )
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
void WrapperServiceModeDiagnosticsPanel::onBackClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeDiagnosticsPanel::onPushButtonPressed( QWidget *widget )
{
    LOG_CLASS_INFO( "WrapperServiceModeDiagnosticsPanel", "onPushButtonPressed" );

    if ( !widget )
        return;

    QString selectedScreen( widget->objectName() );

    if ( selectedScreen.isEmpty() )
        return;

    // wrapper
    if ( "wrapperActuators" == selectedScreen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperActuatorDiag>();
    else if ( "wrapperRunIn" == selectedScreen )
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "MANUFACTURING RUN IN" ) );
        dialog.setText( tr( "You are about to start manufacturing run in.\n\nAre you sure you want to start?" ) );
        dialog.setAllowOverlayClickDismiss( false );
        dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

        int result = dialog.exec();

        LOG_DEBUG( "manf run in result %d", result );

        if ( QDialog::Accepted == result )
        {
            LOG_DEBUG( "Manf Run-In: ACCEPTED", NULL );

            session_->send( Wrapper, WrEvolStartMfgRunin );

            if( TURN_ON_WRAPPER_MENU )
                mainWindow_->switchToModeScreen<ServiceMode, operateNewWrapperRunIn>();
        }
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeDiagnosticsPanel::initialize()
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
    column->addLayout( createButtonLayout( diagnostics_, buttonsPerRow_ ) );
    column->addStretch();
    column->addWidget( backButton );

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
void WrapperServiceModeDiagnosticsPanel::initializeButtons()
{
    // create signal mapper for buttons
    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onPushButtonPressed(QWidget*) ) );

    //
    // WRAPPER
    //

    if ( WrapperDiagnostics == mode_ )
    {
        createNamedButton( diagnostics_, tr( "Actuators" ), "wrapperActuators" );
        createNamedButton( diagnostics_, tr( "Run-In" ), "wrapperRunIn" );
    }
}
