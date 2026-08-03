#pragma once

#include "Enum.hpp"
#include "audio/backend/pipewire/Log.hpp"

#include <frozen/map.h>
#include <frozen/string.h>

#include <QMap>
#include <QSet>
#include <QString>

struct spa_dict;

namespace Audio::PipeWire
{

  class Object
  {
    friend class Backend;

  protected:
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

    ~Object();

    uint32_t id() const { return m_id; }

    QString property(const QString& key) const;

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
