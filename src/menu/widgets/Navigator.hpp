#pragma once

#include "menu/widgets/Breadcrumbs.hpp"
#include "menu/Menu.hpp"

#include <QVBoxLayout>

#include <QWidget>
#include <QStackedWidget>


class Navigator : public QWidget
{
  Q_OBJECT

public:
  explicit Navigator(QWidget *parent = nullptr);
  explicit Navigator(Menu *menu, QWidget *parent = nullptr);

  Menu *menu() { return m_menu; }
  const Menu *menu() const { return m_menu; }

public slots:
  void setMenu(Menu &menu);
  void setPage(Menu::Page &page);
  void resetToRoot() { m_menu->resetToRoot(); }

  void onMenuPageChange(Menu::Page &page);

private:
  QVBoxLayout *m_mainLayout;
  Breadcrumbs *m_breadcrumbs;
  Menu        *m_menu = nullptr;
};
