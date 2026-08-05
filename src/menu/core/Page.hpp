#pragma once

#include "menu/core/Menu.hpp"
#include "menu/core/Entry.hpp"
#include "menu/core/Item.hpp"

#include <QVBoxLayout>

#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>

template<class T>
concept PageType = std::derived_from<T, Menu::Page>;

class Menu::Page : public Menu::Entry
{
  Q_OBJECT

public:
  Page(const QString &name, Page *parent = nullptr);
  virtual ~Page() = default;

  template<ItemType Itm, class... Args>
  Itm *addItem(Args &&... args);

  template<PageType Pge, class... Args>
  Pge *addPage(Args &&... args);

signals:
  void itemAdded(Menu::Item &item);
  void pageAdded(Menu::Page &page);
  void pageRequested(Menu::Page &page);

private:
  void addWidget(QWidget *widget)
  {
    m_layout->insertWidget(m_layout->count() - 1, widget);
  }

private:
  QVBoxLayout *m_layout;
};


#ifndef SKIP_IMPLEMENTATION
#include "Page.tpp" // IWYU pragma: export
#endif
