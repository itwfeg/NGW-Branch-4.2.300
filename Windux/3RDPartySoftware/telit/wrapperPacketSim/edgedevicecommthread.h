#ifndef EDGEDEVICECOMMTHREAD_H
#define EDGEDEVICECOMMTHREAD_H

#include <QThread>
#include <QTimer>

class DiagnosticsEdgeDevice;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Comm thread for edge device.
class edgeDeviceCommThread : public QThread
{
    Q_OBJECT

    typedef edgeDeviceCommThread _Myt;
    typedef QThread _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] edgeDevice  edge device to monitor
     * @param[in] parent  parent object
     */
    edgeDeviceCommThread( DiagnosticsEdgeDevice *edgeDevice, QObject *parent = 0 );

    /// Destructor.
    virtual ~edgeDeviceCommThread();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Exit thread.
    /**
     * @param[in] returnCode  thread return code
     */
    virtual void exit( int returnCode = 0 );

public Q_SLOTS:

    /// Quit thread.
    virtual void quit();

    /// Start thread.
    /**
     * @param[in] priority  thread priority
     */
    virtual void start( Priority priority = InheritPriority );

protected:

    bool exitThread_;                               ///< Exit thread flag.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    virtual void run();

private Q_SLOTS:

    /// Slot for timer timeout.
    void onTimeout();

private:

    unsigned int cycles_;

    DiagnosticsEdgeDevice *edgeDevice_;

    QTimer *updateCycles_;
    QTimer *sendStatusCheck_;

};
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EDGEDEVICECOMMTHREAD_H
