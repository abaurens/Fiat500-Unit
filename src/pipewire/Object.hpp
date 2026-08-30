#pragma once

#include "Enum.hpp"

#include "pipewire/ParamInfo.hpp"

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
      Port,
      Monitor,
      Link
    );

  public:
    Object(u32 id, Type type, const spa_dict *props = nullptr);

    virtual ~Object();

    u32 id() const { return m_id; }
    Type type() const { return m_type; }

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
      if (m_type != T::StaticType)
      {
        Log::warning(u"Object"_s) << *this << "is not of type" << T::StaticType.name();
        return nullptr;
      }

      return static_cast<T*>(this);
    }

    template<std::derived_from<Object> T>
    const T &as() const
    {
      Q_ASSERT(m_type == T::StaticType);
      return *safeAs<T>();
    }

    template<std::derived_from<Object> T>
    T &as()
    {
      Q_ASSERT(m_type == T::StaticType);
      return *safeAs<T>();
    }

    template<class OS>
    friend OS &&operator<<(OS &&os, const Object &obj)
    {
      std::stringstream ss;

      ss << obj.type().name() << " (" << obj.id() << ")";
      os << ss.str();
      return std::forward<OS>(os);
    }

  protected:
    virtual void onPropertyChanged(const QString &name, const QString &newValue, [[maybe_unused]] const QString &oldValue)
    {
      return onPropertyChanged(name, newValue);
    }

    virtual void onPropertyChanged(const QString &name, const QString &value)
    {
    // #ifndef NDEBUG
    //  Log::debug(u"Object"_s) << "Unhandled property" << name << "changed to" << value;
    // #endif
    }

    virtual void onPropertiesChanged([[maybe_unused]] const QSet<QString> &changed) {}

    bool updateParamInfo(const spa_param_info &info)
    {
      return m_paramInfos[info.id].update(info);
    }

    void updateProperties(const spa_dict *props);

  private:
    bool updateProperty(const QString &propertyName, const QString &newValue);

  private:
    Type m_type;
    u32 m_id;
    QHash<u32, ParamInfo> m_paramInfos;
    QMap<QString, QString> m_properties;
  };

}
