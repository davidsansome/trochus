#ifndef MAINWINDOW_UI
#define MAINWINDOW_UI

#include <QMainWindow>

#include <boost/scoped_ptr.hpp>

class MessagingProcess;
class SupervisorManager;
class Ui_MainWindow;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = NULL);
  ~MainWindow();

private slots:
  void SupervisorManagerSocketReady(int);
  void RunCommand();

private:
  boost::scoped_ptr<Ui_MainWindow> ui_;

  boost::scoped_ptr<MessagingProcess> process_;
  boost::scoped_ptr<SupervisorManager> supervisor_manager_;
};

#endif // MAINWINDOW_UI
