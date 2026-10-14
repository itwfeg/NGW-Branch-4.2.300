//========================================================================
//
// DemoWindow.cc
//
// Copyright 2009-2019 Glyph & Cog, LLC
//
//========================================================================

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#if XPDFWIDGET_PRINTING
#include <QProgressDialog>
#endif
#include <QPushButton>
#include <QVBoxLayout>
#include "gmempp.h"
#include "XpdfWidget.h"
#include "DemoWindow.h"

DemoWindow::DemoWindow() {
  demoLabel = new QLabel("XpdfViewer/Qt demo app ::: Copyright 2019 Glyph & Cog, LLC ::: www.glyphandcog.com");

  openBtn = new QPushButton("Open...");
  prevPageBtn = new QPushButton("<");
  nextPageBtn = new QPushButton(">");
  zoomMenu = new QComboBox();
  zoomMenu->addItem("50%", 50.0);
  zoomMenu->addItem("100%", 100.0);
  zoomMenu->addItem("125%", 125.0);
  zoomMenu->addItem("150%", 150.0);
  zoomMenu->addItem("200%", 200.0);
  zoomMenu->addItem("page", XpdfWidget::zoomToPage);
  zoomMenu->addItem("width", XpdfWidget::zoomToWidth);
  zoomMenu->addItem("height", XpdfWidget::zoomToHeight);
  zoomMenu->setCurrentIndex(2);
  modeMenu = new QComboBox();
  modeMenu->addItem("single", XpdfWidget::pdfDisplaySingle);
  modeMenu->addItem("continuous", XpdfWidget::pdfDisplayContinuous);
  modeMenu->addItem("side-by-side single", XpdfWidget::pdfDisplaySideBySideSingle);
  modeMenu->addItem("side-by-side continuous", XpdfWidget::pdfDisplaySideBySideContinuous);
  modeMenu->addItem("horizontal continuous", XpdfWidget::pdfDisplayHorizontalContinuous);
  modeMenu->setCurrentIndex(1);
  findBtn = new QPushButton("Find...");
  copyBtn = new QPushButton("Copy");
#if XPDFWIDGET_PRINTING
  printBtn = new QPushButton("Print...");
#endif
  pageNumLabel = new QLabel("page: ");
  hbox = new QHBoxLayout();
  hbox->addWidget(openBtn);
  hbox->addStretch(1);
  hbox->addWidget(prevPageBtn);
  hbox->addWidget(nextPageBtn);
  hbox->addWidget(zoomMenu);
  hbox->addWidget(modeMenu);
  hbox->addStretch(1);
  hbox->addWidget(findBtn);
  hbox->addWidget(copyBtn);
#if XPDFWIDGET_PRINTING
  hbox->addWidget(printBtn);
#endif
  hbox->addStretch(1);
  hbox->addWidget(pageNumLabel);

  pdf = new XpdfWidget();
  pdf->setDisplayMode(XpdfWidget::pdfDisplayContinuous);
#if 1
  pdf->setLinearSelectMode();
#endif
#if 0
  pdf->enableTouchPan(true);
  pdf->enableTouchZoom(true);
#endif

  vbox = new QVBoxLayout();
  vbox->addWidget(demoLabel);
  vbox->addWidget(pdf);
  vbox->addLayout(hbox);
  setLayout(vbox);

  connect(openBtn, SIGNAL(clicked()), this, SLOT(open()));
  connect(prevPageBtn, SIGNAL(clicked()), this, SLOT(prevPage()));
  connect(nextPageBtn, SIGNAL(clicked()), this, SLOT(nextPage()));
  connect(zoomMenu, SIGNAL(currentIndexChanged(int)),
	  this, SLOT(zoomMenuChanged(int)));
  connect(modeMenu, SIGNAL(currentIndexChanged(int)),
	  this, SLOT(modeMenuChanged(int)));
  connect(findBtn, SIGNAL(clicked()), this, SLOT(find()));
  connect(copyBtn, SIGNAL(clicked()), this, SLOT(copy()));
  connect(pdf, SIGNAL(paintDone(bool)), this, SLOT(paintDone(bool)));
#if XPDFWIDGET_PRINTING
  connect(printBtn, SIGNAL(clicked()), this, SLOT(print()));
  connect(pdf, SIGNAL(printStatus(int,int,int)),
	  this, SLOT(printStatus(int,int,int)));
#endif
  connect(pdf, SIGNAL(midPageChange(int)), this, SLOT(midPageChange(int)));
  connect(pdf, SIGNAL(keyPress(QKeyEvent*)), this, SLOT(keyPress(QKeyEvent*)));
}

