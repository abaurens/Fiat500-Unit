#pragma once

#include "dbus/Types.hpp"

#include "dbus/bluez/Adapter.hpp"
#include "dbus/bluez/Device.hpp"

#include <QObject>
#include <QDBusConnection>

/// List of all bluez objects the Manager class supports.
/// Extend this list and add overloads of `addObject`,
/// `removeObject` and `getObject` to support new types.
#define BLUEZ_OBJECT_TYPELIST \
  Adapter,                    \
  Device

namespace DBus::Bluez
{
  class Manager : public QObject
  {
    Q_OBJECT

  // Public static APi
  public:
    static Manager &instance();

    static bool initialize() { return instance().initializeInstance(); }
    static Adapter *adapter() { return instance().m_adapter; }
    static DeviceMap &devices() { return instance().m_devices; }

  // public signals
  signals:
    void deviceAdded(const QDBusObjectPath &path, DBus::Bluez::Device &device);
    void deviceRemoved(const QDBusObjectPath &path);

  // supported objects implementation
  private:
    void addObject(Adapter &adapter);
    void removeObject(Adapter &device);
    bool getObject(const QDBusObjectPath &path, Adapter *(&adapter));

    void addObject(Device &device);
    void removeObject(Device &device);
    bool getObject(const QDBusObjectPath &path, Device *(&device));

  /**************************\
  |* Implementation details *|
  \**************************/

  // Private members
  private:
    QDBusConnection m_bus;
    Adapter        *m_adapter = nullptr;
    DeviceMap       m_devices;

  // private constructor (singleton)
  private:
    explicit Manager(QObject *parent = nullptr);

  private slots:
    void onInterfacesAdded(const QDBusObjectPath &path, const InterfaceMap &interfaces);
    void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

  // Init functions
  private:
    bool initializeInstance();
    ManagedObjectMap loadManagedObjects();

  // Automatic dynamic object creation system
  private:
    size_t createObjects(const QDBusObjectPath &path, const InterfaceMap &interfaces)
    {
      return tryCreateAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }

    size_t removeObjects(const QDBusObjectPath &path, const QStringList &interfaces)
    {
      return tryRemoveAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }

    template<class ... ObjTypes>
    size_t tryCreateAll(const QDBusObjectPath &path, const InterfaceMap &interfaces)
    {
      // Try to create every given type. Return the number of successfully created objects.
      return (... + static_cast<std::size_t>(tryCreate<ObjTypes>(path, interfaces)));
    }

    template<class ... ObjTypes>
    size_t tryRemoveAll(const QDBusObjectPath &path, const QStringList &interfaces)
    {
      // Try to remove every given type. Return the number of successfully deleted objects.
      return (... + static_cast<std::size_t>(tryRemove<ObjTypes>(path, interfaces)));
    }

    template<class ObjType>
    size_t  tryCreate(const QDBusObjectPath &path, const InterfaceMap &interfaces)
    {
      if (!interfaces.contains(ObjType::InterfaceName))
        return 0;

      qDebug() << "Creating Object" << path.path() << "of type" << ObjType::InterfaceName;

      ObjType *obj = new ObjType(path, interfaces, this);

      addObject(*obj);

      return 1;
    }

    template<class ObjType>
    size_t  tryRemove(const QDBusObjectPath &path, const QStringList &interfaces)
    {
      if (!interfaces.contains(ObjType::InterfaceName.toQString()))
        return 0;

      qDebug() << "Trying to remove Object" << path.path() << "of type" << ObjType::InterfaceName;

      ObjType *obj = nullptr;
      if (getObject(path, obj))
        removeObject(*obj);

      return 1;
    }

  };
}
