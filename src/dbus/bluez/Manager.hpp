#pragma once

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
    void _name##Added(const Object::Path &path, DBus::Bluez::_type &object); \
    void _name##Removed(const Object::Path &path);                           \
  private:                                                                   \
    void addObject(_type &object);                                           \
    void removeObject(_type &object);                                        \
    bool getObject(const Object::Path &path, _type *(&object));              \
    _type##Map m_##_name##s
    // inline bool getAssociatedObject(const Object &source,  _type *(&object)) \
    // { return getObject(source.path(), object); }                             \

namespace DBus::Bluez
{
  class Manager : public QObject
  {
    Q_OBJECT

  // Public static APi
  public:
    static Manager &instance();

    static bool initialize() { return instance().initializeInstance(); }
    static Adapter *adapter() { return instance().m_adapter; }
    static DeviceMap &devices() { return instance().m_devices; }
    static MediaPlayerMap &mediaPlayers() { return instance().m_mediaPlayers; }
    static MediaControlMap &mediaControls() { return instance().m_mediaControls; }

    /// Return an object of type T associated with the given object
    /// (different interfaces of the same dbus object).
    /// Returns nullptr if no such object exists.
    template<class T>
    static T *getObject(const Object &object) { return getObject<T>(object.path()); }

    /// Return an object of type T associated with the given object
    /// (different interfaces of the same dbus object).
    /// Returns nullptr if no such object exists.
    template<class T>
    static T *getObject(const Object *object) { return (object ? getObject<T>(object->path()) : nullptr); }

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
    // inline bool getAssociatedObject(const Object &source, Adapter *(&adapter))
    // { return getObject(source.path(), adapter); }

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
    explicit Manager(QObject *parent = nullptr);

  private slots:
    void onInterfacesAdded(const QDBusObjectPath &path, const InterfaceMap &interfaces);
    void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

  // Init functions
  private:
    bool initializeInstance();
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
  };
}

#include "Manager.tpp"

// To remove warning from "unused" include above
namespace {
  static constexpr int __unused__ = DBus::Bluez::__unused__;
}

#undef BLUEZ_OBJECT_TYPELIST
#undef DECLARE_MANAGED_OBJECT
#undef LIST
