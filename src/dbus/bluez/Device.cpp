#include "dbus/bluez/Device.hpp"

#include <QDBusConnection>
namespace DBus::Bluez
{
  Device::Device(const QDBusObjectPath &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ InterfaceName, path, interfaces, parent }
  {}

  Device::Device(const QDBusObjectPath &path, const PropertyMap &properties, QObject *parent)
    : Object{ InterfaceName, path, properties, parent }
  {}

  bool Device::paired() const { return property<bool>("Paired"); }
  bool Device::trusted() const { return property<bool>("Trusted"); }
  bool Device::blocked() const { return property<bool>("Blocked"); }
  bool Device::connected() const { return property<bool>("Connected"); }
  bool Device::servicesResolved() const { return property<bool>("ServicesResolved"); }

  QString Device::alias() const { return property<QString>("Alias"); }
  QString Device::address() const { return property<QString>("Address"); }

  QDBusPendingReply<> Device::pair() { return callMethod("Pair"); }
  QDBusPendingReply<> Device::trust() { return setTrusted(true); }
  QDBusPendingReply<> Device::untrust() { return setTrusted(false); }
  QDBusPendingReply<> Device::connect() { return callMethod("Connect"); }
  QDBusPendingReply<> Device::disconnect() { return callMethod("Disconnect"); }

  QDBusPendingReply<> Device::setTrusted(bool trusted) { return setProperty("Trusted", trusted); }


  void Device::onPropertyChanged(const QString &name, const QVariant &value)
  {
    const auto property = s_properties.value(name, Property::Unknown);

    switch (property)
    {
    case Property::Alias:
      emit aliasChanged(value.toString());
      break;

    case Property::Connected:
      emit connectedChanged(value.toBool());
      break;

    case Property::Paired:
      emit pairedChanged(value.toBool());
      break;

    case Property::Trusted:
      emit trustedChanged(value.toBool());
      break;

    case Property::Blocked:
      emit blockedChanged(value.toBool());
      break;

    case Property::Player:
      emit playerChanged(value.value<QDBusObjectPath>());
      break;

    case Property::ServicesResolved:
      emit servicesResolvedChanged(value.toBool());
      break;

    default:
     #ifndef NDEBUG
      qDebug() << m_path.path() << ": Unhandled property " << name << "changed to" << value;
     #endif
      break;
    }

    // qDebug() << "Property changed:"
    //          << m_path.path()
    //          << name
    //          << "changed to"
    //          << value;
  }
}
