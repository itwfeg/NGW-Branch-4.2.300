//========================================================================
//
// XpdfItem.cc
//
// Copyright 2016 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <QMutex>
#include <QTimer>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>
#include "config.h"
#include "GList.h"
#include "GlobalParams.h"
#include "ErrorCodes.h"
#include "PDFDoc.h"
#include "Outline.h"
#include "QtQuickPDFCore.h"
#include "XpdfItem.h"

//------------------------------------------------------------------------

// Time (in ms) between incremental updates.
#define incrementalUpdateInterval 100

// Flags for the find() function -- these are also defined in Xpdf.qml
#define findBackward      0x00000001
#define findCaseSensitive 0x00000002
#define findNext          0x00000004
#define findOnePageOnly   0x00000008
#define findWholeWord     0x00000010

//------------------------------------------------------------------------

QMutex XpdfItem::initMutex;

//------------------------------------------------------------------------
// XpdfOutlineModel
//------------------------------------------------------------------------

class XpdfOutlineModel: public QAbstractItemModel {
public:

  XpdfOutlineModel(PDFDoc *docA, QObject *parent = NULL);
  virtual QModelIndex index(int row, int column,
			    const QModelIndex &par = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex &idx) const;
  virtual int rowCount(const QModelIndex &par = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &par = QModelIndex()) const;
  virtual QVariant data(const QModelIndex &idx, int role) const;

  OutlineItem *getOutlineItem(const QModelIndex &idx) const;
  void beginOpenNewDoc();
  void endOpenNewDoc(PDFDoc *docA);

private:

  int getItemRow(OutlineItem *item) const;
  PDFDoc *doc;
};

XpdfOutlineModel::XpdfOutlineModel(PDFDoc *docA, QObject *parent)
  : QAbstractItemModel(parent)
{
  doc = docA;
}

QModelIndex XpdfOutlineModel::index(int row, int column,
				    const QModelIndex &par) const {
  OutlineItem *item;
  GList *children;

  if (par.isValid()) {
    // children of an outline item
    item = (OutlineItem *)par.internalPointer();
    item->open();
    if (item->getKids()) {
      children = item->getKids();
    } else {
      children = NULL;
    }
  } else {
    // root outline items
    if (doc && doc->getOutline() && doc->getOutline()->getItems()) {
      children = doc->getOutline()->getItems();
    } else {
      children = NULL;
    }
  }
  if (!children || row < 0 || row >= children->getLength() || column != 0) {
    return QModelIndex();
  }
  return createIndex(row, 0, children->get(row));
}

QModelIndex XpdfOutlineModel::parent(const QModelIndex &idx) const {
  OutlineItem *item, *par;
  int row;

  if (!idx.isValid()) {
    return QModelIndex();
  }
  item = (OutlineItem *)idx.internalPointer();
  if (!(par = item->getParent())) {
    return QModelIndex();
  }
  if ((row = getItemRow(par)) < 0) {
    return QModelIndex();
  }
  return createIndex(row, 0, par);
}

int XpdfOutlineModel::rowCount(const QModelIndex &par) const {
  OutlineItem *item;
  GList *children;

  if (par.isValid()) {
    // children of an outline item
    item = (OutlineItem *)par.internalPointer();
    item->open();
    if (item->getKids()) {
      children = item->getKids();
    } else {
      children = NULL;
    }
  } else {
    // root outline items
    if (doc && doc->getOutline() && doc->getOutline()->getItems()) {
      children = doc->getOutline()->getItems();
    } else {
      children = NULL;
    }
  }
  if (children) {
    return children->getLength();
  } else {
    return 0;
  }
}

int XpdfOutlineModel::columnCount(const QModelIndex &par) const {
  return 1;
}

QVariant XpdfOutlineModel::data(const QModelIndex &idx, int role) const {
  OutlineItem *item;
  Unicode *title;
  QString s;
  int titleLen, i;

  if (role != Qt::DisplayRole) {
    return QVariant();
  }
  item = (OutlineItem *)idx.internalPointer();
  title = item->getTitle();
  titleLen = item->getTitleLength();
  for (i = 0; i < titleLen; ++i) {
    s.append((QChar)title[i]);
  }
  return QVariant(s);
}

