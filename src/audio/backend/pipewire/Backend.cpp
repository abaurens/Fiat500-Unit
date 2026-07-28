#include <pipewire/pipewire.h>
#include <pipewire/main-loop.h>
#include <pipewire/context.h>
#include <pipewire/core.h>

#include "audio/backend/pipewire/Backend.hpp"

#include <QDebug>

#include <memory>

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
    Q_UNUSED(vdata)

    qDebug()
      << "Removed:"
      << id;
  }

  void Backend::onGlobal(void *vdata, uint32_t id, uint32_t perms, const char *type, uint32_t vers, const struct spa_dict *props)
  {
    Q_UNUSED(vdata)
    Q_UNUSED(perms)
    Q_UNUSED(vers)

    qDebug().nospace() << "PipeWire object: [" << id << "] " << type;
  }

  Backend::Backend() : m_registry{}
  {
    constexpr pw_registry_events registryEvents =
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

    qDebug() << "Connected to PipeWire";
  }

  Backend::~Backend()
  {
    if (m_core)
      pw_core_disconnect(m_core);

    if (m_context)
      pw_context_destroy(m_context);

    if (m_loop)
      pw_main_loop_destroy(m_loop);
  }

  void Backend::run()
  {
    m_thread = std::thread([this] { pw_main_loop_run(m_loop); });

    if (m_thread.joinable())
      m_thread.join();
  }
}
