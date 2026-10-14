/**
 * @file recalldialog.h
 * Dialog for managing recalls.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef RECALLDIALOG_H
#define RECALLDIALOG_H

#include "listdialog.h"

class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for managing recalls.
class RecallDialog : public ListDialog
{
    Q_OBJECT

    typedef RecallDialog _Myt;
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
    explicit RecallDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~RecallDialog();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add items to list.
    /**
     * @param[in] dates  recall dates
     * @param[in] textList  list of recall text description
     * @param[in] wasReadFlags  list of read flags
     * @param[in] cookies  recall cookies
     */
    virtual void addItemsToList( const QStringList& dates, const QStringList& textList, const QStringList& wasReadFlags, const QStringList& cookies );

    /// Add item to list.
    /**
     * @param[in] date  recall date
     * @param[in] text  recall text description
     * @param[in] isNew  @c true if recall is new, @c false otherwise
     * @param[in] cookie  recall cookie
     */
    virtual void addItemToList( const QString& date, const QString& text, bool isNew, const QVariant& cookie );

    /// Callback for session messages.
    /**
     * This base method does nothing. Override to implement your own functionality.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

    /// Request list.
    virtual void requestList();

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QWidget *button );

    /// Slot for list selection changed.
    void onListItemSelectionChanged();

private:

    QSignalMapper *signalMapper_;

    QPushButton *dismiss_;
    QPushButton *delete_;
    QPushButton *view_;

    static const QSize iconSize_;
    static const QSize iconSizeList_;

    /// Initialize.
    void initialize();

    class recallItem
    {
    public:

        recallItem() {}
        recallItem( const recallItem& rhs ) {assignFrom( rhs );}

        recallItem& operator = ( const recallItem& rhs ) {assignFrom( rhs ); return *this;}

        virtual void assignFrom( const recallItem& rhs ) {
            date_ = rhs.date_;
            text_ = rhs.text_;
            isNew_ = rhs.isNew_;
        }

        QString date_;
        QString text_;
        bool isNew_;
    };

    typedef QMap<QString, recallItem> recallItemMap;
    recallItemMap recalls_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // RECALLDIALOG_H
