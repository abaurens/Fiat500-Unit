#include "Log.hpp"
#include "Object.hpp"

#include <spa/utils/dict.h>

namespace Audio::PipeWire
{

  Object::Object(uint32_t id, Type type, const spa_dict *props) : m_type{ type }, m_id{ id }
  {
    if (!props)
      return;

    const spa_dict_item *item;

    Log::debug(u"Object"_s).noquote() << *this << "created with properties:";
    spa_dict_for_each(item, props)
    {
      qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
      m_properties.insert(item->key, item->value);
    }
  }

  Object::~Object()
  {
    Log::debug(u"Object"_s).noquote() << *this << "removed.";
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
