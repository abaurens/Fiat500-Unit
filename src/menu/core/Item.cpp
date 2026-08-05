#include "pch.hpp" // IWYU pragma: keep

#include "Item.hpp"

Menu::Item::Item(const QString &name, Page *parent) : Menu::Entry{ name, parent }, m_layout{ new QHBoxLayout(this) }
{
  Q_ASSERT(parent != nullptr);

  setContentsMargins(0, 0, 0, 0);

  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);
}
