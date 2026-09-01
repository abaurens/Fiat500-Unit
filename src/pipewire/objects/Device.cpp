#include "pch.hpp" // IWYU pragma: keep

#include "Device.hpp"

#include <spa/utils/keys.h>
#include <spa/utils/dict.h>

namespace PipeWire
{

  Device::Device(uint32_t id, pw_device *proxy, const spa_dict *props)
  : Object{ id, StaticType, props },
    Proxy { proxy, pw_device_add_listener },
    m_mediaClass{ parseMediaClass(propertyOr(SPA_KEY_MEDIA_CLASS, "")) }
  {
    //Log::debug(u"Device"_s) << *this << " created with properties: " << props;
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

  MediaClass Device::mediaClass() const
  {
    return m_mediaClass;
  }

  std::optional<QString> Device::bluetoothAddress() const
  {
    if (api() != "bluez5")
      return std::nullopt;

    return tryProperty(SPA_KEY_API_BLUEZ5_ADDRESS);
  }

  void Device::onProps(spa_dict *props)
  {
    if (!props)
      return;

    updateProperties(props);

    //if (api() != "bluez5")
    //  return;
    //
    //Log::debug(u"Device"_s) << *this << " <" << api() << "> received properties:\n" << props;
  }

  void Device::onParamInfos(spa_param_info *params)
  {
    //Log::debug(u"Node"_s) << *this << " received output param update";
  }

  void Device::onInfo(const pw_device_info &info)
  {
    if (info.change_mask & PW_DEVICE_CHANGE_MASK_PROPS && info.props)
      onProps(info.props);

    if (info.change_mask & PW_DEVICE_CHANGE_MASK_PARAMS && info.params)
      onParamInfos(info.params);
  }
}
