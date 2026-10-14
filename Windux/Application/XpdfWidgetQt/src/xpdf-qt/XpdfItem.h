//========================================================================
//
// XpdfItem.h
//
// Copyright 2016 Glyph & Cog, LLC
//
//========================================================================

#ifndef XPDFITEM_H
#define XPDFITEM_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <QtQuick/QQuickItem>
#include <QAbstractItemModel>

class QMutex;
class QTimer;
class QtQuickPDFCore;
class XpdfOutlineModel;

typedef void *XpdfOutlineHandle;

//------------------------------------------------------------------------

class XpdfItem: public QQuickItem {
  Q_OBJECT

  Q_PROPERTY(int flickX            READ flickX         WRITE setFlickX)
  Q_PROPERTY(int flickY            READ flickY         WRITE setFlickY)
  Q_PROPERTY(int flickWidth        READ flickWidth     WRITE setFlickWidth)
  Q_PROPERTY(int flickHeight       READ flickHeight    WRITE setFlickHeight)
  Q_PROPERTY(QString src           READ src            WRITE setSrc)
  Q_PROPERTY(double zoom           READ zoom           WRITE setZoom)
  Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor)

public:

  XpdfItem(QQuickItem *parent = 0);
  virtual ~XpdfItem();

  int flickX() { return mFlickX; }
  void setFlickX(int x);
  int flickY() { return mFlickY; }
  void setFlickY(int y);
  int flickWidth() { return mFlickWidth; }
  void setFlickWidth(int w);
  int flickHeight() { return mFlickHeight; }
  void setFlickHeight(int h);
  void updateScrollParams(int w, int h, int x, int y);
  QString src();
  double zoom();
  void setZoom(double z);
  void setSrc(QString fileName);
  void setSelectionColor(QColor color);

  Q_INVOKABLE int loadFile(QString fileName);
  Q_INVOKABLE int loadFileWithPassword(QString fileName, QString password);
  Q_INVOKABLE int getNumPages();
  Q_INVOKABLE int getCurrentPage();
  Q_INVOKABLE int getMidPage();
  Q_INVOKABLE void gotoPage(int pageNum);
  Q_INVOKABLE void gotoNextPage();
  Q_INVOKABLE void gotoPreviousPage();
  Q_INVOKABLE void scrollToCentered(int page, double x, double y);
  Q_INVOKABLE bool hasSelection();
  Q_INVOKABLE int getCurrentSelectionPage();
  Q_INVOKABLE double getCurrentSelectionX0();
  Q_INVOKABLE double getCurrentSelectionY0();
  Q_INVOKABLE double getCurrentSelectionX1();
  Q_INVOKABLE double getCurrentSelectionY1();
  Q_INVOKABLE QColor selectionColor();
  Q_INVOKABLE bool find(QString text, int flags);
  Q_INVOKABLE QAbstractItemModel *getOutlineModel();
  Q_INVOKABLE QString getOutlineTitle(const QModelIndex &idx);
  Q_INVOKABLE bool getOutlineStartsOpen(const QModelIndex &idx);
  Q_INVOKABLE int getOutlineTargetPage(const QModelIndex &idx);
  Q_INVOKABLE void gotoOutlineTarget(const QModelIndex &idx);

  Q_INVOKABLE void mouseClick(int btn, int x, int y);
  Q_INVOKABLE void pinchStarted(int xCenter, int yCenter, double scale);
  Q_INVOKABLE void pinchUpdated(int xCenter, int yCenter, double scale);

  void update();

  QSGNode *updatePaintNode(QSGNode *oldNode,
			   UpdatePaintNodeData *updatePaintNodeData);

  static void init(const QString &configFileName = QString());

protected:

  virtual void keyPressEvent(QKeyEvent *e);

signals:

  void pageChange(int pageNum);
  void midPageChange(int pageNum);
  void keyPress(QKeyEvent *e);
  void updateFlickable(int w, int h, int x, int y);
  void tileDone();

private slots:

  void tick();

private:

  static void pageChangedCbk(void *data, int pageNum);
  static void midPageChangedCbk(void *data, int pageNum);
  static void tileDoneCbk(void *data);
  static void preLoadCbk(void *data);
  static void postLoadCbk(void *data);

  static QMutex initMutex;

  int mFlickX, mFlickY, mFlickWidth, mFlickHeight;
  bool flickableScrolling;
  double pinchStartZoom;

  QtQuickPDFCore *core;

  QTimer *tickTimer;

  XpdfOutlineModel *outlineModel;
};

#endif // XPDFITEM_H
