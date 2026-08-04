#pragma once

#include "audio/backend/pipewire/Object.hpp"

namespace Audio::PipeWire
{

  class Client : public Object
  {
  public:
    static constexpr Type StaticType = Type::Client;

    Client(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Client() = default;
  };

}

