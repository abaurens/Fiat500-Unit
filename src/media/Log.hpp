#pragma once

// IWYU pragma: private; include "Log.hpp"

namespace Media::Log
{
  static auto info(const QStringView scope = u""_s)     { return ::Log::info(::Log::make_path(u"/Media "_s, scope));     }
  static auto debug(const QStringView scope = u""_s)    { return ::Log::debug(::Log::make_path(u"/Media"_s, scope));    }
  static auto fatal(const QStringView scope = u""_s)    { return ::Log::fatal(::Log::make_path(u"/Media"_s, scope));    }
  static auto warning(const QStringView scope = u""_s)  { return ::Log::warning(::Log::make_path(u"/Media"_s, scope));  }
  static auto critical(const QStringView scope = u""_s) { return ::Log::critical(::Log::make_path(u"/Media "_s, scope)); }
}
