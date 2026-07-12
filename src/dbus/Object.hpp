#pragma once

#include "dbus/Types.hpp"

#include <QObject>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

#include <optional>

namespace DBus
{
  inline constexpr Name Service = "org.bluez";

  class Object : public QObject
  {
    Q_OBJECT

  public:
    using Path = QDBusObjectPath;

    const Path &path() const { return m_path; };

  protected:
    explicit Object(const QString &interfaceName, const Path &path, const InterfaceMap &interfaces, QObject *parent = nullptr);
    explicit Object(const QString &interfaceName, const Path &path, const PropertyMap &properties, QObject *parent = nullptr);

    virtual ~Object();

    template<typename T>
    std::optional<T> tryProperty(const QString &name) const
    {
      auto it = m_properties.find(name);

      if (it == m_properties.cend())
        return std::nullopt;

      return normalizeVariant(*it).template value<T>();
    }

    template<typename T>
    T property(const QString &name) const
    {
      const std::optional<T> &result = tryProperty<T>(name);

      if (!result.has_value())
        qWarning() << "Object" << m_interfaceName << " does not have a property " << name;
      Q_ASSERT(result.has_value());

      return result.value();
    }

    template<typename T>
    T propertyOr(const QString &name, T && defaultValue) const
    {
      return tryProperty<T>(name).value_or(std::forward<T>(defaultValue));
    }

    template<typename T>
    std::optional<T> refreshProperty(const QString &name)
    {
      QVariant value = getProperty(name);

      PropertyMap map;
      map.insert(name, value);

      updateProperties(m_interfaceName, map, {});
      return tryProperty<T>(name);
    }

    /// Uses callMethod_impl to call methods on the current interface.
    template<typename... Args>
    void callMethod(const QString &method, Args &&... args)
    {
      return callMethod_protected(
        method,
        m_interfaceName,
        std::forward<Args>(args)...
      );
    }

    /// Uses callMethod_impl to call the Set method on the "org.freedesktop.DBus.Properties" interface.
    template<typename T>
    void setProperty(const QString &propertyName, const T &value)
    {
      return callMethod_protected(
        "Set", Interface::Properties,
        m_interfaceName, propertyName,
        QVariant::fromValue(QDBusVariant(QVariant::fromValue(value)))
      );
    }

    /// Uses callMethod_impl to call the Get method on the "org.freedesktop.DBus.Properties" interface.
    QVariant getProperty(const QString &propertyName)
    {
      QDBusPendingReply<QDBusVariant> reply = callMethod_impl(
        "Get", Interface::Properties,
        m_interfaceName, propertyName
      );

      reply.waitForFinished();

      if (reply.isError())
      {
        qWarning() << reply.error().message();
        return {};
      }

      const QVariant &property = normalizeVariant(reply.value().variant());

      if (property != m_properties.value(propertyName, {}))
        m_properties[propertyName] = property;
      return property;
    }

    virtual void onPropertyChanged(const QString &name, const QVariant &newValue, [[maybe_unused]] const QVariant &oldValue)
    {
      return onPropertyChanged(name, newValue);
    }

    virtual void onPropertyChanged(const QString &name, const QVariant &value)
    {
     #ifndef NDEBUG
      // qDebug() << m_path.path() << ": Unhandled property " << name << "changed to" << value;
     #endif
    }

  signals:
    void propertiesChanged();
    void propertyChanged(const QStringView name, const QVariant &value);
    void methodCallFailed(const QString &method, const QString &errorName, const QString &message);

  private slots:
    void onPropertiesChanged_dbus(const QString &interface, const QDBusMessage &changed);
    void onMethodCallFinished(QDBusPendingCallWatcher *watcher);

  private:
    static QVariant normalizeVariant(const QVariant &value);

    void subscribeToDBus();
    void unsubscribeFromDBus();
    void updateProperty(const QString &propertyName, const QVariant &newValue);
    void updateProperties(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

    template<typename... Args>
    void callMethod_protected(const QString &method, const QString &interface, Args &&... args)
    {
      QDBusPendingReply<> reply = callMethod_impl(method, interface, std::forward<Args>(args)...);

      QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

      watcher->setProperty("method", method);

      connect(
        watcher, &QDBusPendingCallWatcher::finished,
        this,    &Object::onMethodCallFinished
      );
    }

    /// Generic helper that calls a method on arbitrary D-Bus interface.
    template<typename... Args>
    QDBusPendingReply<> callMethod_impl(const QString &method, const QString &interface, Args &&... args)
    {
      QDBusMessage message = QDBusMessage::createMethodCall(
        Service,
        m_path.path(),
        interface,
        method
      );

      if constexpr (sizeof...(Args) > 0)
        (message << ... << std::forward<Args>(args));

      return QDBusConnection::systemBus().asyncCall(message);
    }

  protected:
    QString     m_interfaceName;
    Path        m_path;
    PropertyMap m_properties;
  };
}
