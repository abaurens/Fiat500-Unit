#pragma once

#include "audio/backend/pipewire/Object.hpp"

namespace Audio::PipeWire
{

  class Device : public Object
  {
  public:
    static constexpr Type StaticType = Type::Device;

    Device(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Device() = default;
  };

}

