#ifndef QWEBVIEWSELECTIONSUPPRESSOR_H
#define QWEBVIEWSELECTIONSUPPRESSOR_H

#include <logmanager.h>

#include <QAbstractSlider>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QRect>
#include <QScrollBar>
#include <QWebFrame>
#include <QWebView>

class QWebViewSelectionSuppressor : public QObject
{
    Q_OBJECT
public:
    QWebViewSelectionSuppressor(QWebView *v)
        : QObject(v), view(v), enabled(false), mousePressed(false)
    {
        Q_ASSERT(view);
        view->setAcceptDrops( false );

        enable();
    }

    inline void enable()
    {
        if (enabled)
            return;
        view->installEventFilter(this);
        enabled = true;
    }

    inline void disable()
    {
        if (!enabled)
            return;
        view->removeEventFilter(this);
        enabled = false;
    }

    inline bool isEnabled() const
    {
        return enabled;
    }

protected:
    inline bool eventFilter(QObject *, QEvent *e);

private:
    QWebView *view;
    bool enabled;
    bool mousePressed;
};

bool QWebViewSelectionSuppressor::eventFilter(QObject *widget, QEvent *e)
{
    //LOG_CLASS_INFO( "QWebViewSelectionSuppressor", "eventFilter" );

    Q_UNUSED( widget );

    switch (e->type()) {
    case QEvent::MouseButtonPress:
        //LOG_DEBUG( "in mouse button press", NULL );
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
            mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
        //LOG_DEBUG( "in mouse button release", NULL );
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
            mousePressed = false;
        break;

    case QEvent::MouseMove:
        //LOG_DEBUG( "in mouse moved", NULL );
        if (mousePressed)
        {
            QPoint pos( static_cast<QMouseEvent*>( e )->pos() );

            QList<QWebFrame*> frames;
            frames.append( view->page()->mainFrame() );

            while ( frames.size() )
            {
                QWebFrame *f( frames.first() );

                // get geometry
                QRect hr( f->scrollBarGeometry( Qt::Horizontal ) );
                QRect vr( f->scrollBarGeometry( Qt::Vertical ) );

                // check geometry
                if ((( !hr.isEmpty() ) && ( hr.contains( pos, false ) )) ||
                    (( !vr.isEmpty() ) && ( vr.contains( pos, false ) )))
                {
                    return false;
                }

                // look for children frames
                QList<QWebFrame*> children( f->childFrames() );

                foreach ( QWebFrame *c, children )
                    frames.append( c );

                frames.removeFirst();
            }

            // FT-678 begin
            // extra logic below, see note...

            // perform hit test
            QWebHitTestResult result( view->page()->mainFrame()->hitTestContent( pos ) );

            // Note:
            //
            // Evaluating the source code to QWebFrame shows that when a scrollbar is hit it will
            // set its internal pointer to null and return bad results. We know for a fact that
            // the pos is within the frame... so if null is returned we can be resonably sure a
            // scrollbar is underneath. Anything other than a scroll bar (according to the QWebFrame
            // code) would return valid results.
            //
            // If this doesn't work out well not sure what can be done... the web kit framework has
            // made it very difficult to detect what is being clicked.
            //
            if ( !result.frame() )
                return false;

            // FT-678 end

            // else no vertical/horizontal scrollbar present or if present no mouse
            // movement in scrollbar so block text selection in webview
            //LOG_DEBUG( "supress selection of text, not in scrollbars", NULL );
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

#endif // QWEBVIEWSELECTIONSUPPRESSOR_H
