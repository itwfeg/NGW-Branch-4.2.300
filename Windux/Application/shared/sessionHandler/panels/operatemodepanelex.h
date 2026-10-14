/**
 * @file operatemodepanelex.h
 * Extended operate mode panel.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OPERATEMODEPANELEX_H
#define OPERATEMODEPANELEX_H

#include "panelwidget.h"

class QHBoxLayout;
class QLabel;
class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Extended operate mode panel.
/**
 * This class is a base class for the extended panels in operate mode.
 *
 * You will find below that this class contains a lot of layouts. This is to offer those who derive
 * from this class the most flexibility in where they put their widgets. Below is a diagram of
 * how the layouts are nested.
 *
 * @code
 * +-[ screenLayout_ (V) ]------------------------------------------------------------------------+
 * |+-[ screenLayoutInner_ (H) ]-----------------------------------------------------------------+|
 * ||+-[ widgetsLayout_ (V) ]-------------------------------------------------------------------+||
 * |||                                                                                          |||
 * ||| +-[ topLayout_ (H) ]-------------------------------------------------------------------+ |||
 * ||| |+-[ topLayoutInner_ (V) ]-+                                                           | |||
 * ||| ||                         |                                                           | |||
 * ||| || title_                  |                          STRETCH                          | |||
 * ||| ||                         |                                                           | |||
 * ||| |+-------------------------+                                                           | |||
 * ||| +--------------------------------------------------------------------------------------+ |||
 * |||                                                                                          |||
 * ||| STRETCH                                                                                  |||
 * |||                                                                                          |||
 * ||| +-[ bottomLayout_ (H) ]----------------------------------------------------------------+ |||
 * ||| |+-[ bottomLayoutInner_ (V) ]---------------------------------------------------------+| |||
 * ||| ||                                                                                    || |||
 * ||| || (empty)                                                                            || |||
 * ||| ||                                                                                    || |||
 * ||| |+------------------------------------------------------------------------------------+| |||
 * ||| +--------------------------------------------------------------------------------------+ |||
 * |||                                                                                          |||
 * ||+------------------------------------------------------------------------------------------+||
 * |+--------------------------------------------------------------------------------------------+|
 * +----------------------------------------------------------------------------------------------+
 */
class OperateModePanelEx : public PanelWidget
{
    Q_OBJECT

    Q_PROPERTY( QString title READ title WRITE setTitle )

    typedef OperateModePanelEx _Myt;
    typedef PanelWidget _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~OperateModePanelEx();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set panel title.
    /**
     * @param[in] value  panel title
     */
    virtual void setTitle( const QString& value );

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    /// Retrieve title of panel.
    /**
     * @return  panel title
     */
    virtual QString title() const;

protected:

    QHBoxLayout *screenLayoutInner_;                ///< Screen layout, contains @p widgetsLayout_

    QVBoxLayout *widgetsLayout_;                    ///< Contains top and bottom layouts

    QHBoxLayout *topLayout_;                        ///< Top layout, contains @p topLayoutInner_
    QVBoxLayout *topLayoutInner_;                   ///< Contains @p title_

    QHBoxLayout *bottomLayout_;                     ///< Bottom layout, contains @p bottomLayoutInner_
    QVBoxLayout *bottomLayoutInner_;                ///< Contains nothing (empty)

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit OperateModePanelEx( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer traits.
    /**
     * @return  traits object
     */
    virtual hobart::backgroundWidgetBase *rendererTraits() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

private:

    QVBoxLayout *screenLayout_;

    QLabel *title_;

    static const bool convertUppercase_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OPERATEMODEPANELEX_H
