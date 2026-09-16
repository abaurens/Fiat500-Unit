#pragma once

#include <pipewire/keys.h>
#include <pipewire/filter.h>

#include "AsyncTask.hpp"

#include "pipewire/AudioFormat.hpp"
#include "pipewire/objects/Node.hpp"
#include "pipewire/handles/Handle.hpp"

#include <QObject>

namespace PipeWire
{
  class Port;

  class Filter : public QObject, Handle<pw_filter, pw_filter_destroy>
  {
    using Super = Handle<pw_filter, pw_filter_destroy>;

    Q_OBJECT

    using Port = std::pair<void *, PipeWire::Port *>;

    struct ConnectionSpec
    {
      Node *node = nullptr;
      u32  maxChannels = std::numeric_limits<u32>::max();
    };

  public:
    using Super::HandleType;

  public:
    explicit Filter(pw_filter *filter);
    ~Filter();

    u32 nodeId() const { return m_nodeId; }

    AsyncTask<bool> connect();

    void *addPort(spa_direction direction, pw_properties *properties = nullptr, size_t userDataSize = 0);

    AsyncTask<u32> waitForNodeId();

    void connectInput(Node &node, u32 maxChannels = std::numeric_limits<u32>::max());
    void connectOutput(Node &node, u32 maxChannels = std::numeric_limits<u32>::max());

    void connectInput(Node *node, u32 maxChannels = std::numeric_limits<u32>::max());
    void connectOutput(Node *node, u32 maxChannels = std::numeric_limits<u32>::max());

  signals:
    void nodeCreated(u32 id);
    void stateChanged(pw_filter_state oldState, pw_filter_state state, std::string_view error);

  private:
    static void onProcess(void *data, spa_io_position *position);
    static void onStateChanged(void *data, pw_filter_state oldState, pw_filter_state state, const char *error);

    void onProcess(spa_io_position &position);
    void onStateChanged(pw_filter_state oldState, pw_filter_state state, std::string_view error);

    template<spa_direction Direction>
    AsyncTask<> connectIO()
    {
      constexpr std::string_view namePrefix = (Direction == SPA_DIRECTION_INPUT ? "input_" : "output_");
      const Local<ConnectionSpec> &conectionSpec = (Direction == SPA_DIRECTION_INPUT ? m_inputSpec : m_outputSpec);

      if (!conectionSpec || !conectionSpec->node)
        co_return;

      const AudioFormat format = co_await conectionSpec->node->waitForAudioFormat();

      u32 count = std::min(format.channels, conectionSpec->maxChannels);

      for (u32 i = 0; i < count; ++i)
      {
        const auto &channel = format.positions[i];

        // const auto nameView = std::views::join(std::array{namePrefix, audioChannelName(channel)});
        const std::string name = std::string{ namePrefix } += audioChannelName(channel);

        addPort(
          Direction,
          pw_properties_new(
            PW_KEY_PORT_NAME,
            name.c_str(),
            nullptr
          )
        );
      }
    }


  private:
    u32 m_nodeId = SPA_ID_INVALID;
    spa_hook m_listener {};

    Local<ConnectionSpec> m_inputSpec;
    Local<ConnectionSpec> m_outputSpec;

    std::vector<Port> m_inputs;
    std::vector<Port> m_outputs;
  };

}
