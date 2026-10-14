/**
 * @file labeledlabelviewer.h
 * Label viewer widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDLABELVIEWER_H
#define LABELEDLABELVIEWER_H

#include "labeledwidget.h"

#include "labelpreviewwidget.h"

#include <labelrenderer.h>

#include <QTemporaryFile>

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Label viewer widget with custom background, highlighting, and label.
class labeledLabelViewer : public labeledWidget<labelPreviewWidget>
{
    Q_OBJECT

    typedef labeledLabelViewer _Myt;
    typedef labeledWidget<labelPreviewWidget> _Mybase;

Q_SIGNALS:

    /// This signal is emitted when a new label is loaded.
    void labelChanged();

    /// This signal is emitted when a label form is selected.
    void labelFormSelected();

    /// Signal for when the print button is clicked.
    void printButtonClicked();

    /// Signal for when the zoom button is clicked.
    void zoomButtonClicked();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledLabelViewer(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /// Destructor.
    virtual ~labeledLabelViewer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve label currently loaded into widget.
    /**
     * @return  pointer to label
     */
    virtual labelPointer label() const;

    /// Retrieve visibility of the print button.
    /**
     * @return  @c true if button is visible, @c false otherwise
     */
    virtual bool printButtonVisible() const;

    /// Retrieve if renderer is read only.
    /**
     * @return  @c true if renderer is read only, @c false otherwise
     */
    virtual bool readOnly() const;

    /// Retrieve currently selected form.
    /**
     * @return  pointer to form selected
     */
    virtual labelForm *selected() const;

    /// Set contents margins.
    /**
     * @param[in] value  margins
     */
    virtual void setContentsMargins( const QMargins& value );

    /// Load new label into widget.
    /**
     * @param[in] value  label to load
     */
    virtual void setLabel( const ::label& value );

    /// Load new label into widget for zoomed operation.
    /**
     * @param[in] value  label to load
     */
    virtual void setLabelZoomed( const ::label& value );

    /// Set nutrifacts id.
    /**
     * Some labels embed nutrifacts data within them.  This id value contains which nutrifacts
     * to use from the nutrifacts store.
     *
     * @param[in] value  nutrifacts id
     */
    virtual void setNutrifactsId( int value );

    /// Set visibility of the print button.
    /**
     * @param[in] value  @c true to make button visible, @c false otherwise
     */
    virtual void setPrintButtonVisible( bool value );

    /// Set renderer read only property.
    /**
     * @param[in] value  @c true to set renderer read only, @c false otherwise
     */
    virtual void setReadOnly( bool value );

    /// Set data store.
    /**
     * @param[in] value  pointer to data store
     */
    virtual void setStore( const dataStoreBase *value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset widget label.
    /**
     * This method will unload any label currently loaded.
     */
    virtual void resetLabel();

    /// Save rendered image to file.
    /**
     * @return  image filename on success, or empty string on failure
     */
    virtual QString saveRenderedImage();

    /// Save zoomed rendered image to file.
    /**
     * @return  image filename on success, or empty string on failure
     */
    virtual QString saveZoomedRenderedImage();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[in,out] event  event
     * @return  @c true to filter, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private:

    pushButton *printButton_;

    LabelRenderer *renderer_;
    LabelRenderer *rendererZoomed_;

    QTemporaryFile file_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDLABELVIEWER_H
