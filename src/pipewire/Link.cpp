#include "pch.hpp"

#include "Link.hpp"

#include <spa/utils/dict.h>

// #include "media/Manager.hpp"

namespace PipeWire
{

  Link::Link(u32 id, const spa_dict *props) : Object{ id, StaticType, props }
  {
    //Log::debug(u"Link"_s).noquote() << *this << "created:";
    //
    //const auto *const outputNode = Media::Manager::node(outputNodeId());
    //const auto *const outputPort = outputNode ? Media::Manager::port(outputPortId()) : nullptr;
    //
    //const auto *const inputNode = Media::Manager::node(inputNodeId());
    //const auto *const inputPort = inputNode ? Media::Manager::port(inputPortId()) : nullptr;
    //
    //Log::debug(u"Link"_s).nospace().noquote()
    //  << "  "
    //  << (outputNode ? outputNode->name() : "NULL")
    //  << ":"
    //  << (outputPort ? outputPort->name() : "NULL");
    //
    //Log::debug(u"Link"_s) << "    ->";
    //
    //Log::debug(u"Link"_s).nospace().noquote()
    //  << "  "
    //  << (inputNode ? inputNode->name() : "NULL")
    //  << ":"
    //  << (inputPort ? inputPort->name() : "NULL");

    //const spa_dict_item *item;
    //Log::debug(u"Link"_s).noquote() << *this << "created with properties:";
    //spa_dict_for_each(item, props)
    //{
    //  qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    //}
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
