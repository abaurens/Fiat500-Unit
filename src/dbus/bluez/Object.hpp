#pragma once

#include "dbus/Types.hpp"

#include <QObject>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusPendingReply>

namespace DBus::Bluez
{
  inline constexpr Name Service = "org.bluez";

  class Object : public QObject
  {
    Q_OBJECT

  public:
    const QDBusObjectPath &path() const { return m_path; };

  protected:
    explicit Object(const QString &interfaceName,
                    const QDBusObjectPath &path,
                    const InterfaceMap &interfaces,
                    QObject *parent = nullptr);

    explicit Object(const QString &interfaceName,
                    const QDBusObjectPath &path,
                    const PropertyMap &properties,
                    QObject *parent = nullptr);

    virtual ~Object();

    template<typename T>
    T property(const QString &name) const
    {
      if (!m_properties.contains(name))
      {
        qWarning() << "Object" << m_interfaceName << " does not have a property " << name;
      }
      Q_ASSERT(m_properties.contains(name));
      return m_properties.value(name).template value<T>();
    }

    /// Uses callMethod_impl to call methods on the current interface.
    template<typename... Args>
    QDBusPendingReply<> callMethod(const QString &method, Args &&... args)
    {
      return callMethod_impl(
        method,
        m_interfaceName,
        std::forward<Args>(args)...
      );
    }

    /// Uses callMethod_impl to call the Set method on the "org.freedesktop.DBus.Properties" interface.
    template<typename T>
    QDBusPendingReply<> setProperty(const QString &propertyName, const T &value)
    {
      return callMethod_impl(
        "Set",
        PropertiesInterface,
        m_interfaceName,
        propertyName,
        QVariant::fromValue(QDBusVariant(QVariant::fromValue(value)))
      );
    }

    virtual void onPropertyChanged(const QString &name, const QVariant &newValue) {}

  signals:
    void propertyChanged(const QStringView name, const QVariant &value);
    void propertiesChanged();

  private slots:
    void onPropertiesChanged_dbus(const QString &interface, const QDBusMessage &changed);

  private:
    void subscribeToDBus();
    void unsubscribeFromDBus();
    void updateProperties(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

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
    QString         m_interfaceName;
    QDBusObjectPath m_path;
    PropertyMap     m_properties;
  };
}
