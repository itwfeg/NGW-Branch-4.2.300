/**
 * @file softkeyswidget.h
 * Widget for displaying softkeys.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SOFTKEYSWIDGET_H
#define SOFTKEYSWIDGET_H

#include <QList>
#include <QWidget>
#include <QStringList>


class DomLoadSoftkeyGroup;
class DomSoftkey;
class MadpOperatorDisplayFields;
class ScaleConfiguration;
class MadpInternalConfig;
class selfServiceHook;

class Session;

class QSignalMapper;

/// Hobart namespace.
namespace hobart {

class softkeyButton;
class softkeyButtonFactory;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class softkeysWidget
 * @brief Widget for displaying softkeys.
 */
class softkeysWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( GroupType createFilter READ createFilter WRITE setCreateFilter )
    Q_PROPERTY( bool editMode READ editMode WRITE setEditMode )
    Q_PROPERTY( GroupType feature READ feature )
    Q_PROPERTY( QSize gridSize READ gridSize WRITE setGridSize )
    Q_PROPERTY( GroupType group READ group )
    Q_PROPERTY( int hSpacing READ hSpacing WRITE setHSpacing )
    Q_PROPERTY( int textAlignment READ textAlignment WRITE setTextAlignment )
    Q_PROPERTY( int vSpacing READ vSpacing WRITE setVSpacing )
    Q_PROPERTY( int fRowColMethod READ fRowColMethod WRITE setFRowColMethod )
    Q_PROPERTY( int fRowColDefButton READ fRowColDefButton WRITE setFRowColDefButton )

    typedef softkeysWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * Emitted when softkey clicked.
     *
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void clicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

    /**
     * Emitted when new cell dropped onto grid.
     *
     * @param[in] w  pointer to widget emitting action
     * @param[in] gridPos  grid pos of new cell
     * @param[in] action  action of new cell
     */
    void createNewCell( QWidget *w, const QPoint& gridPos, int action );

    /**
     * Emitted when cell dragged to new location.
     *
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button dragged
     * @param[in] oldGridPos  old grid pos
     * @param[in] newGridPos  new grid pos
     */
    void moveCell( QWidget *w, QWidget *b, const QPoint& oldGridPos, const QPoint& newGridPos );

