#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>

class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Camera interface widget.
class camera : public QWidget
{
    Q_OBJECT

    typedef camera _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal for image saved.
    /**
     * @param[in] id  image id
     * @param[in] fileName  image filename
     */
    void captureImageSaved( int id, const QString& fileName );

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~camera();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if auto focus is supported.
    /**
     * @return  @c true if supported, @c false otherwise
     */
    virtual bool isAutoFocusSupported() const {return false;}

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Auto focus camera image.
    virtual void autoFocus();

    /// Capture current camera image.
    /**
     * Upon successfull capture the captureImageSaved() signal will be invoked.
     *
     * @param[in] fileName  capture filename
     * @return  image id
     */
    virtual int capture( const QString& fileName = QString() );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    camera( QWidget *parent = 0 );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Capture current camera image.
    /**
     * @param[in] fileName  capture filename
     * @return  image id
     */
    virtual int captureImage( const QString& fileName ) = 0;

private:

    int id_;
    int serial_;

    static int cameraId_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CAMERA_H
