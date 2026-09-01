#pragma once

#include <pipewire/filter.h>

#include "AsyncTask.hpp"

#include "pipewire/handles/Handle.hpp"

#include <QObject>

namespace PipeWire
{

  class Filter : public QObject, Handle<pw_filter, pw_filter_destroy>
  {
    using Super = Handle<pw_filter, pw_filter_destroy>;

    Q_OBJECT

  public:
    using Super::HandleType;

  public:
    explicit Filter(pw_filter *filter);
    ~Filter();

    u32 nodeId() const { return m_nodeId; }

    bool connect();

    void *addPort(spa_direction direction, pw_properties *properties = nullptr, size_t userDataSize = 0);

   AsyncTask<u32> waitForNodeId() const;

  signals:
    void nodeCreated(u32 id);
    void stateChanged(pw_filter_state oldState, pw_filter_state state, std::string_view error);

  private:
    static void onProcess(void *data, spa_io_position *position);
    static void onStateChanged(void *data, pw_filter_state oldState, pw_filter_state state, const char *error);

    void onProcess(spa_io_position &position);
    void onStateChanged(pw_filter_state oldState, pw_filter_state state, std::string_view error);

  private:
    u32 m_nodeId = SPA_ID_INVALID;
    spa_hook m_listener {};
  };

}
