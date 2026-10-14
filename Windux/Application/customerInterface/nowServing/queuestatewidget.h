/**
 * @file queuestatewidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QUEUESTATEWIDGET_H
#define QUEUESTATEWIDGET_H

#include <QWidget>

class serverComm;

class QLabel;
class QString;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Queue state widget.
class queueStateWidget : public QWidget
{
    Q_OBJECT

    typedef queueStateWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    queueStateWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~queueStateWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set now serving text.
    /**
     * @param[in] value  text to display
     */
    virtual void setNowServingText( const QString &value );

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
    virtual bool showQueueSize() const {return showQueueSize_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot to update currently serving.
    void onUpdateCurrentlyServing( int currentlyServing, int queueSize );

    /// Slot for updating date/time.
    void onUpdateCurrentTime();

private:

    QTimer *updateCurrentTime_;

    bool showQueueSize_;

    QLabel *nowServingLabel_;
    QLabel *nowServing_;
    QLabel *queueSizeLabel_;
    QLabel *queueSize_;
    QLabel *dateTime_;

    serverComm *server_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Generate transparent background.
    static QPixmap transparentBackground( const QSize& size );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QUEUESTATEWIDGET_H
