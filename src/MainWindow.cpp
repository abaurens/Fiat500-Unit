#include "dbus/bluez/Manager.hpp"

#include "MainWindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *topLayout = new QHBoxLayout();

  setCentralWidget(centralWidget);
  centralWidget->setLayout(mainLayout);

  m_adapterPanel = new AdapterPanel(*DBus::Bluez::Manager::adapter(), this);
  m_devicePanel = new DevicePanel(this);
  m_mediaPanel = new MediaPanel(this);

  topLayout->addWidget(m_adapterPanel);
  topLayout->addWidget(m_mediaPanel);

  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(m_devicePanel);

  DBus::Bluez::MediaControl *controler = nullptr;

  if (!DBus::Bluez::Manager::mediaPlayers().isEmpty())
  {
    DBus::Bluez::MediaControlMap &controls = DBus::Bluez::Manager::mediaControls();
    DBus::Bluez::MediaPlayerMap &players = DBus::Bluez::Manager::mediaPlayers();

    for (auto it = controls.begin(); it != controls.end(); ++it)
    {
      if (it.value() && !it.value()->player().path().isEmpty())
      {
        controler = it.value();
        qDebug() << "Controler selected:" << controler->path().path();

        break;
      }
    }

    if (controler)
    {
      DBus::Bluez::MediaPlayer *player = players.value(controler->player(), nullptr);

      if (player)
      {
        qDebug() << "Player selected:" << player->path().path();
        m_mediaPanel->setMediaPlayer(player);
      }
      else
      {
        qDebug() << "No player selected";
      }
    }
    else
    {
      qDebug() << "No controler selected:";
    }
  }

  if (!controler)
    return;

  connect(
    controler,    &DBus::Bluez::MediaControl::playerChanged,
    m_mediaPanel, &MediaPanel::setMediaPlayer
  );

  //connect(
  //  &DBus::Bluez::Manager::instance(), &DBus::Bluez::Manager::mediaControlRemoved,
  //  [this](const QDBusObjectPath &path)
  //  {
  //    DBus::Bluez::MediaPlayer *controler = DBus::Bluez::Manager::mediaPlayers().value(path, nullptr);
  //    Q_ASSERT(controler != nullptr);
  //
  //    const DBus::Bluez::MediaPlayer *current = m_mediaPanel->mediaPlayer();
  //
  //    if (current && controler->path() == current->path())
  //      m_mediaPanel->setMediaPlayer(nullptr);
  //  }
  //);
}

MainWindow::~MainWindow() = default;
