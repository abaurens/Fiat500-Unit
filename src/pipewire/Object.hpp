#pragma once

#include "Enum.hpp"

#include <frozen/map.h>
#include <frozen/string.h>

#include <QMap>
#include <QSet>
#include <QString>

struct spa_dict;

namespace PipeWire
{

  class Object
  {
    friend class Backend;

  public:
    MAKE_ENUM(Type,
      Core,
      Module,
      SecurityContext,
      Profiler,
      Factory,
      Node,
      Metadata,
      Client,
      Device,
      Port
    );

  public:
    Object(uint32_t id, Type type, const spa_dict *props = nullptr);

    virtual ~Object();

    uint32_t id() const { return m_id; }

    std::optional<QString> tryProperty(const QString &name) const;
    QString property(const QString& key) const;
    QString propertyOr(const QString &name, const QString &defaultValue) const;
    QString propertyOr(const QString &name, QString &&defaultValue) const;

    template<std::derived_from<Object> T>
    const T *safeAs() const
    {
      if (m_type != T::StaticType)
      {
        Log::warning(u"Object"_s) << *this << "is not of type" << T::StaticType.name();
        return nullptr;
      }

      return static_cast<const T*>(this);
    }

    template<std::derived_from<Object> T>
    T *safeAs()
    {
      return const_cast<T *>(const_cast<const Object *>(this)->safeAs<T>());
    }

    template<std::derived_from<Object> T>
    const T &as() const
    {
      const T *&&result = this->safeAs<T>();

      Q_ASSERT(result != nullptr);
      return result;
    }

    template<std::derived_from<Object> T>
    T &as()
    {
      return const_cast<T &>(const_cast<const Object *>(this)->as<T>());
    }

    template<class OS>
    friend OS &operator<<(OS &&os, const Object &obj)
    {
      std::stringstream ss;

      ss << obj.m_type.name() << " (" << obj.m_id << ")";
      os << ss.str();
      return os;
    }

  protected:
    virtual void onPropertyChanged(const QString &name, const QString &newValue, [[maybe_unused]] const QString &oldValue)
    {
      return onPropertyChanged(name, newValue);
    }

    virtual void onPropertyChanged(const QString &name, const QString &value)
    {
     #ifndef NDEBUG
      Log::debug(u"Object"_s) << "Unhandled property" << name << "changed to" << value;
     #endif
    }

    virtual void onPropertiesChanged([[maybe_unused]] const QSet<QString> &changed) {}

  private:
    bool updateProperty(const QString &propertyName, const QString &newValue);
    void updateProperties(const spa_dict *props);

  private:
    Type m_type;
    uint32_t m_id;
    QMap<QString, QString> m_properties;
  };

}
