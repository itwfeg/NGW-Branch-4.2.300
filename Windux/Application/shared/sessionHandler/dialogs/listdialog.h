/**
 * @file listdialog.h
 * Generic list dialog for choosing stuff.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include "../sessiondialog.h"

#include <QVariant>

class QHBoxLayout;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Generic list dialog for choosing stuff.
/**
 * You will find below that this class contains a lot of layouts. This is to offer those who derive
 * from this class the most flexibility in where they put their widgets. Below is a diagram of
 * how the layouts are nested.
 *
 * @code
 * +-[ screenLayout_ (H) ]------------------------------------------------------------------------+
 * |+-[ screenLayoutInner_ (V) ]-----------------------------------------------------------------+|
 * ||                                                                                            ||
 * || +-[ topLayout_ (H) ]---------------------------------------------------------------------+ ||
 * || |                                                                                        | ||
 * || +----------------------------------------------------------------------------------------+ ||
 * ||                                                                                            ||
 * || list_                                                                                      ||
 * ||                                                                                            ||
 * || +-[ bottomLayout_ (H) ]------------------------------------------------------------------+ ||
 * || |                                                                                        | ||
 * || +----------------------------------------------------------------------------------------+ ||
 * ||                                                                                            ||
 * |+--------------------------------------------------------------------------------------------+|
 * +----------------------------------------------------------------------------------------------+
 */
class ListDialog : public SessionDialog
{
    Q_OBJECT

    Q_PROPERTY( QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize )

    typedef ListDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~ListDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set icon thumbnail size.
    /**
     * @param[in] value  thumbnail size
     */
    virtual void setThumbnailSize( const QSize& value ) {thumbnailSize_ = value;}

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    /// Retrieve icon thumbnail size.
    /**
     * @return  thumbnail size
     */
    virtual QSize thumbnailSize() const;

protected:

    QHBoxLayout *screenLayout_;
    QVBoxLayout *screenLayoutInner_;

    QHBoxLayout *topLayout_;
    QHBoxLayout *bottomLayout_;

    QListWidget *list_;

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit ListDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add item to list.
    /**
     * Convenience function for adding an item to the list. Cookie is added to the list item
     * user role.
     *
     * @param[in] icon  object icon
     * @param[in] text  object text
     * @param[in] cookie  cookie stored within object
     */
    virtual void addItemToList( const QImage& icon, const QString& text, const QVariant& cookie = QVariant() );

    /// Add item to list.
    /**
     * Convenience function for adding an item to the list. Cookie is added to the list item
     * user role.
     *
     * @param[in] icon  object icon
     * @param[in] text  object text
     * @param[in] cookie  cookie stored within object
     */
    virtual void addItemToList( const QPixmap& icon, const QString& text, const QVariant& cookie = QVariant() );

    /// Add item to list.
    /**
     * Convenience function for adding an item to the list. Cookie is added to the list item
     * user role.
     *
     * @param[in] icon  object icon
     * @param[in] text  object text
     * @param[in] cookie  cookie stored within object
     */
    virtual void addItemToList( const QString& icon, const QString& text, const QVariant& cookie = QVariant() );

    /// Add item to list.
    /**
     * Convenience function for adding an item to the list. Cookie is added to the list item
     * user role.
     *
     * @param[in] text  object text
     * @param[in] cookie  cookie stored within object
     */
    virtual void addItemToList( const QString& text, const QVariant& cookie = QVariant() );

    /// Callback for list item clicked.
    /**
     * @param[in] item  item clicked
     * @param[in] cookie  cookie of item clicked
     */
    virtual void onListItemClicked( QListWidgetItem *item, const QVariant& cookie );

private Q_SLOTS:

    /// Slot for list item clicked.
    void listItemClicked( QListWidgetItem *item );

private:

    QSize thumbnailSize_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LISTDIALOG_H
