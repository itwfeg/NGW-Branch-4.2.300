#ifndef FAILURE_H
#define FAILURE_H

#include <QDialog>

namespace Ui {
class failure;
}

class failure : public QDialog
{
    Q_OBJECT

public:
    explicit failure(QWidget *parent, QString warningText);
    ~failure();

private Q_SLOTS:

    /**
     * Go ahead and close up when the Done button is clicked.
     */
    void onDoneClicked();

private:
    Ui::failure *ui;

    QString details_;

    /**
     * Get the party started.
     */
    void initialize();

    /**
     * Last call.
     */
    void exitWarning();
};

#endif // FAILURE_H
