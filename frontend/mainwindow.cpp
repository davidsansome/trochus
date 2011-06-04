#include "mainwindow.h"
#include "messagingprocess.h"
#include "supervisor.h"
#include "supervisormanager.h"
#include "ui_mainwindow.h"

#include <QSocketNotifier>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent),
    ui_(new Ui_MainWindow),
    process_(new MessagingProcess),
    supervisor_manager_(new SupervisorManager)
{
  ui_->setupUi(this);
  connect(ui_->command_line, SIGNAL(returnPressed()), SLOT(RunCommand()));

  QSocketNotifier* notifier = new QSocketNotifier(
        supervisor_manager_->fd(), QSocketNotifier::Read, this);
  connect(notifier, SIGNAL(activated(int)), SLOT(SupervisorManagerSocketReady(int)));
}

MainWindow::~MainWindow() {
}

void MainWindow::SupervisorManagerSocketReady(int) {
  supervisor_manager_->ProcessEvents();
}

void MainWindow::RunCommand() {
  QStringList args = ui_->command_line->text().split(QRegExp("\\s+"));
  ui_->command_line->clear();

  std::list<std::string> std_args;
  foreach (const QString& arg, args) {
    std_args.push_back(arg.toUtf8().constData());
  }

  Supervisor* sup = new Supervisor(std_args, supervisor_manager_.get(), process_.get());
  sup->Start();
}
