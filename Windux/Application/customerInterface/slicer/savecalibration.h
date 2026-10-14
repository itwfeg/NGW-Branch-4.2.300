#ifndef SAVECALIBRATION_H
#define SAVECALIBRATION_H

#include <QDialog>

namespace Ui {
class SaveCalibration;
}

class SaveCalibration : public QDialog
{
    Q_OBJECT
signals:
    void sig_GainChanged(long gain); //Emit signal to weighcalib
public slots:
    void slt_WeigherStatus(char *buffer, int buffLen);
private slots:
    void slt_saveButtonClicked();
    void slt_cancelButtonClicked();
public:
    explicit SaveCalibration(QWidget *parent = 0);
    ~SaveCalibration();

    long getGain() const;
    void setGain(long value);

private:
    Ui::SaveCalibration *ui;
    long gain;
};

#endif // SAVECALIBRATION_H
