#pragma once

#include "dbus/Types.hpp"
#include "dbus/bluez/Object.hpp"

#include <QObject>
#include <QDBusObjectPath>

namespace DBus::Bluez
{
  class MediaPlayer;

  class MediaControl;
  using MediaControlMap = QMap<QDBusObjectPath, MediaControl *>;

  class MediaControl : public Object
  {
    Q_OBJECT

    enum class Property
    {
      Connected,
      Player,

      Unknown = -1
    };

    inline static const QHash<QString, Property> s_properties = {
      DECL_DBUS_PROPERTY(Connected),
      DECL_DBUS_PROPERTY(Player)
    };

  public:
    static constexpr Name InterfaceName = "org.bluez.MediaControl1";

  public:
    explicit MediaControl(const QDBusObjectPath &path,
                         const InterfaceMap &interfaces,
                         QObject *parent = nullptr);

    explicit MediaControl(const QDBusObjectPath &path,
                         const PropertyMap &properties,
                         QObject *parent = nullptr);

    // MediaPlayer *playerObject() const;

    bool connected() const;
    QDBusObjectPath player() const;

  public slots:
    QDBusPendingReply<> play();
    QDBusPendingReply<> pause();
    QDBusPendingReply<> next();
    QDBusPendingReply<> previous();

  signals:
    void connectedChanged(bool connected);
    void playerChanged(DBus::Bluez::MediaPlayer *player);

  private:
    virtual void onPropertyChanged(const QString &name, const QVariant &value) override;
  };
}
