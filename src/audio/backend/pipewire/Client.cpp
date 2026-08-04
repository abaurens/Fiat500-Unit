#include "Client.hpp"

namespace Audio::PipeWire
{

  Client::Client(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, false }
  {}

}
