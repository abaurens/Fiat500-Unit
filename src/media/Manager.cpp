#include "pch.hpp" // IWYU pragma: keep

#include "Manager.hpp"
#include "pipewire/ObjectAwaiter.hpp"
#include "pipewire/objects/Client.hpp"

#define ASSERT_OBJECT_TYPE(id, _type) do {      \
  [[maybe_unused]] const PipeWire::Object *obj; \
  Q_ASSERT(                                     \
      (obj = object(id)) != nullptr             \
    && obj->type() == _type::StaticType         \
  );                                            \
} while (false)

namespace Media
{
  PipeWire::Object *Manager::object(u32 id)
  {
    auto it = instance().m_objects.find(id);

    if (it == instance().m_objects.end())
      return nullptr;

    return it->second.get();
  }

  PipeWire::Device *Manager::deviceForNode(u32 nodeId)
  {
    ASSERT_OBJECT_TYPE(nodeId, PipeWire::Node);

    PipeWire::Node *nde = node(nodeId);

    if (!nde)
      return nullptr;

    const std::optional<u32> id = nde->deviceId();

    if (!id)
      return nullptr;

    return device(*id);
  }

  QList<PipeWire::Link *> Manager::linksForPort(u32 portId, PipeWire::Direction directionFilter)
  {
    ASSERT_OBJECT_TYPE(portId, PipeWire::Port);

    QList<PipeWire::Link *> result;

    const bool acceptInput  = (directionFilter != PipeWire::Direction::Out);
    const bool acceptOutput = (directionFilter != PipeWire::Direction::In);

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != PipeWire::Object::Type::Link)
        continue;

      auto *link = object->safeAs<PipeWire::Link>();

      if (!link)
        continue;

      const bool take =
        (acceptInput && link->inputPortId() == portId)
        ||
        (acceptOutput && link->outputPortId() == portId);

      if (take)
        result.append(link);
    }

    return result;
  }

  QList<PipeWire::Port *> Manager::portsForNode(u32 nodeId)
  {
    ASSERT_OBJECT_TYPE(nodeId, PipeWire::Node);

    QList<PipeWire::Port *> result;

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != PipeWire::Object::Type::Port)
        continue;

      auto *port = object->safeAs<PipeWire::Port>();

      if (port && port->nodeId() == nodeId)
        result.append(port);
    }

    return result;
  }

  QList<PipeWire::Node *> Manager::nodesForDevice(u32 deviceId)
  {
    ASSERT_OBJECT_TYPE(deviceId, PipeWire::Device);

    QList<PipeWire::Node *> result;

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != PipeWire::Object::Type::Node)
        continue;

      auto *node = object->safeAs<PipeWire::Node>();

      if (!node)
        continue;

      const auto nodeDeviceId = node->deviceId();

      if (nodeDeviceId && *nodeDeviceId == deviceId)
        result.append(node);
    }

    return result;
  }

  QList<PipeWire::Node *> Manager::connectedNodes(u32 nodeId, PipeWire::Direction direction)
  {
    QHash<u32, PipeWire::Node *> result;

    for (const auto *port : portsForNode(nodeId))
    {
      if (direction != PipeWire::Direction::Unknown && port->direction() != direction)
        continue;

      for (const auto *link : linksForPort(port->id(), direction))
      {
        u32 remoteNodeId;

        if (link->inputPortId() == port->id())
          remoteNodeId = link->outputNodeId();
        else
          remoteNodeId = link->inputNodeId();

        auto *remote = node(remoteNodeId);

        if (remote && !result.contains(remote->id()))
          result.insert(remote->id(), remote);
      }
    }

    return result.values();
  }

  PipeWire::ObjectAwaiter Manager::waitForObject(u32 id)
  {
    return PipeWire::ObjectAwaiter{ id };
  }




  Manager::Manager(QObject *owner) : ManagerType { owner }, m_pipewire{}
  {}

  bool Manager::initializeInstance()
  {
    Log::debug(u"Manager"_s) << "Initializing";

    connect(
      &m_pipewire, &PipeWire::Context::objectCreated,
      this,        &Manager::onObjectCreated,
      Qt::DirectConnection
    );
    connect(
      &m_pipewire, &PipeWire::Context::objectRemoved,
      this,        &Manager::onObjectRemoved
    );

    m_pipewire.initialize();

    return true;
  }

  void Manager::deinitializeInstance()
  {
    Log::debug(u"Manager"_s) << "Deinitializing";

    m_pipewire.deinitialize();

    disconnect(&m_pipewire, nullptr, this, nullptr);
  }

  /// Create the objects of which we know and supports the type
  Scope<PipeWire::Object> Manager::createObject(u32 id, u32 version, PipeWire::Object::Type type, const spa_dict *props)
  {
    Scope<PipeWire::Object> object = nullptr;

    switch (type)
    {
    case PipeWire::Object::Type::Node:
      return makeScope<PipeWire::Node>(id, m_pipewire.bindNode(id, version), props);
      break;

    case PipeWire::Object::Type::Port:
      return makeScope<PipeWire::Port>(id, props);
      break;

    case PipeWire::Object::Type::Device:
      return makeScope<PipeWire::Device>(id, m_pipewire.bindDevice(id, version), props);
      break;

    case PipeWire::Object::Type::Link:
      return makeScope<PipeWire::Link>(id, props);
      break;

    case PipeWire::Object::Type::Client:
      return makeScope<PipeWire::Client>(id, props);
      break;

    default:
      //object = makeScope<Object>(id, version, type, props);
      break;
    }

    return nullptr;
  }

  void Manager::onObjectRemoved(u32 id)
  {
    auto it = m_objects.find(id);
    if (it == m_objects.cend())
      return;

    emit objectRemoved(*it->second);
    m_objects.erase(it);
  }

  void Manager::onObjectCreated(u32 id, u32 permissions, const char *ctype, u32 version, const spa_dict *props)
  {
    constexpr std::string_view InterfacePrefix = "PipeWire:Interface:";

    Q_UNUSED(permissions)

    std::string_view type(ctype);

    if (!type.starts_with(InterfacePrefix))
    {
      /// non-interface types are not supported
      return;
    }

    type = type.substr(InterfacePrefix.size());
    const PipeWire::Object::Type otype = PipeWire::Object::Type::FromName(type);

    // Create the object
    Scope<PipeWire::Object> object = createObject(id, version, otype, props);
    if (!object)
      return;

    PipeWire::Object &obj = *object;

    m_objects.emplace(id, std::move(object));
    emit objectAdded(obj);
  }
}
