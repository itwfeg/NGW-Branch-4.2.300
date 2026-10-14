#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>
#include <QList>

class QDoubleValidator;
class QIntValidator;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QRadioButton;

///////////////////////////////////////////////////////////////////////////////////////////////////
class calibrationDialog : public QDialog
{
    Q_OBJECT

    typedef calibrationDialog _Myt;
    typedef QDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent
     */
    calibrationDialog( QWidget *parent = 0 );

    /// Destructor.
    virtual ~calibrationDialog();

private slots:

    /// Slot for push button clicked.
    void onPushButtonClicked();

private:

    QLabel *calib_mode_;
    QRadioButton *calib_mode_20pt_;
    QRadioButton *calib_mode_12pt_;

    QLabel *calib_method_;
    QRadioButton *calib_method_kg_;
    QRadioButton *calib_method_lb_;

    QLabel *accel_res_lbl_;
    QLineEdit *accel_res_;

    QLabel *accel_sens_lbl_;
    QLineEdit *accel_sens_;

    QLabel *accx_;
    QLabel *accy_;
    QLabel *accz_;
    QLabel *counts_0kg_;
    QLabel *counts_5kg_;
    QLabel *counts_10kg_;
    QLabel *counts_15kg_;

    QLabel *level_;
    QLabel *front_;
    QLabel *back_;
    QLabel *left_;
    QLabel *right_;

    QLineEdit *accx_level_;
    QLineEdit *accy_level_;
    QLineEdit *accz_level_;
    QLineEdit *counts_level_0kg_;
    QLineEdit *counts_level_5kg_;
    QLineEdit *counts_level_10kg_;
    QLineEdit *counts_level_15kg_;

    QLineEdit *accx_front_;
    QLineEdit *accy_front_;
    QLineEdit *accz_front_;
    QLineEdit *counts_front_0kg_;
    QLineEdit *counts_front_5kg_;
    QLineEdit *counts_front_10kg_;
    QLineEdit *counts_front_15kg_;

    QLineEdit *accx_back_;
    QLineEdit *accy_back_;
    QLineEdit *accz_back_;
    QLineEdit *counts_back_0kg_;
    QLineEdit *counts_back_5kg_;
    QLineEdit *counts_back_10kg_;
    QLineEdit *counts_back_15kg_;

    QLineEdit *accx_left_;
    QLineEdit *accy_left_;
    QLineEdit *accz_left_;
    QLineEdit *counts_left_0kg_;
    QLineEdit *counts_left_5kg_;
    QLineEdit *counts_left_10kg_;
    QLineEdit *counts_left_15kg_;

    QLineEdit *accx_right_;
    QLineEdit *accy_right_;
    QLineEdit *accz_right_;
    QLineEdit *counts_right_0kg_;
    QLineEdit *counts_right_5kg_;
    QLineEdit *counts_right_10kg_;
    QLineEdit *counts_right_15kg_;

    QPlainTextEdit *results_;

    QPushButton *clear_;
    QPushButton *calculate_;
    QPushButton *quit_;

    QPushButton *loadFileValidate_;

    QIntValidator *accel_res_validator_;
    QDoubleValidator *accel_sens_validator_;

    QIntValidator *acc_validator_;
    QIntValidator *counts_validator_;

    QList<QLineEdit*> inputs_;

    /// Initialize.
    void initialize();

    /// Create Layout.
    void createLayout();

    /// Accelerometer Values.
    std::vector<double> accValues() const;

    /// Weigher Values.
    std::vector<double> weigherValues() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CALIBRATIONDIALOG_H
