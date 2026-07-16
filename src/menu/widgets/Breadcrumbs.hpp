#pragma once

#include "menu/Page.hpp"

#include <QWidget>

#include <QHBoxLayout>
#include <QPushButton>

class Breadcrumbs : public QWidget
{
  Q_OBJECT

  class Button;

public:
  explicit Breadcrumbs(QWidget *parent = nullptr);

  qsizetype depth() const { return (m_target ? m_target->depth() : 0); }

signals:
  void pageRequested(Menu::Page &page);

public slots:
  void setPath(const Menu::Page &path);

private slots:
  void requestPage(qsizetype buttonId);

private:
  void adjustLayoutTo(qsizetype depth);
  void trimLayoutTo(qsizetype depth);
  void expandLayoutTo(qsizetype depth);
  void ensureButtonCount(qsizetype count);

private:
  const Menu::Page *m_target;
  QHBoxLayout *m_layout;
  QList<Button *> m_buttons;
  QHash<qsizetype, const Menu::Page *> m_pages;
};

class Breadcrumbs::Button : public QPushButton
{
  Q_OBJECT

public:
  Button(qsizetype id);

signals:
  void clicked(qsizetype id);

private:
  qsizetype m_id;
};
