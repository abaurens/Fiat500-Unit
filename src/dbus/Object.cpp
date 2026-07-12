#include "dbus/Object.hpp"

#include <QDBusConnection>

namespace DBus
{
  Object::Object(const QString &interfaceName, const Object::Path &path, const PropertyMap &properties, QObject *parent)
    : QObject{ parent }, m_interfaceName{ interfaceName }, m_path{ path }, m_properties{ properties }
  {
    subscribeToDBus();
  }

  Object::Object(const QString &interfaceName, const Object::Path &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ interfaceName, path, interfaces.value(interfaceName), parent }
  {}


  Object::~Object()
  {
    unsubscribeFromDBus();
  }

  QVariant Object::normalizeVariant(const QVariant &value)
  {
    if (value.userType() == qMetaTypeId<QDBusArgument>())
    {
      PropertyMap map;

      const QDBusArgument &arg = qvariant_cast<QDBusArgument>(value);
      arg >> map;

      return { map };
    }

    return value;
  }

  void Object::updateProperties(const QString &interface, const PropertyMap &properties, const QStringList &invalidated)
  {
    for (auto it = properties.cbegin(); it != properties.cend(); ++it)
    {
      const QString &propertyName = it.key();
      const QVariant &newValue = it.value();

      updateProperty(propertyName, normalizeVariant(newValue));
    }

    emit propertiesChanged();
  }

  void Object::updateProperty(const QString &propertyName, const QVariant &newValue)
  {
    const QVariant &oldValue = m_properties.value(propertyName);

    if (oldValue == newValue)
      return;

    m_properties[propertyName] = newValue;
    onPropertyChanged(propertyName, newValue, oldValue);
    emit propertyChanged(propertyName, newValue);
  }

  void Object::onPropertiesChanged_dbus(const QString &interface, const QDBusMessage &message)
  {
    const auto &arguments = message.arguments();

    const QStringList invalidated = arguments.at(2).toStringList();
    const auto dbusArg = qvariant_cast<QDBusArgument>(arguments.at(1));

    PropertyMap changed;
    dbusArg >> changed;

    return updateProperties(interface, changed, invalidated);
  }

  void Object::onMethodCallFinished(QDBusPendingCallWatcher *watcher)
  {
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
    {
      const QString method = watcher->property("method").toString();
      const QString name = reply.error().name();
      const QString message = reply.error().message();

      qWarning() << m_interfaceName << method << name << message;

      emit methodCallFailed(method, name, message);
    }

    watcher->deleteLater();
  }

  void Object::subscribeToDBus()
  {
    const bool ok = QDBusConnection::systemBus().connect(
      Service, m_path.path(), Interface::Properties, Method::PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    // qDebug() << "Subscribed to" << Service << m_path.path() << Interface::Properties << Method::PropertiesChanged;

    Q_ASSERT(ok);
  }

  void Object::unsubscribeFromDBus()
  {
    const bool ok = QDBusConnection::systemBus().disconnect(
      Service, m_path.path(), Interface::Properties, Method::PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    // qDebug() << "unsibscribed from" << Service << m_path.path() << Interface::Properties << Method::PropertiesChanged;

    Q_ASSERT(ok);
  }
}
