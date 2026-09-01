#pragma once

// IWYU pragma: private; include "Log.hpp"

namespace PipeWire::Log
{
  static auto info(const QStringView scope = u""_s)     { return ::Log::info(::Log::make_path(u"/PipeWire"_s, scope));     }
  static auto debug(const QStringView scope = u""_s)    { return ::Log::debug(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto fatal(const QStringView scope = u""_s)    { return ::Log::fatal(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto warning(const QStringView scope = u""_s)  { return ::Log::warning(::Log::make_path(u"/PipeWire"_s, scope));  }
  static auto critical(const QStringView scope = u""_s) { return ::Log::critical(::Log::make_path(u"/PipeWire"_s, scope)); }
}

#include <spa/utils/dict.h>

template<class OS>
OS &&operator<<(OS &&os, const spa_dict *const dictionary)
{
  if (!dictionary)
    os << "nulldict";

  size_t len = 0;

  const spa_dict_item *item;
  os << "{";
  spa_dict_for_each(item, dictionary)
  {
    ++len;
    os << "\n  [" << item->key << "] = \"" << item->value << "\"";
  }

  os << (len ? "\n}" : "}");

  return std::forward<OS>(os);
}

template<class OS>
OS &&operator<<(OS &&os, const spa_dict &dictionary)
{
  os << &dictionary;
  return std::forward<OS>(os);
}
