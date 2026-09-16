#include "pch.hpp"

#include "Link.hpp"

#include <spa/utils/dict.h>

// #include "pipewire/Manager.hpp"

namespace PipeWire
{

  Link::Link(u32 id, const spa_dict *props) : Object{ id, StaticType, props }
  {
    //Log::debug(u"Link"_s) << *this << "created:";
    //
    //const auto *const outputNode = Manager::node(outputNodeId());
    //const auto *const outputPort = outputNode ? Manager::port(outputPortId()) : nullptr;
    //
    //const auto *const inputNode = Manager::node(inputNodeId());
    //const auto *const inputPort = inputNode ? Manager::port(inputPortId()) : nullptr;
    //
    //Log::debug(u"Link"_s)
    //  << "  "
    //  << (outputNode ? outputNode->name() : "NULL")
    //  << ":"
    //  << (outputPort ? outputPort->name() : "NULL");
    //
    //Log::debug(u"Link"_s) << "    ->";
    //
    //Log::debug(u"Link"_s)
    //  << "  "
    //  << (inputNode ? inputNode->name() : "NULL")
    //  << ":"
    //  << (inputPort ? inputPort->name() : "NULL");

    //Log::debug(u"Link"_s) << *this << " created with properties: " << props;
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
