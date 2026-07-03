#include "dbus/bluez/Object.hpp"

#include <QDBusConnection>

namespace DBus::Bluez
{
  Object::Object(const QString &interfaceName, const QDBusObjectPath &path, const PropertyMap &properties, QObject *parent)
    : QObject{ parent }, m_interfaceName{ interfaceName }, m_path{ path }, m_properties{ properties }
  {
    subscribeToDBus();
  }

  Object::Object(const QString &interfaceName, const QDBusObjectPath &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ interfaceName, path, interfaces.value(interfaceName), parent }
  {}


  Object::~Object()
  {
    unsubscribeFromDBus();
  }

  void Object::updateProperties(const QString &interface, const PropertyMap &properties, const QStringList &invalidated)
  {
    // qDebug() << "Interface:  " << interface;
    // qDebug() << "Invalidated:" << invalidated;
    // qDebug() << "Properties: ";

    for (auto it = properties.cbegin(); it != properties.cend(); ++it)
    {
      const QString &propertyName = it.key();
      const QVariant oldValue = m_properties.value(propertyName);
      const QVariant &newValue = it.value();

      // qDebug() << "  [" << propertyName << "] = " << newValue << " (was" << oldValue << ")";

      if (oldValue != newValue)
      {
        m_properties[propertyName] = newValue;
        onPropertyChanged(propertyName, newValue);
        emit propertyChanged(propertyName, newValue);
      }
    }

    emit propertiesChanged();
  }

  void Object::onPropertiesChanged_dbus(const QString &interface, const QDBusMessage &message)
  {
    const auto &arguments = message.arguments();

    const QStringList invalidated = arguments.at(2).toStringList();
    const auto dbusArg = qvariant_cast<QDBusArgument>(arguments.at(1));

    PropertyMap changed;
    dbusArg >> changed;

    //qDebug() << "interface:    " << interface;
    //qDebug() << "interfaceName:" << m_interfaceName;
    //Q_ASSERT(interface == m_interfaceName);

    return updateProperties(interface, changed, invalidated);
  }

  void Object::subscribeToDBus()
  {
    const bool ok = QDBusConnection::systemBus().connect(
      Service, m_path.path(), PropertiesInterface, PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    qDebug() << "Subscribed to" << Service << m_path.path() << PropertiesInterface << PropertiesChanged;

    Q_ASSERT(ok);
  }

  void Object::unsubscribeFromDBus()
  {
    const bool ok = QDBusConnection::systemBus().disconnect(
      Service, m_path.path(), PropertiesInterface, PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    qDebug() << "unsibscribed from" << Service << m_path.path() << PropertiesInterface << PropertiesChanged;

    Q_ASSERT(ok);
  }
}
