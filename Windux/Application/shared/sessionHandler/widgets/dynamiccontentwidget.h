/**
 * @file dynamiccontentwidget.h
 * Operate mode widget for showing dynamic content.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef DYNAMICCONTENTWIDGET_H
#define DYNAMICCONTENTWIDGET_H

#include <Messages.h>

#include <QMap>
#include <QStack>
#include <QStackedWidget>
#include <QWidget>

class DomCurrentPLU;
class DomSoftkey;
class DomLoadSoftkeyGroup;
class MadpOperatorDisplayFields;

class QLabel;
class QPushButton;
class QSignalMapper;

/// Hobart namespace.
namespace hobart {

class softkeysWidget;
class webViewer;
class softkeyButton;
class softkeyButtonListWidget;
class softkeysWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct show_cancel_prod_entry_t
 * @brief Show cancel production entry button.
 */
struct show_cancel_prod_entry_t {};

/**
 * Used to show the cancel production entry button.
 */
static const show_cancel_prod_entry_t show_cancel_prod_entry = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dynamicContentWidget
 * @brief Operate mode widget for showing dynamic content.
 */
class dynamicContentWidget : public QWidget
{
    Q_OBJECT

    Q_ENUMS( WidgetType )
    Q_PROPERTY( bool accumulateItems READ isAccumulateItems WRITE setAccumulateItems )
    Q_PROPERTY( WidgetType currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged )
    Q_PROPERTY( int count READ count )
    Q_PROPERTY( WidgetType excludedContent READ excludedContent WRITE setExcludedContent )
    Q_PROPERTY( bool freshProduction READ isFreshProduction WRITE setFreshProduction )
    Q_PROPERTY( QString operatorMessage READ operatorMessage WRITE setOperatorMessage )
    Q_PROPERTY( QPixmap productImage READ productImage WRITE setProductImage RESET resetProductImage )
    Q_PROPERTY( bool specialWidget READ isSpecialWidget )

    typedef dynamicContentWidget _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Widget types.
     */
    enum WidgetType
    {
        // these widgets operator may flip between
        Empty = 0x0001,                                 ///< Empty area with nothing shown.
        NumberPad = 0x0002,                             ///< Fixed (non-popup) number pad.
        Flashkeys = 0x0004,                             ///< Flashkeys widget.
        WebBrowser = 0x0008,                            ///< Web browser widget.
        ProductImage = 0x0010,                          ///< Product image.
        BestSellerList = 0x0020,                        ///< Best sellers.
        RunTotals = 0x0040,                             ///< Run totals for prepack.
        FixedFlashkeys = 0x80,                          ///< PJR-383: Fixed Flash Keys

        // special widget which are shown/hidden programmatically
        AccumulateItems = 0x0100,                       ///< Accumulate items widget.
        FreshProduction = 0x0200,                       ///< Fresh production planning.
        OperatorMessage = 0x0400,                       ///< Operator message.
        TareFlashkeys = 0x0800,                         ///< Tare flashkeys.
        CutTestFlashkeysDefault = 0x1000,               ///< Cutting test flashkeys for the original cut test.
        CutTestFlashkeysOptionOne = 0x2000,             ///< Cutting test flashkeys for the new Option One Cut Test
        CameraOverrideFlashkeys = 0x8000,               ///< Flashkeys to override wrapper camera tray measurement.
    };

Q_SIGNALS:

    /**
     * This signal is emitted whenever the current widget changes.
     *
     * The parameter holds the @p index of the new current widget, or -1 if there isn't a new one
     * (for example, if there are no widgets in the @c dynamicContentWidget).
     *
     * @param[in] index  widget index
     *
     * @sa currentWidget()
     * @sa setCurrentWidget()
     */
    void currentChanged( WidgetType index );

