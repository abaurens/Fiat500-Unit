#include "Menu.hpp"
#include "Page.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QGroupBox>

Menu::Menu(const QString &name, QWidget *parent) : QStackedWidget(parent)
{
  m_root = new Page{ name };

  addWidget(m_root);

  connect(m_root, &Page::pageAdded,     this, &Menu::registerPage);
  connect(m_root, &Page::pageRequested, this, &Menu::setPage);

  resetToRoot();
}

void Menu::setPage(Page &page)
{
  Q_ASSERT(page.root() == m_root);

  emit pageChanged(page);
  setCurrentWidget(&page);
}

void Menu::registerPage(Page &page)
{
  Q_ASSERT(page.root() == m_root);

  addWidget(&page);
}
