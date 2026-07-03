#include "dbus/bluez/Manager.hpp"

#include "MainWindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout();

  setCentralWidget(centralWidget);
  centralWidget->setLayout(mainLayout);

  m_adapterPanel = new AdapterPanel(*DBus::Bluez::Manager::adapter(), this);
  m_devicePanel = new DevicePanel(this);

  mainLayout->addWidget(m_adapterPanel);
  mainLayout->addWidget(m_devicePanel);
}

MainWindow::~MainWindow() = default;