DemoWindow::~DemoWindow() {
}

void DemoWindow::loadPDFFile(const char *pdfFileName) {
  XpdfWidget::ErrorCode err;

  if ((err = pdf->loadFile(pdfFileName)) != XpdfWidget::pdfOk) {
    QMessageBox::warning(this, "DemoWindow warning",
			 "XpdfWidget::loadFile failed");
  }
}

void DemoWindow::open() {
  QString fileName;
  XpdfWidget::ErrorCode err;

  fileName = QFileDialog::getOpenFileName(this, "Open PDF File", QString(),
					  "PDF files (*.pdf)");
  if (!fileName.isEmpty()) {
    if ((err = pdf->loadFile(fileName)) != XpdfWidget::pdfOk) {
      QMessageBox::warning(this, "DemoWindow warning",
			   "XpdfWidget::loadFile failed");
    }
  }
}

void DemoWindow::prevPage() {
  pdf->gotoPreviousPage();
}

void DemoWindow::nextPage() {
  pdf->gotoNextPage();
}

void DemoWindow::zoomMenuChanged(int idx) {
  pdf->zoomCentered(zoomMenu->itemData(idx).toDouble());
}

void DemoWindow::modeMenuChanged(int idx) {
  pdf->setDisplayMode((XpdfWidget::DisplayMode)modeMenu->itemData(idx).toInt());
}

void DemoWindow::find() {
  QString s;
  bool ok;

  s = QInputDialog::getText(this, "Find", "Find text:",
			    QLineEdit::Normal, "", &ok, Qt::Dialog);
  if (ok) {
    pdf->find(s);
  }
}

void DemoWindow::copy() {
  pdf->copySelection();
}

void DemoWindow::paintDone(bool finished) {
#ifndef QT_NO_CURSOR
  if (finished) {
    unsetCursor();
  } else {
    setCursor(Qt::WaitCursor);
  }
#endif
}

#if XPDFWIDGET_PRINTING
void DemoWindow::print() {
  printProgressDlg = new QProgressDialog("Printing", "Abort",
					 1, pdf->getNumPages() + 1, this);
  connect(printProgressDlg, SIGNAL(canceled()), this, SLOT(cancelPrint()));
  printProgressDlg->setWindowModality(Qt::NonModal);
  printProgressDlg->setMinimumDuration(0);
  XpdfWidget::ErrorCode err = pdf->print(true);
  delete printProgressDlg;
  printProgressDlg = NULL;
  if (err != XpdfWidget::pdfOk) {
    QMessageBox::warning(this, "XpdfWidget Demo",
			 "Printing failed", QMessageBox::Ok);
  }
}

void DemoWindow::printStatus(int nextPage, int firstPage, int lastPage) {
  printProgressDlg->setValue(nextPage);
}

void DemoWindow::cancelPrint() {
  pdf->cancelPrint();
}
#endif // XPDFWIDGET_PRINTING

void DemoWindow::midPageChange(int pageNum) { 
 char buf[100];

  sprintf(buf, "page: %d", pageNum);
  pageNumLabel->setText(buf);
}

void DemoWindow::keyPress(QKeyEvent *e) {
  int key = e->key();
  if (key == Qt::Key_N) {
    pdf->gotoNextPage();
  } else if (key == Qt::Key_P) {
    pdf->gotoPreviousPage();
  }
}
