#include "pch.hpp" // IWYU pragma: keep

#include "Port.hpp"

#include <spa/utils/dict.h>

namespace PipeWire
{

  Port::Port(u32 id, const spa_dict *props)
    : Object{ id, StaticType, props }
  {
    //const spa_dict_item *item;
    //Log::debug(u"Port"_s).noquote() << *this << "created with properties:";
    //spa_dict_for_each(item, props)
    //{
    //  qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    //}
  }

  u32 Port::nodeId() const
  {
    return property("node.id").toUInt();
  }

  QString Port::name() const
  {
    return property("port.name");
  }

  QString Port::channel() const
  {
    return propertyOr("audio.channel", "");
  }

  Direction Port::direction() const
  {
    return Direction::FromName(property("port.direction"));
  }

}
