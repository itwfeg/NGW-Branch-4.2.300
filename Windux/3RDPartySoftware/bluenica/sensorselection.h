/**
 * @file sensorselection.h
 * Widget for sensor selection.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SENSORSELECTION
#define SENSORSELECTION

#include "dbquery.h"

#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for sensor selection.
class sensorSelectionWidget : public QWidget
{
    Q_OBJECT

    typedef sensorSelectionWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when item double clicked.
    /**
     * @param[in] id  sensor id
     */
    void itemDoubleClicked( unsigned int id );

    /// Emitted when selection changes.
    void itemSelectionChanged();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    sensorSelectionWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~sensorSelectionWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve selected items.
    /**
     * @return  list of selected items
     */
    virtual QList<unsigned int> selectedItems() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear selection.
    virtual void clearSelection();

    /// Refresh sensor list.
    /**
     * @param[in] age  age filter
     */
    virtual void refresh( unsigned int age = 0 );

    /// Translate text.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for item double clicked.
    void itemDoubleClicked( QTreeWidgetItem *item, int column );

private:

    dbQuery db_;

    bool useCelsius_;

    QTreeWidget *table_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set table headers.
    void setTableHeaders();

    /// Convert temperature.
    qreal convertTemp( qreal temp ) const;

    /// Format temperature.
    QString formatDisplayTemp( qreal temp ) const;

    /// Retrieve formatted sensor age.
    static QString sensorDataAge( qint64 diff );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SENSORSELECTION

