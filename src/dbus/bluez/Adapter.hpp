#pragma once

#include "dbus/Types.hpp"
#include "dbus/bluez/Object.hpp"

#include <QObject>
#include <QDBusObjectPath>

namespace DBus::Bluez
{
  class Adapter : public Object
  {
    Q_OBJECT

    enum class Property
    {
      Powered,
      Discoverable,
      Discovering,
      Alias,
      Address,

      Unknown = -1
    };

    inline static const QHash<QString, Property> s_properties = {
      DECL_DBUS_PROPERTY(Powered),
      DECL_DBUS_PROPERTY(Discoverable),
      DECL_DBUS_PROPERTY(Discovering),
      DECL_DBUS_PROPERTY(Alias),
      DECL_DBUS_PROPERTY(Address)
    };

  public:
    explicit Adapter(const QDBusObjectPath &path,
                     const InterfaceMap &interfaces,
                     QObject *parent = nullptr);

    explicit Adapter(const QDBusObjectPath &path,
                     const PropertyMap &properties,
                     QObject *parent = nullptr);

    bool powered() const;
    bool discoverable() const;

    QString alias() const;
    QString address() const;

    QDBusPendingReply<> setPowered(bool);
    QDBusPendingReply<> setDiscoverable(bool);
    QDBusPendingReply<> setAlias(const QString &alias);

    /// Start/stop a scan for available devices around us
    QDBusPendingReply<> startDiscovery();
    QDBusPendingReply<> stopDiscovery();

  signals:
    void aliasChanged(const QString &alias);
    void poweredChanged(bool powered);
    void discoverableChanged(bool discoverable);
    void discoveringChanged(bool discovering);

  private:
    virtual void onPropertyChanged(const QString &name, const QVariant &newValue) override final;

  private:

  };
}