int XpdfOutlineModel::getItemRow(OutlineItem *item) const {
  OutlineItem *par;
  GList *children;
  int i;

  if (!doc) {
    return -1;
  }
  par = item->getParent();
  if (par) {
    children = item->getKids();
  } else {
    children = doc->getOutline()->getItems();
  }
  for (i = 0; i < children->getLength(); ++i) {
    if (item == (OutlineItem *)children->get(i)) {
      return i;
    }
  }
  return -1;
}

OutlineItem *XpdfOutlineModel::getOutlineItem(const QModelIndex &idx) const {
  return (OutlineItem *)idx.internalPointer();
}

void XpdfOutlineModel::beginOpenNewDoc() {
  doc = NULL;
  beginResetModel();
}

void XpdfOutlineModel::endOpenNewDoc(PDFDoc *docA) {
  doc = docA;
  endResetModel();
}

//------------------------------------------------------------------------

XpdfItem::XpdfItem(QQuickItem *parent):
  QQuickItem(parent)
{
  SplashColor paperColor, matteColor;

  init();

  setFlag(ItemHasContents, true);
  setActiveFocusOnTab(true);

  mFlickX = 0;
  mFlickY = 0;
  mFlickWidth = 0;
  mFlickHeight = 0;
  flickableScrolling = false;
  pinchStartZoom = 100;

  paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
  matteColor[0] = matteColor[1] = matteColor[2] = 0x80;
  core = new QtQuickPDFCore(this, paperColor, matteColor, gFalse);
  core->setPageChangedCbk(pageChangedCbk, this);
  core->setMidPageChangedCbk(midPageChangedCbk, this);
  core->setTileDoneCbk(tileDoneCbk, this);
  core->setPreLoadCbk(preLoadCbk, this);
  core->setPostLoadCbk(postLoadCbk, this);
  connect(this, SIGNAL(tileDone()), this, SLOT(update()));

  tickTimer = new QTimer(this);
  connect(tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
  tickTimer->start(incrementalUpdateInterval);

  outlineModel = NULL;
}

XpdfItem::~XpdfItem() {
  delete tickTimer;
  delete core;
}

void XpdfItem::init(const QString &configFileName) {
#ifdef _WIN32
  char buf[512];
  GString *dir, *path;
  char *configFileName2;
  int n;
#endif

  initMutex.lock();
  if (!globalParams) {
    qmlRegisterType<XpdfItem>("Xpdf", xpdfMajorVersion, xpdfMinorVersion,
			      "XpdfItem");

    try {
#ifdef _WIN32
      // get the executable directory
      n = GetModuleFileNameA(NULL, buf, sizeof(buf));
      if (n <= 0 || n >= sizeof(buf)) {
	// error or path too long for buffer - just use the current dir
	buf[0] = '\0';
      }
      dir = grabPath(buf);

      // load the config file
      path = NULL;
      if (configFileName.isEmpty()) {
	path = appendToPath(dir->copy(), xpdfSysConfigFile);
	configFileName2 = path->getCString();
      } else {
	configFileName2 = (char *)configFileName.toLocal8Bit().constData();
      }
      globalParams = new GlobalParams(configFileName2);
      globalParams->setBaseDir(dir->getCString());
      globalParams->setErrQuiet(gTrue);
      if (path) {
	delete path;
      }

      // set up the base fonts
      appendToPath(dir, "t1fonts");
      globalParams->setupBaseFonts(dir->getCString());
      delete dir;
#else
      globalParams = new GlobalParams(
			     (char *)configFileName.toLocal8Bit().constData());
      globalParams->setErrQuiet(gTrue);
      globalParams->setupBaseFonts(NULL);
#endif
    } catch (GMemException e) {
      // there's no way to return an error code here
      return;
    }
  }
  initMutex.unlock();
}

void XpdfItem::setFlickX(int x) {
  if (mFlickX == x) {
    return;
  }
  mFlickX = x;
  flickableScrolling = true;
  core->scrollTo(mFlickX, mFlickY);
  flickableScrolling = false;
}

void XpdfItem::setFlickY(int y) {
  if (mFlickY == y) {
    return;
  }
  mFlickY = y;
  flickableScrolling = true;
  core->scrollTo(mFlickX, mFlickY);
  flickableScrolling = false;
}

void XpdfItem::setFlickWidth(int w) {
  if (mFlickWidth == w) {
    return;
  }
  mFlickWidth = w;
  core->resize(mFlickWidth, mFlickHeight);
}

void XpdfItem::setFlickHeight(int h) {
  if (mFlickHeight == h) {
    return;
  }
  mFlickHeight = h;
  core->resize(mFlickWidth, mFlickHeight);
}

void XpdfItem::updateScrollParams(int w, int h, int x, int y) {
  // NB: flickableScrolling is set when we're processing a scroll that
  // was initiated by the Flickable -- this avoids loops when
  // modifying the Flickable contentX/Y parameters
  if (!flickableScrolling) {
    emit updateFlickable(w, h, x, y);
  }
}

QString XpdfItem::src() {
  try {
    if (!core->getDoc() || !core->getDoc()->getFileName()) {
      return QString();
    }
    return QString(core->getDoc()->getFileName()->getCString());
  } catch (GMemException e) {
    return QString();
  }
}

void XpdfItem::setSrc(QString fileName) {
  loadFileWithPassword(fileName, "");
}

int XpdfItem::loadFile(QString fileName) {
  return loadFileWithPassword(fileName, "");
}

int XpdfItem::loadFileWithPassword(QString fileName, QString password) {
  GString *passwordStr;
#ifdef _WIN32
  wchar_t *fileNameStr;
  int n, i;
#else
  GString *fileNameStr;
#endif
  int err;

  // QML may sometimes set the src property to an empty string
  if (fileName.isEmpty()) {
    return errNone;
  }

  // the QtQuick FileDialog returns a URL, so we check for a 'file:'
  // prefix here
  // - Windows: "file:///c:/foo.pdf" --> "c:/foo.pdf"
  // - Linux/Mac: "file:///home/user/foo.pdf" --> "/home/user/foo.pdf"
  if (fileName.startsWith("file:///")) {
#ifdef _WIN32
    fileName = fileName.right(fileName.size() - 8);
#else
    fileName = fileName.right(fileName.size() - 7);
#endif
  }

  try {
    if (password.isEmpty()) {
      passwordStr = NULL;
    } else {
      passwordStr = new GString(password.toLocal8Bit().constData());
    }
#ifdef _WIN32
    // this should use QString::toWCharArray(), but Qt builds their
    // library with /Zc:wchar_t-, which confuses things
    n = fileName.length();
    fileNameStr = (wchar_t *)gmallocn(n, sizeof(wchar_t));
    for (i = 0; i < n; ++i) {
      fileNameStr[i] = (wchar_t)fileName[i].unicode();
    }
    err = core->loadFile(fileNameStr, n, passwordStr, passwordStr);
    gfree(fileNameStr);
#else
    fileNameStr = new GString(fileName.toLocal8Bit().constData());
    err = core->loadFile(fileNameStr, passwordStr, passwordStr);
    delete fileNameStr;
#endif
    if (passwordStr) {
      delete passwordStr;
    }
    if (!err) {
      core->displayPage(1, gTrue, gFalse);
    }
    return err;
  } catch (GMemException e) {
    //~ should bring the error code constants in from xpdf-lib
    return -1;
  }
}

double XpdfItem::zoom() {
  try {
    return core->getZoom();
  } catch (GMemException e) {
    return 100;
  }
}

void XpdfItem::setZoom(double z) {
  try {
    core->zoomCentered(z);
  } catch (GMemException e) {
  }
}

QColor XpdfItem::selectionColor() {
  SplashColorPtr col;

  try {
    col = core->getSelectionColor();
    return QColor(col[0], col[1], col[2]);
  } catch (GMemException e) {
    return QColor();
  }
}

void XpdfItem::setSelectionColor(QColor color) {
  SplashColor col;

  try {
    col[0] = (Guchar)color.red();
    col[1] = (Guchar)color.green();
    col[2] = (Guchar)color.blue();
    core->setSelectionColor(col);
  } catch (GMemException e) {
  }
}

int XpdfItem::getNumPages() {
  try {
    if (!core->getDoc()) {
      return -1;
    }
    return core->getDoc()->getNumPages();
  } catch (GMemException e) {
    return -1;
  }
}

int XpdfItem::getCurrentPage() {
  try {
    if (!core->getDoc()) {
      return -1;
    }
    return core->getPageNum();
  } catch (GMemException e) {
    return -1;
  }
}

int XpdfItem::getMidPage() {
  try {
    if (!core->getDoc()) {
      return -1;
    }
    return core->getMidPageNum();
  } catch (GMemException e) {
    return -1;
  }
}

void XpdfItem::gotoPage(int pageNum) {
  try {
    core->displayPage(pageNum, gTrue, gFalse);
  } catch (GMemException e) {
  }
}

void XpdfItem::gotoNextPage() {
  try {
    core->gotoNextPage(1, gTrue);
  } catch (GMemException e) {
  }
}

void XpdfItem::gotoPreviousPage() {
  try {
    core->gotoPrevPage(1, gTrue, gFalse);
  } catch (GMemException e) {
  }
}

void XpdfItem::scrollToCentered(int page, double x, double y) {
  try {
    core->scrollToCentered(page, x, y);
  } catch (GMemException e) {
  }
}

bool XpdfItem::hasSelection() {
  try {
    return (bool)core->hasSelection();
  } catch (GMemException e) {
    return false;
  }
}

int XpdfItem::getCurrentSelectionPage() {
  int page;
  double x0, y0, x1, y1;

  try {
    if (!core->getSelection(&page, &x0, &y0, &x1, &y1)) {
      return 0;
    }
    return page;
  } catch (GMemException e) {
    return 0;
  }
}

double XpdfItem::getCurrentSelectionX0() {
  int page;
  double x0, y0, x1, y1;

  try {
    if (!core->getSelection(&page, &x0, &y0, &x1, &y1)) {
      return 0;
    }
    return x0;
  } catch (GMemException e) {
    return 0;
  }
}

double XpdfItem::getCurrentSelectionY0() {
  int page;
  double x0, y0, x1, y1;

  try {
    if (!core->getSelection(&page, &x0, &y0, &x1, &y1)) {
      return 0;
    }
    return y0;
  } catch (GMemException e) {
    return 0;
  }
}

double XpdfItem::getCurrentSelectionX1() {
  int page;
  double x0, y0, x1, y1;

  try {
    if (!core->getSelection(&page, &x0, &y0, &x1, &y1)) {
      return 0;
    }
    return x1;
  } catch (GMemException e) {
    return 0;
  }
}

double XpdfItem::getCurrentSelectionY1() {
  int page;
  double x0, y0, x1, y1;

  try {
    if (!core->getSelection(&page, &x0, &y0, &x1, &y1)) {
      return 0;
    }
    return y1;
  } catch (GMemException e) {
    return 0;
  }
}

bool XpdfItem::find(QString text, int flags) {
  Unicode *u;
  bool ret;
  int len, i;

  try {
    if (!core->getDoc()) {
      return false;
    }
    len = text.length();
    u = (Unicode *)gmallocn(len, sizeof(Unicode));
    for (i = 0; i < len; ++i) {
      u[i] = (Unicode)text[i].unicode();
    }
    ret = (bool)core->findU(u, len,
			    (flags & findCaseSensitive) ? gTrue : gFalse,
			    (flags & findNext) ? gTrue : gFalse,
			    (flags & findBackward) ? gTrue : gFalse,
			    (flags & findWholeWord) ? gTrue : gFalse,
			    (flags & findOnePageOnly) ? gTrue : gFalse);
    gfree(u);
    return ret;
  } catch (GMemException e) {
    return false;
  }
}

QAbstractItemModel *XpdfItem::getOutlineModel() {
  if (!outlineModel) {
    outlineModel = new XpdfOutlineModel(core->getDoc(), this);
  }
  return outlineModel;
}

QString XpdfItem::getOutlineTitle(const QModelIndex &idx) {
  Unicode *title;
  QString s;
  int i;

  OutlineItem *item = (OutlineItem *)outlineModel->getOutlineItem(idx);
  if (!item) {
    return QString();
  }
  try {
    title = item->getTitle();
    for (i = 0; i < item->getTitleLength(); ++i) {
      s.append((QChar)title[i]);
    }
    return s;
  } catch (GMemException e) {
    return QString();
  }
}

bool XpdfItem::getOutlineStartsOpen(const QModelIndex &idx) {
  OutlineItem *item = (OutlineItem *)outlineModel->getOutlineItem(idx);
  if (!item) {
    return false;
  }
  try {
    return (bool)item->isOpen();
  } catch (GMemException e) {
    return false;
  }
}

int XpdfItem::getOutlineTargetPage(const QModelIndex &idx) {
  OutlineItem *item = (OutlineItem *)outlineModel->getOutlineItem(idx);
  if (!item) {
    return 0;
  }
  try {
    if (!core->getDoc()) {
      return 0;
    }
    return core->getDoc()->getOutlineTargetPage(item);
  } catch (GMemException e) {
    return 0;
  }
}

void XpdfItem::gotoOutlineTarget(const QModelIndex &idx) {
  try {
    OutlineItem *item = (OutlineItem *)outlineModel->getOutlineItem(idx);
    if (!item || !item->getAction()) {
      return;
    }
    core->doAction(item->getAction());
  } catch (GMemException e) {
  }
}

void XpdfItem::keyPressEvent(QKeyEvent *e) {
  int key;

  key = e->key();
  if (key == Qt::Key_Left) {
    core->scrollLeft();
    return;
  } else if (key == Qt::Key_Right) {
    core->scrollRight();
    return;
  } else if (key == Qt::Key_Up) {
    core->scrollUp();
    return;
  } else if (key == Qt::Key_Down) {
    core->scrollDown();
    return;
  } else if (key == Qt::Key_PageUp) {
    core->scrollPageUp();
    return;
  } else if (key == Qt::Key_PageDown ||
	     key == Qt::Key_Space) {
    core->scrollPageDown();
    return;
  }
  emit keyPress(e);
}

void XpdfItem::mouseClick(int btn, int x, int y) {
  if (btn == Qt::LeftButton) {
    core->mouseClick(x, y);
  }
}

void XpdfItem::pinchStarted(int xCenter, int yCenter, double scale) {
  pinchStartZoom = core->getZoomDPI(core->getMidPageNum()) * 100.0 / 72.0;
}

void XpdfItem::pinchUpdated(int xCenter, int yCenter, double scale) {
  double newZoom = scale * pinchStartZoom;
  if (newZoom < 10) {
    newZoom = 10;
  } else if (newZoom > 800) {
    newZoom = 800;
  }
  core->zoomCentered(newZoom);
}

void XpdfItem::update() {
  QQuickItem::update();
}

QSGNode *XpdfItem::updatePaintNode(
			   QSGNode *oldNode,
			   UpdatePaintNodeData *updatePaintNodeData) {
  QSGSimpleTextureNode *node = (QSGSimpleTextureNode *)oldNode;
  if (!node) {
    node = new QSGSimpleTextureNode();
  }
  node->setRect(QRectF(mFlickX, mFlickY, mFlickWidth, mFlickHeight));

  QImage img = core->getImage();
  node->setTexture(window()->createTextureFromImage(img, 0));
  node->setOwnsTexture(true);

  node->markDirty(QSGNode::DirtyGeometry);

  return node;
}

void XpdfItem::tick() {
  core->tick();
}

void XpdfItem::pageChangedCbk(void *data, int pageNum) {
  XpdfItem *item = (XpdfItem *)data;
  emit item->pageChange(pageNum);
}

void XpdfItem::midPageChangedCbk(void *data, int pageNum) {
  XpdfItem *item = (XpdfItem *)data;
  emit item->midPageChange(pageNum);
}

// NB: this function is called on a worker thread.
void XpdfItem::tileDoneCbk(void *data) {
  XpdfItem *item = (XpdfItem *)data;

  emit item->tileDone();
}

void XpdfItem::preLoadCbk(void *data) {
  XpdfItem *item = (XpdfItem *)data;
  if (item->outlineModel) {
    item->outlineModel->beginOpenNewDoc();
  }
}

void XpdfItem::postLoadCbk(void *data) {
  XpdfItem *item = (XpdfItem *)data;
  if (item->outlineModel) {
    item->outlineModel->endOpenNewDoc(item->core->getDoc());
  }
}
