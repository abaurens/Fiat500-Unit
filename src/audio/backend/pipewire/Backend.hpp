#pragma once
#include <spa/utils/hook.h>

#include "Scope.hpp"

#include "audio/backend/pipewire/Object.hpp"
#include "audio/Backend.hpp"

#include <thread>
#include <unordered_map>

struct pw_core;
struct pw_context;
struct pw_registry;
struct pw_main_loop;

namespace Audio::PipeWire
{

  class Backend : public Audio::Backend
  {
  public:
    Backend();
    ~Backend() override;

    virtual void run() override;

    /// TODO:
    ///  - Discover registry
    ///  - Enumerate sinks
    ///  - Enumerate sources
    ///  - Subscribe to hotplug events

  private:
    static void onGlobalRemove(void *data, uint32_t id);
    static void onGlobal(void *data, uint32_t id, uint32_t permissions, const char *type, uint32_t version, const struct spa_dict *props);

  private:
    pw_main_loop *m_loop = nullptr; /// TODO: Change to use pw_loop and let Qt's event loop manage PipeWire events
    pw_context   *m_context = nullptr;
    pw_core      *m_core = nullptr;
    pw_registry  *m_registry = nullptr;

    spa_hook      m_registryListener;

    std::thread   m_thread;

    std::unordered_map<uint32_t, Scope<Object>> m_objects;
  };

}
