#include "pch.hpp" // IWYU pragma: keep

#include "Node.hpp"

namespace Audio::PipeWire
{

  Node::Node(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, true },
    m_mediaClass{ parseMediaClass(propertyOr("media.class", "")) }
  {}

  //bool Node::isBluetooth() const
  //{
  //  return device()->api() == "bluez5";
  //}

  QString Node::name() const
  {
    return property("node.name");
  }

  QString Node::nick() const
  {
    return propertyOr("node.nick", "");
  }

  QString Node::description() const
  {
    return propertyOr("node.description", "");
  }

  Node::MediaClass Node::mediaClass() const
  {
    return m_mediaClass;
  }

}
