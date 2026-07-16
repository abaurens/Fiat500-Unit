#pragma once

#include "menu/Menu.hpp"
#include "menu/Entry.hpp"

#include <QHBoxLayout>

#include <QWidget>

class Menu::Item : public Menu::Entry
{
  Q_OBJECT

public:
  explicit Item(const QString &name, Page *parent);
  virtual ~Item() = default;

signals:

protected:
  QHBoxLayout *m_layout;
};

template<class T>
concept ItemType = std::derived_from<T, Menu::Item>;
