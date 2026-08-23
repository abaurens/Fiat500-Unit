#pragma once
#include "pipewire/Context.hpp"

//#include <spa/utils/hook.h>

#include "Scope.hpp"

#include "AbstractManager.hpp"
#include "pipewire/Object.hpp"
// #include "pipewire/Backend.hpp"

// #include <thread>
#include <unordered_map>

#include <QObject>
//#include <QSocketNotifier>

struct pw_core;
struct pw_context;
struct pw_registry;
struct pw_loop;

namespace Media
{
  class Manager : public QObject, public ManagerType
  {
    Q_OBJECT

    MANAGER_OBJECT

  private slots:
    void onObjectRemoved(u32 id);
    void onObjectCreated(u32 id, u32 permissions, const char *type, u32 version, const spa_dict *props);

    Scope<PipeWire::Object> addObject(uint32_t id, PipeWire::Object::Type type, const spa_dict *props = nullptr);

  private:
    PipeWire::Context m_pipewire;

    std::unordered_map<uint32_t, Scope<PipeWire::Object>> m_objects;
  };

}
