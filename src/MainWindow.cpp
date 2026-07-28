// #include "dbus/bluez/Manager.hpp"

#include "widgets/sizes.hpp"
#include "MainWindow.hpp"

#include "panels/MusicPanel.hpp"
#include "panels/SettingsPanel.hpp"

#ifndef NO_DBUS
# ifdef DEBUG_PANEL
#  include "panels/debugPanel/DebugPanel.hpp"
# endif
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  m_sideBar = new SideBar();

  m_sideBar->addPage<MusicPanel>(QStringLiteral(":/icons-outline/music"));
  m_sideBar->addPage<SettingsPanel>(QStringLiteral(":/icons-outline/settings"));

 #ifndef NO_DBUS
 # ifdef DEBUG_PANEL
  m_sideBar->addPage<DebugPanel>(QStringLiteral(":/icons-outline/bug"));
 # endif
 #endif

  connect(
    m_sideBar, &SideBar::pageSelected,
    [this](int id)
    {
      SettingsPanel *settingPage = m_sideBar->getPage<SettingsPanel>(id);
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
