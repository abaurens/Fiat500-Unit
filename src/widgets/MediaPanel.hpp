#pragma once

#include <QWidget>

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QProgressBar>

#include "dbus/bluez/MediaPlayer.hpp"

class MediaPanel : public QGroupBox
{
  Q_OBJECT

  using MediaPlayer = DBus::Bluez::MediaPlayer;

public:
  explicit MediaPanel(QWidget *parent = nullptr);

  const MediaPlayer *mediaPlayer() const { return m_mediaPlayer; }

public slots:
  void setMediaPlayer(DBus::Bluez::MediaPlayer *controler);

signals:

private slots:
  void setTrackInfo(const DBus::Bluez::TrackInfo &track);

private:
  MediaPlayer  *m_mediaPlayer = nullptr;

private:
  QLabel *m_artist;
  QLabel *m_album;
  QLabel *m_title;

  QPushButton *m_play;
  QPushButton *m_pause;
  QPushButton *m_nextTrack;
  QPushButton *m_previousTrack;

  QProgressBar *m_timeline;
};
