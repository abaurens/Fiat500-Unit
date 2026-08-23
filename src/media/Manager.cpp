#include "pch.hpp" // IWYU pragma: keep

#include "pipewire/Node.hpp"
#include "pipewire/Port.hpp"
#include "pipewire/Link.hpp"
#include "pipewire/Device.hpp"
#include "pipewire/Client.hpp"
#include "pipewire/Object.hpp"

#include "Manager.hpp"

namespace Media
{
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

  Scope<PipeWire::Object> Manager::addObject(uint32_t id, PipeWire::Object::Type type, const spa_dict *props)
  {
    Scope<PipeWire::Object> result;

    switch (type)
    {
    case PipeWire::Object::Type::Node:
      result = PipeWire::Node::Create(id, props);
      break;

    case PipeWire::Object::Type::Port:
      result = make_scope<PipeWire::Port>(id, props);
      break;

    case PipeWire::Object::Type::Client:
      result = make_scope<PipeWire::Client>(id, props);
      break;

    case PipeWire::Object::Type::Device:
      result = make_scope<PipeWire::Device>(id, props);
      break;

    case PipeWire::Object::Type::Link:
      result = make_scope<PipeWire::Link>(id, props);
      break;

    default:
      ;//result = make_scope<Object>(id, type, props);
    }

    if (result)
      m_objects.emplace(id, std::move(result));
    return result;
  }

  void Manager::onObjectRemoved(u32 id)
  {
    auto it = m_objects.find(id);
    if (it == m_objects.cend())
      return;

    m_objects.erase(it);
  }

  void Manager::onObjectCreated(u32 id, u32 permissions, const char *ctype, u32 version, const spa_dict *props)
  {
    constexpr std::string_view InterfacePrefix = "PipeWire:Interface:";

    Q_UNUSED(permissions)
    Q_UNUSED(version)

    std::string_view type(ctype);

    if (!type.starts_with(InterfacePrefix))
    {
      /// non-interface types are not supported
      return;
    }

    type = type.substr(InterfacePrefix.size());
    PipeWire::Object::Type otype = PipeWire::Object::Type::FromName(type);

    //Log::debug(u"Backend::onGlobal"_s).noquote().nospace()
    //  << "Parsing object type '" << ctype
    //  << "' results in Object::Type::" << otype.name() << " (" << otype.value() << ")";
    addObject(id, otype, props);
    //qDebug() << ' ';
  }
}
