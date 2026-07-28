#include "dbus/bluez/Manager.hpp"

#include "DebugPanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

DebugPanel::DebugPanel(QWidget *parent) : QWidget(parent)
{

  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *topLayout = new QHBoxLayout();

  setLayout(mainLayout);

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

  /// This is messy. I think maybe Device and MediaControl should both be in a single object
  connect(
    &DBus::Bluez::Manager::instance(), &DBus::Bluez::Manager::mediaControlAdded,
    [this](const DBus::Object::Path &path, DBus::Bluez::MediaControl &controler)
    {
      if (m_mediaPanel->mediaControler() == nullptr)
      {
        DBus::Bluez::Device *device = m_devicePanel->selectedDevice();
        if (!device || device->path() != controler.path())
          return;

        m_mediaPanel->setMediaControler(&controler);
      }
    }
  );

  connect(
    &DBus::Bluez::Manager::instance(), &DBus::Bluez::Manager::mediaControlRemoved,
    [this](const DBus::Object::Path &path)
    {
      const DBus::Bluez::MediaControl *controler = m_mediaPanel->mediaControler();

      if (controler && controler->path() == path)
        m_mediaPanel->setMediaControler(nullptr);
    }
  );

  connect(
    m_devicePanel, &DevicePanel::deviceSelected,
    [this](DBus::Bluez::Device *device)
    {
      qDebug() << "Selected device for audio playback" << (device ? device->path().path() : "nullptr");
      m_mediaPanel->setMediaControler(Manager::getObject<MediaControl>(device));
    }
  );
}

DebugPanel::~DebugPanel() = default;
