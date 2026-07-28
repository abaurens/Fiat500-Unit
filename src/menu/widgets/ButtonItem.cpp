#include "menu/core/Page.hpp"
#include "menu/widgets/ButtonItem.hpp"

ButtonItem::ButtonItem(const QString &text, Menu::Page *parent)
  : Menu::Item{ text, parent }, m_button{ new QPushButton(text) }
{
  m_layout->addWidget(m_button);

  m_button->setMinimumHeight(40);

  connect(
    m_button, &QPushButton::clicked,
    this,     &ButtonItem::clicked
  );
}
