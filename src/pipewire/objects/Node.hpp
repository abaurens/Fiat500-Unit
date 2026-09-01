#pragma once

#include "pipewire/Bindable.hpp"
#include "pipewire/MediaClass.hpp"
#include "pipewire/AudioFormat.hpp"
#include "pipewire/objects/Object.hpp"

#include <pipewire/node.h>

#include <QStringView>

namespace PipeWire
{

  class Node : public Object, Bindable(node, NODE)
  {
    using Proxy = Bindable(node, NODE);

  public:
    static constexpr Type StaticType = Type::Node;

    Node(u32 id, pw_node *proxy, const spa_dict *props = nullptr);
    virtual ~Node() override = default;

    std::optional<u32> deviceId() const;
    const std::optional<AudioFormat> &audioFormat() const;

    QString name() const;
    QString nick() const;
    QString description() const;
    MediaClass mediaClass() const;

    //void enumerateFormats();

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;

    template<class OS>
    friend OS &&operator<<(OS &&os, const Node &obj)
    {
      std::stringstream ss;

      QString name = obj.nick().isEmpty() ? obj.name() : obj.nick();

      ss << name.toStdString() << " (" << obj.id() << ")";
      os << ss.str();
      return std::forward<OS>(os);
    }

  private:
    void onStateInfo(pw_node_state state);
    void onPropertyInfos(spa_dict *props);
    void onParamInfos(spa_param_info *params, u32 count);
    void onInputPortsInfo(u32 inputPorts);
    void onOutputPortsInfo(u32 outputPorts);

    void onAudioFormatParam(const spa_pod &param, u32 subtype);

    void onFormatParam(const spa_pod &param);

    virtual void onInfo(const pw_node_info &info) override;
    virtual void onParam(u32 id, u32 index, u32 next, const spa_pod &param) override;

  private:
    MediaClass m_mediaClass;
    Local<AudioFormat> m_audioFormat;
  };

}
