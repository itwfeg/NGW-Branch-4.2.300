/**
 * @file labelviewerwidget.h
 * Widget for viewing a label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELVIEWERWIDGET_H
#define LABELVIEWERWIDGET_H

#include "labelrenderer.h"

#include <QScrollArea>

///////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @class LabelViewerWidget
 * @brief Widget for rendering and viewing labels.
 *
 * This widget is a container class for renderer widget.  This class offers scrolling capability
 * for when the label exceeds the boundry of the container.
 */
class LabelViewerWidget : public QScrollArea
{
    Q_OBJECT

    typedef LabelViewerWidget _Myt;
    typedef QScrollArea _Mybase;

Q_SIGNALS:

    /**
     * This signal is emitted when a new label is loaded.
     */
    void labelChanged();

    /**
     * This signal is emitted when a label form is selected.
     */
    void labelFormSelected();

public Q_SLOTS:

    /**
     * Updates the widget unless updates are disabled or the widget is hidden.
     *
     * This function does not cause an immediate repaint; instead it schedules a paint event for
     * processing when Qt returns to the main event loop. This permits Qt to optimize for more
     * speed and less flicker than a call to repaint() does.
     */
    virtual void update()
    {
        // update the renderer
        renderer_.update();

        // update me
        _Mybase::update();
    }

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] readOnly  @c true to set renderer read only, @c false otherwise
     * @param[in] parent  parent widget
     */
    LabelViewerWidget( bool readOnly = true, QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~LabelViewerWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if nutrifacts editing is allowed.
     *
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool isNutrifactsEditingAllowed() const {return renderer_.isNutrifactsEditingAllowed();}

    /**
     * Set if nutrifacts editing is allowed.
     *
     * @param[in] value  @c true if allowed, @c false otherwise
     */
    virtual void setNutrifactsEditingAllowed( bool value ) {renderer_.setNutrifactsEditingAllowed( value );}

    /**
     * Check if label has nutrifact forms.
     *
     * @returns  @c true if nutrifact forms exist, @c false otherwise
     */
    virtual bool hasNutrifacts() const {return renderer_.hasNutrifacts();}

    /**
     * Retrieve label currently loaded into widget.
     *
     * @returns  pointer to label
     */
    virtual labelPointer label() const {return renderer_.label();}

    /**
     * Load new label into widget.
     *
     * @param[in] value  label to load
     */
    virtual void setLabel( const ::label& value ) {renderer_.setLabel( value );}

    /**
     * Reset widget label.
     *
     * This property will unload any label currently loaded.
     */
    virtual void resetLabel() {renderer_.resetLabel();}

    /**
     * Retrieve currently selected form.
     *
     * @returns  pointer to form selected
     */
    virtual labelForm *selected() const {return renderer_.selected();}

    /**
     * Retrieve if renderer is read only.
     *
     * @returns  @c true if renderer is read only, @c false otherwise
     */
    virtual bool readOnly() const {return renderer_.readOnly();}

    /**
     * Set renderer read only property.
     *
     * @param[in] value  @c true to set renderer read only, @c false otherwise
     */
    virtual void setReadOnly( bool value ) {renderer_.setReadOnly( value );}

    /**
     * Set data store.
     *
     * @param[in] value  pointer to data store
     */
    virtual void setStore( const dataStoreBase *value ) {renderer_.setStore( value );}

    /**
     * Set nutrifacts id.
     *
     * Some labels embed nutrifacts data within them.  This id value contains which nutrifacts
     * to use from the nutrifacts store.
     *
     * @param[in] value  nutrifacts id
     */
    virtual void setNutrifactsId( int value ) {renderer_.setNutrifactsId( value );}

private Q_SLOTS:

    /**
     * Slot for when label changes.
     */
    void onLabelChanged();

    /**
     * Slot for when label form is selected.
     */
    void onLabelFormSelected();

private:

    LabelRenderer renderer_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELVIEWERWIDGET_H
