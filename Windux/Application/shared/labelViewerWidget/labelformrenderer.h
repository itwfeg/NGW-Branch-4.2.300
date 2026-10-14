/**
 * @file labelformrenderer.h
 * Renderer class for @c labelForm objects
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELFORMRENDERER_H
#define LABELFORMRENDERER_H

#include <labelform.h>

#include <QPen>
#include <QBrush>

class LabelRenderer;
class QPainter;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelFormRenderer
 * @brief Class for renderering @c labelForm objects.
 *
 * Render's differenty depending on form type.
 */
class labelFormRenderer
{
    typedef labelFormRenderer _Myt;

public:

    /**
     * Renderer mode.
     */
    enum RendererMode
    {
        NORMAL,             ///< Render normally.
        PREVIEW             ///< Render in preview mode (i.e. no boxes, etc.)
    };

    /**
     * Constructor.
     *
     * @param[in] labelRenderer  pointer to @c labelRenderer object
     * @param[in] form  pointer to @c labelForm object
     */
    labelFormRenderer( const LabelRenderer *labelRenderer, const labelForm *form );

    /**
     * Destructor.
     */
    virtual ~labelFormRenderer();

    /**
     * Retrieve renderer mode.
     *
     * @returns  mode
     */
    virtual RendererMode mode() const {return mode_;}

    /**
     * Set renderer mode.
     *
     * @param[in] value  mode
     */
    virtual void setMode( RendererMode value ) {mode_ = value;}

    /**
     * Paint the @c labelForm object.
     *
     * @param[in,out] painter  painter object to render with
     * @param[in] selected  set to @c true if form is selected, @c false otherwise
     */
    virtual void paint( QPainter *painter, bool selected = false );

protected:

    /**
     * Pointer to @c LabelRenderer object (contains data stores).
     */
    const LabelRenderer *labelRenderer_;

    /**
     * Pointer to @c labelForm object which is being rendered.
     */
    const labelForm *form_;

private:

    enum RendererType
    {
        DEFAULT,
        TEXT,
        LINE,
        GRAPHIC
    };

    RendererMode mode_;

    Qt::PenStyle rectPenStyle_;

    QString text_;
    int textFlags_;

    QImage image_;

    // not implemented
    labelFormRenderer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /**
     * Paints the @c labelForm object by label form type.
     */
    template <RendererType _Type>
    void paintImpl( QPainter *painter ) const;

    static QColor formColor_;
    static QColor selectedFormColor_;
    static QColor previewFormColor_;

    static QColor backgroundColor_;
    static QColor previewBackgroundColor_;

    /**
     * Get pen for painter.
     */
    static QPen defaultPainterPen( RendererMode mode, bool selected ) {
        if ( PREVIEW == mode )
            return QPen( previewFormColor_ );
        return QPen( selected ? selectedFormColor_ : formColor_ );
    }

    /**
     * Get brush for painter.
     */
    static QBrush defaultPainterBrush( RendererMode mode ) {
        if ( PREVIEW == mode )
            return QBrush( previewBackgroundColor_ );
        return QBrush( backgroundColor_ );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELFORMRENDERER_H
