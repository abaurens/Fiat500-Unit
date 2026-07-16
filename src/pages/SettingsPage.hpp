#pragma once

#include "menu/widgets/Navigator.hpp"

class SettingsPage : public Navigator
{
  Q_OBJECT

public:
  explicit SettingsPage(QWidget *parent = nullptr);

private:
  Menu *m_memu;
};
