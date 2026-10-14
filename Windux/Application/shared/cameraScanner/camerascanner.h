#ifndef CAMERASCANNER_H
#define CAMERASCANNER_H

#include "barcodetype.h"

#include <QMutex>
#include <QWidget>

class camera;
class decoder;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Camera barcode scanner widget.
class cameraScannerWidget : public QWidget
{
    Q_OBJECT

    typedef cameraScannerWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal for when barcode(s) detected.
    /**
     * @param[in] barcodes  list of barcodes
     */
    void decodedBarcodes( const barcodeList& barcodes );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] c  camera to use
     * @param[in] parent  parent widget
     */
    cameraScannerWidget( camera *c, QWidget *parent = 0 );

    /// Destructor.
    virtual ~cameraScannerWidget();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] e  event
     * @return  @c true to accept event, @c false otherwise
     */
    virtual bool event( QEvent *e );

private Q_SLOTS:

    /// Capture current camera image.
    void capture();

    /// Process camera image.
    void captureImageSaved( int id, const QString& fileName );

private:

    camera *camera_;
    decoder *decoder_;

    QTimer *autoFocusCameraTimer_;
    QTimer *snapshotTimer_;

    QMutex processing_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Process image.
    void processImage( const QString& fileName );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
