#include "Device.hpp"

namespace Audio::PipeWire
{

  Device::Device(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, true }
  {}

}
