/**
 * @file selfserviceoftkeyswidgetG2.h
 * Widget for displaying self service item search softkeys.
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Job E. Crew
 * @originalwork Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICESOFTKEYSWIDGETG2_H
#define SELFSERVICESOFTKEYSWIDGETG2_H

#include <QMap>
#include <QWidget>

class selfServiceHook;
class ScaleConfiguration;
class QLabel;
class QPushButton;
class QSignalMapper;
class QLineEdit;
class QSplitter;
class QStackedWidget;
class QString;

class Session;

class DomLoadSoftkeyGroup;

/// Hobart namespace.
namespace hobart {

class pushButton;
class softkeysWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Widget for displaying self service item search softkeys.
 */
class selfServiceSoftkeysWidgetG2 : public QWidget
{
    Q_OBJECT

    typedef selfServiceSoftkeysWidgetG2 _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * @brief  Emitted when softkey clicked.
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void softkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

public:

    /// Search modes.
    enum itemSearchMode
    {
        searchByAZ,                                 ///< Search by A-Z buttons.
        searchByCat                                 ///< Search by Category buttons.
    };

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief  Constructor.
     * @param[in] s  pointer to session object
     * @param[in] parent  parent widget
     */
    explicit selfServiceSoftkeysWidgetG2(Session *s, selfServiceHook *h, QWidget *parent = 0 );

    /// Destructor.
    virtual ~selfServiceSoftkeysWidgetG2();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Retrieve search mode.
     * @return  search mode
     */
    virtual itemSearchMode searchMode() const;

    /**
     * @brief  Set search mode.
     * @param[in] value  search mode
     */
    virtual void setSearchMode( itemSearchMode value );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Create softkey group from session message.
     * @param[in] m  session message
     */
    virtual void create( const DomLoadSoftkeyGroup* m );

    /// Reload images.
    virtual void reloadImages();

    /// Reset widget to initial state.
    virtual void resetState();

    /// Translate widget to current language.
    virtual void translate();

public Q_SLOTS:

    /**
     * @brief  Clears the results string and text search field.
     * @param[in] resetAll if true, clears the text search field, else it leaves the search letters in it
     */
    virtual void clear(bool resetAll);

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Resize event.
     * @param[in] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    virtual void showEvent( QShowEvent *e );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Send reset message to session.
     * @param[in] force  force message to be sent even when on initial page
     */
    virtual void sendResetMessage( bool force = false );

    /// Update button text for current state.
    virtual void updateCurrentState();

    /// Update search panel width.
    virtual void updateSearchPanelWidth();

private Q_SLOTS:

    /// Slot for button click.
    void onButtonClicked( QWidget *w );

    /// Slot for softkey button click.
    void onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

    /// Slot for button click.
    void onKeyButtonClicked( QWidget *w );

    /// Slot for return key pressed.
    void onCharacterSearchReturnPressed();

    /// Slot for when text changes.
    void onCharacterSearchChanged( const QString& text );

private:

    QSplitter *splitter_;                       /// Splits the area from the MCI and the A-Z buttons

    QWidget *searchPanel_;                      /// The Area that holds the A-Z/Category buttons
    QWidget *buttonsArea_;                      /// This is the area widget that holds the buttons

    QLineEdit *characterSearch_;

    QStackedWidget *searchPanelSoftkeys_;       /// Holds the stack of softkeys for A-Z or Categories

    hobart::softkeysWidget *searchByAZ_;
    hobart::softkeysWidget *searchByCat_;
    hobart::softkeysWidget *results_;

    QWidget *resultsPanel_;
    QStackedWidget *resultsPanelSoftkeys_;

    hobart::pushButton *prevPage_;
    hobart::pushButton *nextPage_;

    Session *session_;
    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    QSignalMapper *buttonSignalMapper_;
    QSignalMapper *keyboardSignalMapper_;

    typedef QMap<QString, hobart::pushButton*> buttonMap;
    buttonMap buttons_;

    int searchByAZIndex_;
    int searchByCatIndex_;
    int resultsIndex_;

    bool showingBestSellers_;

    double searchPanelSize_;

    int searchFieldWidth_;
    int resultsFieldWidth_;

    /// Initialize object.
    void initialize();

    /// Create layouts.
    void createLayout();

    /// Toggle the best sellers button.
    void toggleBestSellersButton();

    /// Tell the user the results of their search
    void updateResultsString(int featureNum);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVICESOFTKEYSWIDGETG2_H
