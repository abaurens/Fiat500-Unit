#pragma once

#include <QSize>
#include <QMargins>

namespace UI
{
  namespace Size
  {
    inline constexpr int      spacing = 3;
    inline constexpr int      margin = 6;
    inline constexpr QMargins margins { margin, margin, margin, margin };

    inline constexpr int font = 13;

    namespace Min
    {
      inline constexpr int height = 35;
      inline constexpr int width  = 100;

      inline constexpr QSize size { width, height };
    }
  }
}

#define APPLY_MIN_HEIGHT(widget) widget->setMinimumHeight(UI::Size::Min::height)
#define APPLY_MIN_WIDTH(widget) widget->setMinimumWidth(UI::Size::Min::width)
#define APPLY_MIN_SIZE(widget) widget->setMinimumSize(UI::Size::Min::size)
