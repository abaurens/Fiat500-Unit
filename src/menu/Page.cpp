#include "Page.hpp"

Menu::Page::Page(const QString &name, Page *parent)
: Entry{ name, parent }, m_layout(new QVBoxLayout(this))
{
  setContentsMargins(0, 0, 0, 0);

  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);

  m_layout->addStretch();
}

Menu::Page::Button::Button(const QString &text, Page &target, Page *parent)
  : Menu::Item{ text, parent }, m_target{ &target }, m_button{ new QPushButton(text) }
{
  setContentsMargins(0, 0, 0, 0);

  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);

  m_layout->addWidget(m_button);

  m_button->setMinimumHeight(40);

  connect(
    m_button, &QPushButton::clicked,
    [this]{ if (m_target) emit clicked(*m_target); }
  );
}
