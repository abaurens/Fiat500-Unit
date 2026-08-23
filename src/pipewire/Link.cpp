#include "pch.hpp"

#include "Link.hpp"

#include <spa/utils/dict.h>

namespace PipeWire
{

  Link::Link(u32 id, const spa_dict *props) : Object{ id, StaticType, props }
  {
    const spa_dict_item *item;

    Log::debug(u"Link"_s).noquote() << *this << "created with properties:";
    spa_dict_for_each(item, props)
    {
      qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    }
  }

  u32 Link::outputNodeId() const
  {
    return property("link.output.node").toUInt();
  }

  u32 Link::outputPortId() const
  {
    return property("link.output.port").toUInt();
  }

  u32 Link::inputNodeId() const
  {
    return property("link.input.node").toUInt();
  }

  u32 Link::inputPortId() const
  {
    return property("link.input.port").toUInt();
  }
}
