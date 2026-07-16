#include "SettingsPage.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QGroupBox>

SettingsPage::SettingsPage(QWidget *parent) : Navigator{ new Menu("Settings"), parent }
{
  [[maybe_unused]] Menu::Page *generalPage;
  [[maybe_unused]] Menu::Page *bluetoothPage;

  generalPage   = menu()->addPage<Menu::Page>("General");
  bluetoothPage = menu()->addPage<Menu::Page>("Bluetooth");
}
