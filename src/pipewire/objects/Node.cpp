#include "pch.hpp" // IWYU pragma: keep

#include "Node.hpp"

#include <spa/utils/keys.h>
#include <spa/utils/dict.h>
#include <spa/param/format-utils.h>
#include <spa/param/audio/format-utils.h>

using namespace std::literals;

//#include "media/Manager.hpp"
//#include "pipewire/Device.hpp"

namespace PipeWire
{

  Node::Node(u32 id, pw_node *proxy, const spa_dict *props)
    : Object{ id, StaticType, props },
      Proxy{ proxy, pw_node_add_listener},
    m_mediaClass{ parseMediaClass(propertyOr(SPA_KEY_MEDIA_CLASS, "")) }
  {
    //if (m_mediaClass == MediaClass::Unknown)
    //  return;

    if (name().startsWith("BAA"))
      Log::debug(u"Node"_s) << *this << " created with properties: " << props;
  }

  //bool Node::isBluetooth() const
  //{
  //  return device()->api() == "bluez5";
  //}

  std::optional<u32> Node::deviceId() const
  {
    std::optional<QString> deviceID = tryProperty("device.id");

    if (!deviceID.has_value())
      return std::nullopt;

    return deviceID->toUInt();
  }

  const std::optional<AudioFormat> &Node::audioFormat() const
  {
    return m_audioFormat;
  }

  QString Node::name() const
  {
    return property("node.name");
  }

  QString Node::nick() const
  {
    return propertyOr("node.nick", "");
  }

  QString Node::description() const
  {
    return propertyOr("node.description", "");
  }

  MediaClass Node::mediaClass() const
  {
    return m_mediaClass;
  }

  //void Node::enumerateFormats()
  //{
  //  pw_node_enum_params(
  //    proxy(),
  //    0,
  //    SPA_PARAM_EnumFormat,
  //    0,
  //    UINT32_MAX,
  //    nullptr
  //  );
  //}

  void Node::onInputPortsInfo(u32 inputPorts)
  {
    //Log::debug(u"Node"_s) << *this << " number of input ports changed";
  }

  void Node::onOutputPortsInfo(u32 outputPorts)
  {
    //Log::debug(u"Node"_s) << *this << " number of output ports changed";
  }

  void Node::onPropertyInfos(spa_dict *props)
  {
    if (!props)
      return;

    updateProperties(props);

    //auto *device = Media::Manager::deviceForNode(id());
    //if (!device || device->api() != "bluez5")
    //  return;
    //
    //Log::debug(u"Node"_s) << *this << " [" << device->name() << "]<" << device->api() << "> received properties:\n" << props;
  }

  void Node::onStateInfo(pw_node_state state)
  {
    //Log::debug(u"Node"_s) << *this << " received output state update";
  }

  void Node::onParamInfos(spa_param_info *params, u32 count)
  {
    for (u32 i = 0; i < count; ++i)
    {
      const auto &param = params[i];

      if (param.id != SPA_PARAM_Format)
        continue;

      // Request an enumeration of the first SPA_PARAM_Format.
      // This will trigger a PipeWire event that we'll catch with Node::onParam(u32, u32, u32, const spa_pod &)
      if (updateParamInfo(param) && param.flags & SPA_PARAM_INFO_READ)
      {
        pw_node_enum_params(
          proxy(),
          0,
          SPA_PARAM_Format,
          0,
          1,
          nullptr
        );
      }
    }
  }

  void Node::onInfo(const pw_node_info &info)
  {
    if (info.change_mask & PW_NODE_CHANGE_MASK_INPUT_PORTS)
      onInputPortsInfo(info.n_input_ports);

    if (info.change_mask & PW_NODE_CHANGE_MASK_OUTPUT_PORTS)
      onOutputPortsInfo(info.n_output_ports);

    if (info.change_mask & PW_NODE_CHANGE_MASK_PROPS && info.props)
      onPropertyInfos(info.props);

    if (info.change_mask & PW_NODE_CHANGE_MASK_PARAMS && info.params && info.n_params > 0)
      onParamInfos(info.params, info.n_params);

    if (info.change_mask & PW_NODE_CHANGE_MASK_STATE)
      onStateInfo(info.state);
  }

  void Node::onAudioFormatParam(const spa_pod &param, u32 subtype)
  {
    // We don't support non-raw format at this stage of the stack.
    if (subtype != SPA_MEDIA_SUBTYPE_raw)
      return;

    spa_audio_info_raw audioInfo {};

    if (spa_format_audio_raw_parse(&param, &audioInfo) < 0)
      return;

    if (m_audioFormat && *m_audioFormat == audioInfo)
      return;

    m_audioFormat.emplace();
    m_audioFormat->format = audioInfo.format;
    m_audioFormat->rate = audioInfo.rate;
    m_audioFormat->channels = audioInfo.channels;

    const u32 count = std::min(audioInfo.channels, AudioFormat::MaxChannels);
    for (u32 i = 0; i < count; ++i)
    {
      m_audioFormat->positions[i] = static_cast<spa_audio_channel>(audioInfo.position[i]);
    }

    //Log::debug(u"Node"_s) << *this << "audio format changed:";
    //qDebug() << "  "
    //  << "format:" << audioInfo.format
    //  << "rate:" << audioInfo.rate
    //  << "channels:" << audioInfo.channels;
    //
    //for (u32 i = 0; i < count; ++i)
    //{
    //  qDebug() << "    "
    //    << "channel[" << i << "] "
    //    << "position: " << audioInfo.position[i];
    //}

  }

  void Node::onFormatParam(const spa_pod &param)
  {
    u32 mediaType;
    u32 mediaSubtype;

    if (spa_format_parse(&param, &mediaType, &mediaSubtype) < 0)
      return;

    switch (mediaType)
    {
    case SPA_MEDIA_TYPE_audio:
      onAudioFormatParam(param, mediaSubtype);
      break;

    case SPA_MEDIA_TYPE_video:
    case SPA_MEDIA_TYPE_image:
    case SPA_MEDIA_TYPE_binary:
    case SPA_MEDIA_TYPE_stream:
    case SPA_MEDIA_TYPE_application:
    default:
      Log::debug(u"Node"_s)
        << *this
        << " media type: " << mediaType
        << "subtype: " << mediaSubtype;
    }
  }

  void Node::onParam(u32 id, u32 index, u32 next, const spa_pod &param)
  {
    switch (id)
    {
      case SPA_PARAM_Format:
        onFormatParam(param);
        break;

      default:
        Log::debug(u"Node"_s) << *this << " received param: " << id << " index: " << index
          << " next: " << next << " pod type: " << param.type << " pod size: " << param.size;
        break;
    }
  }
}
