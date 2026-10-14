#ifndef LABELPLACEMENTWIDGET_H
#define LABELPLACEMENTWIDGET_H

#include <QWidget>
#include <QLabel>

struct printerProp
{
    bool installed;             // true if printer is present in system
    bool automaticRotation;     // true if printer has automatic label rotation (no lever)
    int orientation;            // indicates how printer is mounted, which way is up

    int placement;              // label placement, from system or from product record
    int rotation;               // rotation of label on package, from product record
    bool labelStockUpsideDown;  // true when label stock is upside down
    int verticalOffset;         // vertical spacing of the primary and secondary labels
};

/// Corners.
enum corner
{
    upperRight = 0,
    upperLeft,
    none,
    lowerRight,
    lowerLeft,

    rightOfPrimary,
    leftOfPrimary,
    abovePrimary,
    belowPrimary,

    numPositions
};

class LabelPlacementWidget : public QLabel
{
    Q_OBJECT

    typedef LabelPlacementWidget _Myt;
    typedef QLabel _Mybase;

public:
    explicit LabelPlacementWidget( QWidget *parent = 0 );
    virtual ~LabelPlacementWidget();

    void generatePreview(
        int printerCount,
        int currentPrinter,
        int primaryPos,
        int secondPos,
        QSize& primarySize,
        QSize& secondSize,
        const QVector<printerProp>& printers );

private:
    int primaryRotation_;

    enum printerStations
    {
        first,
        second,
        numStations
    };

private:
    void drawAvailableCorners( QPainter& painter, int currentLoc ) const;
    void handleRelativePositions( QRect& boundingRect, int primaryPos, int secondPos, const QSize& primarySize, const QSize& secondSize ) const;
    void drawAvailableRelativePositions( QPainter& painter, int index, int primaryPos, int secondPos, const QSize& primarySize, const QSize& secondSize, int currentPrinter ) const;
};

#endif // LABELPLACEMENTWIDGET_H
