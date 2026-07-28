#pragma once

#include "panels/debugPanel/AdapterPanel.hpp"
#include "panels/debugPanel/DevicePanel.hpp"
#include "panels/debugPanel/MediaPanel.hpp"

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
