#pragma once

#include "dbus/bluez/Adapter.hpp"

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QRegularExpression>

class AdapterPanel : public QGroupBox
{
  Q_OBJECT
  using Adapter = DBus::Bluez::Adapter;

public:
  explicit AdapterPanel(Adapter &adapter, QWidget *parent = nullptr);

signals:

private:
  Adapter *const m_adapter;

  QRegularExpression m_rxAlias;

  /// Widgets
  QLineEdit *m_alias;
  QCheckBox *m_powered;
  QCheckBox *m_discoverable;
  QCheckBox *m_discovering;

  QPushButton *m_startDiscovery;
  QPushButton *m_stopDiscovery;
  QPushButton *m_setAlias;
};
