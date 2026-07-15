// #include "dbus/bluez/Manager.hpp"

#include "widgets/sizes.hpp"
#include "MainWindow.hpp"

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

static constexpr QSize buttonSize { 100, 120 };
static constexpr QSize buttinIconSize { 70, 70 };

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QVBoxLayout *buttonLayout = new QVBoxLayout();
  m_musicPageButton = new QPushButton();
  m_settingsPageButton = new QPushButton();

  buttonLayout->setSpacing(20);
  buttonLayout->addWidget(m_musicPageButton);
  buttonLayout->addWidget(m_settingsPageButton);
  buttonLayout->addStretch();

  m_musicPage = new MusicPage();
  m_settingsPage = new SettingsPage();

  m_stackedWidget = new QStackedWidget();
  m_stackedWidget->addWidget(m_musicPage);
  m_stackedWidget->addWidget(m_settingsPage);

  connect(
    m_musicPageButton, &QPushButton::clicked,
    [this]() {
      m_stackedWidget->setCurrentIndex(0);
    }
  );

  connect(
    m_settingsPageButton, &QPushButton::clicked,
    [this]() {
      m_stackedWidget->setCurrentIndex(1);
    }
  );

  QFrame *separator = new QFrame();
  separator->setFrameShape(QFrame::VLine);
  separator->setFrameShadow(QFrame::Plain);

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(separator);
  mainLayout->addWidget(m_stackedWidget);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);

  QFont fnt = font();
  fnt.setPointSize(UI::Size::font);
  setFont(fnt);


  m_musicPageButton->setIcon(QIcon(":/icons-outline/music"));
  m_settingsPageButton->setIcon(QIcon(":/icons-outline/settings"));

  m_musicPageButton->setIconSize(buttinIconSize);
  m_settingsPageButton->setIconSize(buttinIconSize);

  m_musicPageButton->setMinimumSize(buttonSize);
  m_settingsPageButton->setMinimumSize(buttonSize);
}

MainWindow::~MainWindow() = default;
