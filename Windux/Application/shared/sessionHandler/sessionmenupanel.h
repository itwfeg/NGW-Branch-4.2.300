/**
 * @file sessionmenupanel.h
 * Menu panel with nagivation options.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONMENUPANEL_H
#define SESSIONMENUPANEL_H

#include "sessiondialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Displays one or more widgets in a dialog fashion.
class SessionMenuPanel : public SessionDialog
{
    Q_OBJECT

    typedef SessionMenuPanel _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] location  location of the panel
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit SessionMenuPanel( DialogLocation location, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~SessionMenuPanel();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve layout alignment.
    /**
     * @return  alignment
     */
    virtual Qt::Alignment layoutAlignment() const {return layoutAlignment_;}

    /// Set layout alignment.
    /**
     * @param[in] value  alignment
     */
    virtual void setLayoutAlignment( Qt::Alignment value ) {layoutAlignment_ = value;}

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add a widget to the panel.
    /**
     * @param[in] widget  widget to add into panel
     */
    virtual void addWidget( QWidget *widget );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority
     */
    virtual int priority() const {return BelowNormal;}

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

private:

    QBoxLayout *layout_;
    Qt::Alignment layoutAlignment_;

    QSize size_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONMENUPANEL_H
