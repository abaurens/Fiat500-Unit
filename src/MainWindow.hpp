#pragma once

#include "widgets/SideBar.hpp"

#include <QMainWindow>
#include <QStringLiteral>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:

private:
  SideBar *m_sideBar;
};
