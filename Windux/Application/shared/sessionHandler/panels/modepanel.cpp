/**
 * @file modepanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "modepanel.h"

#include "../sessionmainwindow.h"
#include "../sessiondialog.h"

#include <backgroundwidgettypes.h>
#include <pushbutton.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

const int ModePanel::menuButtonFontPointSize_ = 10;

///////////////////////////////////////////////////////////////////////////////////////////////////
ModePanel::ModePanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    accessLevel_( 0 )
{
    buttonSize_ = QSize(
        (mainWindow_->width() * 80) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 45) / SessionMainWindow::scalingDividerY_ );

    modeButtonSize_ = QSize(
        (mainWindow_->width() * 90) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 90) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ModePanel::~ModePanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModePanel::mouseReleaseEvent( QMouseEvent *e )
{
    if ( modeButtons_.size() )
    {
        const QRect r( modeButtons_.first()->geometry() );

        if (( e->x() <= r.right() ) && ( e->y() <= r.top() ))
        {
            onDismissClicked();
            return;
        }
    }

    _Mybase::mouseReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *ModePanel::rendererAppearance() const
{
    return new hobart::panelBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *ModePanel::createButton( pushButtonList &buttons, const QString &buttonText, int requiredAccessLevel, bool selected )
{
    LOG_CLASS_INFO( "ModePanel", "createButton" );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::panelButtonBackgroundWidgetRendererAppearance() );

    const QPixmap disabledButtonPixmap( renderButton.drawControl( buttonSize_, hobart::Focused ) );
    const QPixmap enabledButtonPixmap( renderButton.drawControl( buttonSize_, hobart::Normal ) );

    // create the button
    hobart::pushButton *button(
        new hobart::pushButton(
            buttonText,
            this,
            (selected ? disabledButtonPixmap : enabledButtonPixmap) ) );

    // disable the button
    // a) when button is selected
    // b) when operator does not have required access level
    if ( selected )
    {
        button->setEnabled( false );

        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
        setForeground( button );
    }
    else if ( accessLevel_ < requiredAccessLevel )
    {
        button->setEnabled( false );

        LOG_DEBUG( "disabling button '%s' level %d required %d",
            qPrintable( buttonText ),
            accessLevel_,
            requiredAccessLevel );
    }

    buttons.push_back( button );

    return button;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QVBoxLayout *ModePanel::createButtonLayout( const pushButtonList &buttons, int buttonRowSize, const QString &title )
{
    QVBoxLayout *layout( new QVBoxLayout );

    QHBoxLayout *horizontalLayout( NULL );
    int horizontalLayoutButtonCount( 0 );

    // create title
    if ( title.length() )
    {
        QLabel *label( new QLabel( title.toUpper(), this ) );
        label->setAlignment( Qt::AlignLeft );

        layout->addWidget( label );
    }

    // add buttons to layout
    foreach ( QPushButton *button, buttons )
    {
        // create new horizontal layout for this button
        if ( !horizontalLayout )
        {
            horizontalLayout = new QHBoxLayout();
            horizontalLayoutButtonCount = 0;

            layout->addLayout( horizontalLayout );
            layout->setAlignment( horizontalLayout, Qt::AlignLeft );
        }

        // add button to widget
        horizontalLayout->addWidget( button );

        // check how many buttons we have added
        if ( buttonRowSize == ++horizontalLayoutButtonCount )
            horizontalLayout = NULL;
    }

    return layout;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget *ModePanel::createExtendedPanel()
{
    // base class does nothing
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QVBoxLayout *ModePanel::createMenuButtonLayout( SelectedButton selected )
{
    const hobart::backgroundWidgetRenderer renderTransparentButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::transparentBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderEnabledButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderDisabledButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::lightButtonBackgroundWidgetRendererAppearance() );

    QPixmap dismissButtonPixmap( renderTransparentButton.drawControl( QSize( modeButtonSize_.width(), modeButtonSize_.height() / 2 ), hobart::ReadOnly ) );
    QPixmap enabledButtonPixmap( renderEnabledButton.drawControl( modeButtonSize_, hobart::ReadOnly ) );
    QPixmap disabledButtonPixmap( renderDisabledButton.drawControl( modeButtonSize_, hobart::Focused ) );

    // figure out what mode we are currently in
    if ( CurrentModeButtonSelected == selected )
    {
        if ( mainWindow_->isOperateMode() )
            selected = OperateButtonSelected;
        else if ( mainWindow_->isSupervisorMode() )
            selected = SupervisorButtonSelected;
        else if ( mainWindow_->isServiceMode() )
            selected = ServiceButtonSelected;
    }

    hobart::pushButton *dismiss(
        new hobart::pushButton(
            (RightDialogLocation == parentDialog_->location() ? tr( "MENU >" ) : tr( "< MENU" )),
            this,
            dismissButtonPixmap ) );

    hobart::pushButton *operate(
        new hobart::pushButton(
            tr( "Operator\nPages" ),
            this,
            (OperateButtonSelected == selected ? disabledButtonPixmap : enabledButtonPixmap) ) );

    hobart::pushButton *supervisor(
        new hobart::pushButton(
            tr( "Supervisor\nPages" ),
            this,
            (SupervisorButtonSelected == selected ? disabledButtonPixmap : enabledButtonPixmap) ) );

    hobart::pushButton *service(
        new hobart::pushButton(
            tr( "Service\nPages" ),
            this,
            (ServiceButtonSelected == selected ? disabledButtonPixmap : enabledButtonPixmap) ) );

    dismiss->setObjectName( "dismissButton" );
    operate->setObjectName( "operateButton" );
    supervisor->setObjectName( "supervisorButton" );
    service->setObjectName( "serviceButton" );

    modeButtons_.push_back( dismiss );
    modeButtons_.push_back( operate );
    modeButtons_.push_back( supervisor );
    modeButtons_.push_back( service );

    connect( dismiss, SIGNAL( clicked() ), SLOT( onDismissClicked() ) );
    connect( operate, SIGNAL( clicked() ), SLOT( onOperatePagesClicked() ) );
    connect( supervisor, SIGNAL( clicked() ), SLOT( onSupervisorPagesClicked() ) );
    connect( service, SIGNAL( clicked() ), SLOT( onServicePagesClicked() ) );

    // figure out which button is disabled
    hobart::pushButton *disabledButton( NULL );

    if ( OperateButtonSelected == selected )
        disabledButton = operate;
    else if ( SupervisorButtonSelected == selected )
        disabledButton = supervisor;
    else if ( ServiceButtonSelected == selected )
        disabledButton = service;

    // disable the button
    if ( disabledButton )
    {
        disabledButton->setEnabled( false );

        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::gray, hobart::colorPalette::gray ) );
        setForeground( disabledButton );
    }

    // polish the dismiss button
    if ( dismiss )
    {
        QFont f( dismiss->font() );
        f.setBold( true );
        dismiss->setFont( f );

        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::blue ) );
        setForeground( dismiss );
    }

    // polish buttons
    foreach ( QPushButton *button, modeButtons_ )
    {
        QFont f( button->font() );
        f.setPointSize( (window()->height() * menuButtonFontPointSize_) / SessionMainWindow::scalingDividerY_ );
        button->setFont( f );
    }

    // create the layout
    QVBoxLayout *layout( new QVBoxLayout() );
    layout->addStretch();
    layout->addWidget( dismiss );
    layout->addWidget( operate );
    layout->addWidget( supervisor );
    layout->addWidget( service );

    return layout;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModePanel::onDismissClicked()
{
    reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModePanel::onOperatePagesClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModePanel::onSupervisorPagesClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::SupervisorMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModePanel::onServicePagesClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );
    accept();
}
