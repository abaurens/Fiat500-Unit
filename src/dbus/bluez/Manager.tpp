#pragma once

#include "Manager.hpp"

namespace DBus::Bluez
{
  template<class ... ObjTypes>
  size_t Manager::tryCreateAll(const QDBusObjectPath &path, const InterfaceMap &interfaces)
  {
    // Try to create every given type. Return the number of successfully created objects.
    return (... + static_cast<std::size_t>(tryCreate<ObjTypes>(path, interfaces)));
  }

  template<class ... ObjTypes>
  size_t Manager::tryRemoveAll(const QDBusObjectPath &path, const QStringList &interfaces)
  {
    // Try to remove every given type. Return the number of successfully deleted objects.
    return (... + static_cast<std::size_t>(tryRemove<ObjTypes>(path, interfaces)));
  }

  template<class ObjType>
  size_t  Manager::tryCreate(const QDBusObjectPath &path, const InterfaceMap &interfaces)
  {
    if (!interfaces.contains(ObjType::InterfaceName))
      return 0;

    qDebug() << "Trying to creating Object" << path.path() << "of type" << ObjType::InterfaceName;

    ObjType *obj = new ObjType(path, interfaces, this);

    addObject(*obj);

    return 1;
  }

  template<class ObjType>
  size_t  Manager::tryRemove(const QDBusObjectPath &path, const QStringList &interfaces)
  {
    if (!interfaces.contains(ObjType::InterfaceName.toQString()))
      return 0;

    qDebug() << "Trying to remove Object" << path.path() << "of type" << ObjType::InterfaceName;

    ObjType *obj = nullptr;
    if (getObject(path, obj))
      removeObject(*obj);

    return 1;
  }
}
