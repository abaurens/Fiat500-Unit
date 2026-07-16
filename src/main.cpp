#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include "MainWindow.hpp"

#include <QCommandLineParser>
#include <QApplication>
#include <QTranslator>
#include <QLocale>

enum : size_t
{
  opt_no_fullscreen,
  opt_count
};

static bool g_flags[opt_count] = { 0 };

void parseArgs(QApplication &app)
{

  constexpr const char *const commands[opt_count * 3] = {
    "nfc", "no-fullscreen", "Disable fullscreen launch (useful when testing on a PC)"
  };

  QCommandLineParser parser;
  std::vector<QCommandLineOption> options;
  {
    options.reserve(opt_count);

    for (size_t i = 0; i < opt_count; ++i)
    {
      options.emplace_back(
        QStringList{ commands[i * 3], commands[i * 3 + 1] },
        app.tr(commands[i * 3 + 2])
      );

      parser.addOption(options.back());
    }
  }
  parser.process(app);

  for (size_t i = 0; i < opt_count; ++i)
    g_flags[i] = parser.isSet(options[i]);
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  parseArgs(app);

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

 #ifndef NO_DBUS
  registerDBusTypes();
  DBus::Bluez::Manager::initialize();
 #endif

  MainWindow window;

  if (FULLSCREEN_ALLOWED && !g_flags[opt_no_fullscreen])
  {
    window.showFullScreen();
  }
  else
  {
    window.setFixedSize(1280, 800);
    window.show();
  }

  return app.exec();
}
