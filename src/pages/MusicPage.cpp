#include "MusicPage.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

static constexpr int g_titleFontSize = 26;
static constexpr int g_artistFontSize = 24;
static constexpr int g_albumFontSize = 18;

static constexpr QSize g_albumCoverSize { 400, 400 };

MusicPage::MusicPage(QWidget *parent) : QWidget(parent)
{
  /**************************\
  |*    Widgets creation    *|
  \**************************/

  m_cover = new QLabel();
  m_title = new QLabel("Track title goes here");
  m_album = new QLabel("Album title");
  m_artist = new QLabel("Artist name");

  m_playPause = new QPushButton();
  m_nextTrack = new QPushButton();
  m_previousTrack = new QPushButton();

  m_timeline = new QProgressBar();
  m_elapsedTime = new QLabel("00:00", this);
  m_remainingTime = new QLabel("00:00", this);

  m_coverImage.load(":/img/defaultCover");

  /**************************\
  |*    Widgets settings    *|
  \**************************/

  // Cover image
  QHBoxLayout *coverLayout = new QHBoxLayout();
  {
    m_cover->setBackgroundRole(QPalette::Base);
    m_cover->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QPixmap pixmap = QPixmap::fromImage(m_coverImage);
    m_cover->setFixedSize(g_albumCoverSize);
    m_cover->setAlignment(Qt::AlignCenter);
    m_cover->setPixmap(
      pixmap.scaled(
        m_cover->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
      )
    );

    coverLayout->addStretch();
    coverLayout->addWidget(m_cover);
    coverLayout->addStretch();
  }

  // Timeline
  {
    m_timeline->setTextVisible(false);
    m_timeline->setMaximum(10);
    m_timeline->setValue(9);
  }

  // Labels
  {
    m_title->setAlignment(Qt::AlignCenter);
    m_album->setAlignment(Qt::AlignCenter);
    m_artist->setAlignment(Qt::AlignCenter);

    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_album->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_artist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  }

  /**************************\
  |*      Layout setup      *|
  \**************************/

  QHBoxLayout *btnLayout = new QHBoxLayout();
  btnLayout->setSpacing(30);
  btnLayout->addStretch();
  btnLayout->addWidget(m_previousTrack);
  btnLayout->addWidget(m_playPause);
  btnLayout->addWidget(m_nextTrack);
  btnLayout->addStretch();

  QHBoxLayout *timelineLayout = new QHBoxLayout();
  timelineLayout->addSpacing(120);
  timelineLayout->addWidget(m_elapsedTime);
  timelineLayout->addWidget(m_timeline);
  timelineLayout->addWidget(m_remainingTime);
  timelineLayout->addSpacing(120);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addStretch();
  mainLayout->addLayout(coverLayout);
  mainLayout->addSpacing(20);
  mainLayout->addWidget(m_title);
  mainLayout->addWidget(m_artist);
  mainLayout->addWidget(m_album);
  mainLayout->addSpacing(20);
  mainLayout->addLayout(timelineLayout);
  mainLayout->addSpacing(20);
  mainLayout->addLayout(btnLayout);
  mainLayout->addStretch();

  /**************************\
  |*   Rendering settings   *|
  \**************************/

  QFont fnt = font();
  fnt.setPointSize(22);
  setFont(fnt);

  fnt.setPointSize(g_titleFontSize);
  m_title->setFont(fnt);
  fnt.setPointSize(g_artistFontSize);
  m_artist->setFont(fnt);
  fnt.setPointSize(g_albumFontSize);
  m_album->setFont(fnt);

  // Buttons
  {
    m_playPause->setIcon(QIcon(":/icons-filled/player-play"));
    m_nextTrack->setIcon(QIcon(":/icons-filled/player-skip-forward"));
    m_previousTrack->setIcon(QIcon(":/icons-filled/player-skip-back"));

    m_playPause->setIconSize(QSize(56, 56));
    m_nextTrack->setIconSize(QSize(50, 50));
    m_previousTrack->setIconSize(QSize(50, 50));

    m_playPause->setMinimumSize(QSize{82, 82});
    m_nextTrack->setMinimumSize(QSize{74, 74});
    m_previousTrack->setMinimumSize(QSize{74, 74});
  }
}
