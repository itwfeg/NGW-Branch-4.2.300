#ifndef QTAPPPRIVATE_H
#define QTAPPPRIVATE_H

#include <QMutex>
#include <QObject>

class QCoreApplication;
class QThread;

/// QT App for Private Usage.
class qtAppPrivate : public QObject
{
    Q_OBJECT

    typedef qtAppPrivate _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

private slots:

    /// Event loop thread.
    void eventLoopThread();

private:

    static QMutex instanceMutex_;
    static _Myt *instance_;

    QCoreApplication *app_;
    QThread *appThread_;

    // Constructor.
    qtAppPrivate();

    // Destructor.
    ~qtAppPrivate();

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QTAPPPRIVATE_H
