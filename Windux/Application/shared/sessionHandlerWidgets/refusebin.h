/**
 * @file refusebin.h
 * Refuse bin widget for draggable widgets.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef REFUSEBIN_H
#define REFUSEBIN_H

#include <QLabel>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class refuseBinWidget
 * @brief Refuse bin widget for draggable widgets.
 */
class refuseBinWidget : public QLabel
{
    Q_OBJECT

    Q_PROPERTY( QStringList acceptedMimeTypes READ acceptedMimeTypes WRITE setAcceptedMimeTypes )
    Q_PROPERTY( QObjectList acceptedSources READ acceptedSources WRITE setAcceptedSources )
    Q_PROPERTY( QPixmap icon READ icon WRITE setIcon )
    Q_PROPERTY( QStringList rejectedMimeTypes READ rejectedMimeTypes WRITE setRejectedMimeTypes )
    Q_PROPERTY( QObjectList rejectedSources READ rejectedSources WRITE setRejectedSources )

    typedef refuseBinWidget _Myt;
    typedef QLabel _Mybase;

Q_SIGNALS:

    /**
     * Emitted when widget dragged into the refuse bin.
     *
     * @param[in] w  pointer to widget
     */
    void deleteWidget( QWidget *w );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit refuseBinWidget(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~refuseBinWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve accepted mime types (whitelist).
     *
     * @returns  mime types
     */
    virtual QStringList acceptedMimeTypes() const {return acceptedMimeTypes_;}

    /**
     * Retrieve accepted objects (whitelist).
     *
     * @returns  objects
     */
    virtual QObjectList acceptedSources() const {return acceptedSources_;}

    /**
     * Retrieve icon.
     *
     * @returns  pixmap
     */
    virtual QPixmap icon() const {return icon_;}

    /**
     * Retrieve rejected mime types (blacklist).
     *
     * @returns  mime types
     */
    virtual QStringList rejectedMimeTypes() const {return rejectedMimeTypes_;}

    /**
     * Retrieve rejected objects (blacklist).
     *
     * @returns  objects
     */
    virtual QObjectList rejectedSources() const {return rejectedSources_;}

    /**
     * Set accepted mime types (whitelist).
     *
     * @param[in]  mime types
     */
    virtual void setAcceptedMimeTypes( const QStringList& value ) {acceptedMimeTypes_ = value;}

    /**
     * Set accepted objects (whitelist).
     *
     * @param[in]  objects
     */
    virtual void setAcceptedSources( const QObjectList& value ) {acceptedSources_ = value;}

    /**
     * Set icon.
     *
     * @param[in] value  pixmap
     */
    virtual void setIcon( const QPixmap& value );

    /**
     * Set rejected mime types (blacklist).
     *
     * @param[in]  mime types
     */
    virtual void setRejectedMimeTypes( const QStringList& value )  {rejectedMimeTypes_ = value;}

    /**
     * Set rejected objects (blacklist).
     *
     * @param[in]  objects
     */
    virtual void setRejectedSources( const QObjectList& value ) {rejectedSources_ = value;}

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragEnterEvent( QDragEnterEvent *e );

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragMoveEvent( QDragMoveEvent *e );

    /**
     * Drop event.
     *
     * @param[in,out] e  drop event
     */
    virtual void dropEvent( QDropEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

private:

    QPixmap icon_;

    QStringList acceptedMimeTypes_;
    QStringList rejectedMimeTypes_;

    QObjectList acceptedSources_;
    QObjectList rejectedSources_;

    /**
     * Update background palette.
     */
    void updatePaletteBackground();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // REFUSEBIN_H
