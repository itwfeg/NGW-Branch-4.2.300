#ifndef SPEEDKEYCONFIG_H
#define SPEEDKEYCONFIG_H

#include <QDialog>
#include <QMap>
#include <QStringList>
#include <QButtonGroup>
#include <QDoubleValidator>
#include <QList>
#include <QColor>
#include <QComboBox>
#include "speedkey.h"

#include "./hobart/usb/weigherconfiguration.h"
namespace Ui {
class speedKeyConfig;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
class speedKeyConfig : public QDialog
{
    Q_OBJECT

    typedef speedKeyConfig _Myt;
    typedef QDialog _Mybase;

    Ui::speedKeyConfig *ui_;

public:
    typedef QList<QStringList> speedKeyList;

    enum type
    {
        None = 0,
        PresetWeight,
        InfinityMode,

        MAX_KEY_TYPES
    };

    enum typeWeight
    {
        Type_lbs = 0,
        Type_oz,
        Type_Kg,
        Type_g
    };

    explicit speedKeyConfig(QWidget *parent = 0);
    ~speedKeyConfig();

    // Speed Key Ordering:
    // X
    // Y
    // Width
    // Height
    // Type
    // Value1
    // Value2
    // Text
    // TextHAlign
    // TextVAlign
    // TextColor
    // TextFamily
    // TextFontPointSize
    // BackgroundColor
    // BackgroundImage
    // ForegroundImage
    // Value3
    // totalWeight

    static QString configFileName();
    static QSize speedKeyMenuSize();
    static speedKeyList readSpeedKeys();
    static void writeSpeedKeys(const speedKeyList &values);
    static void writeSpeedKeys(const QList<speedKey *> values);
    static void setMode(WeighMode mode);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void onLeftArrowClicked();
    void onRightArrowClicked();
    void onDeleteClicked();
    void onNewClicked();
    void onAdvancedClicked();

    void onCancel();
    void onSave();

    void onTypeChanged(int type) const;
    void onWeightTypeChanged(int type) const;
    void slt_targetWeightChanged(void) const;
    void slt_clearButtonsChecked() const;

    void slt_previewText(QString value);
    void slt_previewTextColor(int index);
    void slt_previewFont(QString value);
    void slt_previewBgColor(int index);

private:
    void initialize();
    void createLayout();

    void updateCurrent() const;
    void saveCurrent();
    void updatePreview(int field, QString value);

    int getCheckedIndex();

    void enableDisablePositionButtons() const;
    void hideComboItem(QComboBox * combo, int row);
    void showComboItem(QComboBox * combo, int row);

    speedKeyList keys_;
    speedKeyList keysOrig_;


    /*
     * [b1, b2, b3, b4, b5, b6, b7, b8...] -- x-max = 3, y-max = 4
     * --------------
     * [ b1, b2, b3 ]
     * [ b4, b5, b6 ]
     * [ b7, b8 ... ]
     * --------------
     * To get the position from index c1,
     *  --- y = c1 / x-max
     *  --- x = c1 % x-max
     */
    QList<QPushButton*> positionButtons;
    QButtonGroup* positionButtonGroup;

    int current_;

    QDoubleValidator* targetWeightValidatorLBS_;
    QDoubleValidator* targetWeightValidatorOZ_;
    QDoubleValidator* targetWeightValidatorKG_;
    QDoubleValidator* targetWeightValidatorG_;
    QDoubleValidator* weightToleranceValidator_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SPEEDKEYCONFIG_H
