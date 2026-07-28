#include "Entry.hpp"
#include "Page.hpp"

Menu::Entry::Entry(const QString &name, Page *parent)
: QWidget{},
  m_parent{ parent }, m_name{ name },
  m_depth{ (parent ? parent->m_depth : 0) + 1 }
{}

Menu::Page *Menu::Entry::root()
{
  if (m_parent == nullptr)
    Q_ASSERT(isOfType<Page>());

  return (m_parent ? m_parent->root() : static_cast<Page*>(this));
}

const Menu::Page *Menu::Entry::root() const
{
  if (m_parent == nullptr)
    Q_ASSERT(isOfType<Page>());

  return (m_parent ? m_parent->root() : static_cast<const Page *>(this));
}
