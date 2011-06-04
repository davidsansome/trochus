#include <QApplication>

#include "logging.h"
#include "mainwindow.h"

int main(int argc, char** argv) {
  logging::Init();

  QApplication a(argc, argv);
  a.setApplicationName("Trochus");
  a.setOrganizationName("Trochus");
  a.setOrganizationDomain("davidsansome.com");

  MainWindow w;
  w.show();

  return a.exec();
}
