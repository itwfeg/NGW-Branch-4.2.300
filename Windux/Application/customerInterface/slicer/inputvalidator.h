#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QObject>
#include <QValidator>

class InputValidator : public QDoubleValidator
{
public:
    InputValidator(double bottom, double top, int decimals, QObject * parent) :
        QDoubleValidator(bottom, top, decimals, parent)
    {
    }

    QValidator::State validate(QString &s, int &i) const
    {
        if (s.isEmpty() || s == ".") {
            return QValidator::Intermediate;
        }
        //cleanup leading zeroes
        s.replace(QRegExp("^[0]*"),"0");
        if(QRegExp("^[0][\\d].*").exactMatch(s)){
            //qDebug() << "match";
            s.remove(0,1);
        }

        QChar decimalPoint = locale().decimalPoint();

        if(s.indexOf(decimalPoint) != -1) {
            int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

            if (charsAfterPoint > decimals()) {
                return QValidator::Invalid;
            }
        }

        bool ok;
        double d = locale().toDouble(s, &ok);
        i = s.length();
        if (ok && d >= bottom() && d <= top()) {
            return QValidator::Acceptable;
        } else {
            return QValidator::Invalid;
        }
    }
};

#endif // INPUTVALIDATOR_H
