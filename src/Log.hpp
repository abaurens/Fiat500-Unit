#pragma once

#include <qDebug>
#include <QString>
#include <QStringLiteral>

using namespace Qt::StringLiterals;

namespace Log
{
  static inline QString make_path(QStringView left, QStringView right, const char separator = '/')
  {
    QString result;

    result.reserve(left.size() + right.size() + 2);

    if (!left.startsWith(separator))
      result.append(separator);

    left.slice(0, left.size() - left.endsWith(separator));
    result.append(left);

    const qsizetype start = right.startsWith(separator);
    const qsizetype count = (right.size() <= start) ? 0 : (right.size() - start - right.endsWith(separator));

    right.slice(start, count);

    if (right.isEmpty())
      return result;

    result.append(separator);
    result.append(right);
    return result;
  }

  static auto info(const QStringView scope = u"/Core"_s)     { return (qInfo().noquote().nospace() << "["<< scope << "]: ");      }
  static auto debug(const QStringView scope = u"/Core"_s)    { return (qDebug().noquote().nospace() << "[" << scope << "]: ");    }
  static auto fatal(const QStringView scope = u"/Core"_s)    { return (qFatal().noquote().nospace() << "[" << scope << "]: ");    }
  static auto warning(const QStringView scope = u"/Core"_s)  { return (qWarning().noquote().nospace() << "[" << scope << "]: ");  }
  static auto critical(const QStringView scope = u"/Core"_s) { return (qCritical().noquote().nospace() << "[" << scope << "]: "); }
}

#include "dbus/Log.hpp"       // IWYU pragma: export
#include "dbus/bluez/Log.hpp" // IWYU pragma: export

#include "media/Log.hpp"       // IWYU pragma: export
#include "media/audio/Log.hpp" // IWYU pragma: export
#include "media/video/Log.hpp" // IWYU pragma: export

#include "pipewire/Log.hpp" // IWYU pragma: export
