#include "MediaPlayer.hpp"


static constexpr uint32_t positionUpdateFrequency = 2; // force updates position 2 times per seconds
static constexpr uint32_t positionUpdateDelay = 1000 / positionUpdateFrequency;

namespace DBus::Bluez
{
  MediaPlayer::MediaPlayer(const Object::Path &path, const PropertyMap &properties, QObject *parent)
  : Object{ InterfaceName, path, properties, parent }, m_timer(new QTimer(this))
  {
    m_timer->setInterval(positionUpdateDelay);

    connect(
      m_timer,  &QTimer::timeout,
      this,     &MediaPlayer::updatePosition
    );

    emit trackChanged(track());
    emit positionChanged(refreshProperty<uint32_t>("Position").value_or(0u));
    onStatusChanged(status());
  }

  MediaPlayer::MediaPlayer(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent)
    : MediaPlayer{ path, interfaces.value(InterfaceName), parent }
  {}

  bool MediaPlayer::repeat() const
  {
    return propertyOr<bool>("Repeat", false);
  }

  bool MediaPlayer::shuffle() const
  {
    return propertyOr<bool>("Shuffle", false);
  }

  uint32_t MediaPlayer::position() const
  {
    return propertyOr<uint32_t>("Position", 0);
  }

  QString MediaPlayer::name() const
  {
    return propertyOr<QString>("Name", {});
  }

  MediaPlayer::Status MediaPlayer::status() const
  {
    return Status::fromString(propertyOr<QString>("Status", {}));
  }

  TrackInfo MediaPlayer::track() const
  {
    return TrackInfo{ propertyOr<PropertyMap>("Track", {}) };
  }

  Object::Path MediaPlayer::device() const
  {
    return property<Object::Path>("Device");
  }


  void MediaPlayer::setRepeat(bool repeat)
  {
    return setProperty<bool>("Repeat", repeat);
  }

  void MediaPlayer::setShuffle(bool shuffle)
  {
    return setProperty<bool>("Shuffle", shuffle);
  }

  void MediaPlayer::play()
  {
    return callMethod("Play");
  }

  void MediaPlayer::pause()
  {
    return callMethod("Pause");
  }

  void MediaPlayer::next()
  {
    return callMethod("Next");
  }

  void MediaPlayer::previous()
  {
    return callMethod("Previous");
  }

  void MediaPlayer::updatePosition()
  {
    emit positionChanged(refreshProperty<uint32_t>("Position").value_or(0u));
  }

  void MediaPlayer::onStatusChanged(const Status status)
  {


    if (status == Status::Stopped || status == Status::Paused)
      m_timer->stop();
    else
      m_timer->start(static_cast<int>(positionUpdateDelay));

    switch (status)
    {
    case Status::Playing:
      emit playing();
      break;

    case Status::Stopped:
      emit stopped();
      break;

    case Status::Paused:
      emit paused();
      break;

    case Status::ForwardSeek:
      emit forwardSeeking();
      break;

    case Status::ReverseSeek:
      emit reverseSeeking();
      break;

    case Status::Error:
      break;

    default:
      Q_ASSERT(false); /// Unsupported at the moment. Should probably display an error
    }

    emit statusChanged(status);
  }

  void MediaPlayer::onPropertyChanged(const QString &name, const QVariant &value)
  {
    const auto property = s_properties.value(name, Property::Unknown);

    switch (property)
    {
    case Property::Name:
      emit nameChanged(value.toString());
      break;

    case Property::Status:

      onStatusChanged(Status::fromString(value.toString()));
      break;

    case Property::Track:
      emit trackChanged(TrackInfo{ value.value<PropertyMap>() });
      break;

    case Property::Position:
      emit positionChanged(value.toUInt());
      break;

    case Property::Repeat:
      emit repeatChanged(value.toBool());
      break;

    case Property::Shuffle:
      emit shuffleChanged(value.toBool());
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