public:

    /**
     * Softkey group types.
     */
    enum GroupType
    {
        FreshServeFixedButtons = 888050,
        ProductGrid = 999001,
        StartScreenButtons = 999002,                ///< Programmable Buttons for the Main screen.
        RunScreenButtons = 999003,                  ///< Programmable Buttons for the Run screen.
        PresetTareGrid = 999004,
        CategoryNavigation = 999005,
        CategoryButtons = 999006,
        MostCommonlyUsedItems = 999007,

        SelfservCategoryButtons = 999008,

        StartScreenCutTestOff = 999012,             ///< Programmable Buttons for the Start a run screen when Cutting test is off.
        RunScreenCutTestOff = 999013,               ///< Programmable Buttons for the Run screen when Cutting test is off.
        StartScreenCutTestOn = 999014,              ///< Programmable Buttons for the Start a run screen when Cutting test is on.
        RunScreenCutTestOn = 999015,                ///< Programmable Buttons for the Run screen when Cutting test is on.

        SelfservAZCategoryButtons = 999016,
        SelfservCategoryResultsButtons = 999017,
        SelfservProductGrid = 999018,

        FixedRowColumnGrid            = 999020,     ///< Programmable Buttons for the Fixed Flash Keys Feature (PJR-383)
        ForcedCameraOverride          = 999025,

        RunScreenNumericKeypadButtons = 999030,     ///< Programmable Buttons for the Run screen when the numeric keypad is hidden (Fastrack).
        BuildAMealScreenButtons       = 999031,     ///< Programmable Buttons for the Build-A-Meal screen (Fastrack).
        BuildAMealSideItemButtons     = 999032,     ///< Programmable Buttons for the Build-A-Meal Side Items (Fastrack).
        MeatSpecSheetScreeenButtons   = 999033,     ///< Programmable Buttons for Meat Specification Sheets screen (Fastrack)
        MeatSpecSheetSideItemButtons  = 999034,     ///< Programmable Buttons for Meat Specification Sheets Side Items (Fastrack)
        RunScreenCutTestOnOptionOne   = 999035,     ///< Programmable Buttons for the Run Screen when the New Cut Test Option 1 is on
        RunScreenEnhancedCenterScreen = 999036,     ///< Programmable Buttons for the Run Screen Enhanced Center Screen (RESERVED, NOT USED YET)

        CameraSystemPredictions       = 999040,     ///< Edgify and Tiliter Camera Predictions for Self Service CAMERA
    };

    /**
     * Action codes.
     */
    enum ActionCode
    {
        Okay,                                       ///< Action okay.
        ContainsSoftkey,                            ///< Cannot perform action due to softkey.
        ContainsSpannedSoftkey,                     ///< Cannot perform due to spanned softkey.
        GridTooSmall                                ///< Cannot perform because grid too small.
    };

    // PJR-383
    enum FixedRowColumnMethod
    {
        FRCM_None,         // 0
        FRCM_TopRow,       // 1
        FRCM_FirstColumn,  // 2
        FRCM_BottomRow,    // 3
        FRCM_LastColumn    // 4
    };

    /// Softkey actions.
    enum SoftkeysAction
    {
        Action_NoAction = 0,
        Action_PrintLabel,
        Action_ChangeScreen,
        Action_BrowseWeb,
        Action_ChangeLanguage,
        Action_LoginOperator,                           // 5
        Action_ResetPrice,
        Action_SetPlatterTare,
        Action_SelectProduct,
        Action_LoadSoftkeyGroup,
        Action_LoadCategory_First,                      // 10
        Action_LoadCategory_Next,
        Action_LoadCategory_Previous,
        Action_ApplyDiscount_CentsOff,
        Action_ApplyDiscount_PercentOff,
        Action_VoidTransaction,                         // 15
        Action_LoadSpecials_First,
        Action_LoadSpecials_Next,
        Action_LoadSpecials_Previous,
        Action_LoadSearch_First,
        Action_LoadSearch_Next,                         // 20
        Action_LoadSearch_Previous,
        Action_PrintExistingExpirationLabel,
        Action_FPP_URL,
        Action_Application_URL,
        Action_BeginCutTest,                            // 25
        Action_EndCutTest,
        Action_SelectAnyProduct,
        Action_PrintRunTotals,
        Action_SwitchTotalType,
        Action_PrintMultipleLabels,                     // 30
        Action_ToggleProductionEntryMode,
        Action_PrintNewExpirationLabel,
        Action_LoadTareSoftkey,
        Action_ChangeLabelPlacementAndPosition,
        Action_ServeNextCustomer,                       // 35
        Action_NowServingQueueManagement,
        Action_ChangeLabelType,
        Action_CutTestHistory,
        Action_LoadAlphabeticSearch_First,
        Action_LoadAlphabeticSearch_Next,               // 40
        Action_LoadAlphabeticSearch_Previous,
        Action_LoadMostCommon_First,
        Action_LoadMostCommon_Next,
        Action_LoadMostCommon_Previous,
        Action_RedisplayCurrentGrid,                    // 45
        Action_ToggleAccumulateItemsMode,
        Action_AddItemToAccumulatedItems,
        Action_RemoveLastItemFromAccumulatedItems,
        Action_ShowAccumulatedItems,
        Action_RemoveAllAccumulatedItems,               // 50
        Action_PrintAccumulatedItems,
        Action_LogoutOperator,
        Action_PlayVideo,
        Action_Portioning,
        Action_LaunchExternalApp,                       // 55
        Action_AddSideItemToMeal,
        Action_AddSideItemToMeatSpecSheet,
        Action_CutTestSuspend,
        Action_CutTestResume,
        Action_ToggleMode,                              // 60
        Action_OpenDocumentFolder,
        Action_TogglePick5Active,
        Action_ToggleExtraShelfLifeDay
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] s  pointer to session object
     * @param[in] parent  parent widget
     */
    explicit softkeysWidget(
        Session *s,
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~softkeysWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve button at specific grid position.
     *
     * @returns  pointer to button
     */
    virtual softkeyButton *at( const QPoint& gridPos ) const;

    /**
     * Retrieve create filter.
     *
     * @returns  filter @c GroupType
     */
    virtual GroupType createFilter() {return filter_;}

    /**
     * Check if possible to expand softkey horizontally.
     *
     * @param[in] button  button to check
     * @param[out] reason  result
     * @returns  @c true if possible, @c false otherwise
     */
    virtual bool canExpandSoftkeyHorz( const softkeyButton *button, ActionCode& reason ) const;

    /**
     * Check if possible to expand softkey vertically.
     *
     * @param[in] button  button to check
     * @param[out] reason  result
     * @returns  @c true if possible, @c false otherwise
     */
    virtual bool canExpandSoftkeyVert( const softkeyButton *button, ActionCode& reason ) const;

    /**
     * Check if possible to remove column.
     *
     * @param[out] reason  result
     * @returns  @c true if possible, @c false otherwise
     */
    virtual bool canRemoveColumn( ActionCode& reason ) const;

    /**
     * Check if possible to remove row.
     *
     * @param[out] reason  result
     * @returns  @c true if possible, @c false otherwise
     */
    virtual bool canRemoveRow( ActionCode& reason ) const;

    /**
     * Retrieve if widget is in edit mode.
     *
     * @returns  @c true if in edit mode, @c false otherwise
     */
    virtual bool editMode() const {return editMode_;}

    /**
     * Retrieve softkey button factory.
     *
     * @returns  pointer to factory object
     */
    virtual softkeyButtonFactory *factory() const {return factory_;}

    /**
     * Retrieve feature.
     *
     * @returns  feature
     */
    virtual GroupType feature() const {return feature_;}

    /**
     * Compute grid position from an (x,y) coordinate.
     *
     * @param[in] pos  coordinate
     * @returns  grid position
     */
    virtual QPoint gridPosAt( const QPoint& pos ) const;

    /**
     * Retrieve grid size.
     *
     * @return  size
     */
    virtual QSize gridSize() const {return gridSize_;}

    /**
     * Retrieve group.
     *
     * @returns  group
     */
    virtual GroupType group() const {return group_;}

    /**
     * Retrieve horizontal spacing.
     *
     * @returns  spacing
     */
    virtual int hSpacing() const {return hSpacing_;}

    /**
     * Retrieve selected button.
     *
     * @returns  pointer to selected button
     */
    virtual softkeyButton *selected() const;

    /**
     * Set create filter.
     *
     * When set, the create() method will ignore all groups other then the filter.
     *
     * @param[in] value  filter @c GroupType
     */
    virtual void setCreateFilter( GroupType value ) {filter_ = value;}

    /**
     * Set if widget is in edit mode.
     *
     * @param[in]  @c true for edit mode, @c false otherwise
     */
    virtual void setEditMode( bool value );

    /**
     * Set grid size.
     *
     * @param[in] value  size
     */
    virtual void setGridSize( const QSize& value );

    /**
     * Set horizontal spacing.
     *
     * @param[in] value  spacing
     */
    virtual void setHSpacing( int value );

    /**
     * Set text alignment.
     *
     * @param[in] value  text alignment
     */
    virtual void setTextAlignment( int value );

    /**
     * Fit the text to the flashkey
     *
     * @param[in] text to fit
     */
    virtual QString setTextToFit( QString fktext );

    /**
     * Set softkey button factory.
     *
     * @param[in] value  pointer to factory object
     */
    virtual void setFactory( softkeyButtonFactory *value );

    /**
     * Set vertical spacing.
     *
     * @param[in] value  spacing
     */
    virtual void setVSpacing( int value );

    /**
     * Retrieve text alignment.
     *
     * @returns  text alignment
     */
    virtual int textAlignment() const {return textAlignment_;}

    /**
     * Retrieve horizontal spacing.
     *
     * @returns  spacing
     */
    virtual int vSpacing() const {return vSpacing_;}

    /**
     * Retrieve Fixed Row/Column Method.
     *
     * @returns  Fixed Row/Column Method
     */
    virtual int fRowColMethod() const {return fixedRowColMethod_;}

    /**
     * Set Fixed Row/Column Method
     *
     * @param[in] value  method
     */

    virtual void setFRowColMethod( int value );

    /**
     * Retrieve Fixed Row/Column Default Button.
     *
     * @returns  Fixed Row/Column Default Button
     */
    virtual int fRowColDefButton() const {return fixedRowColDefButton_;}

    /**
     * Set Fixed Row/Column Default Button
     *
     * @param[in] value  default button
     */
    virtual void setFRowColDefButton( int value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create softkey group from session message.
     *
     * @returns  @c true if message processed, @c false otherwise
     */
    virtual bool create( const DomLoadSoftkeyGroup* m );

    /**
     * toggle background from session message.
     *
     * @returns  @c true if message processed, @c false otherwise
     */
    virtual bool updateToggledBackground(const DomSoftkey* m , softkeyButton *flashkey);

    /**
     * toggle background from session message.
     *
     * @returns  @c true if message processed, @c false otherwise
     */
    virtual bool updatePrimaryToggledBackground(const DomSoftkey* m , softkeyButton *flashkey);

    /**
     * reset background from session message.
     *
     * @returns  @c true if message processed, @c false otherwise
     */
    virtual bool resetPrimaryToggledBackground(const DomSoftkey* m , softkeyButton *flashkey);

    /**
     * Find button with specific action code.
     *
     * @param[in] action  action code
     */
    virtual softkeyButton *findButton( int action ) const;

    /**
     * Resize all softkey buttons.
     */
    virtual void resizeButtons();

    /**
     * Send reset message to session.
     *
     * @param[in] force  force message to be sent even when on initial page
     */
    virtual void sendResetMessage( bool force = false );

public Q_SLOTS:

    /**
     * Edit selected button.
     */
    virtual void editSelection();

    /**
     * Reset button selection.
     */
    virtual void resetSelection();

    /**
     * Reset toggled buttons.
     */
    virtual void resetToggled();

    /**
     * Set selected button.
     *
     * @param[in] value  pointer to selected button
     */
    virtual void setSelected( softkeyButton *value );

    /**
     * Set selected tare button.
     *
     * @param[in] value  pointer to selected tare button
     */
    virtual void setSelfServTareFlashkey( softkeyButton *value );

    /**
     * Set selected tare button.
     *
     * @param[in] value  pointer to selected tare button
     */
    virtual void resetSelfServTareFlashkey();

    /**
     * Set toggled button.
     *
     * @param[in] value  pointer to toggled button
     */
    virtual void setToggled( softkeyButton *value );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Set event drop action.
     *
     * @param[in,out] e  event
     * @returns  drop action
     */
    template <class T>
    Qt::DropAction setDropAction( T *e );

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragEnterEvent( QDragEnterEvent *e );

    /**
     * Drag enter event.
     *
     * @param[in,out] e  drag enter event
     */
    virtual void dragMoveEvent( QDragMoveEvent *e );

    /**
     * Drop event.
     *
     * @param[in,out] e  drop event
     */
    virtual void dropEvent( QDropEvent *e );

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /**
     * Slot for softkey button clicked.
     */
    void onSoftkeyButtonClicked( QWidget *button );

private:

    typedef QList<softkeyButton*> softkeyButtonList;
    softkeyButtonFactory *factory_;

    QSignalMapper *signalMapper_;

    GroupType feature_;
    GroupType group_;

    QStringList randoBackgrounds_;

    QSize gridSize_;
    mutable int hSpacing_;
    mutable int vSpacing_;
    mutable int textAlignment_;
    int numOfDescChars_;
    int quickFeatureNum_;

    int fixedRowColMethod_;
    int fixedRowColDefButton_;

    Session *session_;
    GroupType filter_;

    int clicks_;                                    // count of softkey button presses
    int pages_;                                     // count of softkey messages processed

    bool editMode_;
    bool keysPending_;
    bool isBlinking_;
    bool fixedKeysRefreshed_;

    DomLoadSoftkeyGroup *previous_;
    ScaleConfiguration *scaleConfig_;
    MadpInternalConfig *internalConfigData_;
    MadpOperatorDisplayFields *operatorDisplayFields_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Retrieve buttons.
     */
    softkeyButtonList buttons() const;

    /**
     * Overlaps existing button.
     */
    bool overlapsExistingButton( const softkeyButton *button, const QPoint& pos, const QSize& size ) const;

    /**
     * Calculate button geometry.
     */
    QRect calcButtonGeometry( const QPoint& pos, const QSize& size ) const;

    /**
     * Calculate point coordinates.
     */
    QPoint calcPoint( const QPoint& pos ) const;

    /**
     * Create softkey button.
     */
    softkeyButton *createSoftkeyButton();

    /**
     * Free all softkey buttons.
     */
    void freeSoftkeyButtons();

    /**
     * Render empty (unused) buttons.
     */
    void renderEmptyButtons( QPainter *p );

    /**
     * Make color from string value.
     */
    static QColor makeColor( const QString& c );

    /**
     * return the row clicked for a flashkey that can toggle in a fixed flashkey feature
     */
    int getFixedToggledXCoordinate(int fixedRowOrCol);

    /**
     * return the column clicked for a flashkey that can toggle in a fixed flashkey feature
     */
    int getFixedToggledYCoordinate(int fixedRowOrCol);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SOFTKEYSWIDGET_H
