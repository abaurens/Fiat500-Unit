#include "pch.hpp" // IWYU pragma: keep

#include "Client.hpp"

namespace Audio::PipeWire
{

  Client::Client(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, false }
  {}

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
