#pragma once

// IWYU pragma: private; include "Log.hpp"

#include <spa/utils/dict.h>

namespace PipeWire::Log
{
  static auto info(const QStringView scope = u""_s)     { return ::Log::info(::Log::make_path(u"/PipeWire"_s, scope));     }
  static auto debug(const QStringView scope = u""_s)    { return ::Log::debug(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto fatal(const QStringView scope = u""_s)    { return ::Log::fatal(::Log::make_path(u"/PipeWire"_s, scope));    }
  static auto warning(const QStringView scope = u""_s)  { return ::Log::warning(::Log::make_path(u"/PipeWire"_s, scope));  }
  static auto critical(const QStringView scope = u""_s) { return ::Log::critical(::Log::make_path(u"/PipeWire"_s, scope)); }

  using namespace ::Log;

  using paddedDict = ::Log::padded<spa_dict>;

  template<class OS>
  OS &&dumpDict(const paddedDict &dictionary, OS &&os)
  {
    if (!dictionary.data)
      os << "nulldict";

    const std::string padding(dictionary.padding << 1, ' ');

    size_t len = 0;

    const spa_dict_item *item;
    os << "{";
    spa_dict_for_each(item, dictionary.data)
    {
      ++len;
      os << '\n' << padding << "  [" << item->key << "] = \"" << item->value << "\"";
    }

    if (len)
      os << '\n' << padding;

    os << '}';

    return std::forward<OS>(os);
  }
}

template<class OS>
OS &&operator<<(OS &&os, const PipeWire::Log::paddedDict &dictionary)
{
  return PipeWire::Log::dumpDict(dictionary, std::forward<OS>(os));
}

template<class OS>
OS &&operator<<(OS &&os, const spa_dict *const dictionary)
{
  os << Log::padd(dictionary);
  return std::forward<OS>(os);
}

template<class OS>
OS &&operator<<(OS &&os, const spa_dict &dictionary)
{
  os << Log::padd(dictionary);
  return std::forward<OS>(os);
}
