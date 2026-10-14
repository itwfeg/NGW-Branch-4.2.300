/**
 * @file wrapperservicelabelapplierpanel.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrapperservicelabelapplierpanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../modes/servicemode.h"


#include "../screens/serviceLabelApplierConfig.h"
#include "../screens/serviceNewWrapperLAConfig.h"
#include "../screens/servicePPPDistance.h"

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

const int WrapperServiceModeLAPanel::buttonsPerRow_ = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeLAPanel::WrapperServiceModeLAPanel( int mode, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    mode_( (LabelApplierMode) mode )
{
    setFixedSize( sizeHint() );
    setObjectName( "WrapperServiceModeTestsPanel" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModeLAPanel::~WrapperServiceModeLAPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperServiceModeLAPanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *WrapperServiceModeLAPanel::rendererTraits() const
{
    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *WrapperServiceModeLAPanel::createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel, bool selected )
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
void WrapperServiceModeLAPanel::onBackClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModeLAPanel::initialize()
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
    column->addLayout( createButtonLayout( labelApplier_, buttonsPerRow_ ) );
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
void WrapperServiceModeLAPanel::initializeButtons()
{
    // create signal mapper for buttons
    signalMapper_ = new QSignalMapper( this );
}

