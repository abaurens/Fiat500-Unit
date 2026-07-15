#include "Types.hpp"

#include <QDBusMetaType>

void registerDBusTypes()
{
  qDBusRegisterMetaType<InterfaceMap>();
  qDBusRegisterMetaType<ManagedObjectMap>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const InterfaceMap &map)
{
  argument.beginMap(
    QMetaType::fromType<QString>(),
    QMetaType::fromType<PropertyMap>()
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

QDBusArgument &operator>>(QDBusArgument &argument, InterfaceMap &map)
{
  map.clear();

  argument.beginMap(
    QMetaType::fromType<QString>(),
    QMetaType::fromType<PropertyMap>()
  );

  QString key;
  PropertyMap value;

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

QDBusArgument &operator<<(QDBusArgument &argument, const ManagedObjectMap &map)
{
  argument.beginMap(
    QMetaType::fromType<QDBusObjectPath>(),
    QMetaType::fromType<InterfaceMap>()
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

QDBusArgument &operator>>(QDBusArgument &argument, ManagedObjectMap &map)
{
  map.clear();

  argument.beginMap(
    QMetaType::fromType<QDBusObjectPath>(),
    QMetaType::fromType<InterfaceMap>()
  );

  QDBusObjectPath key;
  InterfaceMap value;

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
