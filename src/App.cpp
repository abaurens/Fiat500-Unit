#include "pch.hpp" // IWYU pragma: keep

#include "App.hpp"
#include "MainWindow.hpp"

#include "media/Manager.hpp"

#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>

#include "AsyncTask.hpp"     // IWYU pragma: keep
#include "DetachedTask.hpp"  // IWYU pragma: keep
#include "SignalAwaiter.hpp" // IWYU pragma: keep

#include "pipewire/keys.h" // IWYU pragma: keep
#include "pipewire/handles/Filter.hpp"

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

Scope<PipeWire::Filter> filter;

// Testing creation of custom PipeWire node
// and it's connection into the existing graph.
static DetachedTask testPipeWire()
{
  Log::debug(u"App"_s) << "Creating new filter";
  filter = Media::Manager::context().createFilter("BAA_Test_Filter");

  Log::debug(u"App"_s) << "Adding INPUT port";
  filter->addPort(
    SPA_DIRECTION_INPUT,
    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Test_Input_Port", nullptr)
  );

  Log::debug(u"App"_s) << "Adding OUTPUT port";
  filter->addPort(
    SPA_DIRECTION_OUTPUT,
    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Test_Output_Port", nullptr)
  );

  Log::debug(u"App"_s) << "Connecting filter";
  const bool result = filter->connect();
  Log::debug(u"App"_s) << "filter connection: " << result;

  Log::debug(u"App"_s) << "Awaiting nodeId...";
  u32 nodeId = co_await filter->waitForNodeId();
  Log::debug(u"App"_s) << "Filter nodeId available: " << nodeId << " == " << filter->nodeId();

  auto &node = co_await Media::Manager::waitForObject(nodeId);
  Log::debug(u"App"_s) << "Filter node is available: " << node;
}





//static AsyncTask<u32> child(AwaitTest &test)
//{
//  Log::debug(u"App::TEST"_s) << "child waiting";
//
//  const u32 value = co_await waitForSignal(&test, &AwaitTest::oneArg);
//
//  Log::debug(u"App::TEST"_s) << "child received: " << value;
//
//  co_return value;
//}
//
//static DetachedTask testAsyncTask(AwaitTest &test)
//{
//  Log::debug(u"App::TEST"_s) << "parent before child";
//
//  const auto value = co_await child(test);
//
//  Log::debug(u"App::TEST"_s) << "parent after child: " << value;
//}


App::App(int &ac, char **av, int flags) : QApplication{ ac, av, flags }
{
  parseArgs();

  loadTranslations();

  setupDBus();

  setupPipewire();

  Log::debug(u"App::TEST"_s) << "Reached end of App::App().";
}










App::~App()
{
  DBus::Bluez::Manager::deinitialize();
  Media::Manager::deinitialize();
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
  Media::Manager::initialize();

  testPipeWire();
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
