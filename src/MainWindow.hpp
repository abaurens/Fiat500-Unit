#pragma once

#include "pages/MusicPage.hpp"
#include "pages/SettingsPage.hpp"

#include <QMainWindow>

#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  QStackedWidget *m_stackedWidget;

  QPushButton *m_musicPageButton;
  QPushButton *m_settingsPageButton;

  MusicPage    *m_musicPage;
  SettingsPage *m_settingsPage;
};
