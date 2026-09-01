#include "pch.hpp"

// PipeWire library needs access to these functions from the root namespace.
// Somehow, it fails to find it with <math.h> using directives. So we redefine the macros here
#define isnormal std::isnormal
#define isinf    std::isinf
#define signbit  std::signbit

#include <pipewire/pipewire.h>
#include <pipewire/main-loop.h>
#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/node.h>

#include "Context.hpp"

namespace PipeWire
{
  Context::Context(QObject *parent) : QObject{ parent }
  {}

  void Context::iterate()
  {
    pw_loop_iterate(m_loop, 0);
  }

  void Context::initialize()
  {
    static const pw_registry_events registryEvents =
    {
      .version       = PW_VERSION_REGISTRY_EVENTS,
      .global        = &Context::onGlobal,
      .global_remove = &Context::onGlobalRemove,
    };

    pw_init(nullptr, nullptr);

    m_loop = pw_loop_new(nullptr);
    if (!m_loop)
      throw std::runtime_error("Unable to create PipeWire loop");

    m_context = pw_context_new(
      m_loop,
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

    m_pipeWireNotifier.emplace(pw_loop_get_fd(m_loop), QSocketNotifier::Read, nullptr);

    connect(
      &m_pipeWireNotifier.value(), &QSocketNotifier::activated,
      this,                        &Context::iterate
    );

    Log::debug(u"PipeWire/Backend"_s) << "Connected to PipeWire";
  }

  void Context::deinitialize()
  {
    if (m_pipeWireNotifier)
    {
      m_pipeWireNotifier->setEnabled(false);
      m_pipeWireNotifier.reset();
    }

    spa_hook_remove(&m_registryListener);

    if (m_registry)
    {
      pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
      m_registry = nullptr;
    }

    if (m_core)
    {
      pw_core_disconnect(m_core);
      m_core = nullptr;
    }

    if (m_context)
    {
      pw_context_destroy(m_context);
      m_context = nullptr;
    }

    if (m_loop)
    {
      pw_loop_destroy(m_loop);
      m_loop = nullptr;
    }

    pw_deinit();
  }

  pw_node *Context::bindNode(u32 id, u32 version)
  {
    return static_cast<pw_node *>(
      pw_registry_bind(
        m_registry,
        id,
        PW_TYPE_INTERFACE_Node,
        std::min(version, u32{PW_VERSION_NODE}),
        0
      )
    );
  }

  pw_device *Context::bindDevice(u32 id, u32 version)
  {
    return bindObject<pw_device, PW_VERSION_DEVICE>(id, version, PW_TYPE_INTERFACE_Device);
  }

  Scope<Filter> Context::createFilter(std::string_view name)
  {
    pw_properties *properties = pw_properties_new(
      PW_KEY_NODE_NAME, name.data(),
      nullptr
    );

    Filter::HandleType *filter = pw_filter_new(
      m_core,
      name.data(),
      properties
    );

    if (!filter)
      return nullptr;

    return makeScope<Filter>(filter);
  }
}


// Static callbacks
void PipeWire::Context::onGlobalRemove(void *vdata, u32 id)
{
  Context *context = static_cast<Context *>(vdata);

  emit context->objectRemoved(id);
}

void PipeWire::Context::onGlobal(void *vdata, u32 id, u32 perms, const char *ctype, u32 vers, const spa_dict *props)
{
  Context *context = static_cast<Context *>(vdata);

  emit context->objectCreated(id, perms, ctype, vers, props);
}

