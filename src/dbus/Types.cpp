#include "pch.hpp" // IWYU pragma: keep

#include "Types.hpp"

#include <QDBusMetaType>

void registerDBusTypes()
{
  qDBusRegisterMetaType<DBus::InterfaceMap>();
  qDBusRegisterMetaType<DBus::ManagedObjectMap>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const DBus::InterfaceMap &map)
{
  argument.beginMap(
    QMetaType::fromType<QString>(),
    QMetaType::fromType<DBus::PropertyMap>()
  );

  for (auto it = map.cbegin(); it != map.cend(); ++it)
  {
    argument.beginMapEntry();
    argument << it.key();
    argument << it.value();
    argument.endMapEntry();
  }

  argument.endMap();
  return argument;
}

QDBusArgument &operator>>(QDBusArgument &argument, DBus::InterfaceMap &map)
{
  map.clear();

  argument.beginMap(
    QMetaType::fromType<QString>(),
    QMetaType::fromType<DBus::PropertyMap>()
  );

  QString key;
  DBus::PropertyMap value;

  while (!argument.atEnd())
  {
    argument.beginMapEntry();
    argument >> key;
    argument >> value;
    argument.endMapEntry();

    map.insert(key, value);
  }

  argument.endMap();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const DBus::ManagedObjectMap &map)
{
  argument.beginMap(
    QMetaType::fromType<QDBusObjectPath>(),
    QMetaType::fromType<DBus::InterfaceMap>()
  );

  for (auto it = map.cbegin(); it != map.cend(); ++it)
  {
    argument.beginMapEntry();
    argument << it.key();
    argument << it.value();
    argument.endMapEntry();
  }
  argument.endMap();

  return argument;
}

QDBusArgument &operator>>(QDBusArgument &argument, DBus::ManagedObjectMap &map)
{
  map.clear();

  argument.beginMap(
    QMetaType::fromType<QDBusObjectPath>(),
    QMetaType::fromType<DBus::InterfaceMap>()
  );

  QDBusObjectPath key;
  DBus::InterfaceMap value;

  while (!argument.atEnd())
  {
    argument.beginMapEntry();
    argument >> key;
    argument >> value;
    argument.endMapEntry();

    map.insert(key, value);
  }

  argument.endMap();
  return argument;
}
