#include "pch.hpp" // IWYU pragma: keep

#include "Device.hpp"

#include <spa/utils/dict.h>

namespace PipeWire
{

  Device::Device(uint32_t id, const spa_dict *props) : Object{ id, StaticType, props }
  {
    const spa_dict_item *item;

    Log::debug(u"Device"_s).noquote() << *this << "created with properties:";
    spa_dict_for_each(item, props)
    {
      qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    }
  }

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
