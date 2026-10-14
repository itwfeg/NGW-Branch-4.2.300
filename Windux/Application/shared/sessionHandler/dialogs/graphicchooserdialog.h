/**
 * @file graphicchooserdialog.h
 * Dialog for choosing a graphic.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef GRAPHICCHOOSERDIALOG_H
#define GRAPHICCHOOSERDIALOG_H

#include "listdialog.h"

class graphic;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for choosing a graphic.
class GraphicChooserDialog : public ListDialog
{
    Q_OBJECT

    typedef GraphicChooserDialog _Myt;
    typedef ListDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit GraphicChooserDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~GraphicChooserDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve selected graphic.
    /**
     * @return  graphic name
     */
    virtual QString selectedGraphic() const;

    /// Set selected graphic.
    /**
     * @param[in] value  graphic name
     */
    virtual void setSelectedGraphic( const QString& value );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add graphic item to list.
    /**
     * @param[in] item  graphic item
     */
    virtual void addItemToList( const graphic& item );

    /// Callback for list item clicked.
    /**
     * @param[in] item  item clicked
     * @param[in] cookie  cookie of item clicked
     */
    virtual void onListItemClicked( QListWidgetItem *item, const QVariant& cookie );

private:

    QString selected_;

    static const QString ext_;

    static const QSize iconSize_;
    static const QSize iconSizeList_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GRAPHICCHOOSERDIALOG_H
