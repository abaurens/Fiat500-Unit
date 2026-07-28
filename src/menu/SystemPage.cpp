#include "SystemPage.hpp"

#include "menu/widgets/ActionItem.hpp"

#include <QApplication>

SystemPage::SystemPage(Menu::Page *parent) : Menu::Page{ tr("Power"), parent }
{
  addItem<ActionItem>(tr("Close app"), &QApplication::closeAllWindows);
  addItem<ButtonItem>(tr("Reboot app"))->setEnabled(false);
  addItem<ButtonItem>(tr("Shutdown device"))->setEnabled(false);
  addItem<ButtonItem>(tr("Reboot device"))->setEnabled(false);
}
