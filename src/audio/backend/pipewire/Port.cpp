#include "Port.hpp"

namespace Audio::PipeWire
{

  Port::Port(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, false }
  {}

  QString Port::name() const
  {
    return property("port.name");
  }

  QString Port::channel() const
  {
    return propertyOr("audio.channel", "");
  }

  uint32_t Port::nodeId() const
  {
    return property("node.id").toInt();
  }

  Port::Direction Port::direction() const
  {
    return Direction::FromName(property("port.direction"));
  }

}
