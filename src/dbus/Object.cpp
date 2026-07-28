#include "dbus/Object.hpp"

#include <QDBusConnection>
#include <QDBusPendingCallWatcher>

namespace DBus
{
  Object::Object(const Name &serviceName, const QString &interfaceName, const Object::Path &path, const PropertyMap &properties, QObject *parent)
    : QObject{ parent }, m_serviceName{ serviceName }, m_interfaceName{ interfaceName }, m_path{ path }, m_properties{ properties }
  {
    subscribeToDBus();
    resyncPropertiesAsync();
  }

  Object::Object(const Name &serviceName, const QString &interfaceName, const Object::Path &path, const InterfaceMap &interfaces, QObject *parent)
    : Object{ serviceName, interfaceName, path, interfaces.value(interfaceName), parent }
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
    if (interface != m_interfaceName)
      return;

    bool changed = false;
    for (auto it = properties.cbegin(); it != properties.cend(); ++it)
    {
      const QString &propertyName = it.key();
      const QVariant &newValue = it.value();

      changed |= updateProperty(propertyName, normalizeVariant(newValue));
    }

    if (changed)
      emit propertiesChanged();
  }

  bool Object::updateProperty(const QString &propertyName, const QVariant &newValue)
  {
    const QVariant &oldValue = m_properties.value(propertyName, {});

    if (oldValue == newValue)
      return false;

    m_properties[propertyName] = newValue;
    onPropertyChanged(propertyName, newValue, oldValue);
    emit propertyChanged(propertyName, newValue);

    return true;
  }

  void Object::resyncProperties()
  {
    updateProperties(m_interfaceName, getAllProperties(), {});
  }

  QDBusPendingReply<PropertyMap> Object::resyncPropertiesAsync()
  {
    QDBusPendingReply<PropertyMap> call = getAllPropertiesAsync();

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    QObject::connect(
      watcher, &QDBusPendingCallWatcher::finished,
      [this](QDBusPendingCallWatcher *call)
      {
        QDBusPendingReply<PropertyMap> reply = *call;

        if (reply.isError())
        {
          Log::warning(u"Object::resyncPropertiesAsync()"_s) << reply.error().message();
          return;
        }

        updateProperties(m_interfaceName, reply.value(), {});
        call->deleteLater();
      }
    );

    return call;
  }

  void Object::onPropertiesChanged_dbus(const QString &interface, const QDBusMessage &message)
  {
    const auto &arguments = message.arguments();

    if (arguments.size() < 3)
      return;

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

      Log::warning(u"Object"_s) << m_interfaceName << method << name << message;

      emit methodCallFailed(method, name, message);
    }

    watcher->deleteLater();
  }

  void Object::subscribeToDBus()
  {
    const bool ok = QDBusConnection::systemBus().connect(
      m_serviceName, m_path.path(), Interface::Properties, Method::PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    // qDebug() << "Subscribed to" << Service << m_path.path() << Interface::Properties << Method::PropertiesChanged;

    Q_ASSERT(ok);
  }

  void Object::unsubscribeFromDBus()
  {
    const bool ok = QDBusConnection::systemBus().disconnect(
      m_serviceName, m_path.path(), Interface::Properties, Method::PropertiesChanged,
      this, SLOT(onPropertiesChanged_dbus(QString, QDBusMessage))
    );

    // qDebug() << "unsibscribed from" << Service << m_path.path() << Interface::Properties << Method::PropertiesChanged;

    Q_ASSERT(ok);
  }
}
