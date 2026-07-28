#pragma once
#define PAGE_TPP

#include "menu/widgets/ActionItem.hpp"
// #include "menu/widgets/PageItem.hpp"

template<ItemType Itm, class... Args>
Itm *Menu::Page::addItem(Args &&... args)
{
  Itm *newItem = new Itm(std::forward<Args>(args)..., this);

  emit itemAdded(*newItem);
  addWidget(newItem);

  return newItem;
}

template<PageType Pge, class... Args>
Pge *Menu::Page::addPage(Args &&... args)
{
  Pge *newPage = new Pge(std::forward<Args>(args)..., this);

  addItem<ActionItem>(
    newPage->name(),
    [this, newPage](){
      emit root()->pageRequested(*newPage);
    }
  );

  emit root()->pageAdded(*newPage);
  return newPage;
}

// Define the Menu::addItem<>(...) and Menu::addPage<>(...) templates
// here because we need Menu::Page to be complete.

template<class Itm, class... Args>
Itm *Menu::addItem(Args &&... args)
{
  return m_root->addItem<Itm>(std::forward<Args>(args)...);
}

template<class Pge, class... Args>
Pge *Menu::addPage(Args &&... args)
{
  return m_root->addPage<Pge>(std::forward<Args>(args)...);
}
