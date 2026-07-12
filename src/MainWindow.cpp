// #include "dbus/bluez/Manager.hpp"

#include "MainWindow.hpp"

#include <QSize>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

static constexpr QSize buttonSize { 100, 120 };

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QVBoxLayout *buttonLayout = new QVBoxLayout();
  m_musicPageButton = new QPushButton(tr("Music"));
  m_settingsPageButton = new QPushButton(tr("Settings"));

  m_musicPageButton->setMinimumSize(buttonSize);
  m_settingsPageButton->setMinimumSize(buttonSize);

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

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(m_stackedWidget);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() = default;
