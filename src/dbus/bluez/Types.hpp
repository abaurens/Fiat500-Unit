#pragma once

#include "dbus/Types.hpp"

namespace DBus::Bluez
{
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
}

// #define TOKEN_AND_NAME(__token) __token, #__token
