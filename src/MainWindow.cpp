#include "dbus/bluez/Manager.hpp"

#include "MainWindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *topLayout = new QHBoxLayout();

  setCentralWidget(centralWidget);
  centralWidget->setLayout(mainLayout);

  m_adapterPanel = new AdapterPanel(*DBus::Bluez::Manager::adapter(), this);
  m_devicePanel = new DevicePanel(this);
  m_mediaPanel = new MediaPanel(this);

  topLayout->addWidget(m_adapterPanel);
  topLayout->addWidget(m_mediaPanel);

  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(m_devicePanel);

  using DBus::Bluez::Manager;
  using DBus::Bluez::MediaControl;

  m_mediaPanel->setMediaControler(Manager::getObject<MediaControl>(m_devicePanel->selectedDevice()));

  connect(
    m_devicePanel, &DevicePanel::deviceSelected,
    [this](DBus::Bluez::Device *device)
    {
      m_mediaPanel->setMediaControler(Manager::getObject<MediaControl>(device));
    }
  );
}

MainWindow::~MainWindow() = default;
