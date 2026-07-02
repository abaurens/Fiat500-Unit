// #include "MainWindow.hpp"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "dbus/bluez/BluetoothManager.hpp"
#include "dbus/Types.hpp"



#include "MainWindow.hpp"

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
  BluetoothManager manager;
  manager.initialize();




  MainWindow w;

  w.connect(
    &w, &MainWindow::nameChanged,
    [&manager](const QString &name) { manager.adapter()->setAlias(name); }
  );

  w.connect(
    &w, &MainWindow::discClicked,
    [&manager]() { manager.adapter()->setDiscoverable(!manager.adapter()->discoverable()); }
  );

  w.show();
  return QApplication::exec();
}
