#pragma once

#include "dbus/Types.hpp"
#include "dbus/Object.hpp"

#include <QObject>

namespace DBus::Bluez
{
  class MediaPlayer;

  class MediaControl;
  using MediaControlMap = QMap<Object::Path, MediaControl *>;

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

    explicit MediaControl(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent = nullptr);

    bool connected() const;
    Object::Path player() const;

  public slots:
    void play();
    void pause();
    void next();
    void previous();

  signals:
    void connectedChanged(bool connected);
    void playerChanged(DBus::Bluez::MediaPlayer *player);

  private:
    void onPlayerChange(const Object::Path &playerPath);
    void onPlayerRemoved(const Object::Path &path);
    explicit MediaControl(const Object::Path &path, const PropertyMap &properties, QObject *parent = nullptr);

    virtual void onPropertyChanged(const QString &name, const QVariant &value) override;
  };
}
