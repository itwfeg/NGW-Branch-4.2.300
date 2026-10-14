/**
 * @file selfservicehookerror.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicehookerror.h"

#include <backgroundwidgettypes.h>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHookError::selfServiceHookError( const QString& title, const QString& message, const selfServiceHook::buttonMap& buttons, QWidget *parent ) :
    _Mybase( parent )
{
    const QSize dialogSize( (window()->width() * 640) / 1024, (window()->height() * 300) / 600 );

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
    dialogTitle->setText( title );

    QFont dialogTitleFont( dialogTitle->font() );
    dialogTitleFont.setPointSize( 18 );
    dialogTitle->setFont( dialogTitleFont );

    // message
    QLabel *dialogMessage( new QLabel( dialog ) );
    dialogMessage->setAlignment( Qt::AlignLeft );
    dialogMessage->setText( message );
    dialogMessage->setWordWrap( true );

    // buttons
    QButtonGroup *dialogButtons( new QButtonGroup( dialog ) );

    for ( sessionHooks::buttonMap::const_iterator i = buttons.begin(); i != buttons.end(); ++i )
    {
        QPushButton *b( new QPushButton( dialog ) );
        b->setText( i.value() );

        QPalette buttonPalette( b->palette() );
        buttonPalette.setColor( b->foregroundRole(), hobart::colorPalette::black );
        b->setPalette( buttonPalette );

        dialogButtons->addButton( b, i.key() );
    }

    connect( dialogButtons, SIGNAL( buttonClicked(int) ), SIGNAL( dialogButtonPress(int) ) );

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

    QList<QAbstractButton*> abstractButtons( dialogButtons->buttons() );

    foreach ( QAbstractButton *b, abstractButtons )
        buttonsLayout->addWidget( b );

    // ----- //

    QVBoxLayout *dialogLayout( new QVBoxLayout( dialog ) );
    dialogLayout->setSpacing( 16 );

    dialogLayout->addWidget( dialogTitle );
    dialogLayout->addWidget( dialogMessage );
    dialogLayout->addStretch();
    dialogLayout->addLayout( buttonsLayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHookError::~selfServiceHookError()
{
}
