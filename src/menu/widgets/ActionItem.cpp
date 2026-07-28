#include "ActionItem.hpp"

void ActionItem::unsetAction()
{
  disconnect(m_button, &QPushButton::clicked, nullptr, nullptr);
  connect(m_button, &QPushButton::clicked, this, &ButtonItem::clicked);
}
