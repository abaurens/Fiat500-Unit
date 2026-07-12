#pragma once

#include "widgets/AdapterPanel.hpp"
#include "widgets/DevicePanel.hpp"
#include "widgets/MediaPanel.hpp"

#include <QMainWindow>

#include <QLineEdit>
#include <QPushButton>


class DebugPanel : public QWidget
{
  Q_OBJECT

public:
  explicit DebugPanel(QWidget *parent = nullptr);
  ~DebugPanel() override;

signals:

private slots:

private:
  AdapterPanel *m_adapterPanel;
  DevicePanel  *m_devicePanel;
  MediaPanel   *m_mediaPanel;
};
