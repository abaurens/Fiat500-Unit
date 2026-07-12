#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#define DEBUG_PANEL true

#if DEBUG_PANEL
# include "Widgets/DebugPanel.hpp"
using WindowType = DebugPanel;
# define showFullScreen show
#else
# include "MainWindow.hpp"
using WindowType = MainWindow;
#endif

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

  WindowType w;

  w.showFullScreen();
  return QApplication::exec();
}
