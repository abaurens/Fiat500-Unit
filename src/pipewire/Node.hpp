#pragma once

#include "pipewire/Object.hpp"

#include <QStringView>

namespace PipeWire
{

  class Node : public Object
  {
  public:
    MAKE_ENUM(MediaClass,
      AudioSink,
      AudioSource,

      VideoSink,
      VideoSource,

      MidiBridge
    );

    static Scope<Node> Create(uint32_t id,  const spa_dict *props = nullptr);

  private:
    Node(uint32_t id, MediaClass mediaClass, std::nullptr_t, const spa_dict *props = nullptr);

  protected:
    Node(uint32_t id, MediaClass mediaClass, const spa_dict *props = nullptr);

  public:
    static constexpr Type StaticType = Type::Node;

    virtual ~Node() override = default;

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
      // Find the separator
      const qsizetype slash = str.indexOf('/');

      // Fail to parse if more than 1 separator
      if (slash != str.lastIndexOf('/'))
        return MediaClass::Unknown;

      // Remove the separator
      QString name{ str };
      name.removeAt(slash);
      //name.removeIf([](auto ch) { return ch == '/'; });

      return MediaClass::FromName(name);
    }

    static constexpr MediaClass parseMediaClass(const std::string_view str)
    {
      // Find the separator
      const size_t slash = str.find_first_of('/');

      // Fail to parse if more than 1 separator
      if (slash != str.find_last_of('/'))
        return MediaClass::Unknown;

      // Remove the separator
      std::string name{ str };
      name.erase(name.begin() + slash);

      return MediaClass::FromName(name);
    }

  private:
    MediaClass m_mediaClass;
  };

}
