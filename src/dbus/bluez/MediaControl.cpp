#include "MediaControl.hpp"
#include "Manager.hpp"

namespace DBus::Bluez
{
  MediaControl::MediaControl(const Object::Path &path, const PropertyMap &properties, QObject *parent)
    : Object{ InterfaceName, path, properties, parent }
  {

    qDebug() << "Media Control created with initial player:" << player().path();
  }

  MediaControl::MediaControl(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent)
    : MediaControl{ path, interfaces.value(InterfaceName), parent }
  {}

  bool MediaControl::connected() const
  {
    return property<bool>("Connected");
  }

  Object::Path MediaControl::player() const
  {
    return propertyOr<Object::Path>("Player", {});
  }

  void MediaControl::play()
  {
    return callMethod("Play");
  }

  void MediaControl::pause()
  {
    return callMethod("Pause");
  }

  void MediaControl::next()
  {
    return callMethod("Next");
  }

  void MediaControl::previous()
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
        const Object::Path playerPath = value.value<Object::Path>();
        qDebug() << "MediaControl.player changed to" << playerPath;
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
