#ifndef PRINTERPLATEPOSITIONING_H
#define PRINTERPLATEPOSITIONING_H

#include "screenwidget.h"

namespace hobart {
    template <class T> class backgroundWidget;
    class labeledPluEdit;
}

class DomCurrentPLU;
class SupervisorMode;
class OperateMode;
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
class QTimer;

static const int NUM_NOTCHES = 20;

class PrinterPlatePositioning : public ScreenWidget
{
    Q_OBJECT

    typedef PrinterPlatePositioning _Myt;
    typedef ScreenWidget _Mybase;

public:
    explicit PrinterPlatePositioning( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );
    virtual ~PrinterPlatePositioning();

    virtual void translate();

protected:
    virtual bool eventFilter( QObject *obj, QEvent *e );
    virtual void onSessionMessage( const SessionMessage& msg );
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    // may need these in the future
    virtual void updateInterface();
    //virtual bool onScreenChanging();

private Q_SLOTS:
    void onScreenChanged();

    void acceptButtonClicked();
    void rejectButtonClicked();

private:
    // state of the wrapper
    enum ngwState
    {
        pause,
        resume,
        reject
    };

    void initialize();
    void createLayout( Qt::DockWidgetArea numPadLocation );
    void generatePreview();
    void drawDestinations( QPainter &painter );
    void drawAlignmentText( QPainter &painter );
    int getScreenSize();
    int getNotch( int distance );

    typedef hobart::backgroundWidget<QWidget> frameWidget;

    OperateMode *operateMode;

    QPushButton *acceptButton;
    QPushButton *rejectButton;

    frameWidget *labelFrame;
    QLabel *preview;

    QSize printerImageSize;
    int edgeOffset;
    int homeLocation;

    QImage *primaryPlateImage;
    QRect primaryPlateImageRect;
    QRect secondaryPlateImageRect;
    QImage *guideImageUpArrow;
    QImage *guideImageDownArrow;
    QRect primaryGuideImageRect;
    QRect secondaryGuideImageRect;

    bool printerDrawn;
    bool isRightDischarge;
    bool isWeightZero;
    bool hasSecondLabel;

    enum PrPlate { unknown, primary, secondary };
    int primaryPlateDistFromHome; // in mm
    int secondaryPlateDistFromHome; // in mm

    int tempPrimaryDist;
    int tempSecondaryDist;

    int packageId;

    QMap<int, float> notchDistances;

    int currentNotchPrimary;
    int currentNotchSecondary;
    int requiredNotchPrimary;
    int requiredNotchSecondary;
    int graphicalNotchDistance;

    enum points { baseLeft, baseRight, top };
    struct Arrowhead
    {
        int baseWidth;
        int height;
    };

    Arrowhead arrowhead;
    QPoint primaryArrowheadPoints[3];
    QPoint secondaryArrowheadPoints[3];

    int actualNotchDistances[NUM_NOTCHES] = {
        12,  25,  38,  50,  63,
        76,  88,  101, 114, 127,
        139, 152, 165, 177, 190,
        203, 215, 228, 241, 252
    };
};

#endif // PRINTERPLATEPOSITIONING_H
