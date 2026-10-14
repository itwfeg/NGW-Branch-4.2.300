/**
 * @file camerapredictionsdialog.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include <backgroundwidgettypes.h>
#include <font.h>
#include <logmanager.h>
#include <palette.h>
#include <pushbutton.h>
#include <softkeyswidget.h>

#include "camerapredictionsdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

QMutex CameraPredictionsDialog::m_;
bool CameraPredictionsDialog::shown_( false );

///////////////////////////////////////////////////////////////////////////////////////////////////
CameraPredictionsDialog::CameraPredictionsDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    setObjectName( "CameraPredictionsDialog" );

    // initialize
    initialize();
    createLayout();

    setShown( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CameraPredictionsDialog::~CameraPredictionsDialog()
{
    setShown( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize CameraPredictionsDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 800) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 500) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::setSoftkeyGroup( const DomLoadSoftkeyGroup *m )
{
    if ( hobart::softkeysWidget::CameraSystemPredictions != m->featureNum() )
        return;

    if ( products_ )
        products_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CameraPredictionsDialog::isShown()
{
    QMutexLocker guard( &m_ );
    return shown_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // softkeys
    if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        setSoftkeyGroup( &m );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::resetFlashkeys( bool force )
{
    if ( products_ )
        products_->sendResetMessage( force );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::onProductSearchClicked()
{
    emit productSearchRequested();

    reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    Q_UNUSED( b )
    Q_UNUSED( param )
    Q_UNUSED( feature )

    if ( products_ != w )
        return;
    else if ( Action_SelectProduct != action )
        return;

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::initialize()
{
    static const QMargins screenMargins( 16, 16, 16, 16 );

    setContentsMargins( screenMargins );

    const QSize buttonSize(
        (mainWindow_->width() * 200) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 50) / SessionMainWindow::scalingDividerY_ );

    bool selfServiceMode( false );

    // title
    title_ = new QLabel( tr( "Please confirm the loose\nitem on the scale" ), this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );

    QFont f( title_->font() );
    f.setPointSize( (3 * font().pointSize()) / 2 );

    title_->setFont( f );

    // product grid
    products_ = new hobart::softkeysWidget( session_, this );
    products_->setAutoFillBackground( true );
    products_->setCreateFilter( hobart::softkeysWidget::CameraSystemPredictions );
    products_->setObjectName( "products" );

    OperateMode *operateMode( mainWindow_->retrieveMode<OperateMode>() );

    if ( operateMode )
    {
        connect( products_, &hobart::softkeysWidget::clicked, operateMode, &OperateMode::handleFlashkey );

        selfServiceMode = operateMode->isSelfService();
    }

    connect( products_, &hobart::softkeysWidget::clicked, this, &_Myt::onSoftkeyClicked );

    if ( selfServiceMode )
    {
        const hobart::backgroundWidgetRenderer renderButton(
            new hobart::panelButtonBackgroundWidget( 15 ),
            new hobart::outlineBackgroundWidgetRendererAppearance( palette().color( foregroundRole() ) ) );

        const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

        // product search
        productSearch_ = new hobart::pushButton( tr( "Product Search" ), this, buttonPixmap );
        productSearch_->setFocusPolicy( Qt::NoFocus );
        productSearch_->setObjectName( "productSearch" );

        // canel
        cancel_ = new hobart::pushButton( tr( "Cancel" ), this, buttonPixmap );
        cancel_->setFocusPolicy( Qt::NoFocus );
        cancel_->setObjectName( "cancel" );
    }
    else
    {
        const hobart::backgroundWidgetRenderer renderButton(
            new hobart::panelButtonBackgroundWidget( 4 ),
            new hobart::darkButtonBackgroundWidgetRendererAppearance() );

        const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

        // product search
        productSearch_ = new hobart::pushButton( tr( "Product Search" ), this, buttonPixmap );
        productSearch_->setFocusPolicy( Qt::NoFocus );
        productSearch_->setObjectName( "productSearch" );
        setForeground( productSearch_ );

        productSearch_->hide(); // hide this button for now, not relevant in operate mode

        // canel
        cancel_ = new hobart::pushButton( tr( "Cancel" ), this, buttonPixmap );
        cancel_->setFocusPolicy( Qt::NoFocus );
        cancel_->setObjectName( "cancel" );
        setForeground( cancel_ );
    }

    connect( productSearch_, &hobart::pushButton::clicked, this, &_Myt::onProductSearchClicked );
    connect( cancel_, &hobart::pushButton::clicked, this, &_Myt::reject );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addStretch();
    buttons->addWidget( productSearch_ );
    buttons->addWidget( cancel_ );
    buttons->addStretch();

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->addWidget( title_ );
    screenLayout->addWidget( products_, 1 );
    screenLayout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraPredictionsDialog::setShown( bool value )
{
    QMutexLocker guard( &m_ );
    shown_ = value;
}
