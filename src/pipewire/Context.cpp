// PipeWire library needs access to these functions from the root namespace.
// Somehow, it fails to find it with <math.h> using directives. So we redefine the macros here
#define isnormal std::isnormal
#define isinf    std::isinf
#define signbit  std::signbit

#include <pipewire/pipewire.h>
#include <pipewire/main-loop.h>
#include <pipewire/context.h>
#include <pipewire/core.h>

#include "Context.hpp"

namespace PipeWire
{

  void Context::onGlobalRemove(void *vdata, uint32_t id)
  {
    Context *context = static_cast<Context *>(vdata);

    emit context->objectRemoved(id);
  }

  void Context::onGlobal(void *vdata, uint32_t id, uint32_t perms, const char *ctype, uint32_t vers, const struct spa_dict *props)
  {
    Context *context = static_cast<Context *>(vdata);

    emit context->objectCreated(id, perms, ctype, vers, props);
  }


  Context::Context(QObject *parent) : QObject{parent}
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

    const int fd = pw_loop_get_fd(m_loop);

    m_pipeWireNotifier = new QSocketNotifier(
      fd,
      QSocketNotifier::Read,
      this
    );

    connect(
      m_pipeWireNotifier, &QSocketNotifier::activated,
      this,               &Context::iterate
    );

    Log::debug(u"PipeWire/Backend"_s) << "Connected to PipeWire";
  }

  void Context::deinitialize()
  {
    if (m_pipeWireNotifier)
    {
      m_pipeWireNotifier->setEnabled(false);
      m_pipeWireNotifier->deleteLater();
      m_pipeWireNotifier = nullptr;
    }

    //pw_main_loop_quit(m_loop);

    //if (m_thread.joinable())
    //  m_thread.join();

    //Log::debug(u"PipeWire/Backend"_s) << "Main loop stopped";

    if (m_core)
      pw_core_disconnect(m_core);

    if (m_context)
      pw_context_destroy(m_context);

    if (m_loop)
      pw_loop_destroy(m_loop);
    //if (m_loop)
    //  pw_main_loop_destroy(m_loop);
  }

}
