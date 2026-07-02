#pragma once

#include "dbus/Types.hpp"
#include "dbus/bluez/Object.hpp"

#include <QObject>
#include <QDBusObjectPath>

namespace DBus::Bluez
{
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
      DECL_DBUS_PROPERTY(ServicesResolved)
    };

  public:
    explicit Device(const QDBusObjectPath &path,
                    const InterfaceMap &interfaces,
                    QObject *parent = nullptr);

    explicit Device(const QDBusObjectPath &path,
                    const PropertyMap &properties,
                    QObject *parent = nullptr);

    /// Is the device paired (may have to ask permission again to connect if not trusted)
    bool paired() const;
    bool trusted() const;
    bool blocked() const; // Blocked device can never connect (blacklisted)
    bool connected() const;
    bool servicesResolved() const;

    QString alias() const;
    QString address() const;

    QDBusPendingReply<> setTrusted(bool trusted);

    QDBusPendingReply<> pair();
    QDBusPendingReply<> trust();
    QDBusPendingReply<> untrust();
    QDBusPendingReply<> connect();
    QDBusPendingReply<> disconnect();

  signals:
    void aliasChanged(const QString &alias);
    void connectedChanged(bool connected);
    void pairedChanged(bool paired);
    void trustedChanged(bool trusted);
    void blockedChanged(bool blocked);
    void servicesResolvedChanged(bool resolved);

  private:
    virtual void onPropertyChanged(const QString &name, const QVariant &newValue) override final;

  private:

  };
}
