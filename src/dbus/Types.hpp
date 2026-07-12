#pragma once

#include <QVariantMap>

#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QStringView>

// #include <concepts>

using PropertyMap      = QVariantMap;
using InterfaceMap     = QMap<QString, PropertyMap>;
using ManagedObjectMap = QMap<QDBusObjectPath, InterfaceMap>;

Q_DECLARE_METATYPE(InterfaceMap)
Q_DECLARE_METATYPE(ManagedObjectMap)

void registerDBusTypes();

QDBusArgument &operator<<(QDBusArgument &argument, const InterfaceMap &map);
QDBusArgument &operator>>(QDBusArgument &argument, InterfaceMap &map);

QDBusArgument &operator<<(QDBusArgument &argument, const InterfaceMap &map);
QDBusArgument &operator>>(QDBusArgument &argument, ManagedObjectMap &map);

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
