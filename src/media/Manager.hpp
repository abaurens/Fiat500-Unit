#pragma once
#include "pipewire/Context.hpp"
#include "pipewire/Object.hpp"

#include "AbstractManager.hpp"
#include "Scope.hpp"

#include <unordered_map>
#include <QObject>

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
