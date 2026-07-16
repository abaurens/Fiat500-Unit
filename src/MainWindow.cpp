// #include "dbus/bluez/Manager.hpp"

#include "widgets/sizes.hpp"
#include "MainWindow.hpp"

#include "pages/MusicPage.hpp"
#include "pages/SettingsPage.hpp"

#ifndef NO_DBUS
# ifdef DEBUG_PANEL
#  include "pages/debugPanel/DebugPanel.hpp"
# endif
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  m_sideBar = new SideBar();

  m_sideBar->addPage<MusicPage>(QStringLiteral(":/icons-outline/music"));
  m_sideBar->addPage<SettingsPage>(QStringLiteral(":/icons-outline/settings"));

 #ifndef NO_DBUS
 # ifdef DEBUG_PANEL
  m_sideBar->addPage<DebugPanel>(QStringLiteral(":/icons-outline/bug"));
 # endif
 #endif

  connect(
    m_sideBar, &SideBar::pageSelected,
    [this](int id)
    {
      SettingsPage *settingPage = m_sideBar->getPage<SettingsPage>(id);
      if (settingPage)
        settingPage->resetToRoot();
    }
  );

  setCentralWidget(m_sideBar);

  QFont fnt = font();
  fnt.setPointSize(UI::Size::font);
  setFont(fnt);
}

MainWindow::~MainWindow() = default;
