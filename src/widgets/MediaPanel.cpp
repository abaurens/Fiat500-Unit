#include "sizes.hpp"
#include "MediaPanel.hpp"
#include "dbus/bluez/Types.hpp"
#include "dbus/bluez/Manager.hpp"

#include <QVBoxLayout>

constexpr auto artist_str = "Artist: %1";
constexpr auto album_str = "Album: %1";
constexpr auto title_str = "Title: %1";

MediaPanel::MediaPanel(QWidget *parent)
  : QGroupBox{ tr("Media"), parent }
{
  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  m_artist = new QLabel(tr(artist_str).arg(""));
  m_album = new QLabel(tr(album_str).arg(""));
  m_title = new QLabel(tr(title_str).arg(""));

  m_play = new QPushButton(tr("Play"));
  m_pause = new QPushButton(tr("Pause"));
  m_nextTrack = new QPushButton(tr("Next"));
  m_previousTrack = new QPushButton(tr("Previous"));

  m_timeline = new QProgressBar();

  setMinimumWidth(300);

  m_timeline->setTextVisible(true);

  QFont fnt = font();
  fnt.setPointSize(UI::Size::font);
  setFont(fnt);

  buttonLayout->addWidget(m_play);
  buttonLayout->addWidget(m_pause);
  buttonLayout->addWidget(m_nextTrack);
  buttonLayout->addWidget(m_previousTrack);

  mainLayout->addWidget(m_artist);
  mainLayout->addWidget(m_album);
  mainLayout->addWidget(m_title);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(m_timeline);

  setLayout(mainLayout);

  APPLY_MIN_SIZE(m_artist);
  APPLY_MIN_SIZE(m_album);
  APPLY_MIN_SIZE(m_title);
  APPLY_MIN_SIZE(m_play);
  APPLY_MIN_SIZE(m_pause);
  APPLY_MIN_SIZE(m_nextTrack);
  APPLY_MIN_SIZE(m_previousTrack);
  APPLY_MIN_SIZE(m_timeline);
}

void MediaPanel::setMediaControler(MediaControl *controler)
{
  if (m_controler)
  {
    disconnect(
      m_controler,  &DBus::Bluez::MediaControl::playerChanged,
      this,         &MediaPanel::setMediaPlayer
    );
  }

  m_controler = controler;

  if (!controler)
    return;

  setMediaPlayer(DBus::Bluez::Manager::getObject<MediaPlayer>(controler->player()));

  connect(
    controler, &DBus::Bluez::MediaControl::playerChanged,
    this,      &MediaPanel::setMediaPlayer
  );
}

void MediaPanel::setMediaPlayer(MediaPlayer *player)
{
  // Don't overwrite the controler if we already have one
  if (player == m_mediaPlayer)
    return;

  if (m_mediaPlayer != nullptr)
  {
    disconnect(m_mediaPlayer, &MediaPlayer::trackChanged, nullptr, nullptr);
    disconnect(m_mediaPlayer, &MediaPlayer::positionChanged, nullptr, nullptr);

    disconnect(m_play,          nullptr, nullptr, nullptr);
    disconnect(m_pause,         nullptr, nullptr, nullptr);
    disconnect(m_nextTrack,     nullptr, nullptr, nullptr);
    disconnect(m_previousTrack, nullptr, nullptr, nullptr);

    disconnect(m_timeline, nullptr, nullptr, nullptr);
  }

  m_mediaPlayer = player;

  const bool playerExist = (m_mediaPlayer != nullptr);

  m_play->setEnabled(playerExist);
  m_pause->setEnabled(playerExist);
  m_nextTrack->setEnabled(playerExist);
  m_previousTrack->setEnabled(playerExist);

  m_timeline->setEnabled(playerExist);

  if (!playerExist)
  {
    setTrackInfo({});
    m_timeline->setValue(0);
    return;
  }

  connect(
    player, &MediaPlayer::trackChanged,
    this,   &MediaPanel::setTrackInfo
  );

  connect(
    player, &MediaPlayer::positionChanged,
    [this](uint32_t pos) {
      pos %= (m_timeline->maximum() + 5);

      uint32_t sec = pos / 1000;

      uint32_t hrs = sec / 3600;
      sec -= (hrs * 3600);

      uint32_t min = sec / 60;
      sec -= (min * 60);

      if (hrs > 0)
      {
        m_timeline->setFormat(QString("%1:%2:%3").arg(hrs).arg(min, 2, 10, '0').arg(sec, 2, 10, '0'));
      }
      else
      {
        m_timeline->setFormat(QString("%1:%2").arg(min, 2, 10, '0').arg(sec, 2, 10, '0'));
      }
      m_timeline->setValue(static_cast<int>(pos));
    }
  );

  connect(
    m_play, &QPushButton::clicked,
    player, &MediaPlayer::play
  );

  connect(
    m_pause, &QPushButton::clicked,
    player, &MediaPlayer::pause
  );

  connect(
    m_nextTrack, &QPushButton::clicked,
    player,      &MediaPlayer::next
  );

  connect(
    m_previousTrack, &QPushButton::clicked,
    player,          &MediaPlayer::previous
  );

  setTrackInfo(player->track());
  m_timeline->setValue(static_cast<int>(player->position()));
}

void MediaPanel::setTrackInfo(const DBus::Bluez::TrackInfo &track)
{
  m_artist->setText(tr(artist_str).arg(track.artist));
  m_album->setText(tr(album_str).arg(track.album));
  m_title->setText(tr(title_str).arg(track.title));

  m_timeline->setValue(0);
  m_timeline->setTextVisible(track.duration > 0);
  m_timeline->setMaximum(std::max(static_cast<int>(track.duration), 1));
}
