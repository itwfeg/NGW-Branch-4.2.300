/**
 * @file mainmenupanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "mainmenupanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/searchdialog.h"
#include "../dialogs/webbrowserdialog.h"

#include "../modes/operatemode.h"

#include "../panels/servicemodepanel.h"
#include "../panels/wrapperservicemodepanel.h"
#include "../panels/wrapperservicemodetestspanel.h"

#include "../screens/plumainscreen.h"

#include <backgroundwidgettypes.h>
#include <toolbutton.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QApplication>

#include <QVBoxLayout>
#include <QToolButton>

#include <UiUtilities.h>

extern bool isDemoMode;

#ifdef QT_DEBUG
    static const bool isDebugBuild = true;
#else
    static const bool isDebugBuild = false;
#endif

const QSize MainMenuPanel::iconSize_( 64, 64 );

///////////////////////////////////////////////////////////////////////////////////////////////////
MainMenuPanel::MainMenuPanel( SessionMainWindow *mainWindow, SessionDialog *parent, SelectedButton selected ) :
    _Mybase( mainWindow, parent ),
    scaleConfig_( session_->scaleConfig ),
    searchEnabled_( true ),
    selected_( selected )
{
    setFixedSize( sizeHint() );
    setObjectName( "MainMenuPanel" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MainMenuPanel::~MainMenuPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize MainMenuPanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 90) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 390) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::resizeEvent( QResizeEvent *e )
{
    const QSize buttonSize(
        (mainWindow_->height() * 70) / SessionMainWindow::scalingDividerY_,
        (mainWindow_->height() * 70) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap enabledButtonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );
    const QPixmap disabledButtonPixmap( renderButton.drawControl( buttonSize, hobart::Focused ) );

    menuButton_->setBackground( (MenuButtonSelected == selected_) ? disabledButtonPixmap : enabledButtonPixmap );

    // PJR-145: Wrap-only
    searchEnabled_ = ( session_->uiUtilities->isWrapOnly() ? false : scaleConfig_->scaleConfig.enableSearchMenuButton() );
    if ( searchEnabled_ )
        searchButton_->setBackground( (SearchButtonSelected == selected_) ? disabledButtonPixmap : enabledButtonPixmap );

    languageButton_->setBackground( (LanguageButtonSelected == selected_) ? disabledButtonPixmap : enabledButtonPixmap );
    helpButton_->setBackground( (HelpButtonSelected == selected_) ? disabledButtonPixmap : enabledButtonPixmap );
    shutdownButton_->setBackground( (ShutdownButtonSelected == selected_) ? disabledButtonPixmap : enabledButtonPixmap );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::onMenuClicked()
{
    mainWindow_->showMenuPanel();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::onSearchClicked()
{
    if( searchEnabled_ )
    {
        mainWindow_->searchKeyPressed();
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::onLanguageClicked()
{
    LOG_CLASS_INFO( "MainMenuPanel", "onLanguageClicked" );

    QString oldLanguage = scaleConfig_->getFPPLanguage();

    // toggle language
    scaleConfig_->currentLanguage = (1 == scaleConfig_->currentLanguage ? 2 : 1);

    // FT-1321
    // language is same or second language is None... nothing to do
    if ( oldLanguage == scaleConfig_->getFPPLanguage() )
    {
        scaleConfig_->currentLanguage = (1 == scaleConfig_->currentLanguage ? 2 : 1);

        accept();
        return;
    }

    // update backend with language change
    session_->send( SoftkeyDev, ExecuteSoftkey, "4," + QString::number( scaleConfig_->currentLanguage ) );

    // set language
    LOG_TRACE( "set language %s...", qPrintable( scaleConfig_->getFPPLanguage() ) );
    mainWindow_->setLanguage( scaleConfig_->getFPPLanguage() );

    // language was not set!
    if ( oldLanguage == mainWindow_->language() )
    {
        // toggle language
        scaleConfig_->currentLanguage = (1 == scaleConfig_->currentLanguage ? 2 : 1);

        // do not exit panel
        return;
    }

    // done!
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::onHelpClicked()
{
    bool isServiceMode( false );

    // check for service mode
    if (( parent()->findChild<ServiceModePanel*>() ) ||
        ( parent()->findChild<WrapperServiceModePanel*>() ) ||
        ( parent()->findChild<WrapperServiceModeTestsPanel*>() ))
    {
        isServiceMode = true;
    }

    // show help
    mainWindow_->showHelp( isServiceMode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::onShutdownClicked()
{
    LOG_CLASS_INFO( "MainMenuPanel", "onShutdownClicked" );

    LOG_WARN( "shutdown button has been pressed", NULL );

    // when demo or debugging as secondary session just close our window
    if (( isDemoMode ) || ( isDebugBuild && mainWindow_->isSecondarySessionType() ))
    {
        LOG_INFO( "closing main window...", NULL );
        mainWindow_->close();
    }
    else
    {
        QString body( "100025,1" );
/*
        // Note:
        // As per Larry, he will checking for the key in the backend and pop appropriate messages
        // to the operator for key removal.

        const MadpKeyActivation *keyActivation = &session_->scaleConfig->keyActivation;

        // when key present ask for key removal first
        if (( keyActivation->isSupervisorKey() ) ||
            ( keyActivation->isServiceKey() ) ||
            ( keyActivation->isSelfServeKey() ))
        {
            body = "100028,1";
        }
*/
        // shutdown!
        LOG_DEBUG( "sending shutdown message...", NULL );
        session_->send( Error, ErrSessionDevice, body );
    }

    // done!
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::initialize()
{
    QList<hobart::toolButton*> buttons;

    // menu button
    buttons.push_back( menuButton_ = new hobart::toolButton(
        QIcon( ":/images/panels/menuIcon.png" ),
        tr( "Menu" ),
        this ) );

    menuButton_->setEnabled( MenuButtonSelected != selected_ );
    menuButton_->setObjectName( "menuButton" );

    connect( menuButton_, SIGNAL( clicked() ), this, SLOT( onMenuClicked() ) );

    // search button
    // PJR-145: Wrap-only
    searchEnabled_ = ( session_->uiUtilities->isWrapOnly() ? false : scaleConfig_->scaleConfig.enableSearchMenuButton() );
    if ( searchEnabled_ )
    {
        buttons.push_back( searchButton_ = new hobart::toolButton(
            QIcon( ":/images/panels/searchIcon.png" ),
            tr( "Search" ),
            this));

    searchButton_->setEnabled( SearchButtonSelected != selected_ );
    searchButton_->setObjectName( "searchButton" );

        connect( searchButton_, SIGNAL( clicked() ), this, SLOT( onSearchClicked() ) );
    }

    // language button
    QString language;
    QString languageText;

    if ( 1 == scaleConfig_->currentLanguage )
        language = scaleConfig_->scaleConfig.secLanguageCode();
    else
        language = scaleConfig_->scaleConfig.languageCode();

    if ( "en" == language )
        languageText = "English";
    else if ( "es" == language )
        languageText = "Español";
    else if ( "fr" == language )
        languageText = "Français";

    buttons.push_back(
        languageButton_ = new hobart::toolButton(
            QIcon( ":/images/panels/languageIcon.png" ),
            languageText,
            this ) );

    languageButton_->setEnabled( LanguageButtonSelected != selected_ );
    languageButton_->setObjectName( "languageButton" );

    connect( languageButton_, SIGNAL( clicked() ), this, SLOT( onLanguageClicked() ) );

    if ( languageText.isEmpty() )
        languageButton_->hide();

    // help button
    buttons.push_back(
        helpButton_ = new hobart::toolButton(
            QIcon( ":/images/panels/helpIcon.png" ),
            tr( "Help" ),
            this ) );

    helpButton_->setEnabled( HelpButtonSelected != selected_ );
    helpButton_->setObjectName( "helpButton" );

    connect( helpButton_, SIGNAL( clicked() ), this, SLOT( onHelpClicked() ) );

    // shutdown button
    buttons.push_back(
        shutdownButton_ = new hobart::toolButton(
            QIcon( ":/images/panels/shutdownIcon.png" ),
            tr( "Shutdown" ),
            this ) );

    shutdownButton_->setEnabled( ShutdownButtonSelected != selected_ );
    shutdownButton_->setObjectName( "shutdownButton" );

    connect( shutdownButton_, SIGNAL( clicked() ), this, SLOT( onShutdownClicked() ) );

    // style each button
    foreach ( hobart::toolButton *button, buttons )
    {
        button->setFocusPolicy( Qt::NoFocus );
        button->setIconSize( iconSize_ );
        button->setToolButtonStyle( Qt::ToolButtonIconOnly );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainMenuPanel::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->setSpacing( 6 );

    layout->addStretch();
    layout->addWidget( menuButton_ );
	
    // PJR-145: Wrap-only
    searchEnabled_ = ( session_->uiUtilities->isWrapOnly() ? false : scaleConfig_->scaleConfig.enableSearchMenuButton() );
    if ( searchEnabled_ )
		layout->addWidget( searchButton_ );
		
    layout->addWidget( languageButton_ );
    layout->addWidget( helpButton_ );
    layout->addWidget( shutdownButton_ );
}

