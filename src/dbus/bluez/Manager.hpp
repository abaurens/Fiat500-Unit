#pragma once

#include "AbstractManager.hpp"

#include "dbus/Types.hpp"

#include "dbus/bluez/Adapter.hpp"
#include "dbus/bluez/Device.hpp"
#include "dbus/bluez/MediaControl.hpp"
#include "dbus/bluez/MediaPlayer.hpp"

#include <QObject>
#include <QDBusConnection>

#define LIST(...) __VA_ARGS__

/// List of all bluez objects the Manager class supports.
/// Extend this list and add overloads of `addObject`,
/// `removeObject` and `getObject` to support new types.
#define BLUEZ_OBJECT_TYPELIST \
  LIST(           \
    Adapter,      \
    Device,       \
    MediaControl, \
    MediaPlayer   \
  )

#define DECLARE_MANAGED_OBJECT(_type, _name)                                 \
    void _name##Added(const Object::Path &path, DBus::Bluez::_type &_name);  \
    void _name##Removed(const Object::Path &path);                           \
  public:                                                                    \
    static _type::Map &_name##s() { return instance().m_##_name##s; }        \
  private:                                                                   \
    void addObject(_type &_name);                                            \
    void removeObject(_type &_name);                                         \
    bool getObject(const Object::Path &path, _type *(&_name))  {             \
      return getObjectImpl(m_##_name##s, path, _name);                       \
    }                                                                        \
  private:                                                                   \
    _type::Map m_##_name##s

namespace DBus::Bluez
{

  class Manager : public QObject, public ManagerType
  {
    Q_OBJECT

    MANAGER_OBJECT


  // Public static APi
  public:
    static Adapter *adapter() { return instance().m_adapter; }

    /// Return an object of type T associated with the given object
    /// (different interfaces of the same dbus object).
    /// Returns nullptr if no such object exists.
    template<class T>
    static T *getObject(const Object &object) { return getObject<T>(object.path()); }

    /// Return an object of type T associated with the given object
    /// (different interfaces of the same dbus object).
    /// Returns nullptr if no such object exists.
    template<class T>
    static T *getObject(const Object *object) { return (object ? getObject<T>(*object) : nullptr); }

    /// Return an of type T associated with the given object path
    /// Returns nullptr if no such object exists.
    template<class T>
    static T *getObject(const Object::Path &path)
    {
      T* object = nullptr;
      instance().getObject(path, object);
      return object;
    }


  // supported objects implementation
  private:
    void addObject(Adapter &adapter);
    void removeObject(Adapter &device);
    bool getObject(const Object::Path &path, Adapter *(&adapter));

  signals:
    DECLARE_MANAGED_OBJECT(Device,       device);
  signals:
    DECLARE_MANAGED_OBJECT(MediaControl, mediaControl);
  signals:
    DECLARE_MANAGED_OBJECT(MediaPlayer,  mediaPlayer);

  /**************************\
  |* Implementation details *|
  \**************************/

  // Private members
  private:
    QDBusConnection m_bus;
    Adapter        *m_adapter = nullptr;

  // private constructor (singleton)
  private:
    // explicit Manager(QObject *parent = nullptr);

  private slots:
    void onInterfacesAdded(const QDBusObjectPath &path, const InterfaceMap &interfaces);
    void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

  // Init functions
  private:
    // bool initializeInstance();
    ManagedObjectMap loadManagedObjects();

  // Automatic dynamic object creation system
  private:
    size_t createObjects(const Object::Path &path, const InterfaceMap &interfaces) {
      return tryCreateAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }

    size_t removeObjects(const Object::Path &path, const QStringList &interfaces) {
      return tryRemoveAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }


    template<class ... ObjTypes>
    size_t tryCreateAll(const Object::Path &path, const InterfaceMap &interfaces);

    template<class ... ObjTypes>
    size_t tryRemoveAll(const Object::Path &path, const QStringList &interfaces);


    template<class ObjType>
    size_t  tryCreate(const Object::Path &path, const InterfaceMap &interfaces);

    template<class ObjType>
    size_t  tryRemove(const Object::Path &path, const QStringList &interfaces);


    template<std::derived_from<Object> ObjType, void (Manager::*Signal)(const Object::Path &, ObjType &)>
    bool addObjectImpl(typename ObjType::Map &container, ObjType &object)
    {
      const Object::Path &path = object.path();
      auto it = container.find(path);

      if (it != container.end())
        removeObject(object);

      container.insert(path, &object);

      emit (this->*Signal)(path, object);
      return true;
    }

    template<std::derived_from<Object> ObjType, void (Manager::*Signal)(const Object::Path &)>
    bool removeObjectImpl(typename ObjType::Map &container, ObjType &object)
    {
      const Object::Path &path = object.path();

      auto it = container.find(path);
      if (it == container.end())
        return false;

      emit (this->*Signal)(path);

      delete *it;
      container.erase(it);
      return true;
    }

    template<std::derived_from<Object> ObjType>
    static bool getObjectImpl(typename ObjType::Map &container, const Object::Path &path, ObjType *(&object))
    {
      auto it = container.find(path);

      if (it == container.cend())
        return false;

      object = *it;
      return true;
    }

  };
}

#include "Manager.tpp"
#undef MANAGER_TPP

#undef BLUEZ_OBJECT_TYPELIST
#undef DECLARE_MANAGED_OBJECT
#undef LIST
