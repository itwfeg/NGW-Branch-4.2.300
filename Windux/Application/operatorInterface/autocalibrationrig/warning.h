#ifndef WARNING_H
#define WARNING_H

#include <QWidget>

namespace Ui {
class Warning;
}

class Warning : public QWidget
{
    Q_OBJECT

public:
    explicit Warning(QWidget *parent, QString warningText);
    ~Warning();

private Q_SLOTS:

    /**
     * Go ahead and leave when the Done button is clicked.
     */
    void onDoneClicked();

private:
    Ui::Warning *ui;

    QString details_;

    /**
     * Get the party started.
     */
    void initialize();

    /**
     * Time to go.
     */
    void exitWarning();

};

#endif // WARNING_H
