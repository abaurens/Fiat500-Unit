#pragma once

#include <spa/param/audio/format-utils.h>

#include <algorithm>

namespace PipeWire
{
  struct AudioFormat
  {
  private:
    using Channels = std::array<spa_audio_channel, SPA_AUDIO_MAX_CHANNELS>;

  public:
    static constexpr u32 MaxChannels = SPA_AUDIO_MAX_CHANNELS;

  public:
    bool operator==(const AudioFormat &other) const
    {
      if (format != other.format || rate != other.rate || channels != other.channels)
        return false;

      return std::equal(
        positions.begin(),
        positions.begin() + channels,
        other.positions.begin()
      );
    }

    bool operator==(const spa_audio_info_raw &other) const
    {
      if (format != other.format || rate != other.rate || channels != other.channels)
        return false;

      for (u32 i = 0; i < channels; ++i)
      {
        if (positions[i] != other.position[i])
          return false;
      }

      return true;
    }

    friend bool operator==(const spa_audio_info_raw &lhs, const AudioFormat &rhs)
    {
      return rhs == lhs;
    }

  public:
    spa_audio_format format;
    u32 rate;
    u32 channels;
    Channels positions {};
  };

}
