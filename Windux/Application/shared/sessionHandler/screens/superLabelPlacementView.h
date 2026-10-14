#ifndef SUPERLABELPLACEMENTVIEW_H
#define SUPERLABELPLACEMENTVIEW_H

// new label placement screen for supervisor mode

#include "screenwidget.h"
#include "labelplacementwidget.h"

namespace hobart {
template <class T> class backgroundWidget;
class labeledPluEdit;
}

class DomCurrentPLU;
class SupervisorMode;
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
class QPushButton;

class superLabelPlacementView : public ScreenWidget
{
    Q_OBJECT

    typedef superLabelPlacementView _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superLabelPlacementView( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~superLabelPlacementView();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:
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
     * @param[in] secondaryRelativeToPrimary  secondary label placement as it relates to the primary label position
     * @return  corner label will be placed
     */
    virtual corner calculateLabelPlacement( int printer ) const;

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

    /// Request configuration.
    virtual void requestConfig();

    /// Update interface.
    virtual void updateInterface();

    /// Handle screen changing.
    virtual bool onScreenChanging();


private Q_SLOTS:

    /// Slot for combo box index changed.
    void onComboBoxCurrentIndexChanged( QWidget *widget );

    /// Slot for label rotation changed.
    void onLabelRotationChanged( int value );

    /// Slot for when screen changes.
    void onScreenChanged();

    void doneButtonClicked();

private:

    typedef hobart::backgroundWidget<QWidget> frameWidget;

    bool ignoreSignals_;
    bool registered_;

    bool labelPlacementFromProduct_;
    bool doneButtonClicked_;

    int printerCount_;
    QVector<printerProp> printers_;

    DomCurrentPLU *product_;
    SupervisorMode *supervisorMode_;

    QPushButton *doneButton;

    DomEvolWrapperConfig *evolWrapperConfig_;
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation_;
    MadpPeripheralList *peripheralList_;
    MadpPeripheralList *prevPeripheralList_;
    MadpScaleConfig *scaleConfig_;

    QSignalMapper *comboBoxCurrentIndexChanged_;

    // label placement
    frameWidget *labelFrame_;

    LabelPlacementWidget *preview_;

    QLabel *printerLabel_;
    QComboBox *printer_;

    QLabel *labelPlacementLabel_;
    QComboBox *labelPlacement_;

    QLabel *labelRotationLabel_;
    QDial *labelRotation_;

    QSize primaryLabelSize_;
    QSize secondaryLabelSize_;
    corner primaryLabelPos_;
    corner secondaryLabelPos_;
    bool primaryLabelMoved_;
    bool secondaryLabelMoved_;
    int previousPrimaryPosition_;
    int previousSecondaryPosition_;

    int primaryRotation_;
    int secondaryRotation_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout( Qt::DockWidgetArea numPadLocation );

    /// Update interface.
    void updateInterfaceInternal( bool enabled );

    /// Generate preview of label placement.
    void generatePreview();

    /// need to change secondary position if it is relative to primary in certain situations
    void updateSecondaryOnPrimaryMove();
};

#endif // SUPERLABELPLACEMENTVIEW_H
