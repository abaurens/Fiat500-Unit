#include "pch.hpp" // IWYU pragma: keep

#include "SettingsPanel.hpp"

// #include "menu/widgets/ActionItem.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QGroupBox>

#include <QApplication>

SettingsPanel::SettingsPanel(QWidget *parent) : Navigator{ new Menu("Settings"), parent }
{
  [[maybe_unused]] Menu::Page *generalPage;
  [[maybe_unused]] Menu::Page *bluetoothPage;

  generalPage   = menu()->addPage<Menu::Page>(tr("General"));
  bluetoothPage = menu()->addPage<Menu::Page>(tr("Bluetooth"));

  m_systemPage  = menu()->addPage<SystemPage>();
}
