#include "sizes.hpp"
#include "DevicePanel.hpp"

#include "dbus/bluez/Manager.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

DevicePanel::DevicePanel(QWidget *parent)
  : QGroupBox{ tr("Devices"), parent }
{
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  QVBoxLayout *detailsLayout = new QVBoxLayout();
  QHBoxLayout *btnLine1Layout = new QHBoxLayout();
  QHBoxLayout *btnLine2Layout = new QHBoxLayout();

  m_deviceList = new QListWidget();
  m_deviceList->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

  m_detailsPanel = new QGroupBox(tr("Details"));
  {
    QHBoxLayout *deviceNameLayout = new QHBoxLayout();
    m_deviceName = new QLabel();
    deviceNameLayout->setSpacing(0);
    deviceNameLayout->addWidget(new QLabel(tr("Name :")));
    deviceNameLayout->addWidget(m_deviceName);

    QHBoxLayout *deviceAddressLayout = new QHBoxLayout();
    m_deviceAddress = new QLabel();
    deviceAddressLayout->setSpacing(0);
    deviceAddressLayout->addWidget(new QLabel(tr("Address :")));
    deviceAddressLayout->addWidget(m_deviceAddress);

    QHBoxLayout *deviceRSSILayout = new QHBoxLayout();
    m_deviceRSSI = new QLabel();
    deviceRSSILayout->setSpacing(0);
    deviceRSSILayout->addWidget(new QLabel(tr("RSSI :")));
    deviceRSSILayout->addWidget(m_deviceRSSI);

    m_devicePaired = new QCheckBox(tr("Paired"));
    m_devicetrusted = new QCheckBox(tr("Trusted"));
    m_deviceConnected = new QCheckBox(tr("Connected"));
    m_deviceServicesResolved = new QCheckBox(tr("Services resolved"));

    m_pairDevice = new QPushButton(tr("Pair"));
    m_connectDevice = new QPushButton(tr("Connect"));
    m_disconnectDevice = new QPushButton(tr("Disconnect"));
    m_trustDevice = new QPushButton(tr("Trust"));
    m_untrustDevice = new QPushButton(tr("Untrust"));

    m_detailsPanel->setLayout(detailsLayout);
    detailsLayout->addLayout(deviceNameLayout);
    detailsLayout->addLayout(deviceAddressLayout);
    detailsLayout->addLayout(deviceRSSILayout);
    // detailsLayout->addWidget(new QSpacerItem(this));
    detailsLayout->addWidget(m_devicePaired);
    detailsLayout->addWidget(m_devicetrusted);
    detailsLayout->addWidget(m_deviceConnected);
    detailsLayout->addWidget(m_deviceServicesResolved);

    detailsLayout->addLayout(btnLine1Layout);
    detailsLayout->addLayout(btnLine2Layout);

    btnLine1Layout->addWidget(m_pairDevice);
    btnLine1Layout->addWidget(m_connectDevice);
    btnLine1Layout->addWidget(m_disconnectDevice);

    btnLine2Layout->addWidget(m_trustDevice);
    btnLine2Layout->addWidget(m_untrustDevice);
  }

  mainLayout->addWidget(m_deviceList);
  mainLayout->addWidget(m_detailsPanel);


  m_devicePaired->setDisabled(true);
  m_devicetrusted->setDisabled(true);
  m_deviceConnected->setDisabled(true);
  m_deviceServicesResolved->setDisabled(true);

  QFont fnt = font();
  fnt.setPointSize(UI::Size::font);
  setFont(fnt);


  mainLayout->setSpacing(UI::Size::margin);
  mainLayout->setContentsMargins(UI::Size::margins);

  detailsLayout->setSpacing(UI::Size::spacing);
  btnLine1Layout->setSpacing(UI::Size::spacing);
  btnLine2Layout->setSpacing(UI::Size::spacing);

  APPLY_MIN_WIDTH(m_detailsPanel);
  APPLY_MIN_WIDTH(m_deviceName);
  APPLY_MIN_WIDTH(m_deviceAddress);
  APPLY_MIN_WIDTH(m_deviceRSSI);
  APPLY_MIN_WIDTH(m_devicePaired);
  APPLY_MIN_WIDTH(m_devicetrusted);
  APPLY_MIN_WIDTH(m_deviceConnected);
  APPLY_MIN_WIDTH(m_deviceServicesResolved);

  APPLY_MIN_SIZE(m_pairDevice);
  APPLY_MIN_SIZE(m_connectDevice);
  APPLY_MIN_SIZE(m_disconnectDevice);
  APPLY_MIN_SIZE(m_trustDevice);
  APPLY_MIN_SIZE(m_untrustDevice);

  /// Device list
  {
    for (Device *device : DBus::Bluez::Manager::devices())
      addDevice(device->path(), *device);

    connect(
      &DBus::Bluez::Manager::instance(), &DBus::Bluez::Manager::deviceAdded,
      this,                              &DevicePanel::addDevice
    );

    connect(
      &DBus::Bluez::Manager::instance(), &DBus::Bluez::Manager::deviceRemoved,
      this,                              &DevicePanel::removeDevice
    );

    connect(
      m_deviceList, &QListWidget::itemSelectionChanged,
      [this]()
      {
        const QList<QListWidgetItem *> &items = m_deviceList->selectedItems();

        if (items.isEmpty())
          return selectDevice(nullptr);

        QListWidgetItem *item = items.front();
        QDBusObjectPath path = item->data(Qt::UserRole).value<QDBusObjectPath>();

        selectDevice(DBus::Bluez::Manager::devices().value(path));
      }
    );
  }
}

