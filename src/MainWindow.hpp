#pragma once

#include "widgets/AdapterPanel.hpp"
#include "widgets/DevicePanel.hpp"

#include <QMainWindow>

#include <QLineEdit>
#include <QPushButton>


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

signals:

private slots:

private:
  AdapterPanel *m_adapterPanel;
  DevicePanel  *m_devicePanel;
};
