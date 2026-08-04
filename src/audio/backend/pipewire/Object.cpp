#include "Log.hpp"
#include "Object.hpp"

#include <spa/utils/dict.h>

namespace Audio::PipeWire
{

  Object::Object(uint32_t id, Type type, const spa_dict *props, const bool log) : m_type{ type }, m_id{ id }
  {
    if (!props)
      return;

    const spa_dict_item *item;

    if (log)
      Log::debug(u"Object"_s).noquote() << *this << "created with properties:";
    spa_dict_for_each(item, props)
    {
      if (log)
        qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
      m_properties.insert(item->key, item->value);
    }
  }

  Object::~Object()
  {
    // Log::debug(u"Object"_s).noquote() << *this << "removed.";
  }

  std::optional<QString> Object::tryProperty(const QString &name) const
  {
    auto it = m_properties.find(name);

    if (it == m_properties.cend())
      return std::nullopt;

    return *it;
  }

  QString Object::property(const QString &name) const
  {
    const std::optional<QString> &result = tryProperty(name);

    if (!result.has_value())
      Log::warning(u"Object"_s) << m_id << "does not have a property named" << name;
    Q_ASSERT(result.has_value());

    if (!result.has_value())
      return {};

    return result.value();
  }

  QString Object::propertyOr(const QString &name, const QString &defaultValue) const
  {
    return tryProperty(name).value_or(defaultValue);
  }

  QString Object::propertyOr(const QString &name, QString &&defaultValue) const
  {
    return tryProperty(name).value_or(std::forward<QString>(defaultValue));
  }

  bool Object::updateProperty(const QString &name, const QString &newValue)
  {
    const auto it = m_properties.find(name);

    QString oldValue;

    if (it != m_properties.cend())
    {
      oldValue = it.value();
      if (newValue == oldValue)
        return false;
    }

    m_properties.insert(name, newValue);
    onPropertyChanged(name, newValue, oldValue);
    return true;
  }

  void Object::updateProperties(const spa_dict *props)
  {
    if (!props)
      return;

    const spa_dict_item *item;

    QSet<QString> changed;

    spa_dict_for_each(item, props)
    {
      const QString name = item->key;
      const QString value = item->value;

      if (updateProperty(item->key, item->value))
        changed.insert(name);
    }

    if (!changed.empty())
      onPropertiesChanged(changed);
  }

}
