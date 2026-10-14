/**
 * @file flashkeyeditor.h
 * Flashkey editor for operate and supervisor modes.
 *
 * @copyright Copyright (c) 2013 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef FLASHKEYEDITOR_H
#define FLASHKEYEDITOR_H

#include "screenwidget.h"

#include <Messages.h>

namespace hobart {
class dynamicContentWidget;
class numericLineEdit;
class refuseBinWidget;
class softkeyButton;
class softkeyButtonListWidget;
class softkeysWidget;
}

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QScrollArea;
class QSignalMapper;
class QSplitter;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class FlashkeyEditor
 * @brief Flashkey editor for operate and supervisor modes.
 */
class FlashkeyEditor : public ScreenWidget
{
    Q_OBJECT

    typedef FlashkeyEditor _Myt;
    typedef ScreenWidget _Mybase;

    Q_PROPERTY( QString currentFeature READ currentFeature WRITE setCurrentFeature )
    Q_PROPERTY( QString currentGroup READ currentGroup WRITE setCurrentGroup )

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit FlashkeyEditor( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~FlashkeyEditor();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve the current feature.
     *
     * @returns  current feature
     */
    virtual QString currentFeature() const;

    /**
     * Retrieve the current group.
     *
     * @returns  current group
     */
    virtual QString currentGroup() const;

    /**
     * Set the current feature.
     *
     * @param[in] value  feature
     */
    virtual void setCurrentFeature( const QString& value );

    /**
     * Set the current group.
     *
     * @param[in] value  group
     */
    virtual void setCurrentGroup( const QString& value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Enable only specific features for operate mode.
     *
     * This function will trim down the editable features to only those passed in.
     *
     * @note:
     * This function only works in operate mode.
     *
     * @param[in] features  list of features to enable
     */
    virtual void enableOperateFeatures( const QStringList& features );
    
    /**
     * Load flashkey feature and group.
     *
     * Use an empty group string to request default feature.
     *
     * @param[in] feature  feature
     * @param[in] group  group
     */
    virtual void load( const QString& feature, const QString& group = QString() );

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set the current feature.
     *
     * @param[in] value  feature
     */
    virtual void setCurrentFeatureInternal( const QString& value );

    /**
     * Set the current group.
     *
     * @param[in] value  group
     */
    virtual void setCurrentGroupInternal( const QString& value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

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

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /**
     * Slot for widget clicked.
     */
    void onClicked( QWidget *widget );

    /**
     * Slot for current index changed.
     */
    void onCurrentIndexChanged( QWidget *widget );

    /**
     * Slot for widget editing finished.
     */
    void onEditingFinished( QWidget *widget );

    /**
     * Slot for flashkey button clicked.
     */
    void onFlashkeyClicked( QWidget *w, QWidget *f, int action, const QString& param, int feature );

    /**
     * Slot for flashkey new cell.
     */
    void onFlashkeyCreateNewCell( QWidget *w, const QPoint& gridPos, int action );

    /**
     * Slot for flashkey deleted.
     */
    void onFlashkeyDeleteCell( QWidget *f );

    /**
     * Slot for flashkey move cell.
     */
    void onFlashkeyMoveCell( QWidget *w, QWidget *b, const QPoint& oldGridPos, const QPoint& newGridPos );

    /**
     * Slot for screen changed.
     */
    void onScreenChanged();

private:

    enum EditMode
    {
        GridEditMode,
        CellEditMode,
        CreateNewCellMode
    };

    QSignalMapper *clickedSignal_;
    QSignalMapper *editingFinishedSignal_;
    QSignalMapper *currentIndexChangedSignal_;

    QSplitter *splitter_;

    QWidget *mainPane_;
    QWidget *rightPane_;

    QWidget *editGridWidgets_;
    QWidget *editCellWidgets_;
    QWidget *createNewCellWidgets_;
    QWidget *operateModeWidgets_;

    QString featureDefaultGroupNum_;
    QSize featureGridSize_;
    int featureHorizSpacing_;
    int featureVertSpacing_;
    int featureFixedRowColumn_;
    int featureDefaultButton_;

    QString actionSoftkeyGroup_;
    DomSoftkey selected_;
    
    DomSoftkeyStyle style_;

    QTimer *setFocusActionParam_;
    QTimer *setFocusActionParamNumeric_;

    bool isNewSoftkey_;
    bool isOperateColorChange;

    int newSoftkeyAction_;

    bool isMoveSoftkey_;
    bool isMoveSoftkeySetVisible_;
    QPoint moveSoftkeyGridPos_;

    bool isUpdateSoftkey_;

    QMap<int, QString> actions_;
    QMap<int, QString> screenChangeActions_;

    bool twoLabelAppliersAttached_;

    //
    // main pane widgets
    //

    QLabel *featureLabel_;
    QComboBox *feature_;

    QLabel *groupLabel_;
    hobart::numericLineEdit *group_;

    QLabel *actionLabel_;
    QComboBox *action_;

    QLabel *actionParamLabel_;
    QLineEdit *actionParam_;

    hobart::numericLineEdit *actionParamNumeric_;

    QComboBox *screenChangeAction_;

    QComboBox *playVideoAction_;
    
    QComboBox *swipeAction_;
    QComboBox *departments_;

    QComboBox *totalTypesAction_;

    QComboBox *launchExternalAppAction_;

    hobart::dynamicContentWidget *dynamicContent_;
    hobart::softkeysWidget *flashkeys_;
    hobart::softkeysWidget *bestSellers_;

    //
    // edit grid widgets
    //

    QPushButton *deleteGroup_;

    QLabel *defaultGroupLabel_;
    hobart::numericLineEdit *defaultGroup_;

    QLabel *horzSpacingLabel_;
    hobart::numericLineEdit *horzSpacing_;

    QLabel *vertSpacingLabel_;
    hobart::numericLineEdit *vertSpacing_;

    QPushButton *editCell_;

    QLabel *rowsLabel_;
    QPushButton *addRow_;
    QPushButton *removeRow_;

    QLabel *columnsLabel_;
    QPushButton *addColumn_;
    QPushButton *removeColumn_;

    QLabel *heightLabel_;
    QPushButton *addHeight_;
    QPushButton *removeHeight_;

    QLabel *widthLabel_;
    QPushButton *addWidth_;
    QPushButton *removeWidth_;

    QPushButton *createNewCell_;

    hobart::refuseBinWidget *refuseBin_;

    // PJR-383
    QLabel *fixedRowColumnLabel_;
    QComboBox *fixedRowColumn_;
    int fixedRowColumnIndex_;

    QLabel *fixedRowColumnDefaultButtonLabel_;
    hobart::numericLineEdit *fixedRowColumnDefault_;

    //
    // edit cell widgets
    //

    QLabel *labelPlacement1Label_;
    QComboBox *labelPlacement1_;

    QLabel *labelRotation1Label_;
    QComboBox *labelRotation1_;

    QLabel *labelPlacement2Label_;
    QComboBox *labelPlacement2_;

    QLabel *labelRotation2Label_;
    QComboBox *labelRotation2_;

    QLabel *printerLabel_;
    QComboBox *printer_;				//	The printer to change the label type for.
    QString labelTypeNum_;

    QGroupBox *webPageParameters_;
    QCheckBox *webPageUsesStoreNumber_;
    QCheckBox *webPageUsesDepartment_;
    QCheckBox *webPageUsesPlu_;
    QCheckBox *webPageUsesQty_;
    QCheckBox *webPageUsesLang_;
    QCheckBox *webPageUsesModel_;
    QPushButton *webPageUsesAll_;
    QPushButton *webPageUsesNone_;

    QLabel *trayLengthLabel_;
    hobart::numericLineEdit *trayLength_;

    QLabel *trayWidthLabel_;
    hobart::numericLineEdit *trayWidth_;

    QLabel *trayLipHeightLabel_;
    hobart::numericLineEdit *trayLipHeight_;

    QLabel *displayLocationLabel_;
    QComboBox *displayLocation_;

    QLabel *imageLabel_;
    QComboBox *image_;
    QLabel *pyScriptLabel_;
    QComboBox *pyScript_;
    QLabel *pyScriptIntervalLabel_;
    hobart::numericLineEdit *pyScriptInterval_;

    // PJR-383
    QLabel *toggledImageLabel_;
    QComboBox *toggledImage_;

    QLabel *backgroundColorLabel_;
    QComboBox *backgroundColor_;
    int backgroundColorIndex_;

    QLabel *textColorLabel_;
    QComboBox *textColor_;
    int textColorIndex_;

    QLabel *textAlignmentLabel_;
    QComboBox *textAlignment_;

    QLabel *primaryTextLabel_;
    QLineEdit *primaryText_;

    QLabel *secondaryTextLabel_;
    QLineEdit *secondaryText_;

    QPushButton *cancelCellEdit_;
    QPushButton *cellEditDone_;

    //
    // new cell widgets
    //

    hobart::softkeyButtonListWidget *actionList_;

    QPushButton *cancelCellCreate_;

    //
    // operate mode widgets
    //

    QCheckBox *enableBestSellerList_;

    QLabel *operateBackgroundColorLabel_;
    QComboBox *operateBackgroundColor_;
    int operateBackgroundColorIndex_;

    hobart::softkeyButtonListWidget *operateActionList_;

    hobart::refuseBinWidget *operateRefuseBin_;

    /**
     * Initialize widget.
     */
    void initialize();

    /**
     * Create widget layout.
     */
    void createLayout();

    /**
     * Set the current softkey.
     */
    void setCurrentSoftkey(const DomSoftkey& m);

    /**
     * Set the current softkey default description.
     */
    void setCurrentSoftkeyDefaultDescription( hobart::softkeyButton *flashkey );

    /**
     * Set focus to the action parameter.
     */
    void setFocusActionParam( int msec = 0 );

    /**
     * Set child widgets enabled.
     */
    void setScreenEnabled( bool enabled );

    /**
     * Set action param label text.
     */
    void setActionParamLabelText();

    /**
     * Set color item text descriptions.
     */
    void setColorComboBoxItemText( QComboBox *w );

    /**
     * Update interface for mode.
     */
    void showMode( EditMode mode );

    /**
     * Update interface for specific action.
     */
    void showActionFields( int action, bool isNewAction = true );

    /**
     * Read softkey (136,20).
     */
    void readSoftkey( const QPoint& pos ) const;

    /**
     * Write softkey (136,22).
     */
    void writeSoftkey() const;

    /**
     * Delete softkey (136,24).
     */
    void deleteSoftkey( const QPoint& pos ) const;

    /**
     * Save softkey (136,26).
     */
    void saveSoftkey() const;

    /**
     * Create new softkey (136,32).
     */
    void createNewSoftkey( const QPoint& pos ) const;

    /**
     * Read softkey feature (136,40).
     */
    void readSoftkeyFeature( const QString& feature ) const;

    /**
     * Save softkey feature (136,46)
     */
    void saveSoftkeyFeature() const;

    /**
     * Read softkey style (136,60).
     */
    void readSoftkeyStyle( const QString& feature ) const;

    /**
     * Save softkey style (136,66).
     */
    void saveSoftkeyStyle() const;

    /**
     * Read softkey group (136,80).
     */
    void readSoftkeyGroup( const QString& group ) const;

    /**
     * Delete softkey group (136,84).
     */
    void deleteSoftkeyGroup( const QString& group ) const;

    /**
     * Save softkey group (136,86).
     */
    void saveSoftkeyGroup() const;

    /**
     * Create new softkey group (136,92).
     */
    void createNewSoftkeyGroup( const QString& group ) const;

    /**
     * Load softkey group (136,93).
     */
    void loadSoftkeyGroup( const QString& feature, const QString& group ) const;

    /**
     * Set item text of combo box.
     */
    static void setComboBoxItemTextFromData( QComboBox *w, const QString& data, const QString& text );

    /**
     * Set item text of combo box.
     */
    static void setComboBoxItemTextFromData( QComboBox *w, int data, const QString& text );

    /**
     * Make a color from string.
     */
    static QColor makeColor( const QString& c );

    /**
     * Translate canned string to specific language.
     */
    static QString translate( const QString& lang, const QString& s );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FLASHKEYEDITOR_H
