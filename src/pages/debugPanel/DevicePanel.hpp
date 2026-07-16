#pragma once

#include "dbus/bluez/Device.hpp"

#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>

class DevicePanel : public QGroupBox
{
  Q_OBJECT

  using Device = DBus::Bluez::Device;

public:
  explicit DevicePanel(QWidget *parent = nullptr);

  Device *selectedDevice() const { return m_selectedDevice; }

signals:
  void deviceSelected(DBus::Bluez::Device *device);

private slots:
  void addDevice(const DBus::Object::Path &path, DBus::Bluez::Device &device);
  void removeDevice(const DBus::Object::Path &path);

  void selectDevice(DBus::Bluez::Device *device);
  void onSelectDevice(DBus::Bluez::Device *device);

  void selectedDeviceAliasChanged(const QString &alias);
  // void selectedDeviceRSSIChanged(const QString &alias);
  void selectedDevicePairedChanged(bool paired);
  void selectedDeviceTrustedChanged(bool trusted);
  void selectedDeviceConnectedChanged(bool connected);
  void selectedDeviceServicesResolvedChanged(bool servicesResolved);

private:
  void connectDevice(Device *device);
  void disconnectDevice(const Device *const device);

private:
  Device *m_selectedDevice = nullptr;

  QHash<DBus::Object::Path, QListWidgetItem *> m_items;

private:
  QListWidget *m_deviceList;

  /// Device details
  QGroupBox *m_detailsPanel;
  QLabel    *m_deviceName;
  QLabel    *m_deviceAddress;
  QLabel    *m_deviceRSSI;
  QCheckBox *m_devicePaired;
  QCheckBox *m_devicetrusted;
  QCheckBox *m_deviceConnected;
  QCheckBox *m_deviceServicesResolved;

  QPushButton *m_pairDevice;
  QPushButton *m_connectDevice;
  QPushButton *m_disconnectDevice;
  QPushButton *m_trustDevice;
  QPushButton *m_untrustDevice;
};
