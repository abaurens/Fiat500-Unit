#pragma once

#include <QWidget>

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QProgressBar>

#include "dbus/bluez/MediaPlayer.hpp"
#include "dbus/bluez/MediaControl.hpp"

class MediaPanel : public QGroupBox
{
  Q_OBJECT

  using MediaPlayer = DBus::Bluez::MediaPlayer;
  using MediaControl = DBus::Bluez::MediaControl;

public:
  explicit MediaPanel(QWidget *parent = nullptr);

  const MediaPlayer *mediaPlayer() const { return m_mediaPlayer; }
  const MediaControl *mediaControler() const { return m_controler; }

public slots:
  void setMediaControler(DBus::Bluez::MediaControl *controler);

signals:

private slots:
  void setMediaPlayer(DBus::Bluez::MediaPlayer *player);
  void setTrackInfo(const DBus::Bluez::TrackInfo &track);

private:
  MediaControl  *m_controler = nullptr;
  MediaPlayer   *m_mediaPlayer = nullptr;

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
