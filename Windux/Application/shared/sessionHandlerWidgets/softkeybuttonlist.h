/**
 * @file softkeybuttonlist.h
 * List widget of softkey buttons.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SOFTKEYBUTTONLIST_H
#define SOFTKEYBUTTONLIST_H

#include <QListWidget>
#include <QStyledItemDelegate>

/// Hobart namespace.
namespace hobart {

class softkeyButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class softkeyButtonListWidget
 * @brief Softkey button list widget.
 */
class softkeyButtonListWidget : public QListWidget
{
    Q_OBJECT

    Q_PROPERTY( int rowHeight READ rowHeight WRITE setRowHeight )

    typedef softkeyButtonListWidget _Myt;
    typedef QListWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit softkeyButtonListWidget(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~softkeyButtonListWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve row height.
     *
     * @returns  row height
     */
    virtual int rowHeight() const {return rowHeight_;}

    /**
     * Set row height.
     *
     * @param[in] value  row height
     */
    virtual void setRowHeight( int value );

    /**
     * Retrieve softkey button widget from list.
     *
     * @param[in] index  list index
     * @returns  pointer to button or NULL on failure
     */
    virtual softkeyButton *softkeyButtonItem( int index ) const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Add a softkey button item to the list.
     *
     * @param[in] text  softkey button text
     * @param[in] action  softkey action
     */
    virtual void addSoftkeyButtonItem( const QString& text, int action );

    /**
     * Add a softkey button item to the list.
     *
     * @param[in] b  softkey button
     */
    virtual void addSoftkeyButtonItem( softkeyButton *b );

    /**
     * Insert a softkey button item into the list.
     *
     * @param[in] row  location
     * @param[in] text  softkey button text
     * @param[in] action  softkey action
     */
    virtual void insertSoftkeyButtonItem( int row, const QString& text, int action );

    /**
     * Insert a softkey button item into the list.
     *
     * @param[in] row  location
     * @param[in] b  softkey button
     */
    virtual void insertSoftkeyButtonItem( int row, softkeyButton *b );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Set event drop action.
     *
     * @param[in,out] e  event
     * @returns  drop action
     */
    template <class T>
    Qt::DropAction setDropAction( T *e );

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragEnterEvent( QDragEnterEvent *e );

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragMoveEvent( QDragMoveEvent *e );

    /**
     * Drop event.
     *
     * @param[in,out] e  drop event
     */
    virtual void dropEvent( QDropEvent *e );

private Q_SLOTS:

    /**
     * Slot for button destroyed.
     */
    void onSoftkeyButtonDestroyed( QObject *obj );

private:

    int rowHeight_;

    /**
     * Create a new softkey button.
     */
    softkeyButton *createSoftkeyButton( const QString& text, int action );

    /**
     * Create a new softkey button.
     */
    softkeyButton *createSoftkeyButton( const QByteArray& a );

    /**
     * Groom softkey button.
     */
    void groomSoftkeyButton( softkeyButton *b ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @class softkeyButtonListItemDelegate
 * @brief Softkey button list item deletage.
 */
class softkeyButtonListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    typedef softkeyButtonListItemDelegate _Myt;
    typedef QStyledItemDelegate _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit softkeyButtonListItemDelegate(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~softkeyButtonListItemDelegate();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set fixed height.
     *
     * @param[in] value  height
     */
    virtual void setFixedHeight( int value ) {fixedHeight_ = value;}

    /**
     * Set fixed width.
     *
     * @param[in] value  width
     */
    virtual void setFixedWidth( int value ) {fixedWidth_ = value;}

    /**
     * Size hint.
     *
     * @param[in] option  options
     * @param[in] index  index
     * @returns  size
     */
    virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex& index ) const;

private:

    int fixedHeight_;
    int fixedWidth_;

    softkeyButtonListWidget *list_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SOFTKEYBUTTONLIST_H
