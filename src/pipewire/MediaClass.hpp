#pragma once

#include "Enum.hpp"

namespace PipeWire
{
  MAKE_ENUM(MediaClass,
    AudioSink, AudioSource, AudioDuplex,

    StreamOutputAudio, StreamInputAudio,

    VideoSink, VideoSource,

    MidiBridge
  );

  inline constexpr MediaClass parseMediaClass(const QStringView str)
  {
    const qsizetype separatorCount = str.count('/');

    // Fail to parse if more than 2 or no separators
    if (separatorCount == 0 || separatorCount > 2)
      return MediaClass::Unknown;

    QString name{ str };

    qsizetype pos = 0;
    // Loop as long as we find separators to remove
    while ((pos = name.indexOf('/', pos)) != -1)
    {
      name.removeAt(pos);
    }

    return MediaClass::FromName(name);
  }

  inline constexpr MediaClass parseMediaClass(const std::string_view str)
  {
    const size_t separatorCount = std::count(str.cbegin(), str.cend(), '/');

    // Fail to parse if more than 2 or no separators
    if (separatorCount == 0 || separatorCount > 2)
      return MediaClass::Unknown;

    std::string name{ str };

    size_t pos = 0;
    // Loop as long as we find separators to remove
    while ((pos = name.find_first_of('/', pos)) != std::string::npos)
    {
      name.erase(name.begin() + pos);
    }

    return MediaClass::FromName(name);
  }
}
