#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <QDBusMessage>

namespace DBG
{
  constexpr bool log_initialize = true;
  constexpr bool log_interface_added = false;
  constexpr bool log_interface_removed = false;

  constexpr bool log_adapter_added = false;
  constexpr bool log_adapter_removed = false;
  constexpr bool log_device_added = false;
  constexpr bool log_device_removed = false;
  constexpr bool log_control_added = true;
  constexpr bool log_control_removed = true;
}

#define LOG_DBG(_case, _msg) do { if constexpr (DBG::log_##_case) { qDebug() << _msg; } } while(0)

// Bluez::Object types support
namespace DBus::Bluez
{
  // Adapter object
  void Manager::addObject(Adapter &adapter)
  {
    // We don't want to replace the current adapter.
    if (m_adapter)
    {
      LOG_DBG(interface_added, "Adapter appeared:" << adapter.path());
      LOG_DBG(interface_added, "  We currently only support 1 adapter...");
      return;
    }

    LOG_DBG(adapter_added,
            "Adapter set:"
              << adapter.alias()
              << adapter.address()
            );

    m_adapter = &adapter;
  }

  void Manager::removeObject(Adapter &adapter)
  {
    const Object::Path &path = adapter.path();

    if (!m_adapter || path != m_adapter->path())
      return;

    LOG_DBG(adapter_added, "Adapter removed:" << adapter.path().path());

    delete m_adapter;
    m_adapter = nullptr;
  }

  bool Manager::getObject(const Object::Path &path, Adapter *(&adapter))
  {
    if (!m_adapter || path != m_adapter->path())
      return false;

    adapter = m_adapter;
    return true;
  }

  // Device object
  void Manager::addObject(Device &device)
  {
    m_devices.insert(device.path(), &device);

    LOG_DBG(device_added,
            "Device added:"
              << device.alias()
              << device.address()
              << device.connected()
            );

    emit deviceAdded(device.path(), device);
  }

  void Manager::removeObject(Device &device)
  {
    const Object::Path &path = device.path();

    auto it = m_devices.find(path);
    if (it == m_devices.end())
      return;

    LOG_DBG(device_removed, "Device removed:" << path.path());

    emit deviceRemoved(path);

    delete *it;
    m_devices.erase(it);
  }

  bool Manager::getObject(const Object::Path &path, Device *(&device))
  {
    auto it = m_devices.find(path);

    if (it == m_devices.end())
      return false;

    device = *it;
    return true;
  }

  // MediaControl object
  void Manager::addObject(MediaControl &controler)
  {
    m_mediaControls.insert(controler.path(), &controler);

    LOG_DBG(control_added,
        "MediaControl added:"
      << controler.path().path()
      << controler.connected()
    );

    emit mediaControlAdded(controler.path(), controler);
  }

  void Manager::removeObject(MediaControl &controler)
  {
    const Object::Path &path = controler.path();

    auto it = m_mediaControls.find(path);
    if (it == m_mediaControls.end())
      return;

    LOG_DBG(control_removed, "MediaControl removed:" << path.path());

    emit mediaControlRemoved(path);

    delete *it;
    m_mediaControls.erase(it);
  }

  bool Manager::getObject(const Object::Path &path, MediaControl *(&controler))
  {
    auto it = m_mediaControls.find(path);

    if (it == m_mediaControls.end())
      return false;

    controler = *it;
    return true;
  }

  // MediaControl object
  void Manager::addObject(MediaPlayer &player)
  {
    m_mediaPlayers.insert(player.path(), &player);

    LOG_DBG(control_added,
        "MediaPlayer added:"
      << (player.name().isEmpty() ? player.path().path() : player.name())
      << player.path().path()
    );

    emit mediaPlayerAdded(player.path(), player);
  }

  void Manager::removeObject(MediaPlayer &player)
  {
    const Object::Path &path = player.path();

    auto it = m_mediaPlayers.find(path);
    if (it == m_mediaPlayers.end())
      return;

    LOG_DBG(control_removed, "MediaPlayer removed:" << path.path());

    emit mediaPlayerRemoved(path);

    it.value()->deleteLater();
    m_mediaPlayers.erase(it);
  }

  bool Manager::getObject(const Object::Path &path, MediaPlayer *(&player))
  {
    auto it = m_mediaPlayers.find(path);

    if (it == m_mediaPlayers.end())
      return false;

    player = *it;
    return true;
  }
}

// Implementation details
namespace DBus::Bluez
{
  Manager &Manager::instance()
  {
    static Manager g_manager;

    return g_manager;
  }

  Manager::Manager(QObject *parent)
    : QObject{ parent }, m_bus(QDBusConnection::systemBus())
  {}

  bool Manager::initializeInstance()
  {
    if (!m_bus.isConnected())
    {
      qDebug() << "Unable to connect to system bus";
      return false;
    }

    LOG_DBG(initialize, "Connected to system bus");

    ManagedObjectMap objects = loadManagedObjects();

    LOG_DBG(initialize, "Objects:" << objects.size());

    for (auto it = objects.cbegin(); it != objects.cend(); ++it)
    {
      const InterfaceMap &interfaces = it.value();

      // Try to create the object with each of the passed types
      createObjects(it.key(), interfaces);
    }

    // DEBUG InterfaceMap operator
    {
      QDBusArgument arg;

      InterfaceMap map;

      arg << map;

      qDebug() << "ok";
    }

    m_bus.connect(
      Service, RootPath, Interface::ObjectManager, Method::InterfacesAdded,
      this, SLOT(onInterfacesAdded(QDBusObjectPath, InterfaceMap))
    );
    m_bus.connect(
      Service, RootPath, Interface::ObjectManager, Method::InterfacesRemoved,
      this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList))
    );

    return true;
  }

  ManagedObjectMap Manager::loadManagedObjects()
  {
    const QDBusMessage message = QDBusMessage::createMethodCall(
      Service, RootPath, Interface::ObjectManager, Method::GetManagedObjects
    );

    const QDBusMessage reply = m_bus.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
      qCritical() << reply.errorMessage();
      return {};
    }

    const QVariant first = reply.arguments().at(0);
    const QDBusArgument dbusArg = first.value<QDBusArgument>();

    ManagedObjectMap objects;
    dbusArg >> objects;

    return objects;
  }

  void Manager::onInterfacesAdded(const Object::Path &path, const InterfaceMap &interfaces)
  {
    LOG_DBG(interface_added, "Added:" << path.path());

    createObjects(path, interfaces);
  }

  void Manager::onInterfacesRemoved(const Object::Path &path, const QStringList &interfaces)
  {
    LOG_DBG(interface_removed, "Removed:" << path.path());

    removeObjects(path, interfaces);
  }
}
