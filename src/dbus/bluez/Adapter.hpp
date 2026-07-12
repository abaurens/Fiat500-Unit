#pragma once

#include "dbus/Types.hpp"
#include "dbus/Object.hpp"

#include <QObject>

namespace DBus::Bluez
{
  class Adapter;
  using AdapterMap = QMap<Object::Path, Adapter *>;

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
    static constexpr Name InterfaceName = "org.bluez.Adapter1";

  public:
    explicit Adapter(const Object::Path &path, const InterfaceMap &interfaces, QObject *parent = nullptr);
    bool powered() const;
    bool discoverable() const;
    bool discovering() const;

    QString alias() const;
    QString address() const;

  public slots:
    void setPowered(bool);
    void setDiscoverable(bool);
    void setAlias(const QString &alias);

    /// Start/stop a scan for available devices around us
    void startDiscovery();
    void stopDiscovery();

  signals:
    void aliasChanged(const QString &alias);
    void poweredChanged(bool powered);
    void discoverableChanged(bool discoverable);
    void discoveringChanged(bool discovering);

  private:
    explicit Adapter(const Object::Path &path, const PropertyMap &properties, QObject *parent = nullptr);

    virtual void onPropertyChanged(const QString &name, const QVariant &newValue) override final;
  };
}
