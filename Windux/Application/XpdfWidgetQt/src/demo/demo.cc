#include <QApplication>
#include "DemoWindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  DemoWindow *win = new DemoWindow();
  if (argc >= 2) {
    win->loadPDFFile(argv[1]);
  }
  win->show();

  return QApplication::exec();
}
