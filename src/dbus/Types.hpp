#pragma once

#include <QVariantMap>

#include <QStringView>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusObjectPath>

#include <QStringLiteral>

namespace DBus
{
  using PropertyMap      = QVariantMap;
  using InterfaceMap     = QMap<QString, PropertyMap>;
  using ManagedObjectMap = QMap<QDBusObjectPath, InterfaceMap>;
}

Q_DECLARE_METATYPE(DBus::InterfaceMap)
Q_DECLARE_METATYPE(DBus::ManagedObjectMap)

void registerDBusTypes();

QDBusArgument &operator<<(QDBusArgument &argument, const DBus::InterfaceMap &map);
QDBusArgument &operator>>(QDBusArgument &argument, DBus::InterfaceMap &map);

QDBusArgument &operator<<(QDBusArgument &argument, const DBus::ManagedObjectMap &map);
QDBusArgument &operator>>(QDBusArgument &argument, DBus::ManagedObjectMap &map);

#define DECL_DBUS_PROPERTY(__property) { #__property, Property::__property }

namespace DBus
{
  struct Name : public std::string_view
  {
    template<class... Args>
    constexpr Name(Args &&... args) : std::string_view(std::forward<Args>(args)...) {}

    QString toQString() const
    {
      return QString(data());
    }

    QDBusObjectPath toPath() const
    {
      return QDBusObjectPath(data());
    }

    operator QString() const
    {
      return toQString();
    }

    operator QDBusObjectPath() const
    {
      return toPath();
    }
  };

  namespace Method
  {
    inline constexpr Name GetManagedObjects = "GetManagedObjects";
    inline constexpr Name PropertiesChanged = "PropertiesChanged";
    inline constexpr Name InterfacesRemoved = "InterfacesRemoved";
    inline constexpr Name InterfacesAdded   = "InterfacesAdded";
  }

  namespace Interface
  {
    inline constexpr Name ObjectManager = "org.freedesktop.DBus.ObjectManager";
    inline constexpr Name Properties    = "org.freedesktop.DBus.Properties";
  }

  inline constexpr Name RootPath = "/";
}
