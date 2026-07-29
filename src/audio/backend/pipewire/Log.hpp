#pragma once

#include "../../Log.hpp"

namespace Audio::PipeWire::Log
{
  static auto info(const QStringView scope = u""_s)     { return ::Audio::Log::info(::Log::make_path(u"/PipeWire"_s, scope));     }
  static auto debug(const QStringView scope = u""_s)    { return ::Audio::Log::debug(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto fatal(const QStringView scope = u""_s)    { return ::Audio::Log::fatal(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto warning(const QStringView scope = u""_s)  { return ::Audio::Log::warning(::Log::make_path(u"/PipeWire"_s, scope));  }
  static auto critical(const QStringView scope = u""_s) { return ::Audio::Log::critical(::Log::make_path(u"/PipeWire"_s, scope)); }
}
