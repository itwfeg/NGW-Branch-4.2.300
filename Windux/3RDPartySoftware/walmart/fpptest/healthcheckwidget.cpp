/**
 * @file healthcheckwidget.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "healthcheckwidget.h"

#include <QtDebug>

#include <QCheckBox>
#include <QDir>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkInterface>
#include <QPushButton>
#include <QRadioButton>
#include <QTcpSocket>
#include <QVBoxLayout>

// uncomment to use old meta events method for file downloads
//#define USE_META_DOWNLOADS

///////////////////////////////////////////////////////////////////////////////////////////////////
healthCheckWidget::healthCheckWidget( QWidget *parent ) :
    _Mybase( parent ),
    state_( webServerState::Okay )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
healthCheckWidget::~healthCheckWidget()
{
    closeWebServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::setState( webServerState::type value )
{
    if ( value == state_ )
        return;

    Q_EMIT stateChanged( state_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::closeWebServer()
{
    if ( server_.isListening() )
        server_.close();

    close_->setEnabled( false );
    open_->setEnabled( true );
    port_->setEnabled( true );
    options_->setEnabled( false );
    callback_->setEnabled( false );
    updateConfig_->setEnabled( false );
    applySettings_->setEnabled( false );
    fetchProducts_->setEnabled( false );
    upgradeFirmware_->setEnabled( false );
    installFeature_->setEnabled( false );
    loadLabels_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::openWebServer()
{
    const QIntValidator *v( dynamic_cast<const QIntValidator*>( port_->validator() ) );

    // validate port
    QString portStr( port_->text() );
    int pos( 0 );

    if (( !v ) || ( QValidator::Acceptable != v->validate( portStr, pos ) ))
    {
        qDebug() << "invalid port" << portStr;
        return;
    }

    // start server
    if ( !server_.listen( QHostAddress::Any, portStr.toUShort() ) )
    {
        qDebug() << "failed to open server" << server_.errorString();
        return;
    }

    close_->setEnabled( true );
    open_->setEnabled( false );
    port_->setEnabled( false );
    options_->setEnabled( true );
    callback_->setEnabled( true );
    updateConfig_->setEnabled( true );
    applySettings_->setEnabled( true );
    fetchProducts_->setEnabled( true );
    upgradeFirmware_->setEnabled( true );
    installFeature_->setEnabled( true );
    loadLabels_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

#if !defined( USE_META_DOWNLOADS )
    upgradeFirmwareNumSplitsTitle_->hide();
    upgradeFirmwareNumSplits_->hide();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::processClientBuffer( QTcpSocket *client, QByteArray &buffer )
{
    static const QByteArray crlf( "\r\n" );

    // retrieve header
    QStringList header;

    int headerLen( 0 );
    int offset;

    while ( -1 != (offset = buffer.indexOf( crlf, headerLen )) )
    {
        const QString s( buffer.mid( headerLen, offset - headerLen ) );

        headerLen = offset + crlf.length();

        // we have whole header
        if ( !s.length() )
            break;

        header.append( s );
        qDebug() << s;
    }

    // we do not have the entire header yet
    if ( offset < 0 )
        return;

    // check for body
    QByteArray body;

    foreach ( const QString &s, header )
        if ( s.contains( "Content-Length: " ) )
        {
            int contentLength( s.mid( 15 ).toInt() );

            if ( contentLength )
            {
                // we do not have entire body yet
                if (  buffer.length() < (headerLen + contentLength) )
                    return;

                body = buffer.mid( headerLen, contentLength );
                break;
            }
        }

    if ( body.isEmpty() )
    {
        // for GET empty body is fine
        if ( "GET" != buffer.left( 3 ) )
            return;
    }

    // pull entire message off buffer
    buffer = buffer.mid( headerLen + body.length() );

    // process
    Q_EMIT processRequest( client, header, body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onClientDestroyed()
{
    QTcpSocket *client( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( client ) && ( clientBuffers_.contains( client ) ))
    {
        qDebug() << "client removed" << client;
        clientBuffers_.remove( client );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onClientReadyRead()
{
    QTcpSocket *client( dynamic_cast<QTcpSocket*>( sender() ) );

    if ( client )
    {
        QByteArray buffer;

        // retrieve existing buffer
        if ( clientBuffers_.contains( client ) )
            buffer = clientBuffers_[client];

        // read socket
        buffer.append( client->readAll() );

        // process
        processClientBuffer( client, buffer );

        // save state
        clientBuffers_[client] = buffer;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onLineEditEditingFinished()
{
    const QString urlCallback( callbackUrl_->text() );

    if ( updateConfigUrl_ == sender() )
        emit eventSettingsChanged( "UPDATE_CONFIGURATION",
                updateConfigEnabled_->isChecked(),
                updateConfigUrl_->text(),
                updateConfigCallbackForced_->isChecked(),
                updateConfigCallbackEnabled_->isChecked() ? urlCallback : QString() );

    else if ( applySettingsUrl_ == sender() )
        emit eventSettingsChanged( "APPLY_SETTINGS",
                applySettingsEnabled_->isChecked(),
                applySettingsUrl_->text(),
                applySettingsCallbackForced_->isChecked(),
                applySettingsCallbackEnabled_->isChecked() ? urlCallback : QString() );

    else if ( fetchProductsUrl_ == sender() )
        emit eventSettingsChanged( "FETCH_PLU",
                fetchProductsEnabled_->isChecked(),
                fetchProductsUrl_->text(),
                fetchProductsCallbackForced_->isChecked(),
                fetchProductsCallbackEnabled_->isChecked() ? urlCallback : QString() );

    else if ( upgradeFirmwareUrl_ == sender() )
        emit eventSettingsChanged( "UPGRADE_FIRMWARE",
                upgradeFirmwareEnabled_->isChecked(),
                upgradeFirmwareUrl_->text(),
                upgradeFirmwareCallbackForced_->isChecked(),
                upgradeFirmwareCallbackEnabled_->isChecked() ? urlCallback : QString(),
                upgradeFirmwareFile_->text(),
                upgradeFirmwareNumSplits_->text().toInt() );

    else if ( installFeatureUrl_ == sender() )
        emit eventSettingsChanged( "INSTALL_FEATURE",
                installFeatureEnabled_->isChecked(),
                installFeatureUrl_->text(),
                installFeatureCallbackForced_->isChecked(),
                installFeatureCallbackEnabled_->isChecked() ? urlCallback : QString(),
                installFeatureFile_->text(),
                installFeatureNumSplits_->text().toInt() );

    else if ( loadLabelsUrl_ == sender() )
        emit eventSettingsChanged( "LOAD_LABELS",
                loadLabelsEnabled_->isChecked(),
                loadLabelsUrl_->text(),
                loadLabelsCallbackForced_->isChecked(),
                loadLabelsCallbackEnabled_->isChecked() ? urlCallback : QString(),
                loadLabelsFolder_->text() );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onPushButtonClicked()
{
    // close
    if ( close_ == sender() )
        closeWebServer();

    // open
    else if ( open_ == sender() )
        openWebServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onRadioButtonClicked()
{
    webServerState::type newState;

    // handle state
    if ( okay_ == sender() )
        newState = webServerState::Okay;
    else if ( okayNoContent_ == sender() )
        newState = webServerState::OkayNoContent;
    else if ( incompleteHeader_ == sender() )
        newState = webServerState::IncompleteHeader;
    else if ( malformedHeader_ == sender() )
        newState = webServerState::MalformedHeader;
    else if ( malformedBody_ == sender() )
        newState = webServerState::MalformedBody;
    else if ( noResponse_ == sender() )
        newState = webServerState::NoResponse;
    else if ( closeSocket_ == sender() )
        newState = webServerState::CloseSocket;
    else if ( moved_ == sender() )
        newState = webServerState::Moved;
    else if ( unauthorized_ == sender() )
        newState = webServerState::Unauthorized;
    else if ( notFound_ == sender() )
        newState = webServerState::NotFound;
    else if ( exception_ == sender() )
        newState = webServerState::ThrowException;
    else
    {
        const QString urlCallback( callbackUrl_->text() );

        if (( updateConfigEnabled_ == sender() ) || ( updateConfigCallbackEnabled_ == sender() ) || ( updateConfigCallbackForced_ == sender() ))
            emit eventSettingsChanged( "UPDATE_CONFIGURATION",
                    updateConfigEnabled_->isChecked(),
                    updateConfigUrl_->text(),
                    updateConfigCallbackForced_->isChecked(),
                    updateConfigCallbackEnabled_->isChecked() ? urlCallback : QString() );

        else if (( applySettingsEnabled_ == sender() ) || ( applySettingsCallbackEnabled_ == sender() ) || ( applySettingsCallbackForced_ == sender() ))
            emit eventSettingsChanged( "APPLY_SETTINGS",
                    applySettingsEnabled_->isChecked(),
                    applySettingsUrl_->text(),
                    applySettingsCallbackForced_->isChecked(),
                    applySettingsCallbackEnabled_->isChecked() ? urlCallback : QString() );

        else if (( fetchProductsEnabled_ == sender() ) || ( fetchProductsCallbackEnabled_ == sender() ) || ( fetchProductsCallbackForced_ == sender() ))
            emit eventSettingsChanged( "FETCH_PLU",
                    fetchProductsEnabled_->isChecked(),
                    fetchProductsUrl_->text(),
                    fetchProductsCallbackForced_->isChecked(),
                    fetchProductsCallbackEnabled_->isChecked() ? urlCallback : QString() );

        else if (( upgradeFirmwareEnabled_ == sender() ) || ( upgradeFirmwareCallbackEnabled_ == sender() ) || ( upgradeFirmwareCallbackForced_ == sender() ))
            emit eventSettingsChanged( "UPGRADE_FIRMWARE",
                    upgradeFirmwareEnabled_->isChecked(),
                    upgradeFirmwareUrl_->text(),
                    upgradeFirmwareCallbackForced_->isChecked(),
                    upgradeFirmwareCallbackEnabled_->isChecked() ? urlCallback : QString(),
                    upgradeFirmwareFile_->text(),
                    upgradeFirmwareNumSplits_->text().toInt() );

        else if (( installFeatureEnabled_ == sender() ) || ( installFeatureCallbackEnabled_ == sender() ) || ( installFeatureCallbackForced_ == sender() ))
            emit eventSettingsChanged( "INSTALL_FEATURE",
                    installFeatureEnabled_->isChecked(),
                    installFeatureUrl_->text(),
                    installFeatureCallbackForced_->isChecked(),
                    installFeatureCallbackEnabled_->isChecked() ? urlCallback : QString(),
                    installFeatureFile_->text(),
                    installFeatureNumSplits_->text().toInt() );

        else if (( loadLabelsEnabled_ == sender() ) || ( loadLabelsCallbackEnabled_ == sender() ) || ( loadLabelsCallbackForced_ == sender() ))
            emit eventSettingsChanged( "LOAD_LABELS",
                    loadLabelsEnabled_->isChecked(),
                    loadLabelsUrl_->text(),
                    loadLabelsCallbackForced_->isChecked(),
                    loadLabelsCallbackEnabled_->isChecked() ? urlCallback : QString(),
                    loadLabelsFolder_->text() );

        return;
    }

    setState( newState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onServerAcceptError( QAbstractSocket::SocketError error )
{
    qDebug() << "server accept error" << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::onServerNewConnection()
{
    // accept all clients
    while ( server_.hasPendingConnections() )
    {
        QTcpSocket *client( server_.nextPendingConnection() );

        connect( client, SIGNAL(destroyed()), SLOT(onClientDestroyed()) );
        connect( client, SIGNAL(disconnected()), client, SLOT(deleteLater()) );
        connect( client, SIGNAL(readyRead()), SLOT(onClientReadyRead()) );

        qDebug() << "client added" << client;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::initialize()
{
    connect( &server_, SIGNAL(acceptError(QAbstractSocket::SocketError)), SLOT(onServerAcceptError(QAbstractSocket::SocketError)) );
    connect( &server_, SIGNAL(newConnection()), SLOT(onServerNewConnection()) );

    portLabel_ = new QLabel( tr( "Port:" ), this );

    port_ = new QLineEdit( this );
    port_->setText( "8081" );
    port_->setValidator( new QIntValidator( 1, 65535, this ) );

    open_ = new QPushButton( tr( "Open" ), this );
    connect( open_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    close_ = new QPushButton( tr( "Close" ), this );
    close_->setEnabled( false );
    connect( close_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    const QString prefix( "http://" + netAddress() + ":" + port_->text() );

    // ---- //

    options_ = new QGroupBox( tr( "Server Options" ), this );
    options_->setEnabled( false );

    okay_ = new QRadioButton( tr( "Okay" ), options_ );
    okay_->setChecked( true );
    connect( okay_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    okayNoContent_ = new QRadioButton( tr( "Okay (204)" ), options_ );
    connect( okayNoContent_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    incompleteHeader_ = new QRadioButton( tr( "Incomplete Header" ), options_ );
    connect( incompleteHeader_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    malformedHeader_ = new QRadioButton( tr( "Bad Header" ), options_ );
    connect( malformedHeader_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    malformedBody_ = new QRadioButton( tr( "Bad Body" ), options_ );
    connect( malformedBody_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    noResponse_ = new QRadioButton( tr( "Do not respond" ), options_ );
    connect( noResponse_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    closeSocket_ = new QRadioButton( tr( "Close socket" ), options_ );
    connect( closeSocket_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    moved_ = new QRadioButton( tr( "Moved (301)" ), options_ );
    connect( moved_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    unauthorized_ = new QRadioButton( tr( "Unauthorized (401)" ), options_ );
    connect( unauthorized_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    notFound_ = new QRadioButton( tr( "Not Found (404)" ), options_ );
    connect( notFound_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    exception_ = new QRadioButton( tr( "Throw Exception" ), options_ );
    connect( exception_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    // ---- //

    callbackTitle_ = new QLabel( tr( "Callback" ), this );

    callback_ = new QWidget( this );
    callback_->setEnabled( false );

    callbackUrl_ = new QLineEdit( callback_ );
    callbackUrl_->setText( prefix + "/Callback" );
    connect( callbackUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    // ---- //

    updateConfigTitle_ = new QLabel( tr( "Update Configuration" ), this );

    updateConfig_ = new QWidget( this );
    updateConfig_->setEnabled( false );

    updateConfigEnabled_ = new QCheckBox( tr( "Send" ), updateConfig_ );
    updateConfigEnabled_->setChecked( false );
    connect( updateConfigEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    updateConfigCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), updateConfig_ );
    updateConfigCallbackEnabled_->setChecked( false );
    connect( updateConfigCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    updateConfigCallbackForced_ = new QCheckBox( tr( "Forced" ), updateConfig_ );
    updateConfigCallbackForced_->setChecked( false );
    connect( updateConfigCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    updateConfigUrl_ = new QLineEdit( updateConfig_ );
    updateConfigUrl_->setText( prefix + "/UpdateConfiguration?initializeScaleFlg=false" );
    connect( updateConfigUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    // ---- //

    applySettingsTitle_ = new QLabel( tr( "Apply Settings" ), this );

    applySettings_ = new QWidget( this );
    applySettings_->setEnabled( false );

    applySettingsEnabled_ = new QCheckBox( tr( "Send" ), applySettings_ );
    applySettingsEnabled_->setChecked( false );
    connect( applySettingsEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    applySettingsCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), applySettings_ );
    applySettingsCallbackEnabled_->setChecked( false );
    connect( applySettingsCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    applySettingsCallbackForced_ = new QCheckBox( tr( "Forced" ), applySettings_ );
    applySettingsCallbackForced_->setChecked( false );
    connect( applySettingsCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    applySettingsUrl_ = new QLineEdit( applySettings_ );
    applySettingsUrl_->setText( prefix + "/ApplySettings?rebootFlg=false" );
    connect( applySettingsUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    // ---- //

    fetchProductsTitle_ = new QLabel( tr( "Fetch Products" ), this );

    fetchProducts_ = new QWidget( this );
    fetchProducts_->setEnabled( false );

    fetchProductsEnabled_ = new QCheckBox( tr( "Send" ), fetchProducts_ );
    fetchProductsEnabled_->setChecked( false );
    connect( fetchProductsEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    fetchProductsCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), fetchProducts_ );
    fetchProductsCallbackEnabled_->setChecked( false );
    connect( fetchProductsCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    fetchProductsCallbackForced_ = new QCheckBox( tr( "Forced" ), fetchProducts_ );
    fetchProductsCallbackForced_->setChecked( false );
    connect( fetchProductsCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    fetchProductsUrl_ = new QLineEdit( fetchProducts_ );
    fetchProductsUrl_->setText( prefix + "/FetchPlu" );
    connect( fetchProductsUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    // ---- //

    upgradeFirmwareTitle_ = new QLabel( tr( "Upgrade Firmware" ), this );

    upgradeFirmware_ = new QWidget( this );
    upgradeFirmware_->setEnabled( false );

    upgradeFirmwareEnabled_ = new QCheckBox( tr( "Send" ), upgradeFirmware_ );
    upgradeFirmwareEnabled_->setChecked( false );
    connect( upgradeFirmwareEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    upgradeFirmwareCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), upgradeFirmware_ );
    upgradeFirmwareCallbackEnabled_->setChecked( false );
    connect( upgradeFirmwareCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    upgradeFirmwareCallbackForced_ = new QCheckBox( tr( "Forced" ), upgradeFirmware_ );
    upgradeFirmwareCallbackForced_->setChecked( false );
    connect( upgradeFirmwareCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    upgradeFirmwareUrl_ = new QLineEdit( upgradeFirmware_ );
#if defined( USE_META_DOWNLOADS )
    upgradeFirmwareUrl_->setText( prefix + "/UpgradeFirmware?fileName=" );
#else
    upgradeFirmwareUrl_->setText( prefix + "/UpgradeFirmware" );
#endif
    connect( upgradeFirmwareUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    upgradeFirmwareFileTitle_ = new QLabel( tr( "Filename:" ), upgradeFirmware_ );

    upgradeFirmwareFile_ = new QLineEdit( upgradeFirmware_ );
    upgradeFirmwareFile_->setText( findFileWithPattern( "*core*.tgz" ) );
    connect( upgradeFirmwareFile_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    upgradeFirmwareNumSplitsTitle_ = new QLabel( tr( "Parts:" ), upgradeFirmware_ );

    upgradeFirmwareNumSplits_ = new QLineEdit( upgradeFirmware_ );
    upgradeFirmwareNumSplits_->setText( "14" );
    connect( upgradeFirmwareNumSplits_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    // ---- //

    installFeatureTitle_ = new QLabel( tr( "Install Feature" ), this );

    installFeature_ = new QWidget( this );
    installFeature_->setEnabled( false );

    installFeatureEnabled_ = new QCheckBox( tr( "Send" ), installFeature_ );
    installFeatureEnabled_->setChecked( false );
    connect( installFeatureEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    installFeatureCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), installFeature_ );
    installFeatureCallbackEnabled_->setChecked( false );
    connect( installFeatureCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    installFeatureCallbackForced_ = new QCheckBox( tr( "Forced" ), installFeature_ );
    installFeatureCallbackForced_->setChecked( false );
    connect( installFeatureCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    installFeatureUrl_ = new QLineEdit( installFeature_ );
#if defined( USE_META_DOWNLOADS )
    installFeatureUrl_->setText( prefix + "/InstallFeature?fileName=" );
#else
    installFeatureUrl_->setText( prefix + "/InstallFeature" );
#endif
    connect( installFeatureUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    installFeatureFileTitle_ = new QLabel( tr( "Filename:" ), installFeature_ );

    installFeatureFile_ = new QLineEdit( installFeature_ );
    installFeatureFile_->setText( findFileWithPattern( "*feature*.tgz" ) );
    connect( installFeatureFile_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

    installFeatureNumSplitsTitle_ = new QLabel( tr( "Parts:" ), installFeature_ );

    installFeatureNumSplits_ = new QLineEdit( installFeature_ );
    installFeatureNumSplits_->setText( "1" );
    connect( installFeatureNumSplits_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

#if !defined( USE_META_DOWNLOADS )
    installFeatureNumSplitsTitle_->hide();
    installFeatureNumSplits_->hide();
#endif

    // ---- //

    loadLabelsTitle_ = new QLabel( tr( "Load Labels" ), this );

    loadLabels_ = new QWidget( this );
    loadLabels_->setEnabled( false );

    loadLabelsEnabled_ = new QCheckBox( tr( "Send" ), loadLabels_ );
    loadLabelsEnabled_->setChecked( false );
    connect( loadLabelsEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    loadLabelsCallbackEnabled_ = new QCheckBox( tr( "Perform Callback" ), loadLabels_ );
    loadLabelsCallbackEnabled_->setChecked( false );
    connect( loadLabelsCallbackEnabled_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    loadLabelsCallbackForced_ = new QCheckBox( tr( "Forced" ), loadLabels_ );
    loadLabelsCallbackForced_->setChecked( false );
    connect( loadLabelsCallbackForced_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    loadLabelsUrl_ = new QLineEdit( loadLabels_ );
#if defined( USE_META_DOWNLOADS )
    loadLabelsUrl_->setText( prefix + "/LoadLabels?fileName=" );
#else
    loadLabelsUrl_->setText( prefix + "/LoadLabels" );
#endif
    connect( loadLabelsUrl_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));

#if defined( USE_META_DOWNLOADS )
    loadLabelsFolderTitle_ = new QLabel( tr( "Folder (Set Name):" ), loadLabels_ );
#else
    loadLabelsFolderTitle_ = new QLabel( tr( "Filename:" ), loadLabels_ );
#endif

    loadLabelsFolder_ = new QLineEdit( loadLabels_ );
#if defined( USE_META_DOWNLOADS )
    loadLabelsFolder_->setText( "US-WM-HOB-HTI-DELI_20210122" );
#else
    loadLabelsFolder_->setText( findFileWithPattern( "*.ht" ) );
#endif
    connect( loadLabelsFolder_, SIGNAL(editingFinished()), SLOT(onLineEditEditingFinished()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckWidget::createLayout()
{
    QHBoxLayout *server( new QHBoxLayout() );
    server->addWidget( portLabel_ );
    server->addWidget( port_ );
    server->addWidget( open_ );
    server->addWidget( close_ );
    server->addStretch();

    QHBoxLayout *options( new QHBoxLayout( options_ ) );
    options->addWidget( okay_ );
    options->addWidget( okayNoContent_ );
    options->addWidget( incompleteHeader_ );
    options->addWidget( malformedHeader_ );
    options->addWidget( malformedBody_ );
    options->addWidget( noResponse_ );
    options->addWidget( closeSocket_ );
    options->addWidget( moved_ );
    options->addWidget( unauthorized_ );
    options->addWidget( notFound_ );
    options->addWidget( exception_ );

    QHBoxLayout *callback( new QHBoxLayout( callback_ ) );
    callback->setContentsMargins( QMargins() );
    callback->addWidget( callbackUrl_, 1 );

    QHBoxLayout *updateConfig( new QHBoxLayout( updateConfig_ ) );
    updateConfig->setContentsMargins( QMargins() );
    updateConfig->addWidget( updateConfigUrl_, 1 );
    updateConfig->addWidget( updateConfigEnabled_ );
    updateConfig->addWidget( updateConfigCallbackEnabled_ );
    updateConfig->addWidget( updateConfigCallbackForced_ );

    QHBoxLayout *applySettings( new QHBoxLayout( applySettings_ ) );
    applySettings->setContentsMargins( QMargins() );
    applySettings->addWidget( applySettingsUrl_, 1 );
    applySettings->addWidget( applySettingsEnabled_ );
    applySettings->addWidget( applySettingsCallbackEnabled_ );
    applySettings->addWidget( applySettingsCallbackForced_ );

    QHBoxLayout *fetchProducts( new QHBoxLayout( fetchProducts_ ) );
    fetchProducts->setContentsMargins( QMargins() );
    fetchProducts->addWidget( fetchProductsUrl_, 1 );
    fetchProducts->addWidget( fetchProductsEnabled_ );
    fetchProducts->addWidget( fetchProductsCallbackEnabled_ );
    fetchProducts->addWidget( fetchProductsCallbackForced_ );

    QHBoxLayout *upgradeFirmwareTop( new QHBoxLayout );
    upgradeFirmwareTop->setContentsMargins( QMargins() );
    upgradeFirmwareTop->addWidget( upgradeFirmwareUrl_, 1 );
    upgradeFirmwareTop->addWidget( upgradeFirmwareEnabled_ );
    upgradeFirmwareTop->addWidget( upgradeFirmwareCallbackEnabled_ );
    upgradeFirmwareTop->addWidget( upgradeFirmwareCallbackForced_ );

    QHBoxLayout *upgradeFirmwareBottom( new QHBoxLayout );
    upgradeFirmwareBottom->setContentsMargins( QMargins() );
    upgradeFirmwareBottom->addWidget( upgradeFirmwareFileTitle_ );
    upgradeFirmwareBottom->addWidget( upgradeFirmwareFile_, 1 );
    upgradeFirmwareBottom->addWidget( upgradeFirmwareNumSplitsTitle_ );
    upgradeFirmwareBottom->addWidget( upgradeFirmwareNumSplits_ );

    QVBoxLayout *upgradeFirmware( new QVBoxLayout( upgradeFirmware_ ) );
    upgradeFirmware->setContentsMargins( QMargins() );
    upgradeFirmware->addLayout( upgradeFirmwareTop );
    upgradeFirmware->addLayout( upgradeFirmwareBottom );

    QHBoxLayout *installFeatureTop( new QHBoxLayout );
    installFeatureTop->setContentsMargins( QMargins() );
    installFeatureTop->addWidget( installFeatureUrl_, 1 );
    installFeatureTop->addWidget( installFeatureEnabled_ );
    installFeatureTop->addWidget( installFeatureCallbackEnabled_ );
    installFeatureTop->addWidget( installFeatureCallbackForced_ );

    QHBoxLayout *installFeatureBottom( new QHBoxLayout );
    installFeatureBottom->setContentsMargins( QMargins() );
    installFeatureBottom->addWidget( installFeatureFileTitle_ );
    installFeatureBottom->addWidget( installFeatureFile_, 1 );
    installFeatureBottom->addWidget( installFeatureNumSplitsTitle_ );
    installFeatureBottom->addWidget( installFeatureNumSplits_ );

    QVBoxLayout *installFeature( new QVBoxLayout( installFeature_ ) );
    installFeature->setContentsMargins( QMargins() );
    installFeature->addLayout( installFeatureTop );
    installFeature->addLayout( installFeatureBottom );

    QHBoxLayout *loadLabelsTop( new QHBoxLayout );
    loadLabelsTop->setContentsMargins( QMargins() );
    loadLabelsTop->addWidget( loadLabelsUrl_, 1 );
    loadLabelsTop->addWidget( loadLabelsEnabled_ );
    loadLabelsTop->addWidget( loadLabelsCallbackEnabled_ );
    loadLabelsTop->addWidget( loadLabelsCallbackForced_ );

    QHBoxLayout *loadLabelsBottom( new QHBoxLayout );
    loadLabelsBottom->setContentsMargins( QMargins() );
    loadLabelsBottom->addWidget( loadLabelsFolderTitle_ );
    loadLabelsBottom->addWidget( loadLabelsFolder_, 1 );

    QVBoxLayout *loadLabels( new QVBoxLayout( loadLabels_ ) );
    loadLabels->setContentsMargins( QMargins() );
    loadLabels->addLayout( loadLabelsTop );
    loadLabels->addLayout( loadLabelsBottom );

    QFormLayout *urls( new QFormLayout() );
    urls->addRow( callbackTitle_, callback_ );
    urls->addRow( updateConfigTitle_, updateConfig_ );
    urls->addRow( applySettingsTitle_, applySettings_ );
    urls->addRow( fetchProductsTitle_, fetchProducts_ );
    urls->addRow( upgradeFirmwareTitle_, upgradeFirmware_ );
    urls->addRow( installFeatureTitle_, installFeature_ );
    urls->addRow( loadLabelsTitle_, loadLabels_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addLayout( server );
    form->addWidget( options_ );
    form->addLayout( urls );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthCheckWidget::findFileWithPattern( const QString& pattern )
{
    QStringList filters;
    filters.append( pattern );

    QDir d;
    d.setNameFilters( filters );

    const QStringList files( d.entryList( QDir::Files | QDir::NoSymLinks,  QDir::Name ) );

    foreach ( const QString& f, files )
    {
        if ( f.contains( "_part" ) )
            continue;

        return f;
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthCheckWidget::netAddress()
{
    QString result( "127.0.0.1" );

    const QList<QNetworkInterface> ifaces( QNetworkInterface::allInterfaces() );

    foreach ( const QNetworkInterface& iface, ifaces )
    {
        const QNetworkInterface::InterfaceFlags flags( iface.flags() );

        if ( !(QNetworkInterface::IsRunning & flags) )
            continue;
        else if ( QNetworkInterface::IsLoopBack & flags )
            continue;

        // prefer wifi over wired for my personal pc... wifi is connected to the test network
        if ( QNetworkInterface::Wifi == iface.type() )
            ;
        else if ( QNetworkInterface::Ethernet == iface.type() )
        {
            if ( "127.0.0.1" != result )
                continue;
        }
        else
        {
            continue;
        }

        const QList<QNetworkAddressEntry> addresses( iface.addressEntries() );

        foreach ( const QNetworkAddressEntry& address, addresses )
            if ( QAbstractSocket::IPv4Protocol == address.ip().protocol() )
                result = address.ip().toString();
    }

    return result;
}
