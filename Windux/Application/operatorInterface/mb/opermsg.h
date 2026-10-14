#ifndef OPERMSG_H
#define OPERMSG_H

#include <QDialog>

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

    virtual void setLogoVisible( bool value );

    virtual void setMessage( const QString& value );
    virtual QString message() const;

    virtual void setMessageFontFamily( const QString& value );
    virtual QString messageFontFamily() const;

    virtual void setMessageFontPointSize( int value );
    virtual int messageFontPointSize() const;

    virtual void setTimer( int value );

    static QSize defaultSize();
    static bool isSlicerDevice();

protected:

    virtual void resizeEvent( QResizeEvent *e );

private:

    QTimer *timer_;

    QLabel *logo_;
    QLabel *message_;

    void initialize();
    void createLayout();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPERMSG_H
