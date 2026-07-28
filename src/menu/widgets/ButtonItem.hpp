#pragma once

#define SKIP_IMPLEMENTATION

#include "menu/core/Page.hpp"
#include "menu/core/Item.hpp"

#include <QPushButton>

class ButtonItem : public Menu::Item
{
  Q_OBJECT

public:
  explicit ButtonItem(const QString &text, Menu::Page *parent);
  virtual ~ButtonItem() = default;

signals:
  void clicked();

protected:
  QPushButton *m_button;
};
