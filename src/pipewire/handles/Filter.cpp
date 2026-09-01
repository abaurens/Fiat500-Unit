#include "Filter.hpp" // IWYU pragma: keep

#include "SignalAwaiter.hpp"

namespace PipeWire
{
  Filter::Filter(pw_filter *filter) : Super{ filter }
  {
    static constexpr pw_filter_events events {
      .version = PW_VERSION_FILTER_EVENTS,
      .state_changed = &Filter::onStateChanged,
      .process = &Filter::onProcess,
    };

    pw_filter_add_listener(
      filter,
      &m_listener,
      &events,
      this
    );
  }

  Filter::~Filter()
  {
    spa_hook_remove(&m_listener);
  }

  bool Filter::connect()
  {
    const int result = pw_filter_connect(
      handle(),
      PW_FILTER_FLAG_NONE,
      nullptr,
      0
    );

    return result >= 0;
  }

  void *Filter::addPort(spa_direction direction, pw_properties *properties, size_t userDataSize)
  {
    return pw_filter_add_port(
      handle(),
      direction,
      PW_FILTER_PORT_FLAG_MAP_BUFFERS,
      userDataSize,
      properties,
      nullptr,
      0
    );
  }

  AsyncTask<u32> Filter::waitForNodeId() const
  {
    if (m_nodeId != SPA_ID_INVALID)
      co_return m_nodeId;

    co_return co_await waitForSignal<Filter>(this, &Filter::nodeCreated);
  }

  void Filter::onProcess(spa_io_position &position)
  {
    // rien pour le moment
  }

  MAKE_ENUM(FilterState,
    UNCONNECTED,
    CONNECTING,
    PAUSED,
    STREAMING
  );
  void Filter::onStateChanged(pw_filter_state c_oldState, pw_filter_state c_state, std::string_view error)
  {

    const FilterState state = FilterState::FromNumeric(c_state);
    const FilterState oldState = FilterState::FromNumeric(c_oldState);

    Log::debug(u"Filter"_s) << "State changed from " << oldState << " to " << state;

    emit stateChanged(c_oldState, c_state, error);


    const u32 id = pw_filter_get_node_id(handle());

    if (id != SPA_ID_INVALID && m_nodeId == SPA_ID_INVALID)
    {
      m_nodeId = id;
      emit nodeCreated(id);
    }
  }



}


void PipeWire::Filter::onProcess(void *data, spa_io_position *position)
{
  if (!position)
    return;

  auto *that = static_cast<Filter *>(data);
  that->onProcess(*position);
}

void PipeWire::Filter::onStateChanged(void *data, pw_filter_state oldState, pw_filter_state state, const char *error)
{
  Filter *that = static_cast<Filter*>(data);
  return that->onStateChanged(oldState, state, error ? error : "");
}
