#pragma once

#include "menu/core/Page.hpp"
#include <QWidget>

class SystemPage : public Menu::Page
{
  Q_OBJECT

public:
  explicit SystemPage(Menu::Page *parent = nullptr);

signals:
};
