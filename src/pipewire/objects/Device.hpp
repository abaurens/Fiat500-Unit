#pragma once

#include <pipewire/device.h>

#include "pipewire/Bindable.hpp"
#include "pipewire/MediaClass.hpp"
#include "pipewire/objects/Object.hpp"

namespace PipeWire
{

  class Device final : public Object, Bindable(device, DEVICE)
  {
    using Proxy = Bindable(device, DEVICE);

  public:
    static constexpr Type StaticType = Type::Device;

    Device(uint32_t id, pw_device *proxy, const spa_dict *props = nullptr);

    virtual ~Device() override = default;

    QString api() const;
    QString name() const;
    QString description() const;
    MediaClass mediaClass() const;

    std::optional<QString> bluetoothAddress() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;

    void onProps(spa_dict *props);
    void onParamInfos(spa_param_info *params);
    virtual void onInfo(const pw_device_info &info) override;

  private:
    MediaClass m_mediaClass;
  };

}

