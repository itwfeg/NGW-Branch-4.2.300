/**
 * @file predictionwidget.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "predictionwidget.h"

#include <HobartCustomerHook.h>

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
PredictionWidget::PredictionWidget( QWidget *parent ) :
    _Mybase( parent ),
    scanDataExists_( false )
{
    // initialize
    initialize();
    createLayout();

    // create server for callbacks
    callback_ = new QTcpServer( this );
    callback_->listen( QHostAddress::LocalHost );

    connect( callback_, &QTcpServer::newConnection, this, &_Myt::onNewConnection );

    // register callback server with hook
    const QString action( "ServerSettings|127.0.0.1|" + QString::number( callback_->serverPort() ) );

    hobart_cust_hook_custom_action( action.toLatin1(), nullptr, nullptr, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::onButtonClicked()
{
    if ( predict_ == sender() )
    {
        // predict
        hobart_cust_hook_custom_action( "Predict", nullptr, nullptr, 0 );

        image_->clear();

        results_->clear();

        scanDataExists_ = false;
        onTextChanged();
    }
    else if ( setGroundTruth_ == sender() )
    {
        const QString action( "LabelLastResult|" + pluCode_->text() + "|EdgifyTrainerApp" );

        // last label result
        hobart_cust_hook_custom_action( action.toLatin1(), nullptr, nullptr, 0 );

        scanDataExists_ = false;
        onTextChanged();
    }
    else if ( exit_ == sender() )
    {
        emit exitScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::onNewConnection()
{
    // accept pending connections
    while ( callback_->hasPendingConnections() )
    {
        QTcpSocket *sock( callback_->nextPendingConnection() );

        connect( sock, &QTcpSocket::readyRead, this, &_Myt::onReadyRead );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::onReadyRead()
{
    static const QByteArray header( "RTDO\x1f" );
    static const QByteArray footer( "\x1e" );

    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( sender() ) );

    if ( !sock )
        return;

    const QByteArray a( sock->readAll() );

    qDebug() << a;

    // check for proper header and footer
    if (( a.left( header.size() ) == header ) &&
        ( a.right( footer.size() ) == footer ))
    {
        const QByteArray payload( a.mid( header.size(), a.size() - header.size() - footer.size() ) );

        const QList<QByteArray> fields( payload.split( ',' ) );

        if (( 7 <= fields.size() ) && ( "HRPredictResults" == fields[0] ))
        {
            const int result( fields[1].toInt() );

            if ( result )
                qWarning() << "bad scan result" << result;
            else
            {
                const QString filename( fields[2] );

                const int numResults( fields[3].toInt() );
                const QList<QByteArray> results( fields[4].split( '|' ) );

                const int numProbs( fields[5].toInt() );
                const QList<QByteArray> probs( fields[6].split( '|' ) );

                // check filename exists
                if (( filename.length() ) && ( QFile::exists( filename ) ))
                {
                    const QPixmap capture( filename );

                    // scale image to half the screen size
                    const int w( width() / 2 );
                    const int h( height() / 2 );

                    image_->setPixmap( capture.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
                }

                // show results
                results_->clear();

                if (( numResults == results.size() ) && ( numProbs == probs.size() ) && ( numResults == numProbs ))
                {
                    QString text;

                    for ( int i( 0 ); i < numResults; ++i )
                    {
                        text += results[i];
                        text += " (";
                        text += probs[i];
                        text += ")\n";
                    }

                    results_->setPlainText( text );
                }

                // set visible
                image_->setVisible( true );

                pluCodeLabel_->setVisible( true );
                pluCode_->setVisible( true );
                setGroundTruth_->setVisible( true );

                results_->setVisible( true );

                scanDataExists_ = true;
                onTextChanged();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::onTextChanged()
{
    bool enabled( scanDataExists_ );
    enabled &= !pluCode_->text().isEmpty();

    setGroundTruth_->setEnabled( enabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::initialize()
{
    // image
    image_ = new QLabel( this );
    image_->setObjectName( "image" );
    image_->setVisible( false );

    // plu code label
    pluCodeLabel_ = new QLabel( tr( "PLU:" ), this );
    pluCodeLabel_->setObjectName( "pluCodeLabel" );
    pluCodeLabel_->setVisible( false );

    // plu code
    pluCode_ = new QLineEdit( this );
    pluCode_->setObjectName( "pluCode" );
    pluCode_->setVisible( false );

    connect( pluCode_, &QLineEdit::textChanged, this, &_Myt::onTextChanged );

    // set ground truth
    setGroundTruth_ = new QPushButton( this );
    setGroundTruth_->setEnabled( false );
    setGroundTruth_->setVisible( false );

    setGroundTruth_->setMinimumHeight( 64 );
    setGroundTruth_->setFocusPolicy( Qt::NoFocus );
    setGroundTruth_->setIcon( QPixmap( "res/truth.png") );
    setGroundTruth_->setIconSize( QSize( 32, 32 ) );
    setGroundTruth_->setObjectName( "setGroundTruth" );
    setGroundTruth_->setText( tr( "Set Ground Truth" ) );

    connect( setGroundTruth_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // results
    results_ = new QPlainTextEdit( this );
    results_->setObjectName( "results" );
    results_->setVisible( false );

    // predict
    predict_ = new QPushButton( this );
    predict_->setMinimumHeight( 64 );
    predict_->setFocusPolicy( Qt::NoFocus );
    predict_->setIcon( QPixmap( "res/camera.png") );
    predict_->setIconSize( QSize( 32, 32 ) );
    predict_->setObjectName( "predict" );
    predict_->setText( tr( "Predict" ) );
    connect( predict_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // exit
    exit_ = new QPushButton( this );
    exit_->setMinimumHeight( 64 );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Back" ) );
    connect( exit_, &QPushButton::clicked, this, &_Myt::onButtonClicked );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PredictionWidget::createLayout()
{
    QHBoxLayout *image( new QHBoxLayout );
    image->addStretch();
    image->addWidget( image_ );
    image->addStretch();

    QHBoxLayout *pluCode( new QHBoxLayout );
    pluCode->addWidget( pluCodeLabel_ );
    pluCode->addWidget( pluCode_, 1 );

    QVBoxLayout *upperLeft( new QVBoxLayout );
    upperLeft->addLayout( image );
    upperLeft->addStretch();
    upperLeft->addLayout( pluCode );
    upperLeft->addWidget( setGroundTruth_ );
    upperLeft->addStretch();

    QHBoxLayout *upper( new QHBoxLayout );
    upper->addLayout( upperLeft );
    upper->addWidget( results_ );

    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( predict_ );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( upper, 1 );
    layout->addLayout( buttons );
}
