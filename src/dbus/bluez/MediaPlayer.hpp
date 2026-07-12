#pragma once

#include "dbus/bluez/Types.hpp"
#include "dbus/Object.hpp"

#include <QTimer>
#include <QObject>

#include <array>

namespace DBus::Bluez
{
  class MediaPlayer;
  using MediaPlayerMap = QMap<Object::Path, MediaPlayer *>;

  class MediaPlayer : public Object
  {
    Q_OBJECT

    enum class Property
    {
      Name,
      Status,
      Track,
      Position,
      Repeat,
      Shuffle,

      Unknown = -1
    };

    inline static const QHash<QString, Property> s_properties = {
      DECL_DBUS_PROPERTY(Name),
      DECL_DBUS_PROPERTY(Status),
      DECL_DBUS_PROPERTY(Track),
      DECL_DBUS_PROPERTY(Position),
      DECL_DBUS_PROPERTY(Repeat),
      DECL_DBUS_PROPERTY(Shuffle)
    };

    struct Status
    {
      enum : size_t
      {
        Playing,
        Stopped,
        Paused,
        ForwardSeek,
        ReverseSeek,
        Error
      };

    private:
      using Value = decltype(Playing);

      inline static constexpr std::array<Name, 6> s_names = {
        "playing",
        "stopped",
        "paused",
        "forward-seek",
        "reverse-seek",
        "error"
      };

    public:
      static Status fromString(const QString &statusStr)
      {
        for (size_t i = 0; i < s_names.size(); ++i)
          if (s_names[i] == statusStr)
            return Value{ i };
        return Error;
      }

    public:
      constexpr Status(Value v) : m_value { v } {}

      inline constexpr Name name() const { return s_names[m_value]; }

      inline constexpr operator Name() const { return name(); }
      inline constexpr operator Value() const { return m_value; }

      inline constexpr bool operator==(const Value &other) const { return m_value == other; }
      inline constexpr bool operator!=(const Value &other) const { return m_value != other; }

      inline constexpr bool operator==(const Status &other) const { return m_value == other.m_value; }
      inline constexpr bool operator!=(const Status &other) const { return m_value != other.m_value; }

    private:
      Value m_value;
    };

  public:
    static constexpr Name InterfaceName = "org.bluez.MediaPlayer1";

  public:
    explicit MediaPlayer(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent = nullptr);

    bool repeat() const;
    bool shuffle() const;

    uint32_t position() const;

    QString name() const;
    Status status() const;

    TrackInfo track() const;

    Object::Path device() const;

  public slots:
    void setRepeat(bool repeat);
    void setShuffle(bool shuffle);

    void play();
    void pause();
    void next();
    void previous();

  signals:
    void repeatChanged(bool);
    void shuffleChanged(bool);

    void positionChanged(uint32_t);

    void nameChanged(const QString &);
    void statusChanged(const DBus::Bluez::MediaPlayer::Status);

    void trackChanged(const DBus::Bluez::TrackInfo &);

    void playing();
    void paused();
    void stopped();
    void forwardSeeking();
    void reverseSeeking();

  private slots:
    void updatePosition();

  private:
    explicit MediaPlayer(const Object::Path &path, const PropertyMap &properties, QObject *parent = nullptr);

    void onStatusChanged(const Status status);
    virtual void onPropertyChanged(const QString &name, const QVariant &value) override;

  private:
    QTimer *m_timer = nullptr;
  };
}

#undef DECL_TRACK_DATA
