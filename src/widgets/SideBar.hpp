#pragma once

#include <QVBoxLayout>

#include <QPushButton>
#include <QButtonGroup>
#include <QStackedWidget>

class SideBar : public QWidget
{
  Q_OBJECT

  class Button;

public:
  explicit SideBar(QWidget *parent = nullptr);

signals:
  void pageSelected(int);

public slots:
  void selectPage(int pageId);
  void selectPage(QWidget &page);

public:
  template<class T, class... Args>
  T *addPage(const QString &icon, Args&&... args);

  template<class T>
  T *getPage(int pageId)
  {
    return dynamic_cast<T*>(m_stack->widget(pageId));
  }

private:
  QVBoxLayout     *m_buttons;
  QStackedWidget  *m_stack;
  QButtonGroup    *m_buttonGroup;
};

class SideBar::Button : public QPushButton
{
  Q_OBJECT

  static constexpr QSize buttonSize { 100, 120 };
  static constexpr QSize buttinIconSize { 70, 70 };

  friend class SideBar;

public:
  explicit Button(const QString &icon, QWidget *parent = nullptr);
};


template<class T, class... Args>
T *SideBar::addPage(const QString &icon, Args&&... args)
{
  T *page = new T(std::forward<Args>(args)...);
  Button *button = new Button(icon);

  const int id = m_stack->count();

  m_buttonGroup->addButton(button, id);
  m_buttons->insertWidget(id, button);
  m_stack->addWidget(page);

  button->setChecked(id == 0);
  if (id == 0)
    m_stack->setCurrentWidget(page);

  return page;
}
