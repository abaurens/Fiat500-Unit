#include "Node.hpp"

namespace Audio::PipeWire
{

  Node::Node(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, false }
  {}

  QString Node::name() const
  {
    return property("node.name");
  }

  QString  Node::description() const
  {
    return propertyOr("node.description", "");
  }

  QString  Node::mediaClass() const
  {
    return propertyOr("media.class", "");
  }

  QString  Node::nick() const
  {
    return propertyOr("node.nick", "");
  }
}
