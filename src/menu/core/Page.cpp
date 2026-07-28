#include "Page.hpp"

Menu::Page::Page(const QString &name, Page *parent)
: Entry{ name, parent }, m_layout(new QVBoxLayout(this))
{
  setContentsMargins(0, 0, 0, 0);

  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);

  m_layout->addStretch();
}
