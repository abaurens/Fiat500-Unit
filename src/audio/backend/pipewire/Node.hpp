#pragma once

#include "audio/backend/pipewire/Object.hpp"

#include <QStringView>

namespace Audio::PipeWire
{

  class Node final : public Object
  {
  public:
    MAKE_ENUM(MediaClass,
      AudioSink,
      AudioSource,

      VideoSink,
      VideoSource,

      MidiBridge
    );

  public:
    static constexpr Type StaticType = Type::Node;

    Node(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Node() = default;

    QString name() const;
    QString nick() const;
    QString description() const;
    MediaClass mediaClass() const;

    template<std::derived_from<Object> T> T &as() = delete;
    template<std::derived_from<Object> T> T *safeAs() = delete;
    template<std::derived_from<Object> T> const T &as() const = delete;
    template<std::derived_from<Object> T> const T *safeAs() const = delete;

  private:
  public:
    static constexpr MediaClass parseMediaClass(const QStringView str)
    {
      const qsizetype slash = str.indexOf('/');

      if (slash != str.lastIndexOf('/'))
        return MediaClass::Unknown;

      QString name{ str };
      name.removeIf([](auto ch) { return ch == '/'; });

      return MediaClass::FromName(name);
    }

  private:
    MediaClass m_mediaClass;
  };

}
