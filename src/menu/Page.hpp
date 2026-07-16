#pragma once

#include "menu/Menu.hpp"
#include "menu/Entry.hpp"
#include "menu/Item.hpp"

#include <QVBoxLayout>

#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>

template<class T>
concept PageType = std::derived_from<T, Menu::Page>;

class Menu::Page : public Menu::Entry
{
  Q_OBJECT

  class Button;

public:
  Page(const QString &name, Page *parent = nullptr);
  virtual ~Page() = default;

  template<ItemType Itm, class... Args>
  Itm *addItem(const QString &name, Args &&... args);

  template<PageType Pge, class... Args>
  Pge *addPage(const QString &name, Args &&... args);

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

class Menu::Page::Button : public Menu::Item
{
  Q_OBJECT

public:
  Button(const QString &text, Menu::Page &target, Menu::Page *parent);

signals:
  void clicked(Menu::Page &page);

private:
  Menu::Page  *m_target;
  QPushButton *m_button;
};

template<ItemType Itm, class... Args>
Itm *Menu::Page::addItem(const QString &name, Args &&... args)
{
  Itm *newItem = new Itm(name, std::forward<Args>(args)..., this);

  emit itemAdded(*newItem);
  addWidget(newItem);

  return newItem;
}

template<PageType Pge, class... Args>
Pge *Menu::Page::addPage(const QString &name, Args &&... args)
{
  Pge *newPage = new Pge(name, std::forward<Args>(args)..., this);

  Page::Button *button = addItem<Page::Button>(name, *newPage);

  connect(
    button, &Button::clicked,
    root(), &Page::pageRequested
  );

  emit root()->pageAdded(*newPage);
  return newPage;
}

// Define the Menu::addPage<> template here because we need Menu::Page to be complete
template<class T>
Menu::Page *Menu::addPage(const QString &name)
{
  return m_root->addPage<T>(name);
}
