#pragma once

#include <spa/param/param.h>

namespace PipeWire
{
  class ParamInfo
  {
  public:
    /// @brief Updates the cached flags.
    /// @return true on first observation or when the flags changed.
    bool update(const spa_param_info &info)
    {
      if (m_flags == info.flags)
        return false;

      m_flags = info.flags;
      return true;
    }

  private:
    Local<u32> m_flags = 0;
  };
}
