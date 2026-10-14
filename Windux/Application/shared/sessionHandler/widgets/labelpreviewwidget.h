/**
 * @file labelpreviewwidget.h
 * Widget for previewing labels.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELPREVIEWWIDGET_H
#define LABELPREVIEWWIDGET_H

#include <QWidget>

class LabelRenderer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelPreviewWidget
 * @brief Widget for previewing labels.
 */
class labelPreviewWidget : public QWidget
{
    Q_OBJECT

    typedef labelPreviewWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// This signal is emitted when widget is clicked.
    void clicked();

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit labelPreviewWidget(
        QWidget *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] renderer  pointer to @c LabelRenderer object
     * @param[in] parent  parent widget
     */
    explicit labelPreviewWidget(
        LabelRenderer *renderer,
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~labelPreviewWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if widget is read only.
     *
     * @returns  @c true if read only, @c false otherwise
     */
    virtual bool isReadOnly() const {return true;}

    /**
     * Retrieve label renderer.
     *
     * @returns  pointer to @c LabelRenderer object
     */
    virtual LabelRenderer *renderer() const {return renderer_;}

    /**
     * Set if control is read only.
     *
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value );

    /**
     * Set label renderer.
     *
     * @param[in] value  pointer to @c LabelRenderer object
     */
    virtual void setRenderer( LabelRenderer *value );

public Q_SLOTS:

    /**
     * Slot to render the label.
     */
    virtual void render();

protected:

    /**
     * Mouse release event.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

private:

    LabelRenderer *renderer_;

    QImage label_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELPREVIEWWIDGET_H
