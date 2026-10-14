/**
 * @file selfservicecamerapredictions.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicecamerapredictions.h"
#include "selfservicehook.h"
#include "selfservicesoftkeybutton.h"

#include <backgroundwidgettypes.h>
#include <pixmap.h>
#include <pushbutton.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <ScaleClasses.h>
#include <ScaleConfiguration.h>
#include <Session.h>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceCameraPredictions::selfServiceCameraPredictions( selfServiceHook *h, const DomLoadSoftkeyGroup *m, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h )
{
    const QSize dialogSize( (window()->width() * 800) / 1024, (window()->height() * 500) / 600 );
    const QSize buttonSize( (window()->width() * 200) / 1024, (window()->height() * 50) / 600 );

    const hobart::backgroundWidgetRenderer renderDialog(
        new hobart::rectangleBackgroundWidget(),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    const QPixmap dialogBackground( renderDialog.drawControl( dialogSize, hobart::ReadOnly ) );

    // ----- //

    setAutoFillBackground( true );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    QPalette p( palette() );
    p.setColor( backgroundRole(), QColor( 0, 0, 0, 127 ) );
    setPalette( p );

    // dialog
    QWidget *dialog( new QWidget( this ) );
    dialog->setAutoFillBackground( true );
    dialog->setFixedSize( dialogBackground.size() );

    QPalette dialogPalette( dialog->palette() );
    dialogPalette.setBrush( dialog->backgroundRole(), dialogBackground );
    dialogPalette.setColor( dialog->foregroundRole(), hobart::colorPalette::gray );
    dialog->setPalette( dialogPalette );

    QFont dialogFont( dialog->font() );
    dialogFont.setPointSize( 14 );
    dialog->setFont( dialogFont );

    // title
    QLabel *dialogTitle( new QLabel( dialog ) );
    dialogTitle->setAlignment( Qt::AlignCenter );
    dialogTitle->setText( tr( "Please confirm the loose\nitem on the scale" ) );

    QFont dialogTitleFont( dialogTitle->font() );
    dialogTitleFont.setPointSize( 20 );
    dialogTitle->setFont( dialogTitleFont );

    // product grid
    hobart::softkeysWidget *products = new hobart::softkeysWidget( hook_->session(), dialog );
    products->setAutoFillBackground( true );
    products->setCreateFilter( hobart::softkeysWidget::CameraSystemPredictions );

    products->setFactory( new hobart::selfServiceSoftkeyButtonFactory() );

    connect( products, &hobart::softkeysWidget::clicked, this, &_Myt::softkeyClicked );
    connect( products, &hobart::softkeysWidget::clicked, this, &_Myt::deleteLater );

    products->create( m );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 15 ),
        new hobart::outlineBackgroundWidgetRendererAppearance( palette().color( foregroundRole() ) ) );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

    // product search
    hobart::pushButton *productSearch = new hobart::pushButton( tr( "Product Search" ), dialog, buttonPixmap );
    productSearch->setFocusPolicy( Qt::NoFocus );

    connect( productSearch, &hobart::pushButton::clicked, this, &_Myt::productSearchRequested );
    connect( productSearch, &hobart::pushButton::clicked, this, &_Myt::deleteLater );

    // no product search!!
    Session *s = hook_->session();
    ScaleConfiguration *config = s->scaleConfig;

    if ( SelfServicePrintOption::SelfServOption_SearchNone == config->scaleConfig.selfServeSearchScreenState() )
        productSearch->hide();

    // cancel
    hobart::pushButton *cancel = new hobart::pushButton( tr( "Cancel" ), dialog, buttonPixmap );
    cancel->setFocusPolicy( Qt::NoFocus );

    connect( cancel, &hobart::pushButton::clicked, this, &_Myt::deleteLater );

    // ----- //

    QVBoxLayout *screenLayout_v( new QVBoxLayout() );

    screenLayout_v->addStretch();
    screenLayout_v->addWidget( dialog );
    screenLayout_v->addStretch();

    QHBoxLayout *screenLayout_h( new QHBoxLayout( this ) );

    screenLayout_h->addStretch();
    screenLayout_h->addLayout( screenLayout_v );
    screenLayout_h->addStretch();

    // ----- //

    QHBoxLayout *buttonsLayout( new QHBoxLayout() );
    buttonsLayout->addStretch();
    buttonsLayout->addWidget( productSearch );
    buttonsLayout->addWidget( cancel );
    buttonsLayout->addStretch();

    QVBoxLayout *dialogLayout( new QVBoxLayout( dialog ) );
    dialogLayout->setSpacing( 16 );

    dialogLayout->addWidget( dialogTitle );
    dialogLayout->addWidget( products, 1 );
    dialogLayout->addLayout( buttonsLayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceCameraPredictions::~selfServiceCameraPredictions()
{
}
