#include "pch.hpp" // IWYU pragma: keep

#include "Device.hpp"

namespace Audio::PipeWire
{

  Device::Device(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props, false }
  {}

  QString Device::api() const
  {
    return property("device.api");
  }

  QString Device::name() const
  {
    return property("device.name");
  }

  QString Device::description() const
  {
    return propertyOr("device.description", "");
  }
}
