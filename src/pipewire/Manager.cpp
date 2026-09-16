#include "pch.hpp" // IWYU pragma: keep

#include "Manager.hpp"
// #include "SignalAwaiter.hpp"
#include "pipewire/objects/Client.hpp"

#define ASSERT_OBJECT_TYPE(id, _type) do {      \
  [[maybe_unused]] const Object *obj;           \
  Q_ASSERT(                                     \
      (obj = object(id)) != nullptr             \
    && obj->type() == _type::StaticType         \
  );                                            \
} while (false)

namespace PipeWire
{
  Object *Manager::object(u32 id)
  {
    auto it = instance().m_objects.find(id);

    if (it == instance().m_objects.end())
      return nullptr;

    return it->second.get();
  }

  Device *Manager::deviceForNode(u32 nodeId)
  {
    ASSERT_OBJECT_TYPE(nodeId, Node);

    Node *nde = node(nodeId);

    if (!nde)
      return nullptr;

    const std::optional<u32> id = nde->deviceId();

    if (!id)
      return nullptr;

    return device(*id);
  }

  QList<Link *> Manager::linksForPort(u32 portId, Direction directionFilter)
  {
    ASSERT_OBJECT_TYPE(portId, Port);

    QList<Link *> result;

    const bool acceptInput  = (directionFilter != Direction::Out);
    const bool acceptOutput = (directionFilter != Direction::In);

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != Object::Type::Link)
        continue;

      auto *link = object->safeAs<Link>();

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

  QList<Port *> Manager::portsForNode(u32 nodeId)
  {
    ASSERT_OBJECT_TYPE(nodeId, Node);

    QList<Port *> result;

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != Object::Type::Port)
        continue;

      auto *port = object->safeAs<Port>();

      if (port && port->nodeId() == nodeId)
        result.append(port);
    }

    return result;
  }

  QList<Node *> Manager::nodesForDevice(u32 deviceId)
  {
    ASSERT_OBJECT_TYPE(deviceId, Device);

    QList<Node *> result;

    for (auto &[id, object] : instance().m_objects)
    {
      if (object->type() != Object::Type::Node)
        continue;

      auto *node = object->safeAs<Node>();

      if (!node)
        continue;

      const auto nodeDeviceId = node->deviceId();

      if (nodeDeviceId && *nodeDeviceId == deviceId)
        result.append(node);
    }

    return result;
  }

  QList<Node *> Manager::connectedNodes(u32 nodeId, Direction direction)
  {
    QHash<u32, Node *> result;

    for (const auto *port : portsForNode(nodeId))
    {
      if (direction != Direction::Unknown && port->direction() != direction)
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


  //AsyncTask<Object &> Manager::waitForObject(u32 id)
  //{
  //  return waitForObject(
  //    [id](Object &object) { return object.id() == id; }
  //  );
  //}

  //AsyncTask<Object &> Manager::waitForObject(u32 id)
  //{
  //  return ObjectAwaiter{ id };
  //}

  Manager::Manager(QObject *owner) : ManagerType { owner }, m_pipewire{}
  {}

  bool Manager::initializeInstance()
  {
    Log::debug(u"Manager"_s) << "Initializing";

    connect(
      &m_pipewire, &Context::objectCreated,
      this,        &Manager::onObjectCreated,
      Qt::DirectConnection
    );
    connect(
      &m_pipewire, &Context::objectRemoved,
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
  Scope<Object> Manager::createObject(u32 id, u32 version, Object::Type type, const spa_dict *props)
  {
    Scope<Object> object = nullptr;

    switch (type)
    {
    case Object::Type::Node:
      return makeScope<Node>(id, m_pipewire.bindNode(id, version), props);
      break;

    case Object::Type::Port:
      return makeScope<Port>(id, props);
      break;

    case Object::Type::Device:
      return makeScope<Device>(id, m_pipewire.bindDevice(id, version), props);
      break;

    case Object::Type::Link:
      return makeScope<Link>(id, props);
      break;

    case Object::Type::Client:
      return makeScope<Client>(id, props);
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
    const Object::Type otype = Object::Type::FromName(type);

    // Create the object
    Scope<Object> object = createObject(id, version, otype, props);
    if (!object)
      return;

    Object &obj = *object;

    m_objects.emplace(id, std::move(object));

    emit objectAdded(obj);
    switch (obj.type())
    {
    case Object::Type::Node:
      emit nodeAdded(static_cast<Node&>(obj));
      break;
    case Object::Type::Device:
      emit deviceAdded(static_cast<Device&>(obj));
      break;
    case Object::Type::Port:
      emit portAdded(static_cast<Port&>(obj));
      break;
    case Object::Type::Link:
      emit linkAdded(static_cast<Link&>(obj));
      break;
    default:
      break;
    }
  }
}
