#ifndef OPERMSG_H
#define OPERMSG_H

#include <QDialog>
#include <QIcon>

class QLabel;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////
class operMsg : public QDialog
{    
    typedef operMsg _Myt;
    typedef QDialog _Mybase;

    Q_OBJECT
    
public:

    operMsg( QWidget *parent = 0 );
    virtual ~operMsg();

    virtual void setMessage( const QString& value );
    virtual QString message() const;

    virtual void setMessageFontFamily( const QString& value );
    virtual QString messageFontFamily() const;

    virtual void setMessageFontPointSize( int value );
    virtual int messageFontPointSize() const;

    virtual void setTimer( int value );

    static QSize defaultSize();

protected:

    virtual void resizeEvent( QResizeEvent *e );

public slots:

    void ppButtonToggle(bool isDown);
    void scButtonToggle(bool isDown);
    void okayButtonClick();
private:

    QTimer *timer_;

    QLabel *logo_;
    QLabel *message_;
    QPushButton *serviceCounterButton_;
    QPushButton *prePackButton_;
    QPushButton *okayButton_;

    QIcon checkOnIcon;
    QIcon noIcon;

    void initialize();
    void createLayout();
    bool changeScaleModel(QString modelNumber);
    bool writeContentsToFile(QString fileName, QStringList theContents);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPERMSG_H
