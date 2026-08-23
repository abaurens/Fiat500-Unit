#pragma once

#include "pipewire/Object.hpp"

namespace PipeWire
{

  class Device final : public Object
  {
  public:
    static constexpr Type StaticType = Type::Device;

    Device(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Device() override = default;

    QString api() const;
    QString name() const;
    QString description() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;
  };

}

