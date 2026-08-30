#pragma once

#include "pipewire/Object.hpp"
#include "pipewire/Direction.hpp"

namespace PipeWire
{

  class Port final : public Object
  {
  public:
    static constexpr Type StaticType = Type::Port;

    Port(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Port() override = default;

    u32 nodeId() const;
    QString name() const;
    QString channel() const;
    Direction direction() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;
  };

}

