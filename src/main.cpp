#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include "MainWindow.hpp"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

using BluetoothManager = DBus::Bluez::Manager;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  //QTranslator translator;
  //const QStringList uiLanguages = QLocale::system().uiLanguages();
  //for (const QString &locale : uiLanguages)
  //{
  //  const QString baseName = "Fiat500-Unit_" + QLocale(locale).name();
  //  if (translator.load(":/i18n/" + baseName))
  //  {
  //    a.installTranslator(&translator);
  //    break;
  //  }
  //}

  registerDBusTypes();
  DBus::Bluez::Manager::initialize();

  MainWindow w;

  w.show();
  return QApplication::exec();
}
