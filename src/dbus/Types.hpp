#pragma once

#include <QVariantMap>

#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusObjectPath>

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

    operator QString() const
    {
      return QString(data());
    }
  };

  inline constexpr Name PropertiesInterface = "org.freedesktop.DBus.Properties";
  inline constexpr Name PropertiesChanged   = "PropertiesChanged";
  inline constexpr Name RootPath            = "/";

  //template<std::unsigned_integral T = unsigned int>
  //struct Property
  //{
  //  constexpr Property(Name name, T value)
  //    : m_name(name), m_value(value)
  //  {}
  //
  //  T value() const { return m_value; }
  //  Name name() const { return m_name; }
  //
  //  operator T() const { return value(); }
  //  operator Name() const { return name(); }
  //
  //private:
  //  Name m_name;
  //  T    m_value;
  //};
}
