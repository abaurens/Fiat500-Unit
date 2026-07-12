#include "MediaControl.hpp"
#include "Manager.hpp"

namespace DBus::Bluez
{
  MediaControl::MediaControl(const QDBusObjectPath &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ InterfaceName, path, interfaces, parent }
  { }

  MediaControl::MediaControl(const QDBusObjectPath &path, const PropertyMap &properties, QObject *parent)
    : Object{ InterfaceName, path, properties, parent }
  { }

  bool MediaControl::connected() const
  {
    return property<bool>("Connected");
  }

  QDBusObjectPath MediaControl::player() const
  {
    return propertyOr<QDBusObjectPath>("Player", {});
  }

  QDBusPendingReply<> MediaControl::play()
  {
    return callMethod("Play");
  }

  QDBusPendingReply<> MediaControl::pause()
  {
    return callMethod("Pause");
  }

  QDBusPendingReply<> MediaControl::next()
  {
    return callMethod("Next");
  }

  QDBusPendingReply<> MediaControl::previous()
  {
    return callMethod("Previous");
  }

  void MediaControl::onPropertyChanged(const QString &name, const QVariant &value)
  {
    const auto property = s_properties.value(name, Property::Unknown);

    switch (property)
    {

    case Property::Connected:
      {
        emit connectedChanged(value.toBool());
        break;
      }

    case Property::Player:
      {
        const QDBusObjectPath playerPath = value.value<QDBusObjectPath>();
        MediaPlayer *const player = Manager::mediaPlayers().value(playerPath, nullptr);
        emit playerChanged(player);
        break;
      }

    default:
#ifndef NDEBUG
      qDebug() << m_path.path() << ": Unhandled property " << name << "changed to" << value;
#endif
      break;
    }
  }
}
