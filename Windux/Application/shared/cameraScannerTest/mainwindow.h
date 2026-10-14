#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <barcodetype.h>

#include <QList>
#include <QMainWindow>

class camera;

class QHBoxLayout;
class QWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

private Q_SLOTS:

    void onDecodedBarcodes( const barcodeList& barcodes );

private:

    QWidget *central_;
    QHBoxLayout *centralLayout_;

    void initialize();

    static QList<camera*> createCameras();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
