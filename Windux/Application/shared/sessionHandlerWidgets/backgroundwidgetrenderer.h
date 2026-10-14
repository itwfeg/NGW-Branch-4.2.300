/**
 * @file backgroundwidgetrenderer.h
 * Background rendering for background widgets.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef BACKGROUNDWIDGETRENDERER_H
#define BACKGROUNDWIDGETRENDERER_H

#include <QPixmap>

/// Hobart namespace.
namespace hobart {

class backgroundWidgetBase;
class widgetRendererAppearanceBase;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget states.
enum rendererState
{
    Normal,                                     ///< Widget may receive focus, but does not have it currently.
    ReadOnly,                                   ///< Widget is read only.
    Focused                                     ///< Widget currently has focus.
};

/// Abstract renderer for background widget types.
class backgroundWidgetRenderer
{
    typedef backgroundWidgetRenderer _Myt;

    typedef backgroundWidgetBase _WidgetType;
    typedef widgetRendererAppearanceBase _Appearance;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] tr  widget to render
     * @param[in] a  appearance
     */
    backgroundWidgetRenderer( _WidgetType *tr, _Appearance *a );

    /// Destructor.
    virtual ~backgroundWidgetRenderer();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Draw control background.
    /**
     * @param[in] size  widget size
     * @param[in] state  control state
     * @return  pixmap of widget background
     */
    virtual QPixmap drawControl( const QSize &size, rendererState type ) const;

    /// Draw drop shadow.
    /**
     * @param[in] size  widget size
     * @return  pixmap of drop shadow
     */
    virtual QPixmap drawDropShadow( const QSize &size ) const;

protected:

    backgroundWidgetBase *traits_;                  ///< Widget to render.
    widgetRendererAppearanceBase *appearance_;      ///< Appearance.

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // BACKGROUNDWIDGETRENDERER_H
