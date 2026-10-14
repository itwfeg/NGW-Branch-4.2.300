#ifndef SPEEDKEY_H
#define SPEEDKEY_H

#include <QLabel>

///////////////////////////////////////////////////////////////////////////////////////////////////
class speedKey : public QLabel
{
    Q_OBJECT

    typedef speedKey _Myt;
    typedef QLabel _Mybase;


public:
    speedKey(QWidget *parent = 0);
    ~speedKey();


    virtual int x() const {return x_;}
    virtual int y() const {return y_;}
    virtual int w() const {return w_;}
    virtual int h() const {return h_;}

    virtual int type() const {return type_;}
    virtual QString val1() const {return val1_;}
    virtual QString val2() const {return val2_;}
    virtual QString val3() const {return val3_;}

    virtual QString text() const {return text_;}
    virtual int textHAlign() const {return textHAlign_;}
    virtual int textVAlign() const {return textVAlign_;}
    virtual QColor textColor() const {return textColor_;}
    virtual QString textFontFamily() const {return textFontFamily_;}
    virtual int textFontPointSize() const {return textFontPointSize_;}
    virtual QColor backgroundColor() const {return backgroundColor_;}
    virtual QString backgroundImage() const {return backgroundImage_;}
    virtual QString foregroundImage() const {return foregroundImage_;}



    virtual float totalWeight() const {return totalWeight_;}
    virtual void setTotalWeight(float weight){totalWeight_ = weight;}

    virtual void create(const QSize &menuSize, const QStringList &value);

signals:
    void clicked(); //Emit signal to mainwindow

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    int x_;
    int y_;
    int w_;
    int h_;

    int type_;
    QString val1_;
    QString val2_;
    QString val3_;


    QString text_;
    int textHAlign_;
    int textVAlign_;
    QColor textColor_;
    QString textFontFamily_;
    int textFontPointSize_;

    QColor backgroundColor_;
    QString backgroundImage_;

    QString foregroundImage_;

    float totalWeight_;
    QPixmap invertPixmap(const QPixmap * src);

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPEEDKEY_H
