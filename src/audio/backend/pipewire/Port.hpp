#pragma once

#include "Enum.hpp"
#include "audio/backend/pipewire/Object.hpp"

namespace Audio::PipeWire
{

  class Port : public Object
  {
  public:
    MAKE_ENUM_CI(Direction, In, Out);

    static constexpr Type StaticType = Type::Port;

    Port(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Port() = default;

    QString name() const;
    QString channel() const;
    uint32_t nodeId() const;
    Direction direction() const;
  };

}

