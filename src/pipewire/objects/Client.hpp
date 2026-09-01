#pragma once

#include "pipewire/objects/Object.hpp"

namespace PipeWire
{

  class Client final : public Object
  {
  public:
    static constexpr Type StaticType = Type::Client;

    Client(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Client() override = default;

    pid_t pid() const;
    uid_t uid() const;
    gid_t gid() const;
    QString applicationName() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;
  };

}