    /**
     * Emitted when flashkey clicked.
     *
     * @param[in] w  pointer to widget emitting action
     * @param[in] f  pointer to flashkey clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void flashkeyClicked( QWidget *w, QWidget *f, int action, const QString& param, int feature );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] widgets  widgets in the dynamic area
     * @param[in] parent  parent widget
     */
    explicit dynamicContentWidget( WidgetType widgets = Empty, QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~dynamicContentWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Returns the count of child widgets.
     *
     * @returns  widget count
     */
    virtual int count() const;

    /**
     * Returns the current widget index.
     *
     * @returns  index of current widget
     */
    virtual WidgetType currentIndex() const;

    /**
     * Returns the current widget, or 0 if there are no child widgets.
     *
     * @returns  pointer to widget
     *
     * @sa currentIndex()
     * @sa setCurrentWidget()
     */
    virtual QWidget *currentWidget() const;

    /**
     * Returns which widgets are excluded from selection.
     *
     * Widgets in this filter cannot be selected via the content selector buttons.
     *
     * @returns  excluded widgets
     */
    virtual WidgetType excludedContent() const;

    /**
     * Returns the index of the given widget @p w, or -1 if the given widget @p w is not a child of
     * the @c dynamicContentWidget.
     *
     * @param[in] w  pointer to widget
     *
     * @sa currentIndex()
     * @sa widget()
     */
    virtual WidgetType indexOf( QWidget *w ) const;

    /**
     * Check for accumulate items special widget.
     *
     * @returns  @c true if accumulate items widget shown, @c false otherwise
     */
    virtual bool isAccumulateItems() const {return isSpecialWidget( AccumulateItems );}

    /**
     * Check for excluded content widget.
     *
     * @returns  @c true if excluded content widget shown, @c false otherwise
     */
    virtual bool isExcludedContentWidget() const {return (currentIndex() & excludedContent());}

    /**
     * Check for fresh production special widget.
     *
     * @returns  @c true if fresh production widget shown, @c false otherwise
     */
    virtual bool isFreshProduction() const {return isSpecialWidget( FreshProduction );}

    /**
     * Check for special widget.
     *
     * @returns  @c true if special widget shown, @c false otherwise
     */
    virtual bool isSpecialWidget() const {return (special_.size() > 0);}

    /**
     * Check for special widget @p t.
     *
     * @param[in] t  index of widget
     * @returns  @c true if special widget shown, @c false otherwise
     */
    virtual bool isSpecialWidget( WidgetType t ) const;

    /**
     * Retrieve operator message.
     *
     * @returns  message
     */
    virtual QString operatorMessage() const;

    /**
     * Retrieve product image.
     *
     * @returns  pixmap
     */
    virtual QPixmap productImage() const;

    /**
     * Reset product image.
     */
    virtual void resetProductImage();

    /**
     * Set accumulate items mode.
     *
     * @param[in] value  @c true to enable accumulate items, @c false otherwise
     */
    virtual void setAccumulateItems( bool value );

    /**
     * Set cutting test mode.
     *
     * @param[in] value  @c true to enable cut test, @c false otherwise
     */
    virtual void setCuttingTest( bool value );

    /**
     * Set flashkey group.
     *
     * @param[in] value  softkey group session message
     */
    virtual void setFlashkeys( const DomLoadSoftkeyGroup *value );

    /**
     * Set flashkey toggled background.
     *
     * @param[in] value  softkey session message
     */
    virtual void setToggledFlashkey(const DomSoftkey *value );

    /**
     * Set flashkey toggled background.
     *
     * @param[in] value  softkey session message
     */
    virtual void setPrimaryToggledFlashkey(int featureNum, const DomSoftkey *value );

    /**
     * Set flashkey group feature number.
     *
     * @param[in] value  softkey group number
     */
    virtual void setFlashkeysFeature( int value );

    /**
     * Set fresh production mode.
     *
     * @param[in] value  @c true to enable fresh production, @c false otherwise
     * @param[in] product  pointer to plu record
     * @param[in] fields  pointer to operator display fields
     */
    virtual void setFreshProduction( bool value, const DomCurrentPLU *product = NULL, const MadpOperatorDisplayFields *fields = NULL, const QString &weight = NULL );

    /// Set online ordering product url.
    /**
     * Override the FPP url.
     * @param[in] url  url for online ordering product details
     */
    virtual void setOnlineOrderingProductUrl( const QString& url );

    /**
     * Sets the operator message.
     *
     * @param[in] value  message
     */
    virtual void setOperatorMessage( const QString& value );

    /**
     * Sets the operator message and title.
     *
     * @param[in] title  message title
     * @param[in] value  message
     */
    virtual void setOperatorMessage( const QString& title, const QString& value );

    /**
     * Sets the operator message and title.
     *
     * This version will show the cancel production entry button.
     *
     * @param[in] title  message title
     * @param[in] value  message
     */
    virtual void setOperatorMessage( const QString& title, const QString& value, show_cancel_prod_entry_t );

    /**
     * Set product image.
     *
     * @param[in] value  pixmap
     */
    virtual void setProductImage( const QPixmap& value );

    /**
     * Set background palette of widget.
     *
     * @param[in] index  widget type
     * @param[in] brush  background
     */
    virtual void setWidgetBackground( WidgetType index, const QBrush& brush );

    /**
     * Returns the widget at the given @p index, or 0 if there is no such widget.
     *
     * @param[in] index  index of widget
     * @returns  pointer to widget
     *
     * @sa currentWidget()
     * @sa indexOf()
     */
    virtual QWidget *widget( WidgetType index ) const;

    /**
     * Returns the widget of type @c T.
     *
     * @returns  pointer to widget
     *
     * @sa currentWidget()
     * @sa indexOf()
     *
     * @tparam T  widget object type
     */
    template <class T>
    QWidget *widget() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Reset flashkeys to initial state.
     *
     * @param[in] force  force message to be sent even when on initial page
     */
    virtual void resetFlashkeys( bool force = false );

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public Q_SLOTS:

    /**
     * Hide content selector buttons.
     */
    virtual void hideContentSelectors();

    /**
     * Hide special widget.
     */
    virtual void hideSpecialWidget();

    /**
     * Resize fresh production frame.
     */
    virtual void resizeProdPlanning() {calcProdPlanningFrame();}

    /**
     * Set which widgets are excluded from selection.
     *
     * Widgets in this filter cannot be selected via the content selector buttons.
     *
     * @param[in]  excluded widgets
     */
    virtual void setExcludedContent( WidgetType index );

    /**
     * Sets the current widget to the widget at @p index.
     *
     * @param[in] index  index of widget
     */
    virtual void setCurrentIndex( WidgetType index );

    /**
     * Sets the current widget @p w to be the specified widget. The new current widget must
     * already be contained in this stacked widget.
     *
     * @param[in] w  pointer to widget
     *
     * @sa currentWidget()
     * @sa setCurrentIndex()
     */
    virtual void setCurrentWidget( QWidget *w );

    /**
     * Show content selector buttons.
     */
    virtual void showContentSelectors();

    /**
     * Show special widget.
     *
     * @note
     * Special widgets are show until hidden.
     *
     * @param index  widget to show
     */
    virtual void showSpecialWidget( WidgetType index );

protected:

    enum {WIDTH = 590};                                 ///< Dialog width.
    enum {HEIGHT = 285};                                ///< Dialog height.

    enum {HEIGHT_LARGE = 370};                          ///< Dialog height for large dca.

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set special widget mode.
     *
     * Some widgets are associated with a mode (i.e. accumulate items, fresh production) and
     * need to be added to the bottom of the special widget stack. This method performs this
     * action.
     *
     * @param[in] t  index of widget
     * @param[in] value  @c true to make visible, @c false otherwise
     */
    virtual void setSpecialWidgetMode( WidgetType t, bool value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Appends the given widget @p w to the @c dynamicContentWidget and returns the index position.
     * Ownership of widget is passed on to the @c dynamicContentWidget.
     *
     * If the @c dynamicContentWidget is empty before this function is called, widget becomes the
     * current widget.
     *
     * @param[in] widget  widget to add
     * @param[in] index  widget index
     * @returns  @c true if added, @c false otherwise
     *
     * @sa insertWidget()
     * @sa removeWidget()
     * @sa setCurrentWidget()
     */
    virtual bool addWidget( QWidget *w, WidgetType index );

    /**
     * Removes widget @p w from the @c dynamicContentWidget. i.e., widget @p w is not deleted but
     * simply removed from the stacked layout, causing it to be hidden.
     *
     * @note
     * Ownership of widget @p w reverts to the application.
     *
     * @sa addWidget()
     * @sa insertWidget()
     * @sa currentWidget()
     */
    virtual void removeWidget( QWidget *w );

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

private Q_SLOTS:

    /**
     * Slot for button clicked.
     */
    void onButtonClicked( QWidget *w );

    /**
     * Slot for current widget changed.
     */
    void onCurrentChanged( int index );

    /**
     * Slot for operator message cancel button clicked.
     */
    void onOperatorMessageCancelButtonClicked();

    /**
     * Slot for widget removal.
     */
    void onWidgetRemoved( int index );

private:

    QStackedWidget *stackedWidget_;

    QPushButton *prev_;
    QPushButton *next_;

    QSignalMapper *buttons_;
    bool buttonsVisible_;

    bool propigateSignals_;

    QMap<WidgetType, int> widgetToIndexMap_;
    QMap<int, WidgetType> indexToWidgetMap_;

    WidgetType excludedContent_;

    QStack<WidgetType> special_;
    WidgetType originalWidget_;

    softkeysWidget *flashkeys_;
    softkeysWidget *bestSellerList_;
    softkeysWidget *fixedFlashkeys_;
    softkeysWidget *tareFlashkeys_;
    softkeysWidget *cutTestFlashkeysDefault_;
    softkeysWidget *cutTestFlashkeysOptionOne_;

    QWidget *productImageWidget_;
    QLabel* productImage_;

    QWidget *runTotals_;

    webViewer *fpp_;
    QPushButton *hideFpp_;

    QWidget *operatorMessageWidget_;
    QLabel* operatorMessageTitle_;
    QLabel* operatorMessage_;
    QPushButton *operatorMessageCancelButton_;
    int operatorMessageCancelButtonClicks_;

    DomSoftkey selected_;
    hobart::softkeyButton *prev_flashkey;

    QString onlineOrderingProductUrl_;
    bool onlineOrderingShown_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Create widgets.
     */
    void createWidgets( WidgetType widgets );

    /**
     * Map widget type to index.
     */
    int widgetToIndex( WidgetType t ) const;

    /**
     * Map index to widget type.
     */
    WidgetType indexToWidget( int index ) const;

    /**
     * Get production planning URL.
     */
    QString prodPlanningUrl( const DomCurrentPLU *product, const MadpOperatorDisplayFields *fields, const QString &weight ) const;

    /**
     * Adjust production planning widget dimensions.
     */
    void calcProdPlanningFrame();

    /**
     * Generate exit button.
     */
    static QPixmap exitButtonPixmap( const QSize &size );

    /// return the row clicked for a flashkey that can toggle in a fixed flashkey feature
    int getFixedToggledXCoordinate(int fixedRowOrCol);

    /// return the column clicked for a flashkey that can toggle in a fixed flashkey feature
    int getFixedToggledYCoordinate(int fixedRowOrCol);

};

template <class T>
QWidget *dynamicContentWidget::widget() const
{
    QWidget *w = NULL;

    // iterate widgets, find one of type T
    for ( int i = 0; i < count(); ++i )
        if ( (w = stackedWidget_->widget( i )) )
            if ( dynamic_cast<T*>( w ) )
                return w;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // DYNAMICCONTENTWIDGET_H
