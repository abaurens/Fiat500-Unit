#pragma once

#include "pipewire/Object.hpp"

namespace PipeWire
{

  class Link final : public Object
  {
  public:

    static constexpr Type StaticType = Type::Link;

    Link(u32 id, const spa_dict *props = nullptr);

    virtual ~Link() override = default;

    u32 outputNodeId() const;
    u32 outputPortId() const;

    u32 inputNodeId() const;
    u32 inputPortId() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;
  };

}
