/**
 * @file labeledgraphicedit.h
 * Graphic name edit widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDGRAPHICEDIT_H
#define LABELEDGRAPHICEDIT_H

#include "labeledlineedit.h"

class SessionMainWindow;

class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledGraphicEdit
 * @brief Graphic name edit widget.
 */
class labeledGraphicEdit : public labeledLineEdit
{
    Q_OBJECT

    typedef labeledGraphicEdit _Myt;
    typedef labeledLineEdit _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledGraphicEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledGraphicEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if widget has focus.
     *
     * @returns  @c true if focused, @c false otherwise
     */
    virtual bool hasFocus() const {return (( dialogOpen_ ) || ( _Mybase::hasFocus() ));}

protected:

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:

    /**
     * Slot for selection changed.
     */
    void onSelectionChanged();

    /**
     * Slot for timer timeout.
     */
    void onTimeout();

private:

    bool dialogOpen_;

    QTimer *timer_;

    SessionMainWindow *mainWindow_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDGRAPHICEDIT_H
