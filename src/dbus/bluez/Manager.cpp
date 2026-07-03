#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <QDBusMessage>

namespace DBG
{
  constexpr bool log_initialize = false;
  constexpr bool log_interface_added = false;
  constexpr bool log_interface_removed = false;
  constexpr bool log_device_added = true;
  constexpr bool log_device_removed = true;

}

#define LOG_DBG(_case, _msg) do { if constexpr (DBG::log_##_case) { qDebug() << _msg; } } while(0)

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

      /// TODO: I should probably build my Bluez::Objects with a
      /// factory instead of manually discriminating the types.
      if (interfaces.contains("org.bluez.Adapter1"))
      {
        m_adapter = new Adapter(it.key(), interfaces, this);

        LOG_DBG(initialize,
              "Adapter created:"
           << m_adapter->alias()
           << m_adapter->address()
           << m_adapter->powered()
        );
      }

      if (interfaces.contains("org.bluez.Device1"))
      {
        //Device *device = new Device(it.key(), interfaces, this);
        //m_devices.insert(it.key(), device);

        addDevice(it.key(), interfaces);

        //LOG_DBG(initialize,
        //    "Device found:"
        //  << device->alias()
        //  << device->address()
        //  << device->connected()
        //);
      }
    }

    m_bus.connect(
      Service, "/",
      "org.freedesktop.DBus.ObjectManager",
      "InterfacesAdded",
      this, SLOT(onInterfacesAdded(QDBusObjectPath, InterfaceMap))
    );
    m_bus.connect(
      Service, "/",
      "org.freedesktop.DBus.ObjectManager",
      "InterfacesRemoved",
      this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList))
    );

    return true;
  }

  ManagedObjectMap Manager::loadManagedObjects()
  {
    const QDBusMessage message = QDBusMessage::createMethodCall(
      "org.bluez", "/",
      "org.freedesktop.DBus.ObjectManager",
      "GetManagedObjects"
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

    if (interfaces.contains(Device::InterfaceName))
    {
      LOG_DBG(interface_added, "Device connected:" << path.path());

      if constexpr (DBG::log_interface_added)
      {
        if (interfaces.isEmpty())
          LOG_DBG(interface_added, "Interfaces: [ EMPTY ]");
        else
          LOG_DBG(interface_added, "Interfaces:");
        for (auto it = interfaces.cbegin(); it != interfaces.cend(); ++it)
        {
          LOG_DBG(interface_added, "  [" << it.key() << "] = " << it.value());
        }
      }

      if constexpr (DBG::log_interface_added)
      {
        const PropertyMap &properties = interfaces.value("org.bluez.Device1");

        if (properties.isEmpty())
          LOG_DBG(interface_added, "Device properties: [ EMPTY ]");
        else
          LOG_DBG(interface_added, "Device properties:");
        for (auto it = properties.cbegin(); it != properties.cend(); ++it)
          LOG_DBG(interface_added, "  [" << it.key() << "] = " << it.value());
      }

      addDevice(path, interfaces);
    }

    if (interfaces.contains(Adapter::InterfaceName))
    {
      LOG_DBG(interface_added, "Adapter appeared:" << path.path());
      LOG_DBG(interface_added, "  We currently only support 1 adapter...");
    }
  }

  void Manager::onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces)
  {
    LOG_DBG(interface_removed, "Removed:" << path.path());

    if (interfaces.contains(Device::InterfaceName.toQString()))
      removeDevice(path);
  }

  Device &Manager::addDevice(const QDBusObjectPath &path, const InterfaceMap &interfaces)
  {
    Device *device = new Device(path, interfaces, this);

    m_devices.insert(path, device);

    LOG_DBG(device_added,
        "Device added:"
      << device->alias()
      << device->address()
      << device->connected()
    );

    emit deviceAdded(path, *device);
    return *device;
  }

  void Manager::removeDevice(const QDBusObjectPath &path)
  {
    auto it = m_devices.find(path);

    if (it == m_devices.end())
      return;


    LOG_DBG(device_removed, "Device removed:" << path.path());

    emit deviceRemoved(path);

    delete *it;
    m_devices.erase(it);
  }

  void Manager::removeDevice(const Device &device)
  {
    return removeDevice(device.path());
  }
}
