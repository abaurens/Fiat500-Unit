#include "pch.hpp" // IWYU pragma: keep

#include "Client.hpp"

#include <spa/utils/dict.h>

namespace PipeWire
{

  Client::Client(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props }
  {
    //const spa_dict_item *item;
    //Log::debug(u"Client"_s).noquote() << *this << "created with properties:";
    //spa_dict_for_each(item, props)
    //{
    //  qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    //}
  }

  pid_t Client::pid() const
  {
    return property("pipewire.sec.pid").toInt();
  }

  uid_t Client::uid() const
  {
    return property("pipewire.sec.uid").toInt();
  }

  gid_t Client::gid() const
  {
    return property("pipewire.sec.gid").toInt();
  }

  QString Client::applicationName() const
  {
    return property("application.name");
  }
}
