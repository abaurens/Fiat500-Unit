#pragma once

#include "audio/backend/pipewire/Object.hpp"

namespace Audio::PipeWire
{

  class Node : public Object
  {
  public:
    static constexpr Type StaticType = Type::Node;

    Node(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Node() = default;

    QString name() const;
    QString description() const;
    QString mediaClass() const;
    QString nick() const;
  };

}
