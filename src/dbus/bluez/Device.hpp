#pragma once

#include "dbus/Types.hpp"
#include "dbus/Object.hpp"

#include <QObject>

namespace DBus::Bluez
{
  class Device;
  using DeviceMap = QMap<Object::Path, Device *>;

  class Device : public Object
  {
    Q_OBJECT

    enum class Property
    {
      Alias,
      Address,
      Connected,
      Paired,
      Trusted,
      Blocked,
      // Player,
      ServicesResolved,

      Unknown = -1
    };

    inline static const QHash<QString, Property> s_properties = {
      DECL_DBUS_PROPERTY(Alias),
      DECL_DBUS_PROPERTY(Address),
      DECL_DBUS_PROPERTY(Connected),
      DECL_DBUS_PROPERTY(Paired),
      DECL_DBUS_PROPERTY(Trusted),
      DECL_DBUS_PROPERTY(Blocked),
      // DECL_DBUS_PROPERTY(Player),
      DECL_DBUS_PROPERTY(ServicesResolved)
    };

  public:
    inline static constexpr Name InterfaceName = "org.bluez.Device1";

  public:
    explicit Device(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent = nullptr);

    /// Is the device paired (may have to ask permission again to connect if not trusted)
    bool paired() const;
    bool trusted() const;
    bool blocked() const; // Blocked device can never connect (blacklisted)
    bool connected() const;
    bool servicesResolved() const;

    QString alias() const;
    QString address() const;

    void setTrusted(bool trusted);

    void pair();
    void trust();
    void untrust();
    void connect();
    void disconnect();

  signals:
    void aliasChanged(const QString &alias);
    void connectedChanged(bool connected);
    void pairedChanged(bool paired);
    void trustedChanged(bool trusted);
    void blockedChanged(bool blocked);
    // void playerChanged(const DBus::Object::Path &path);
    void servicesResolvedChanged(bool resolved);

  private:
    explicit Device(const Object::Path &path, const PropertyMap &properties, QObject *parent = nullptr);

    virtual void onPropertyChanged(const QString &name, const QVariant &newValue) override final;
  };
}
