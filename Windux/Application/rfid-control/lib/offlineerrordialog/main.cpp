
#include "offlineerrordialog.h"

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QStringList>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setOverrideCursor( QCursor( Qt::BlankCursor ) );

    OfflineErrorDialog d;

    int displayTimer( 0 );

    // iterate arguments
    const QStringList arg( a.arguments() );

    QString value;

    for ( int i( arg.count() ); i--; )
    {
        const QString v( arg[i] );

        qDebug() << v;

        if ( "-w" == v )
            d.setErrorWidth( value.toInt() );
        else if ( "-h" == v )
            d.setErrorHeight( value.toInt() );
        else if ( "-dw" == v )
            d.setFixedWidth( value.toInt() );
        else if ( "-dh" == v )
            d.setFixedHeight( value.toInt() );
        else if ( "-fg" == v )
            d.setErrorForegroundColor( value );
        else if ( "-bg" == v )
            d.setErrorBackgroundColor( value );
        else if ( "-msg" == v )
            d.setErrorMessage( value );
        else if ( "-a" == v )
            d.setErrorAcceptText( value );
        else if ( "-r" == v )
            d.setErrorRejectText( value );
        else if ( "-ff" == v )
            d.setErrorFontFamily( value );
        else if ( "-fps" == v )
            d.setErrorFontPointSize( value.toInt() );
        else if ( "-capture" == v )
            d.capturePrimaryScreen();
        else if ( "-trans" == v )
            d.setAttribute( Qt::WA_TranslucentBackground, true );
        else if ( "-timer" == v )
            displayTimer = value.toInt();
        else
        {
            value = v;
        }
    }

    // generate error dialog
    d.generateForm();

    // show!!
    d.showFullScreen();

    d.activateWindow();
    d.raise();

    // setup timer
    if ( 0 < displayTimer )
        QTimer::singleShot( displayTimer, &d, SLOT(accept()) );

    return a.exec();
}
