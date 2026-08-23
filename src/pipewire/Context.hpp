#pragma once

#include <spa/utils/hook.h>

struct spa_dict;

struct pw_loop;
struct pw_core;
struct pw_context;
struct pw_registry;

#include <QObject>
#include <QSocketNotifier>

namespace PipeWire
{

  class Context : public QObject
  {
    Q_OBJECT

  public:
    explicit Context(QObject *parent = nullptr);

    void initialize();
    void deinitialize();

  signals:
    void objectRemoved(u32 id);
    void objectCreated(u32 id, u32 perms, const char *type, u32 version, const struct spa_dict *props);

  private slots:
    void iterate();

  private:
    static void onGlobalRemove(void *data, uint32_t id);
    static void onGlobal(void *data, uint32_t id, uint32_t permissions, const char *type, uint32_t version, const spa_dict *props);

  private:
    pw_loop      *m_loop = nullptr;
    pw_context   *m_context = nullptr;
    pw_core      *m_core = nullptr;
    pw_registry  *m_registry = nullptr;

    spa_hook      m_registryListener;

    QSocketNotifier *m_pipeWireNotifier = nullptr;
  };

}
