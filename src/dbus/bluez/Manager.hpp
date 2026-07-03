#pragma once

#include "dbus/Types.hpp"

#include "dbus/bluez/Adapter.hpp"
#include "dbus/bluez/Device.hpp"

#include <QObject>
#include <QDBusConnection>

namespace DBus::Bluez
{
  class Manager : public QObject
  {
    Q_OBJECT

  public:
    static Manager &instance();

    static bool initialize() { return instance().initializeInstance(); }
    static Adapter *adapter() { return instance().m_adapter; }
    static DeviceMap &devices() { return instance().m_devices; }

  signals:
    void deviceAdded(const QDBusObjectPath &path, DBus::Bluez::Device &device);
    void deviceRemoved(const QDBusObjectPath &path);

  private:
    explicit Manager(QObject *parent = nullptr);

  private slots:
    void onInterfacesAdded(const QDBusObjectPath &path, const InterfaceMap &interfaces);
    void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

  private:
    bool initializeInstance();
    ManagedObjectMap loadManagedObjects();

    Device &addDevice(const QDBusObjectPath &path, const InterfaceMap &interfaces);

    void    removeDevice(const QDBusObjectPath &path);
    void    removeDevice(const Device &path);


  private:
    QDBusConnection m_bus;
    Adapter *m_adapter = nullptr;
    DeviceMap m_devices;
  };
}
