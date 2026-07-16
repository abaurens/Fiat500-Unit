#include "Navigator.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QGroupBox>

Navigator::Navigator(QWidget *parent) : Navigator(nullptr, parent)
{}

Navigator::Navigator(Menu *menu, QWidget *parent) : QWidget(parent)
{
  m_mainLayout = new QVBoxLayout();
  m_breadcrumbs = new Breadcrumbs();

  setLayout(m_mainLayout);

  m_mainLayout->addWidget(m_breadcrumbs);

  connect(m_breadcrumbs, &Breadcrumbs::pageRequested, this, &Navigator::setPage);

  if (menu)
    setMenu(*menu);
}

void Navigator::setMenu(Menu &menu)
{
  if (m_menu)
  {
    m_menu->resetToRoot();
    m_mainLayout->removeWidget(m_menu);
    disconnect(m_menu, &Menu::pageChanged, this, &Navigator::onMenuPageChange);
  }

  m_menu = &menu;

  if (m_menu)
  {
    connect(m_menu, &Menu::pageChanged, this, &Navigator::onMenuPageChange);
    m_mainLayout->addWidget(m_menu);
    m_menu->resetToRoot();
  }
}

void Navigator::setPage(Menu::Page &page)
{
  m_menu->setPage(page);
}

void Navigator::onMenuPageChange(Menu::Page &page)
{
  m_breadcrumbs->setPath(page);
}
