#pragma once

#include "dbus/bluez/Adapter.hpp"
#include "dbus/bluez/Device.hpp"

#include <QObject>
#include <QDBusConnection>

class BluetoothManager : public QObject
{
  Q_OBJECT

public:
  explicit BluetoothManager(QObject *parent = nullptr);

  bool initialize();

  DBus::Bluez::Adapter *adapter() const { return m_adapter; }

private slots:
  void onInterfacesAdded(const QDBusObjectPath &path, const QVariantMap &interfaces);
  void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

private:
  ManagedObjectMap loadManagedObjects();

private:
  QDBusConnection m_bus;
  DBus::Bluez::Adapter *m_adapter = nullptr;
  QMap<QDBusObjectPath, DBus::Bluez::Device *> m_devices;
};
