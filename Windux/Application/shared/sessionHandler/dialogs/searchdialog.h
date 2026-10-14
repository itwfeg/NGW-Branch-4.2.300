/**
 * @file searchdialog.h
 * Dialog for searching plu description.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "listdialog.h"

class QCheckBox;
class QIntValidator;
class QLabel;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class SearchDialog
 * @brief Dialog for searching plu description.
 */
class SearchDialog : public ListDialog
{
    Q_OBJECT

    typedef SearchDialog _Myt;
    typedef ListDialog _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit SearchDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /**
     * Destructor.
     */
    virtual ~SearchDialog();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve selected item number.
     *
     * @returns  plu number
     */
    virtual QString selectedItemNumber() const {return selected_;}

protected:

    /**
     * Search states.
     */
    enum State
    {
        Initial,                ///< Initial state (i.e. please type something)
        Searching,              ///< Searching for data
        NoResults,              ///< No results found
        Done                    ///< Done searching have results
    };

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set current search state.
     *
     * @param[in] value  current state
     */
    virtual void setCurrentState( State value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    /**
     * Callback for list item clicked.
     *
     * @param[in] item  item clicked
     * @param[in] cookie  cookie of item clicked
     */
    virtual void onListItemClicked( QListWidgetItem *item, const QVariant& cookie );

    /**
     * Callback for session messages.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for return key pressed.
     */
    void onPluDescriptionReturnPressed();

    /**
     * Slot for when text changes.
     */
    void onPluDescriptionTextChanged( const QString& text );

    /**
     * Slot for search type.
     */
    void onSearchTypeToggled( bool value );

private:

    QLineEdit *pluDescription_;
    QCheckBox *searchType_;

    QLabel *currentState_;
    QLabel *howManyPLUs_;
    QLabel *addMoreChars_;

    QIntValidator *validator_;

    QString selected_;

    static const QSize iconSize_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SEARCHDIALOG_H
