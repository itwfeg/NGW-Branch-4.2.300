/**
 * @file clientcommdialog.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientcommdialog.h"

#include <QByteArray>
#include <QLabel>
#include <QPlainTextEdit>
#include <QStringList>
#include <QTabWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
clientCommDialog::clientCommDialog( const QStringList &header, const QByteArray &body, QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize( header, body );
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clientCommDialog::clientCommDialog( const QStringList &requestHeader, const QByteArray &requestBody, const QStringList &responseHeader, const QByteArray &responseBody, QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize( requestHeader, requestBody, responseHeader, responseBody );
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clientCommDialog::~clientCommDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize clientCommDialog::sizeHint() const
{
    QWidget *parent( parentWidget() );

    if ( !parent )
        return _Mybase::sizeHint();

    // find topmost parent
    while ( parent->parentWidget() )
        parent = parent->parentWidget();

    // base dialog size off parent size
    QSize s( parent->size() );

    s.rheight() *= 90;
    s.rheight() /= 100;

    s.rwidth() *= 50;
    s.rwidth() /= 100;

    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommDialog::initialize( const QStringList &requestHeader, const QByteArray &requestBody, const QStringList &responseHeader, const QByteArray &responseBody )
{
    setWindowFlags( windowFlags() | Qt::WindowMaximizeButtonHint );
    setWindowTitle( tr( "Request Details" ) );

    tabs_ = new QTabWidget( this );

    request_ = new QWidget( this );

    // generate request header
    foreach ( const QString &s, requestHeader )
    {
        QLabel *label( new QLabel( request_ ) );
        label->setText( s );

        requestHeader_.append( label );
    }

    // generate request body
    requestBody_ = new QPlainTextEdit( request_ );
    requestBody_->setPlainText( requestBody );

    if ( requestBody.isEmpty() )
        requestBody_->setEnabled( false );

    // ---- //

    if ( responseBody.isEmpty() )
    {
        response_ = nullptr;
        return;
    }

    response_ = new QWidget( this );

    // generate response header
    foreach ( const QString &s, responseHeader )
    {
        QLabel *label( new QLabel( response_ ) );
        label->setText( s );

        responseHeader_.append( label );
    }

    // generate response body
    responseBody_ = new QPlainTextEdit( response_ );
    responseBody_->setPlainText( responseBody );

    if ( responseBody.isEmpty() )
        responseBody_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommDialog::createLayout()
{
    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addWidget( tabs_ );

    // ---- //

    QVBoxLayout *requestForm( new QVBoxLayout( request_ ) );

    foreach ( QWidget *w, requestHeader_ )
        requestForm->addWidget( w );

    requestForm->addWidget( requestBody_, 1 );

    tabs_->addTab( request_, tr( "Request" ) );

    // ---- //

    if ( response_ )
    {
        QVBoxLayout *responseForm( new QVBoxLayout( response_ ) );

        foreach ( QWidget *w, responseHeader_ )
            responseForm->addWidget( w );

        responseForm->addWidget( responseBody_, 1 );

        tabs_->addTab( response_, tr( "Response" ) );
    }
}
