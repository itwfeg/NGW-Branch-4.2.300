/**
 * @file currentlyservingwidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CURRENTLYSERVINGWIDGET_H
#define CURRENTLYSERVINGWIDGET_H

#include <QWidget>

class playlistWidget;
class queueStateWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Currently serving screen.
class currentlyServingWidget : public QWidget
{
    Q_OBJECT

    typedef currentlyServingWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    currentlyServingWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~currentlyServingWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve playlist.
    /**
     * @return  playlist
     */
    virtual QString playlist() const;

    /// Set now serving text.
    /**
     * @param[in] value  text to display
     */
    virtual void setNowServingText( const QString &value );

    /// Set playlist.
    /**
     * @param[in] value  playlist
     */
    virtual void setPlaylist( const QString& value );

    /// Set queue size text.
    /**
     * @param[in] value  text to display
     */
    virtual void setQueueSizeText( const QString &value );

    /// Set if showing queue size.
    /**
     * @param[in] value  @c true if showing, @c false otherwise
     */
    virtual void setShowQueueSize( bool value );

    /// Check if showing queue size.
    /**
     * @return  @c true if showing, @c false otherwise
     */
    virtual bool showQueueSize() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

private:

    queueStateWidget *queueState_;
    playlistWidget *adverts_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CURRENTLYSERVINGWIDGET_H
