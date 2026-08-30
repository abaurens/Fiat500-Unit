#pragma once

#include <spa/utils/hook.h>
#include <pipewire/proxy.h>

#define Bindable(_type, _TYPE) BindableImpl<pw_##_type, pw_##_type##_events, pw_##_type##_info, PW_VERSION_##_TYPE##_EVENTS>

namespace PipeWire
{

  template<class Proxy, class Events, class Infos, u32 Version>
  class BindableImpl
  {
    using AddListenerFunc = int(Proxy*, struct spa_hook*, const Events*, void*);

  public:
    BindableImpl(Proxy *proxy, AddListenerFunc addListener) : m_proxy{ proxy }
    {
      static constexpr Events events =
      {
        .version = Version,
        .info = &BindableImpl::onInfo,
        .param = &BindableImpl::onParam,
      };

      if (proxy)
      {
        addListener(
          m_proxy,
          &m_listener,
          &events,
          this
        );
      }
    }

    virtual ~BindableImpl()
    {
      if (!m_proxy)
        return;

      spa_hook_remove(&m_listener);
      pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_proxy));
    }

  protected:
    Proxy *proxy() { return m_proxy; }
    const Proxy *proxy() const { return m_proxy; }

  private:
    static void onInfo(void *data, const Infos *info)
    {
      if (!info)
        return;

      BindableImpl *that = static_cast<BindableImpl *>(data);
      return that->onInfo(*info);
    }

    static void onParam(void *data, int seq, u32 id, u32 index, u32 next, const spa_pod *param)
    {
      if (!param)
        return;

      BindableImpl *that = static_cast<BindableImpl *>(data);
      return that->onParam(id, index, next, *param);
    }

    virtual void onInfo(const Infos &info) {};
    virtual void onParam(u32 id, u32 index, u32 next, const spa_pod &param) {};

  private:
    Proxy *m_proxy = nullptr;
    spa_hook m_listener {};
  };

}
