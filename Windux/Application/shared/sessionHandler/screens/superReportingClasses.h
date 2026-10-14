/**
 * @file superReportingClasses.h
 * Reporting classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SUPERREPORTINGCLASSES_H
#define SUPERREPORTINGCLASSES_H

#include "screenwidget.h"

#include <QItemDelegate>

class QLabel;
class QPushButton;
class QSignalMapper;
class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class superReportingClassesItemDelegate
 * @brief Item delegate for class entry.
 */
class superReportingClassesItemDelegate : public QItemDelegate
{
    Q_OBJECT

    typedef superReportingClassesItemDelegate _Myt;
    typedef QItemDelegate _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent object
     */
    explicit superReportingClassesItemDelegate( QObject *parent = 0 );

    //////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create editor widget.
     *
     * @param[in] parent  parent widget
     * @param[in] option  options
     * @param[in] index model index
     * @returns  pointer to editor
     */
    virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class superReportingClasses
 * @brief Reporting classes.
 */
class superReportingClasses : public ScreenWidget
{
    Q_OBJECT

    typedef superReportingClasses _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  session main window
     * @param[in] parent  parent widget
     */
    explicit superReportingClasses( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~superReportingClasses();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for button press.
     */
    void onButtonClicked( QWidget *button );

    /**
     * Slot for item changed.
     */
    void onItemChanged( QTreeWidgetItem *item, int column );

    /**
     * Slot for item clicked.
     */
    void onItemClicked( QTreeWidgetItem *item, int column );

    /**
     * Slot for item double clicked.
     */
    void onItemDoubleClicked( QTreeWidgetItem *item, int column );

    /**
     * Slot for item selection changed.
     */
    void onItemSelectionChanged();

private:

    QLabel *title_;

    QPushButton *insertRow_;
    QPushButton *insertChild_;
    QPushButton *delete_;

    QTreeWidget *table_;

    QSignalMapper *buttons_;

    QTreeWidgetItem *current_;
    bool dirty_;
    bool dirtyRefresh_;
    int clicks_;

    bool insert_;
    QString insertClass_;
    QString insertReportingClass_;

    static const int fontPointSize_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Add child nodes.
     */
    void parseChildren( const QString& data );

    /**
     * Create a tree widget item.
     */
    QTreeWidgetItem *createItem( const QString& value );

    /**
     * Find a tree widget item.
     */
    QTreeWidgetItem *findItem( const QString& value );

    /**
     * Set item editable or not.
     */
    void setItemEditable( QTreeWidgetItem *item, int column );

    /**
     * Set current item.
     */
    void setCurrentItem( QTreeWidgetItem *item );

    /**
     * Set current item.
     */
    void saveCurrentItem();

    /**
     * Tokenize classes tree list.
     */
    static QVector<QString> tokenizeTreeList( const QString& listOf );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SUPERREPORTINGCLASSES_H
