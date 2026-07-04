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
}

#define LOG_DBG(_case, _msg) do { if constexpr (DBG::log_##_case) { qDebug() << _msg; } } while(0)

// Bluez::Object types support
namespace DBus::Bluez
{
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
    const QDBusObjectPath &path = adapter.path();

    if (!m_adapter || path != m_adapter->path())
      return;

    LOG_DBG(adapter_added, "Adapter removed:" << adapter.path().path());

    delete m_adapter;
    m_adapter = nullptr;
  }

  bool Manager::getObject(const QDBusObjectPath &path, Adapter *(&adapter))
  {
    if (!m_adapter || path != m_adapter->path())
      return false;

    adapter = m_adapter;
    return true;
  }


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
    const QDBusObjectPath &path = device.path();

    auto it = m_devices.find(path);
    if (it == m_devices.end())
      return;

    LOG_DBG(device_removed, "Device removed:" << path.path());

    emit deviceRemoved(path);

    delete *it;
    m_devices.erase(it);
  }

  bool Manager::getObject(const QDBusObjectPath &path, Device *(&device))
  {
    auto it = m_devices.find(path);

    if (it == m_devices.end())
      return false;

    device = *it;
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

  void Manager::onInterfacesAdded(const QDBusObjectPath &path, const InterfaceMap &interfaces)
  {
    LOG_DBG(interface_added, "Added:" << path.path());

    //if (interfaces.contains(Device::InterfaceName))
    //{
    //  LOG_DBG(interface_added, "Device connected:" << path.path());
    //
    //  if constexpr (DBG::log_interface_added)
    //  {
    //    if (interfaces.isEmpty())
    //      LOG_DBG(interface_added, "Interfaces: [ EMPTY ]");
    //    else
    //      LOG_DBG(interface_added, "Interfaces:");
    //    for (auto it = interfaces.cbegin(); it != interfaces.cend(); ++it)
    //    {
    //      LOG_DBG(interface_added, "  [" << it.key() << "] = " << it.value());
    //    }
    //  }
    //
    //  if constexpr (DBG::log_interface_added)
    //  {
    //    const PropertyMap &properties = interfaces.value("org.bluez.Device1");
    //
    //    if (properties.isEmpty())
    //      LOG_DBG(interface_added, "Device properties: [ EMPTY ]");
    //    else
    //      LOG_DBG(interface_added, "Device properties:");
    //    for (auto it = properties.cbegin(); it != properties.cend(); ++it)
    //      LOG_DBG(interface_added, "  [" << it.key() << "] = " << it.value());
    //  }
    //
    //  addDevice(path, interfaces);
    //}

    createObjects(path, interfaces);
  }

  void Manager::onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces)
  {
    LOG_DBG(interface_removed, "Removed:" << path.path());

    removeObjects(path, interfaces);
  }
}
