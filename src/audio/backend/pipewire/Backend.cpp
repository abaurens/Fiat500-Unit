#include "pch.hpp" // IWYU pragma: keep

// PipeWire library needs access to these functions from the root namespace.
// Somehow, it fails to find it with <math.h> using directives. So we redefine the macros here
#define isnormal std::isnormal
#define isinf    std::isinf
#define signbit  std::signbit

#include <pipewire/pipewire.h>
#include <pipewire/main-loop.h>
#include <pipewire/context.h>
#include <pipewire/core.h>

#include "audio/backend/pipewire/Node.hpp"
#include "audio/backend/pipewire/Port.hpp"
#include "audio/backend/pipewire/Client.hpp"
#include "audio/backend/pipewire/Device.hpp"
#include "audio/backend/pipewire/Backend.hpp"

#include <QDebug>

#include <memory>
#include <string_view>

namespace Audio
{

  std::unique_ptr<Audio::Backend> Backend::create()
  {
    return std::make_unique<PipeWire::Backend>();
  }

}

namespace Audio::PipeWire
{

  void Backend::onGlobalRemove(void *vdata, uint32_t id)
  {
    Backend *backend = static_cast<Backend *>(vdata);

    auto it = backend->m_objects.find(id);
    if (it == backend->m_objects.cend())
      return;

    backend->m_objects.erase(it);
  }

  void Backend::onGlobal(void *vdata, uint32_t id, uint32_t perms, const char *ctype, uint32_t vers, const struct spa_dict *props)
  {
    constexpr std::string_view InterfacePrefix = "PipeWire:Interface:";

    Backend *backend = static_cast<Backend *>(vdata);

    Q_UNUSED(perms)
    Q_UNUSED(vers)

    std::string_view type(ctype);

    if (!type.starts_with(InterfacePrefix))
    {
      /// non-interface types are not supported
      return;
    }

    type = type.substr(InterfacePrefix.size());
    Object::Type otype = Object::Type::FromName(type);

    //Log::debug(u"Backend::onGlobal"_s).noquote().nospace()
    //  << "Parsing object type '" << ctype
    //  << "' results in Object::Type::" << otype.name() << " (" << otype.value() << ")";
    backend->addObject(id, otype, props);
    //qDebug() << ' ';
  }

  Backend::Backend() : m_registry{}
  {
    static const pw_registry_events registryEvents =
    {
      .version       = PW_VERSION_REGISTRY_EVENTS,
      .global        = &Backend::onGlobal,
      .global_remove = &Backend::onGlobalRemove,
    };

    pw_init(nullptr, nullptr);

    m_loop = pw_main_loop_new(nullptr);

    if (!m_loop)
      throw std::runtime_error("Unable to create PipeWire main loop");

    m_context = pw_context_new(
      pw_main_loop_get_loop(m_loop),
      nullptr,
      0
    );

    if (!m_context)
      throw std::runtime_error("Unable to create PipeWire context");

    m_core = pw_context_connect(
      m_context,
      nullptr,
      0
    );

    if (!m_core)
      throw std::runtime_error("Unable to connect to PipeWire server");

    m_registry = pw_core_get_registry(
      m_core,
      PW_VERSION_REGISTRY,
      0
    );

    if (!m_registry)
      throw std::runtime_error("Unable to get PipeWire registry");

    pw_registry_add_listener(
      m_registry,
      &m_registryListener,
      &registryEvents,
      this
    );

    Log::debug(u"PipeWire/Backend"_s) << "Connected to PipeWire";
  }

  Backend::~Backend()
  {
    pw_main_loop_quit(m_loop);

    if (m_thread.joinable())
      m_thread.join();

    Log::debug(u"PipeWire/Backend"_s) << "Main loop stopped";

    if (m_core)
      pw_core_disconnect(m_core);

    if (m_context)
      pw_context_destroy(m_context);

    if (m_loop)
      pw_main_loop_destroy(m_loop);
  }

  void Backend::run()
  {
    Log::debug(u"PipeWire/Backend"_s) << "Starting main loop";
    m_thread = std::thread([this] { pw_main_loop_run(m_loop); });
  }

  Scope<Object> Backend::addObject(uint32_t id, Object::Type type, const spa_dict *props)
  {
    Scope<Object> result;

    switch (type)
    {
    case Object::Type::Node:
      result = make_scope<Node>(id, props);
      break;

    case Object::Type::Port:
      result = make_scope<Port>(id, props);
      break;

    case Object::Type::Client:
      result = make_scope<Client>(id, props);
      break;

    case Object::Type::Device:
      result = make_scope<Device>(id, props);
      break;

    default:
      ;//result = make_scope<Object>(id, type, props);
    }

    if (result)
      m_objects.emplace(id, std::move(result));
    return result;
  }
}
