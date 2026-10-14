/**
 * @file selfservicecamerapredictionsG2.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicecamerapredictionsG2.h"
#include "ui_selfservicecamerapredictionsG2.h"

#include "selfservicehook.h"
//#include "selfservicesoftkeybutton.h"

#include "selfservicesoftkeyswidgetG2.h"
#include "selfservicesoftkeybuttonG2.h"

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
selfServiceCameraPredictionsG2::selfServiceCameraPredictionsG2( selfServiceHook *h, const DomLoadSoftkeyGroup *m, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfServiceCameraPredictionsG2),
    hook_( h )
{
    ui->setupUi(this);

    const QSize dialogSize( (window()->width() * 800) / 1024, (window()->height() * 500) / 600 );
    const QSize buttonSize( (window()->width() * 200) / 1024, (window()->height() * 50) / 600 );

    screen_ = ui->screenContainer;

    // ----- //

    setAutoFillBackground( true );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // product grid
    hobart::softkeysWidget *products = new hobart::softkeysWidget( hook_->session(), ui->cameraPage );
    products->setAutoFillBackground( true );
    products->setCreateFilter( hobart::softkeysWidget::CameraSystemPredictions );

    products->setFactory( new hobart::selfServiceSoftkeyButtonFactoryG2() );

    connect( products, &hobart::softkeysWidget::clicked, this, &_Myt::softkeyClicked );
    connect( products, &hobart::softkeysWidget::clicked, this, &_Myt::deleteLater );

    ui->cameraPage->layout()->addWidget( products );

    products->create( m );

    connect( ui->productSearch, &hobart::pushButton::clicked, this, &_Myt::productSearchRequested );
    connect( ui->productSearch, &hobart::pushButton::clicked, this, &_Myt::deleteLater );

    // no product search!!
    Session *s = hook_->session();
    ScaleConfiguration *config = s->scaleConfig;

    if ( SelfServicePrintOption::SelfServOption_SearchNone == config->scaleConfig.selfServeSearchScreenState() )
        ui->productSearch->hide();

    // cancel
    //hobart::pushButton *cancel = new hobart::pushButton( tr( "Cancel" ), dialog, buttonPixmap );
    ui->cancel->setFocusPolicy( Qt::NoFocus );

    connect( ui->cancel, &hobart::pushButton::clicked, this, &_Myt::deleteLater );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceCameraPredictionsG2::~selfServiceCameraPredictionsG2()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceCameraPredictionsG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}
