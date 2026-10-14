/**
 * @file labelrenderer.h
 * Widget for rendering a label
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELRENDERER_H
#define LABELRENDERER_H

#include <label.h>
#include <lockingdataobject.h>
#include <datastore.h>

#include <QMap>
#include <QSharedPointer>
#include <QWidget>

class QLineEdit;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef labelGuard
 * @brief Guarded label object.
 */
typedef lockingDataObject<label> labelGuard;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef labelPointer
 * @brief Shared pointer to lockable label.
 *
 * This pointer object allows shared access to the label resource.
 */
typedef QSharedPointer< labelGuard > labelPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class LabelRenderer
 * @brief Widget for rendering a label.
 */
class LabelRenderer : public QWidget
{
    Q_OBJECT

    typedef LabelRenderer _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * This signal is emitted when a new label is loaded.
     */
    void labelChanged();

    /**
     * This signal is emitted when a label form is selected.
     */
    void labelFormSelected();

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] readOnly  @c true to set renderer read only, @c false otherwise
     * @param[in] parent  parent widget
     */
    LabelRenderer( bool readOnly = true, QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~LabelRenderer();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if nutrifacts editing is allowed.
     *
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool isNutrifactsEditingAllowed() const {return allowNutrifactsEditing_;}

    /**
     * Set if nutrifacts editing is allowed.
     *
     * @param[in] value  @c true if allowed, @c false otherwise
     */
    virtual void setNutrifactsEditingAllowed( bool value ) {allowNutrifactsEditing_ = value;}

    /**
     * Check if label has nutrifact forms.
     *
     * @returns  @c true if nutrifact forms exist, @c false otherwise
     */
    virtual bool hasNutrifacts() const;

    /**
     * Retrieve label currently loaded into widget.
     *
     * @returns  pointer to label
     */
    virtual labelPointer label() const {return label_;}

    /**
     * Load new label into widget.
     *
     * @param[in] value  label to load
     */
    virtual void setLabel( const ::label& value );

    /**
     * Reset widget label.
     *
     * This property will unload any label currently loaded.
     */
    virtual void resetLabel();

    /**
     * Retrieve currently selected form.
     *
     * @returns  pointer to form selected
     */
    virtual labelForm *selected() const {return selected_;}

    /**
     * Retrieve if renderer is read only.
     *
     * @returns  @c true if renderer is read only, @c false otherwise
     */
    virtual bool readOnly() const {return readOnly_;}

    /**
     * Set renderer read only property.
     *
     * @param[in] value  @c true to set renderer read only, @c false otherwise
     */
    virtual void setReadOnly( bool value );

    /**
     * Retrieve data store.
     *
     * @returns  pointer to data store
     */
    virtual const dataStoreBase *store() const {return dataStore_;}

    /**
     * Set data store.
     *
     * @param[in] value  pointer to data store
     */
    virtual void setStore( const dataStoreBase *value ) {dataStore_ = value;}

    /**
     * Retrieve nutrifacts id.
     *
     * @returns  nutrifacts id
     */
    virtual int nutrifactsId() const {return nutrifactsId_;}

    /**
     * Set nutrifacts id.
     *
     * Some labels embed nutrifacts data within them.  This id value contains which nutrifacts
     * to use from the nutrifacts store.
     *
     * @param[in] value  nutrifacts id
     */
    virtual void setNutrifactsId( int value ) {nutrifactsId_ = value;}

    /**
     * Check if preview mode.
     *
     * @returns  @c true if preview mode, @c false otherwise
     */
    virtual bool isPreviewMode() const {return previewMode_;}

    /**
     * Set if preview mode.
     *
     * @param[in] value  @c true if preview mode, @c false otherwise
     */
    virtual void setPreviewMode( bool value ) {previewMode_ = value;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Render label to a @c QImage object.
     *
     * @returns  rendered label
     */
    virtual QImage toImage() const;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Event handler for painting the widget.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    /**
     * Event handler for mouse press.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /**
     * Event handler for mouse moving.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

    /**
     * Event handler for a drag item entering the widget.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragEnterEvent( QDragEnterEvent *e );

    /**
     * Event handler for dropping an item onto the widget.
     *
     * @param[in,out] e  drop event
     */
    virtual void dropEvent( QDropEvent *e );

private Q_SLOTS:

    /**
     * Slot for text edited.
     */
    void onNutrifactTextEdited( QWidget *w );

private:

    QPoint startPos_;

    labelPointer label_;
    labelForm *selected_;

    bool readOnly_;
    bool allowNutrifactsEditing_;
    bool previewMode_;

    // data store for rendering
    const dataStoreBase *dataStore_;

    // which nutrifacts object to use
    int nutrifactsId_;

    typedef QPair<int, QString> columnKeyPair;
    typedef QMap<QLineEdit*, columnKeyPair > nutrifactLabelFormMap;

    QSignalMapper *nutrifactLabelFormSignalMapper_;
    nutrifactLabelFormMap nutrifactLabelForms_;

    /**
     * Render label using painter.
     */
    void render( QPainter *painter ) const;

    /**
     * Create nutrifact label forms.
     */
    void allocNutrifactLabelForms();

    /**
     * Destroy nutrifact label forms.
     */
    void freeNutrifactLabelForms();

    /**
     * Check if form is nutrifact form.
     */
    bool isNutrifactLabelForm( const labelForm& form, int& column, QString &key, QString& value ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELRENDERER_H
