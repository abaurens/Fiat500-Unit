#pragma once
#include "pipewire/Context.hpp"
#include "pipewire/Direction.hpp"
#include "pipewire/ObjectAwaiter.hpp"

#include "pipewire/objects/Node.hpp"
#include "pipewire/objects/Port.hpp"
#include "pipewire/objects/Link.hpp"
#include "pipewire/objects/Device.hpp"
#include "pipewire/objects/Object.hpp"

#include "Scope.hpp"
#include "AbstractManager.hpp"

#include <QObject>
#include <unordered_map>

#define DECLARE_MANAGED_OBJECT(_type, _name)  \
public:                                       \
  static _type *_name(u32 id) {               \
    PipeWire::Object *obj = object(id);       \
    if (!obj)                                 \
      return nullptr;                         \
    return obj->safeAs<_type>();              \
  }

namespace Media
{
  class Manager : public QObject, public ManagerType
  {
    Q_OBJECT

    MANAGER_OBJECT

    DECLARE_MANAGED_OBJECT(PipeWire::Link,   link)
    DECLARE_MANAGED_OBJECT(PipeWire::Node,   node)
    DECLARE_MANAGED_OBJECT(PipeWire::Port,   port)
    DECLARE_MANAGED_OBJECT(PipeWire::Device, device)

  public:
    static PipeWire::Context &context() { return instance().m_pipewire; }

    static PipeWire::Object *object(u32 id);

    static PipeWire::Device *deviceForNode(u32 nodeId);

    static QList<PipeWire::Port *> portsForNode(u32 nodeId);
    static QList<PipeWire::Node *> nodesForDevice(u32 deviceId);
    static QList<PipeWire::Link *> linksForPort(u32 portId, PipeWire::Direction directionFilter = PipeWire::Direction::Unknown);

    static QList<PipeWire::Node *> connectedNodes(u32 nodeId, PipeWire::Direction direction = PipeWire::Direction::Unknown);

    static PipeWire::ObjectAwaiter waitForObject(u32 id);

  signals:
    void objectAdded(PipeWire::Object &object);
    void objectRemoved(PipeWire::Object &object);

  private slots:
    void onObjectRemoved(u32 id);
    void onObjectCreated(u32 id, u32 permissions, const char *type, u32 version, const spa_dict *props);

  private:
    Scope<PipeWire::Object> createObject(u32 id, u32 version, PipeWire::Object::Type type, const spa_dict *props = nullptr);

  private:
    PipeWire::Context m_pipewire;

    std::unordered_map<u32, Scope<PipeWire::Object>> m_objects;
  };
}

#undef DECLARE_MANAGED_OBJECT