void DevicePanel::addDevice(const QDBusObjectPath &path, Device &device)
{
  QListWidgetItem *item = new QListWidgetItem(device.alias());

  item->setData(Qt::UserRole, QVariant::fromValue(path));

  m_items[path] = item;
  m_deviceList->addItem(item);
}

void DevicePanel::removeDevice(const QDBusObjectPath &path)
{
  QListWidgetItem *item = m_items.take(path);

  if (item)
    delete item;
}

void DevicePanel::selectDevice(Device *device)
{
  if (device == m_selectedDevice)
    return;

  m_detailsPanel->setEnabled(!!device);

  const QString &name = (device ? device->alias() : "");
  const QString &address = (device ? device->address() : "");
  const QString &RSSI = (device ? "" : "");

  const bool paired = (device ? device->paired() : false);
  const bool trusted = (device ? device->trusted() : false);
  const bool connected = (device ? device->connected() : false);
  const bool serviceResolved = (device ? device->servicesResolved() : false);

  m_deviceName->setText(name);
  m_deviceAddress->setText(address);
  m_deviceRSSI->setText(RSSI);

  m_devicePaired->setChecked(paired);
  m_devicetrusted->setChecked(trusted);
  m_deviceConnected->setChecked(connected);
  m_deviceServicesResolved->setChecked(serviceResolved);


  // disconnect previously selected device
  disconnectDevice(m_selectedDevice);

  if (!device)
  {
    m_selectedDevice = nullptr;

    qDebug() << "Selection changed to NULL";
    return;
  }

  m_selectedDevice = device;
  connectDevice(device);

  qDebug() << "Selection changed to" << device->alias();
}

void DevicePanel::selectedDeviceAliasChanged(const QString &alias)
{ m_deviceName->setText(alias); }

//void DevicePanel::selectedDeviceRSSIChanged(const QString &alias)
//{ }

void DevicePanel::selectedDevicePairedChanged(bool paired)
{ m_devicePaired->setChecked(paired); }

void DevicePanel::selectedDeviceTrustedChanged(bool trusted)
{ m_devicetrusted->setChecked(trusted); }

void DevicePanel::selectedDeviceConnectedChanged(bool connected)
{ m_deviceConnected->setChecked(connected); }

void DevicePanel::selectedDeviceServicesResolvedChanged(bool servicesResolved)
{ m_deviceServicesResolved->setChecked(servicesResolved); }


void DevicePanel::connectDevice(Device *device)
{
  if (!device)
    return;

  connect(
    device, &Device::aliasChanged,
    this,   &DevicePanel::selectedDeviceAliasChanged
  );

  //connect(
  //  device, &Device::RSSIChanged,
  //  this,   &DevicePanel::selectedDeviceRSSIChanged
  //);

  connect(
    device, &Device::pairedChanged,
    this,   &DevicePanel::selectedDevicePairedChanged
  );

  connect(
    device, &Device::trustedChanged,
    this,   &DevicePanel::selectedDeviceTrustedChanged
  );

  connect(
    device, &Device::connectedChanged,
    this,   &DevicePanel::selectedDeviceConnectedChanged
  );

  connect(
    device, &Device::servicesResolvedChanged,
    this,   &DevicePanel::selectedDeviceServicesResolvedChanged
  );


  connect(
    m_pairDevice, &QPushButton::clicked,
    device,       &Device::pair
  );

  connect(
    m_connectDevice, &QPushButton::clicked,
    device,          &Device::connect
  );

  connect(
    m_disconnectDevice, &QPushButton::clicked,
    device,             &Device::disconnect
  );

  connect(
    m_trustDevice, &QPushButton::clicked,
    device,        &Device::trust
  );

  connect(
    m_untrustDevice, &QPushButton::clicked,
    device,          &Device::untrust
  );
}

void DevicePanel::disconnectDevice(const Device *const device)
{
  if (!device)
    return;

  disconnect(m_pairDevice,       nullptr, device, nullptr);
  disconnect(m_connectDevice,    nullptr, device, nullptr);
  disconnect(m_disconnectDevice, nullptr, device, nullptr);
  disconnect(m_trustDevice,      nullptr, device, nullptr);
  disconnect(m_untrustDevice,    nullptr, device, nullptr);

  disconnect(device, nullptr, this, nullptr);
}
