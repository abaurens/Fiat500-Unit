#include "pch.hpp" // IWYU pragma: keep

#include "Manager.hpp"
#include "Types.hpp"

#include <QDBusMessage>

namespace DBG
{
  constexpr bool log_initialize = true;
  constexpr bool log_interface_added = false;
  constexpr bool log_interface_removed = false;

  constexpr bool log_adapter_added = false;
  constexpr bool log_adapter_removed = false;
  constexpr bool log_device_added = false;
  constexpr bool log_device_removed = false;
  constexpr bool log_mediaControl_added = true;
  constexpr bool log_mediaControl_removed = true;
  constexpr bool log_mediaPlayer_added = true;
  constexpr bool log_mediaPlayer_removed = true;
}

#define LOG_DBG(_scope, _case, _msg) do { if constexpr (DBG::log_##_case) { Log::debug(_scope) << _msg; } } while(0)
#define TYP_LOG_DBG(_scope, _case, _type, _msg) do { if constexpr (DBG::log_##_case) { (Log::debug(_scope).noquote() << _type::TypeName).quote() << _msg; } } while(0)

#define OBJ_TYPE_OF(_name) std::remove_pointer_t<typename decltype(m_##_name##s)::mapped_type>

#define OBJ_MAP_NAME(_name) m_##_name##s
#define OBJ_ADDED_SIGNAL(_name) _name##Added
#define OBJ_REMOVED_SIGNAL(_name) _name##Removed

#define DEFINE_MANAGED_OBJECT(_name) \
  void Manager::addObject(OBJ_TYPE_OF(_name) &_name)                                                \
  {                                                                                                 \
    using ObjType = OBJ_TYPE_OF(_name);                                                             \
    if (!addObjectImpl<ObjType, &Manager::OBJ_ADDED_SIGNAL(_name)>(OBJ_MAP_NAME(_name), _name))     \
      return;                                                                                       \
    TYP_LOG_DBG(u"Manager"_s, _name##_added, ObjType, "added:" << _name);                           \
  }                                                                                                 \
  void Manager::removeObject( OBJ_TYPE_OF(_name) &_name)                                            \
  {                                                                                                 \
    using ObjType = OBJ_TYPE_OF(_name);                                                             \
    if (removeObjectImpl<ObjType, &Manager::OBJ_REMOVED_SIGNAL(_name)>(OBJ_MAP_NAME(_name), _name)) \
      TYP_LOG_DBG(u"Manager"_s, mediaControl_removed, ObjType, "removed:" << _name.path().path());  \
  }

// Bluez::Object types support
namespace DBus::Bluez
{
  // Adapter object
  void Manager::addObject(Adapter &adapter)
  {
    // We don't want to replace the current adapter.
    if (m_adapter)
    {
      TYP_LOG_DBG(u"Manager"_s, interface_added, Adapter, "appeared:" << adapter.path());
      TYP_LOG_DBG(u"Manager"_s, interface_added, Adapter, "  We currently only support 1 adapter...");
      return;
    }

    TYP_LOG_DBG(u"Manager"_s, adapter_added, Adapter, "set:" << adapter);

    m_adapter = &adapter;
  }

  void Manager::removeObject(Adapter &adapter)
  {
    const Object::Path &path = adapter.path();

    if (!m_adapter || path != m_adapter->path())
      return;

    TYP_LOG_DBG(u"Manager"_s, adapter_added, Adapter, "removed:" << adapter.path().path());

    delete m_adapter;
    m_adapter = nullptr;
  }

  bool Manager::getObject(const Object::Path &path, Adapter *(&adapter))
  {
    if (!m_adapter || path != m_adapter->path())
      return false;

    adapter = m_adapter;
    return true;
  }

  // Device object
  DEFINE_MANAGED_OBJECT(device);

  // MediaControl object
  DEFINE_MANAGED_OBJECT(mediaControl);

  // Player object
  DEFINE_MANAGED_OBJECT(mediaPlayer);
}

// Implementation details
namespace DBus::Bluez
{
  Manager::Manager(QObject *owner) : ManagerType { owner }, m_bus{ QDBusConnection::systemBus() }
  {}

  bool Manager::initializeInstance()
  {
    if (!m_bus.isConnected())
    {
      Log::critical(u"Manager"_s) << "Unable to connect to system bus";
      return false;
    }

    LOG_DBG(u"Manager"_s, initialize, "Connected to system bus");

    ManagedObjectMap objects = loadManagedObjects();

    LOG_DBG(u"Manager"_s, initialize, "Objects:" << objects.size());

    for (auto it = objects.cbegin(); it != objects.cend(); ++it)
    {
      const InterfaceMap &interfaces = it.value();

      // Try to create the object with each of the passed types
      createObjects(it.key(), interfaces);
    }

    m_bus.connect(
      ServiceName, RootPath, Interface::ObjectManager, Method::InterfacesAdded,
      this, SLOT(onInterfacesAdded(QDBusObjectPath, DBus::InterfaceMap))
    );
    m_bus.connect(
      ServiceName, RootPath, Interface::ObjectManager, Method::InterfacesRemoved,
      this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList))
    );

    return true;
  }

  void Manager::deinitializeInstance()
  {
    Log::debug(u"Manager"_s) << "Deinitializing";

    m_bus.disconnect(
      ServiceName, RootPath, Interface::ObjectManager, Method::InterfacesRemoved,
      this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList))
    );
    m_bus.disconnect(
      ServiceName, RootPath, Interface::ObjectManager, Method::InterfacesAdded,
      this, SLOT(onInterfacesAdded(QDBusObjectPath, DBus::InterfaceMap))
    ),
    m_adapter = nullptr;
  }

  ManagedObjectMap Manager::loadManagedObjects()
  {
    const QDBusMessage message = QDBusMessage::createMethodCall(
      ServiceName, RootPath, Interface::ObjectManager, Method::GetManagedObjects
    );

    const QDBusMessage reply = m_bus.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
      Log::critical(u"Manager"_s) << reply.errorMessage();
      return {};
    }

    const QVariant first = reply.arguments().at(0);
    const QDBusArgument dbusArg = first.value<QDBusArgument>();

    ManagedObjectMap objects;
    dbusArg >> objects;

    return objects;
  }

  void Manager::onInterfacesAdded(const Object::Path &path, const InterfaceMap &interfaces)
  {
    LOG_DBG(u"Manager"_s, interface_added, "Added:" << path.path());

    createObjects(path, interfaces);
  }

  void Manager::onInterfacesRemoved(const Object::Path &path, const QStringList &interfaces)
  {
    LOG_DBG(u"Manager"_s, interface_removed, "Removed:" << path.path());

    removeObjects(path, interfaces);
  }
}
