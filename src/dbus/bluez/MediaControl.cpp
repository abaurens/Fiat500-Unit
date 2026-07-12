#include "MediaControl.hpp"
#include "Manager.hpp"

namespace DBus::Bluez
{
  MediaControl::MediaControl(const Object::Path &path, const PropertyMap &properties, QObject *parent)
    : Object{ InterfaceName, path, properties, parent }
  {
    connect(
      &Manager::instance(), &Manager::mediaPlayerRemoved,
      [this](const Object::Path &path)
      {
        if (path == player())
        {
          // qDebug() << "Media player got removed without a controler update";
          onPlayerChange({});
        }
      }
    );

    connect(
      &Manager::instance(), &Manager::mediaPlayerAdded,
      [this](const Object::Path &path)
      {
        if (path == player())
        {
          // qDebug() << "Media player was added without a controler update";
          onPlayerChange(path);
        }
      }
    );

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

  void MediaControl::onPlayerChange(const Object::Path &playerPath)
  {
    qDebug() << "MediaControl.player changed to" << playerPath;

    MediaPlayer *const player = Manager::mediaPlayers().value(playerPath, nullptr);

    emit playerChanged(player);
  }

  void MediaControl::onPropertyChanged(const QString &name, const QVariant &value)
  {
    const auto property = s_properties.value(name, Property::Unknown);

    switch (property)
    {

    case Property::Connected:
      emit connectedChanged(value.toBool());
      break;

    case Property::Player:
      onPlayerChange(value.value<Object::Path>());
      break;

    default:
#ifndef NDEBUG
      // qDebug() << m_path.path() << ": Unhandled property " << name << "changed to" << value;
#endif
      break;
    }
  }
}
