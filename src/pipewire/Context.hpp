#pragma once

#include <spa/utils/hook.h>

struct spa_dict;

struct pw_loop;
struct pw_core;
struct pw_node;
struct pw_device;
struct pw_context;
struct pw_registry;

#include <pipewire/core.h>

#include <QObject>
#include <QSocketNotifier>

#include "pipewire/handles/Filter.hpp" // IWYU pragma: keep

namespace PipeWire
{
  class Context : public QObject
  {
    Q_OBJECT

  public:
    explicit Context(QObject *parent = nullptr);

    void initialize();
    void deinitialize();

    template<class Proxy, u32 MaxVersion>
    Proxy *bindObject(u32 id, u32 version, const char *interfaceType)
    {
      return static_cast<Proxy *>(
        pw_registry_bind(
          m_registry,
          id,
          interfaceType,
          std::min(version, u32{MaxVersion}),
          0
        )
      );
    }

    pw_node *bindNode(u32 id, u32 version);
    pw_device *bindDevice(u32 id, u32 version);

    //template<DerivedFromHandle T>
    //Scope<T> createHandle(const QString &name) { return Scope<T>{}; }

    Scope<Filter> createFilter(std::string_view name);

  signals:
    void objectRemoved(u32 id);
    void objectCreated(u32 id, u32 perms, const char *type, u32 version, const spa_dict *props);

  private slots:
    void iterate();

  private:
    static void onGlobalRemove(void *data, u32 id);
    static void onGlobal(void *data, u32 id, u32 permissions, const char *type, u32 version, const spa_dict *props);

  private:
    pw_loop      *m_loop = nullptr;
    pw_context   *m_context = nullptr;
    pw_core      *m_core = nullptr;
    pw_registry  *m_registry = nullptr;

    spa_hook      m_registryListener;

    Local<QSocketNotifier> m_pipeWireNotifier = std::nullopt;
  };

}
