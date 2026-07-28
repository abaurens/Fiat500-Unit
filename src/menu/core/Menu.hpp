#pragma once

#include <QVBoxLayout>

#include <QWidget>
#include <QStackedWidget>

class Menu : public QStackedWidget
{
  Q_OBJECT

public:
  class Entry;
  class Item;
  class Page;

public:
  explicit Menu(const QString &name, QWidget *parent = nullptr);

  Page &root() { return *m_root; };
  const Page &root() const { return *m_root; };

  template<class Itm, class... Args>
  inline Itm *addItem(Args &&... args);

  template<class Pge, class... Args>
  inline Pge *addPage(Args &&... args);

signals:
  void pageChanged(Menu::Page &page);

public slots:
  void resetToRoot() { setPage(*m_root); }
  void setPage(Menu::Page &page);

private slots:
  void registerPage(Menu::Page &page);

private:
  Page *m_root = nullptr;
};
