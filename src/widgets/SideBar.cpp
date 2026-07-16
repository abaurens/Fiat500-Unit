#include "SideBar.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QOverload>

#include <pages/SettingsPage.hpp>

SideBar::SideBar(QWidget *parent) : QWidget{ parent }
{
  QHBoxLayout *mainLayout = new QHBoxLayout();

  m_buttons = new QVBoxLayout();
  m_stack = new QStackedWidget();

  m_buttonGroup = new QButtonGroup(this);
  m_buttonGroup->setExclusive(true);

  m_buttons->setSpacing(20);
  m_buttons->addStretch();

  QFrame *separator = new QFrame();
  separator->setFrameShape(QFrame::VLine);
  separator->setFrameShadow(QFrame::Plain);

  mainLayout->addLayout(m_buttons);
  mainLayout->addWidget(separator);
  mainLayout->addWidget(m_stack);

  setLayout(mainLayout);

  connect(
    m_buttonGroup, &QButtonGroup::idClicked,
    this, qOverload<int>(&SideBar::selectPage)
  );
}

void SideBar::selectPage(int pageId)
{
  if (pageId >= m_stack->count())
    return;

  m_stack->setCurrentIndex(pageId);
  emit pageSelected(pageId);
}

void SideBar::selectPage(QWidget &page)
{
  return selectPage(m_stack->indexOf(&page));
}

SideBar::Button::Button(const QString &icon, QWidget *parent) : QPushButton{ parent }
{
  setCheckable(true);
  setIcon(QIcon(icon));
  setIconSize(Button::buttinIconSize);
  setMinimumSize(Button::buttonSize);
}
