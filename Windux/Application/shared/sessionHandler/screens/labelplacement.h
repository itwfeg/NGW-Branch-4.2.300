/**
 * @file labelplacement.h
 * Screen for adjusting label placement settings.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELPLACEMENT_H
#define LABELPLACEMENT_H

#include "pluoperatescreen.h"

namespace hobart {
template <class T> class backgroundWidget;
class labeledPluEdit;
}

/*
class LabelPlacementWidget;
struct printerProp;
*/
#include "../widgets/labelplacementwidget.h"

class DomHeightBox;
class DomEvolLabelApplierConfig;
class DomEvolWrapperConfig;
class DomEvolWrapperSystemInformation;
class DomLabelApplierConfig;
class DomUpdateRunModeField;
class MadpPeripheralList;
class MadpScaleConfig;

class QComboBox;
class QDial;
class QLabel;
class QPushButton;
class QSignalMapper;
class QCheckBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Screen for adjusting label placement settings.
class LabelPlacementScreen : public PluOperateScreen
{
    Q_OBJECT

    typedef LabelPlacementScreen _Myt;
    typedef PluOperateScreen _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit LabelPlacementScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~LabelPlacementScreen();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    /// Label placements.
    // I don't think this is used anymore, and we are just using the corner enum with leading and trailing edge
    // being handled in the backend
    enum labelPlacement
    {
        rightSide = 0,                              ///< Label placed on right side.
        leftSide,                                   ///< Label placed on left side.

        defaultSetting,                         ///< Use default setting.

        // non-evolution label appliers
        leadingEdge = rightSide,                    ///< Leading edge of package.
        trailingEdge = leftSide                     ///< Trailing edge of package.
    };

    /// Package direction after being wrapped.
    enum packageDirection
    {
        rightToLeft = 0,                            ///< Package travels from right to left.
        leftToRight = 1                             ///< Package travels from left to right.
    };



    enum labels
    {
        first,
        second,
        maxLabels
    };

    // ========================================================================
    // Properties
    // ========================================================================

    /// Calculate label placement for printer.
    /**
     * @param[in] printer  which printer
     * @return  corner label will be placed
     */
    virtual corner calculateLabelPlacement( int printer ) const;

    /// Screen that gets displayed when swiping (gesturing) the screen right.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenRight() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[in] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Request configuration.
    virtual void requestConfig();

    /// Update interface.
    virtual void updateInterface();

    virtual bool onScreenChanging();

private Q_SLOTS:

    /// Slot for combo box index changed.
    void onComboBoxCurrentIndexChanged( QWidget *widget );

    /// Slot for PLU data changed.
    void onCurrentProductChanged( bool nof );

    /// Slot for PLU field changes.
    void onCurrentProductUpdateField( const DomUpdateRunModeField& m );

    /// Slot for label rotation changed.
    void onLabelRotationChanged( int value );

    /// Slot to handle number pad moving.
    void onNumberPadMoved( Qt::DockWidgetArea where );

    /// Slot for when screen changes.
    void onScreenChanged();

private:

    typedef hobart::backgroundWidget<QWidget> frameWidget;

    /*
    struct printerProp
    {
        bool installed;             // true if printer is present in system
        bool automaticRotation;     // true if printer has automatic label rotation (no lever)
        int orientation;            // indicates how printer is mounted, which way is up

        int placement;              // label placement, from system or from product record
        int rotation;               // rotation of label on package, from product record
    };
    */

    bool ignoreSignals_;
    bool registered_;

    bool labelPlacementFromProduct_;

    packageDirection direction_;

    int printerCount_;
    QVector<printerProp> printers_;

    DomEvolWrapperConfig *evolWrapperConfig_;
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation_;
    MadpPeripheralList *peripheralList_;
    MadpPeripheralList *prevPeripheralList_;
    MadpScaleConfig *scaleConfig_;

    QSignalMapper *comboBoxCurrentIndexChanged_;

    // messages
    frameWidget *messageFrame_;

    // label placement
    frameWidget *labelFrame_;

    LabelPlacementWidget *preview_;
    bool previewPackageClicked_;

    QLabel *printerLabel_;
    QComboBox *printer_;

    QLabel *labelPlacementLabel_;
    QComboBox *labelPlacement_;

    QLabel *labelVerticalAdjLabel_;
    QComboBox *labelVerticalAdjustment_;

    QLabel *labelRotationLabel_;
    QDial *labelRotation_;
    QCheckBox *syncRotation_;
    DomHeightBox *heightBox_;
    // QLabel *lever_;

    // QLabel *leverMessageIcon_;
    // QLabel *leverMessage_;

    QSize primaryLabelSize_;    // graphical size of label
    QSize secondaryLabelSize_;  // graphical size of label
    corner primaryLabelPos_;
    corner secondaryLabelPos_;
    bool primaryLabelMoved_;
    bool secondaryLabelMoved_;
    int previousPrimaryPosition_;
    int previousSecondaryPosition_;

    int primaryRotation_;

    int precedence_;
    QString trayName_;
    QString wrapBoxId_;
    QString heightBoxIndex_;
    QString wrapBoxIndex_;

    QVector<QString> paramArray_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout( Qt::DockWidgetArea numPadLocation );

    /// Update for configuration change.
    void updateConfig();

    /// Update interface.
    void updateInterfaceInternal( bool enabled );

    /// Generate preview of label placement.
    void generatePreview();

    /// Generate preview of level orientation.
    // void generateLeverOrientation();

    /// Check for AWS.
    bool isAWS() const;

    /// Check if label applier is CLA EPSAC2.
    bool isCLA_EPSAC2() const;

    /// Check for EWS.
    bool isEWS() const;

    bool isNGWWrapper() const;

    /// Adjust the rotation, if this is a CLAS.
    int adjustRotation(int rotation);		//	HTA-99

    /// for drawing the possible positions for the currently selected label
    void drawAvailableCorners(QPainter &painter, int currentLoc);

    /// for drawing the possible positions relative to the primary label
    void drawAvailableRelativePositions(QPainter &painter, int index);

    /// drawing the secondary label when it is relative to primary
    //void handleRelativePositions(QRect &boundingRect);

    /// to show and hide the combobox options
    //void updateLabelPlacementComboItems();

    /// need to change secondary position if it is relative to primary in certain situations
    void updateSecondaryOnPrimaryMove();

    /// save the label configuration to the product record
    void savePlacementAndRotation();

    corner handleTwoPrinterClickEvent(QMouseEvent *mouseEvent, bool &nothingClicked);
    corner handleOnePrinterClickEvent(QMouseEvent *mouseEvent);

    void writePlacementOffset(int printer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELPLACEMENT_H
