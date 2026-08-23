#pragma once

// IWYU pragma: private; include "Log.hpp"

namespace Media::Audio::Log
{
  static auto info(const QStringView scope = u""_s)     { return ::Media::Log::info(::Log::make_path(u"/Audio"_s, scope));     }
  static auto debug(const QStringView scope = u""_s)    { return ::Media::Log::debug(::Log::make_path(u"/Audio"_s, scope));    }
  static auto fatal(const QStringView scope = u""_s)    { return ::Media::Log::fatal(::Log::make_path(u"/Audio"_s, scope));    }
  static auto warning(const QStringView scope = u""_s)  { return ::Media::Log::warning(::Log::make_path(u"/Audio"_s, scope));  }
  static auto critical(const QStringView scope = u""_s) { return ::Media::Log::critical(::Log::make_path(u"/Audio"_s, scope)); }
}
