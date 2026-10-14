/**
 * @file accumulateitems.h
 * Accumulate items widget that appears in the dca area.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef ACCUMULATEITEMS_H
#define ACCUMULATEITEMS_H

#include <QWidget>

#include <Session.h>

class OperateMode;

class QLabel;
class QPushButton;
class QSignalMapper;
class QTreeWidget;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class accumulateItemsWidget
 * @brief Accumulate items widget that appears in the dca area.
 */
class accumulateItemsWidget : public QWidget
{
    Q_OBJECT

    typedef accumulateItemsWidget _Myt;
    typedef QWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit accumulateItemsWidget(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~accumulateItemsWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public Q_SLOTS:

    /**
     * Update list of items.
     */
    virtual void updateList();

protected:

    enum {WIDTH = 590};                                 ///< Dialog width.
    enum {HEIGHT = 285};                                ///< Dialog height.

    enum {HEIGHT_LARGE = 370};                          ///< Dialog height for large dca.

    Session *session_;                                  ///< Pointer to session.

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Callback for session messages.
     *
     * This base method does nothing. Override to implement your own functionality.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  shoe event
     */
    virtual void showEvent( QShowEvent *e );

private Q_SLOTS:

    /**
     * Slot for button clicked.
     */
    void onButtonClicked( QWidget *button );

    /**
     * Slot to handle selection changing.
     */
    void onTableItemSelectionChanged();

    /**
     * Slot to handle session messages.
     */
    void onSessionMessageReceiveInternal( const SessionMessage& msg );

private:

    QTreeWidget *table_;

    QLabel *numberOfEntriesLabel_;
    QLabel *numberOfEntries_;

    QLabel *totalLabel_;
    QLabel *total_;

    QPushButton *addItem_;
    QPushButton *addItemPrint_;
    QPushButton *removeItem_;
    QPushButton *removeAllItems_;
    QSignalMapper *buttons_;

    QLabel *hideSpacer_;
    QPushButton *hide_;

    OperateMode *operateMode_;

    bool needToPrintPrintAccumulatedItems_;
    bool alreadyHereSoGetOut;
    int accumulatedItemsCount_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Check for item in operate mode.
     */
    bool isCurrentProduct() const;

    /**
     * Update button states and text.
     */
    void updateButtons();

    /**
     * Generate exit button.
     */
    static QPixmap exitButtonPixmap( const QSize &size );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // ACCUMULATEITEMS_H
