/**
  * @file shortListEditor.h
  * @brief Widget to allow editing of the EAN string.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup shortListEditorCsu
  * @{
  */

#ifndef SHORTLISTEDITOR_H
#define SHORTLISTEDITOR_H

#include <QWidget>

#include "../screens/screenwidget.h"
#include "digitEntry.h"

class SessionMainWindow;
class DomCurrentPLU;

namespace Ui {
class ShortListEditor;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class ShortListEditor
 * @brief Widget to allow the ean string to be created and modified.
 */

class ShortListEditor : public QWidget
{
    Q_OBJECT
    
    typedef ShortListEditor _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] mainWindow  the @c SessionMainWindow object
     */
    explicit ShortListEditor(QWidget *parent, SessionMainWindow *mainWindow );

    /**
     * Destructor.
     */
    virtual ~ShortListEditor();

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();


signals:
    /**
     * SIGNAL broadcast to anyone listening when the Done button is pressed.
     *
     * @param[in] msg  session message
     */
    void shortListEditorDoneClicked(bool);

    /**
     * SIGNAL broadcast to anyone listening when the Cancel button is pressed.
     *
     * @param[in] msg  session message
     */
    void shortListEditorCancelClicked();

public Q_SLOTS:


    /**
    * SLOT method called when the Done button is selected. Returns the user back to the
    * ChangeAllFields page and saves the current Short List (default COOL) Format String
    *
    * @param[in] none
    * @returns  none
    */
    void updateDoneFormating();

    /**
    * SLOT method called when the Cancel button is selected. Returns the user back to the
    * ChangeAllFields page and does not save the current Short List (default cool) String
    *
    * @param[in] none
    * @returns  none
    */
    void updateCancelFormating();

    /**
    * SLOT method called when the Start over button is selected. Just clears the current COOL
    *
    * @param[in] none
    * @returns  none
    */
    void clearCoolText();



private Q_SLOTS:
    /**
     * Slot to handle session messages.
     */
    void onSessionMessageReceiveInternal( const SessionMessage& msg );

    /**
     * Method used to add a Production Step to the new COOL being created
     *
     * @param[in] row  row is the row clicked in the Production Step table.
     * @param[in] col  col will always be 0 since the table is a single column item.
     * @returns  none
     */
    void addStep(int row, int col);

    /**
     * Method used to add a Country to the new COOL being created
     *
     * @param[in] row  row is the row clicked in the Country table.
     * @param[in] col  col will always be 0 since the table is a single column item.
     * @returns  none
     */
    void addCountry(int row, int col);

protected:

    /**
     * Callback for session messages.
     *
     * This base method does nothing. Override to implement your own functionality.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );



private:
    Ui::ShortListEditor *ui;

    SessionMainWindow *mainWindow_;
    Session *session_;

    DomCurrentPLU *currentPLUData;

    QStringList currentCountryArray;

    QString countryList;
    QString stepList;
    QString coolText;
    QString currentCountryList;

    int currentCategoryNumber;
    int coolRecordNum;
    int countryListSize;
    int stepListSize;
    int stepSelectedRow;
    int countrySelectedRow;

    bool ignoreSignals_;
    bool hasChanged_;


    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to build and update the Country variables and set up the country list
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void countrySetup(const SessionMessage &msg);

    /**
     * Method used to build and update the Production Steps variables and set up the steps list
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void stepSetup(const SessionMessage &msg);

    /**
     * Method used to build and update the Production Steps variables and set up the steps list
     *
     * @param[in] theList  theList is the list of steps or countries.
     * @returns  none
     */
    void buildCOOLLists(QString theList, int numOfItems);

    /**
     * Overridding the sizeHint() for this specific UI's characteristics (h/w)
     * @return QSize
     */
    QSize sizeHint() const;


};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SHORTLISTEDITOR_H
