#include "dbus/bluez/BluetoothManager.hpp"
#include "dbus/Types.hpp"

#include <QDBusMessage>

BluetoothManager::BluetoothManager(QObject *parent)
  : QObject{ parent }, m_bus(QDBusConnection::systemBus())
{}

bool BluetoothManager::initialize()
{
  if (!m_bus.isConnected())
  {
    qDebug() << "Unable to connect to system bus";
    return false;
  }

  qDebug() << "Connected to system bus";

  ManagedObjectMap objects = loadManagedObjects();

  qDebug() << "Objects:" << objects.size();

  for (auto it = objects.cbegin(); it != objects.cend(); ++it)
  {
    const InterfaceMap &interfaces = it.value();

    /// TODO: I should probably build my Bluez::Objects with a
    /// factory instead of manually discriminating the types.
    if (interfaces.contains("org.bluez.Adapter1"))
    {
      m_adapter = new DBus::Bluez::Adapter(it.key(), interfaces, this);

      qDebug() << "Adapter created:"
               << m_adapter->alias()
               << m_adapter->address()
               << m_adapter->powered();
    }

    if (interfaces.contains("org.bluez.Device1"))
    {
      auto *device = new DBus::Bluez::Device(it.key(), interfaces, this);
      m_devices.insert(it.key(), device);

      qDebug() << "Device found:"
               << device->alias()
               << device->address()
               << device->connected();
    }
  }

  m_bus.connect(
    DBus::Bluez::Service, "/",
    "org.freedesktop.DBus.ObjectManager",
    "InterfacesAdded",
    this, SLOT(onInterfacesAdded(QDBusObjectPath, QVariantMap))
  );
  m_bus.connect(
    DBus::Bluez::Service, "/",
    "org.freedesktop.DBus.ObjectManager",
    "InterfacesRemoved",
    this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList))
  );

  return true;
}

ManagedObjectMap BluetoothManager::loadManagedObjects()
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

void BluetoothManager::onInterfacesAdded(const QDBusObjectPath &path, const QVariantMap &interfaces)
{
  qDebug() << "Added:" << path.path();

  if (interfaces.contains("org.bluez.Device1"))
  {
    qDebug() << "Device connected:" << path.path();

    PropertyMap properties = interfaces.value("org.bluez.Device1").toMap();

    auto *device = new DBus::Bluez::Device(path, properties, this);
    m_devices.insert(path, device);

    qDebug() << "Device properties:";
    for (auto it = properties.cbegin(); it != properties.cend(); ++it)
      qDebug() << "[" << it.key() << "] = " << it.value();

    qDebug() << "\nDevice created:"
             << device->alias()
             << device->address()
             << device->connected();
  }

  if (interfaces.contains("org.bluez.Adapter1"))
  {
    qDebug() << "Adapter appeared:" << path.path();
  }
}

void BluetoothManager::onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces)
{
  qDebug() << "Removed:" << path.path();

  if (interfaces.contains("org.bluez.Device1"))
  {
    qDebug() << "Device removed:" << path.path();

    auto it = m_devices.find(path);

    if (it != m_devices.end())
    {
      delete *it;
      m_devices.erase(it);
    }
  }
}
