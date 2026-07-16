#include "Item.hpp"

Menu::Item::Item(const QString &name, Page *parent) : Menu::Entry{ name, parent }, m_layout{ new QHBoxLayout(this) }
{
  Q_ASSERT(parent != nullptr);
}
