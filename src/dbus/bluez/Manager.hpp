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

#define DECLARE_MANAGED_OBJECT(_type, _name)                                    \
    void _name##Added(const QDBusObjectPath &path, DBus::Bluez::_type &object); \
    void _name##Removed(const QDBusObjectPath &path);                           \
  private:                                                                      \
    void addObject(_type &device);                                              \
    void removeObject(_type &device);                                           \
    bool getObject(const QDBusObjectPath &path, _type *(&device));              \
    _type##Map m_##_name##s

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

  // supported objects implementation
  private:
    void addObject(Adapter &adapter);
    void removeObject(Adapter &device);
    bool getObject(const QDBusObjectPath &path, Adapter *(&adapter));

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
    size_t createObjects(const QDBusObjectPath &path, const InterfaceMap &interfaces) {
      return tryCreateAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }

    size_t removeObjects(const QDBusObjectPath &path, const QStringList &interfaces) {
      return tryRemoveAll<BLUEZ_OBJECT_TYPELIST>(path, interfaces);
    }

    template<class ... ObjTypes>
    size_t tryCreateAll(const QDBusObjectPath &path, const InterfaceMap &interfaces);

    template<class ... ObjTypes>
    size_t tryRemoveAll(const QDBusObjectPath &path, const QStringList &interfaces);

    template<class ObjType>
    size_t  tryCreate(const QDBusObjectPath &path, const InterfaceMap &interfaces);

    template<class ObjType>
    size_t  tryRemove(const QDBusObjectPath &path, const QStringList &interfaces);
  };
}

#include "Manager.tpp"

#undef BLUEZ_OBJECT_TYPELIST
#undef DECLARE_MANAGED_OBJECT
#undef LIST
