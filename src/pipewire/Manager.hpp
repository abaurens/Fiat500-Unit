#pragma once
#include "pipewire/Context.hpp"
#include "pipewire/Direction.hpp"

#include "pipewire/objects/Node.hpp"
#include "pipewire/objects/Port.hpp"
#include "pipewire/objects/Link.hpp"
#include "pipewire/objects/Device.hpp"
#include "pipewire/objects/Object.hpp"

#include "Scope.hpp"
#include "SignalAwaiter.hpp"
#include "AbstractManager.hpp"

#include <QObject>
#include <unordered_map>

#define DECLARE_MANAGED_OBJECT(_type, _name)  \
public:                                       \
  static _type *_name(u32 id) {               \
    Object *obj = object(id);                 \
    if (!obj)                                 \
      return nullptr;                         \
    return obj->safeAs<_type>();              \
  }                                           \
  Q_SIGNAL void _name##Added(_type &_name);   \
  Q_SIGNAL void _name##Removed(_type &_name)

namespace PipeWire
{
  class Manager : public QObject, public ManagerType
  {
    Q_OBJECT

    MANAGER_OBJECT

    DECLARE_MANAGED_OBJECT(Link,   link);
    DECLARE_MANAGED_OBJECT(Node,   node);
    DECLARE_MANAGED_OBJECT(Port,   port);
    DECLARE_MANAGED_OBJECT(Device, device);

  public:
    static Context &context() { return instance().m_pipewire; }

    static Object *object(u32 id);

    static Device *deviceForNode(u32 nodeId);

    static QList<Port *> portsForNode(u32 nodeId);
    static QList<Node *> nodesForDevice(u32 deviceId);
    static QList<Link *> linksForPort(u32 portId, Direction directionFilter = Direction::Unknown);

    static QList<Node *> connectedNodes(u32 nodeId, Direction direction = Direction::Unknown);

    template<std::predicate<Object&> Predicate>
    static AsyncTask<Object &> waitForObject(Predicate &&predicate)
    {
      for (auto &[_, object] : instance().m_objects)
      {
        if (std::invoke(predicate, *object))
          co_return *object;
      }

      co_return co_await waitForSignal(
        &instance(), &Manager::objectAdded,
        std::forward<Predicate>(predicate)
      );
    }

    static AsyncTask<Object &> waitForObject(u32 id)
    {
      return waitForObject(
        [id](Object &object) { return object.id() == id; }
      );
    }

    template<std::derived_from<Object> T, std::predicate<T&> Predicate>
    static AsyncTask<T &> waitFor(Predicate &&predicate)
    {
      Object &object = co_await waitForObject(
        [predicate = std::forward<Predicate>(predicate)](Object &object) mutable {
          T *obj = object.safeAs<T>();
          return obj && std::invoke(predicate, *obj);
        }
      );

      co_return object.as<T>();
    }

    template<std::derived_from<Object> T>
    static AsyncTask<T &> waitFor(u32 id)
    {
      return waitFor<T>(
        [id](T &object) { return object.id() == id; }
      );
    }

    template<std::derived_from<Object> T>
    static QList<T *> objects()
    {
      QList<T *> result;

      //result.reserve(instance().m_objects.size());
      for (const auto &[id, object] : instance().m_objects)
      {
        if (auto *typed = object->safeAs<T>())
          result.append(typed);
      }

      return result;
    }

  signals:
    void objectAdded(PipeWire::Object &object);
    void objectRemoved(PipeWire::Object &object);

    void nodeAudioFormatChanged(PipeWire::Node &node, PipeWire::AudioFormat &format);

  private slots:
    void onObjectRemoved(u32 id);
    void onObjectCreated(u32 id, u32 permissions, const char *type, u32 version, const spa_dict *props);

  private:
    Scope<Object> createObject(u32 id, u32 version, Object::Type type, const spa_dict *props = nullptr);

  private:
    Context m_pipewire;

    std::unordered_map<u32, Scope<Object>> m_objects;
  };
}

#undef DECLARE_MANAGED_OBJECT
