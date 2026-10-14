#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "XpdfItem.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  XpdfItem::init();

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/test1.qml")));

  return app.exec();
}
