#pragma once

#include "menu/widgets/Navigator.hpp"

#include "menu/SystemPage.hpp"

class SettingsPanel : public Navigator
{
  Q_OBJECT

public:
  explicit SettingsPanel(QWidget *parent = nullptr);

private:
  Menu *m_memu;

  SystemPage *m_systemPage;
};
