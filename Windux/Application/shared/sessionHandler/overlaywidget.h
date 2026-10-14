/**
 * @file overlaywidget.h
 * Overlay widget class.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class OverlayWidget
 * @brief Overlay widget to handle painting.
 */
class OverlayWidget : public QWidget
{
    typedef OverlayWidget _Myt;
    typedef QWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit OverlayWidget( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~OverlayWidget();

protected:

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OVERLAYWIDGET_H
