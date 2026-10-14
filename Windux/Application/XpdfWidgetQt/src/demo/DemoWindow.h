//========================================================================
//
// DemoWindow.h
//
// Copyright 2009 Glyph & Cog, LLC
//
//========================================================================

#ifndef DEMOWINDOW_H
#define DEMOWINDOW_H

#include <QWidget>

class QComboBox;
class QHBoxLayout;
class QLabel;
class QProgressDialog;
class QPushButton;
class QVBoxLayout;

class XpdfWidget;

//------------------------------------------------------------------------

class DemoWindow: public QWidget {
  Q_OBJECT

public:

  DemoWindow();
  virtual ~DemoWindow();

  void loadPDFFile(const char *pdfFileName);

  virtual QSize sizeHint() const { return QSize(800, 600); }

private slots:

  void open();
  void prevPage();
  void nextPage();
  void zoomMenuChanged(int idx);
  void modeMenuChanged(int idx);
  void find();
  void copy();
  void paintDone(bool finished);
#if XPDFWIDGET_PRINTING
  void print();
  void printStatus(int nextPage, int firstPage, int lastPage);
  void cancelPrint();
#endif
  void midPageChange(int pageNum);
  void keyPress(QKeyEvent *e);

private:

  QVBoxLayout *vbox;
  QHBoxLayout *hbox;
  QLabel *demoLabel;
  QPushButton *openBtn;
  QPushButton *prevPageBtn;
  QPushButton *nextPageBtn;
  QComboBox *zoomMenu;
  QComboBox *modeMenu;
  QPushButton *findBtn;
  QPushButton *copyBtn;
#if XPDFWIDGET_PRINTING
  QPushButton *printBtn;
  QProgressDialog *printProgressDlg;
#endif
  XpdfWidget *pdf;
  QLabel *pageNumLabel;
};

#endif
