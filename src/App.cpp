#include "App.hpp"
#include "Enum.hpp"
#include "MainWindow.hpp"

#include "audio/Manager.hpp"

#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <frozen/unordered_map.h>

#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>

MAKE_ENUM(Command,
  opt_no_fullscreen
);

struct commandDefinition
{
  QChar       shortName;
  DBus::Name  longName;
  DBus::Name  description;
};

constexpr frozen::unordered_map<size_t, commandDefinition, Command::count()> g_commands = {
  {
   Command::opt_no_fullscreen.value(), commandDefinition{ QChar{ 0 }, "no-fullscreen", "Disable fullscreen launch (useful when testing on a PC)" }
  }
};

bool g_flags[Command::count()] = { false };

App::App(int &ac, char **av, int flags) : QApplication{ ac, av, flags }
{

  parseArgs();

  loadTranslations();

  setupDBus();

  setupPipewire();
}

App::~App()
{

}

void App::parseArgs()
{
  QCommandLineParser parser;
  std::vector<QCommandLineOption> options;
  {
    options.reserve(Command::count());

    for (size_t i = 0; i < Command::count(); ++i)
    {
      options.emplace_back(
        QStringList{
          g_commands.at(i).shortName,
          g_commands.at(i).longName.data()
        },
        tr(g_commands.at(i).description.data())
      );

      parser.addOption(options.back());
    }
  }
  parser.process(*this);

  for (size_t i = 0; i < Command::count(); ++i)
    g_flags[i] = parser.isSet(options[i]);
}

void App::setupDBus()
{
  registerDBusTypes();
  DBus::Bluez::Manager::initialize();
}

void App::setupPipewire()
{
  Audio::Manager::initialize();
  Audio::Manager::backend().run();
}

void App::loadTranslations()
{
  QTranslator translator;

  const QStringList uiLanguages = QLocale::system().uiLanguages();

  for (const QString &locale : uiLanguages)
  {
    const QString baseName = "Fiat500-Unit_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName))
    {
      installTranslator(&translator);
      break;
    }
  }
}

int App::run()
{
  MainWindow window;

  if (FULLSCREEN_ALLOWED && !g_flags[Command::opt_no_fullscreen.value()])
  {
    window.showFullScreen();
  }
  else
  {
    window.setFixedSize(1280, 800);
    window.show();
  }

  m_window = &window;
  return exec();
}
