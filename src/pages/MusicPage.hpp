#pragma once

#include <QImage>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QProgressBar>

class MusicPage : public QWidget
{
  Q_OBJECT

public:
  explicit MusicPage(QWidget *parent = nullptr);

private:
  QImage m_coverImage;

  QLabel *m_cover = nullptr;
  QLabel *m_title = nullptr;
  QLabel *m_album = nullptr;
  QLabel *m_artist = nullptr;

  QPushButton *m_playPause = nullptr;
  QPushButton *m_nextTrack = nullptr;
  QPushButton *m_previousTrack = nullptr;

  QLabel       *m_elapsedTime = nullptr;
  QLabel       *m_remainingTime = nullptr;
  QProgressBar *m_timeline = nullptr;
};
