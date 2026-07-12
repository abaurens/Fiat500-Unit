#include "dbus/bluez/Adapter.hpp"

namespace DBus::Bluez
{
  Adapter::Adapter(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ InterfaceName, path, interfaces, parent }
  {}

  Adapter::Adapter(const Object::Path &path, const PropertyMap &properties, QObject *parent)
    : Object{ InterfaceName, path, properties, parent }
  {}

  QString Adapter::address() const { return property<QString>("Address"); }
  QString Adapter::alias() const { return property<QString>("Alias"); }

  bool Adapter::powered() const { return property<bool>("Powered"); }
  bool Adapter::discoverable() const { return property<bool>("Discoverable"); }
  bool Adapter::discovering() const { return property<bool>("Discovering"); }

  void Adapter::setAlias(const QString &alias)
  {
    return Object::setProperty("Alias", alias);
  }

  void Adapter::setPowered(bool powered)
  {
    return Object::setProperty("Powered", powered);
  }

  void Adapter::setDiscoverable(bool discoverable)
  {
    return Object::setProperty("Discoverable", discoverable);
  }

  void Adapter::stopDiscovery() { return Object::callMethod("StopDiscovery"); }
  void Adapter::startDiscovery() { return Object::callMethod("StartDiscovery"); }

  void Adapter::onPropertyChanged(const QString &name, const QVariant &value)
  {
    const auto property = s_properties.value(name, Property::Unknown);

    switch (property)
    {
    case Property::Powered:
      emit poweredChanged(value.toBool());
      break;

    case Property::Discoverable:
      emit discoverableChanged(value.toBool());
      break;

    case Property::Discovering:
      emit discoveringChanged(value.toBool());
      break;

    case Property::Alias:
      emit aliasChanged(value.toString());
      break;

    default:
      break;
    };

    // qDebug() << "Property changed:"
    //          << m_path.path()
    //          << name
    //          << "changed to"
    //          << value;
  }
}
