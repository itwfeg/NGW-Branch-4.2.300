/**
 * @file dcadialog.h
 * Dialog base class that appears on top of the operator dynamic content area.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef DCADIALOG_H
#define DCADIALOG_H

#include "../sessiondialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog base class that appears on top of the operator dynamic content area.
class DynamicContentAreaDialog : public SessionDialog
{
    Q_OBJECT

    typedef DynamicContentAreaDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~DynamicContentAreaDialog();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit DynamicContentAreaDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if dialog can be shown in dynamic content area.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowDynamicContentArea() const {return true;}

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DCADIALOG_H
