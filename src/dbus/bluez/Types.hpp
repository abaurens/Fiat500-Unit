#pragma once

#include "dbus/Types.hpp"

namespace DBus::Bluez
{
  inline constexpr Name ServiceName = "org.bluez";

  inline QString firstOr(const QStringList &list, const QString &def = "")
  { return list.isEmpty() ? def : list.constFirst(); }

  struct TrackInfo
  {
    QString  title    = "";
    QString  artist   = "";
    QString  album    = "";
    uint32_t duration = 0;

    TrackInfo() = default;

    TrackInfo(const PropertyMap &properties)
      : title  {         properties.value("Title",    {}).toString()      },
      artist   { firstOr(properties.value("Artist",   {}).toStringList()) },
      album    {         properties.value("Album",    {}).toString()      },
      duration {         properties.value("Duration", {}).toUInt()        }
    {
      // qDebug() << "Track Infos:" << properties;
    }
  };

  namespace Log
  {
    static auto info(const QStringView scope = u""_s)     { return ::DBus::Log::info(::Log::make_path(u"/BlueZ/"_s, scope));     }
    static auto debug(const QStringView scope = u""_s)    { return ::DBus::Log::debug(::Log::make_path(u"/BlueZ/"_s, scope));    }
    static auto fatal(const QStringView scope = u""_s)    { return ::DBus::Log::fatal(::Log::make_path(u"/BlueZ/"_s, scope));    }
    static auto warning(const QStringView scope = u""_s)  { return ::DBus::Log::warning(::Log::make_path(u"/BlueZ/"_s, scope));  }
    static auto critical(const QStringView scope = u""_s) { return ::DBus::Log::critical(::Log::make_path(u"/BlueZ/"_s, scope)); }
  }

}
