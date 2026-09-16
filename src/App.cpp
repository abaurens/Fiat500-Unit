#include "pch.hpp" // IWYU pragma: keep

#include "App.hpp"
#include "MainWindow.hpp"

#include "pipewire/Manager.hpp"
#include "pipewire/Direction.hpp"

#include "dbus/bluez/Manager.hpp"
#include "dbus/Types.hpp"

#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>

#include "DetachedTask.hpp"  // IWYU pragma: keep

#include "pipewire/link.h"
#include "pipewire/keys.h" // IWYU pragma: keep
#include "pipewire/handles/Filter.hpp"

#include "MainWindow.hpp"

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

struct LinkHandle
{
  pw_core *m_core;
  pw_proxy *m_proxy;
};

static LinkHandle createLink(pw_core *core, u32 outNode, u32 outPort, u32 inNode, u32 inPort)
{
  auto *properties = pw_properties_new(
    PW_KEY_LINK_OUTPUT_NODE, std::to_string(outNode).c_str(),
    PW_KEY_LINK_OUTPUT_PORT, std::to_string(outPort).c_str(),
    PW_KEY_LINK_INPUT_NODE,  std::to_string(inNode).c_str(),
    PW_KEY_LINK_INPUT_PORT,  std::to_string(inPort).c_str(),
    nullptr
  );

  auto *proxy = static_cast<pw_proxy *>(
    pw_core_create_object(
      core,
      "link-factory",
      PW_TYPE_INTERFACE_Link,
      PW_VERSION_LINK,
      &properties->dict,
      0
    )
  );

  pw_properties_free(properties);

  return {
    .m_core = core,
    .m_proxy = proxy
  };
}

// Testing creation of custom PipeWire node
// and it's connection into the existing graph.
static DetachedTask testPipeWire()
{
  using namespace std::chrono_literals;

  static std::vector<LinkHandle> links;
  PipeWire::Node *bluetoothNode = nullptr;
  PipeWire::Node *dacNode = nullptr;

  // Get DAC node
  {
    auto deviceFilter = [](PipeWire::Device &device) {
      if (device.api() != "alsa")
        return false;

      if (device.tryProperty("device.nick") != "ICUSBAUDIO7D")
        return false;

      return true;
    };

    PipeWire::Device &dacDevice = co_await PipeWire::Manager::waitFor<PipeWire::Device>(deviceFilter);

    Log::debug(u"App::TEST"_s) << "DAC device: " << dacDevice;

    for (auto *node : PipeWire::Manager::nodesForDevice(dacDevice.id()))
    {
      if (node->tryProperty("audio.channels") != "4")
        continue;

      dacNode = node;
      break;
    }

    if (dacNode)
    {
      Log::debug(u"App::TEST"_s)
        << "DAC node: "
        << dacNode->name()
        << " (" << dacNode->id() << ')';
    }
    else
    {
      Log::debug(u"App::TEST"_s) << "DAC node: nullptr";
    }
  }

  // Get bluetooth node
  {
    auto deviceFilter = [](PipeWire::Device &device) {
      if (device.api() == "bluez5")
        return false;

      if (device.bluetoothAddress() != "78:64:C0:27:83:A9")
        return false;

      return true;
    };

    //co_await waitForSignal<QMainWindow>(&App::instance().window(), &QMainWindow::iconSizeChanged).timeout(0s);

    PipeWire::Device &bluetoothDevice = co_await PipeWire::Manager::waitFor<PipeWire::Device>(deviceFilter);

    Log::debug(u"App::TEST"_s) << "Bluetooth device: " << bluetoothDevice;

    for (auto *node : PipeWire::Manager::nodesForDevice(bluetoothDevice.id()))
    {
      if (node->tryProperty("api.bluez5.profile") != "a2dp-source")
        continue;

      if (node->tryProperty("media.class") != "Stream/Output/Audio")
        continue;

      if (node->mediaClass() != PipeWire::MediaClass::StreamOutputAudio)
        continue;

      bluetoothNode = node;
      break;
    }

    Log::debug(u"App::TEST"_s)
      << "Bluetooth node: "
      << bluetoothNode->name()
      << " (" << bluetoothNode->id() << ')';
  }

  // Create the filter
  {
    Log::debug(u"App::TEST"_s) << "Creating new filter";
    filter = PipeWire::Manager::context().createFilter("BAA_Test_Filter");
  }

  filter->connectInput(bluetoothNode, 2);
  filter->connectOutput(dacNode, 4);

  // Create input ports
  //{
  //  Log::debug(u"App::TEST"_s) << "Adding INPUT port";
  //  filter->addPort(
  //    SPA_DIRECTION_INPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Input_R", nullptr)
  //  );
  //  filter->addPort(
  //    SPA_DIRECTION_INPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Input_L", nullptr)
  //  );
  //}

  // Create output ports
  //{
  //  Log::debug(u"App::TEST"_s) << "Adding OUTPUT port";
  //  filter->addPort(
  //    SPA_DIRECTION_OUTPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Output_FR", nullptr)
  //  );
  //  filter->addPort(
  //    SPA_DIRECTION_OUTPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Output_FL", nullptr)
  //  );
  //  filter->addPort(
  //    SPA_DIRECTION_OUTPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Output_RR", nullptr)
  //  );
  //  filter->addPort(
  //    SPA_DIRECTION_OUTPUT,
  //    pw_properties_new(PW_KEY_PORT_NAME, "BAA_Filter_Output_RL", nullptr)
  //  );
  //}

  // Activate the filter
  u32 nodeId = 0;
  PipeWire::Node *node = nullptr;
  {
    Log::debug(u"App::TEST"_s) << "Connecting filter";
    const bool result = co_await filter->connect();
    Log::debug(u"App::TEST"_s) << "filter connection: " << result;

    Log::debug(u"App::TEST"_s) << "Awaiting nodeId...";
    nodeId = co_await filter->waitForNodeId();
    Log::debug(u"App::TEST"_s) << "Filter nodeId available: " << nodeId;

    node = &co_await PipeWire::Manager::waitFor<PipeWire::Node>(nodeId);
    if (!node)
      co_return;

    Log::debug(u"App::TEST"_s) << "Filter node is available: " << *node;
  }


  PipeWire::Port *inL = nullptr;
  PipeWire::Port *inR = nullptr;

  PipeWire::Port *outFL = nullptr;
  PipeWire::Port *outFR = nullptr;
  PipeWire::Port *outRL = nullptr;
  PipeWire::Port *outRR = nullptr;

  // Get created I/O ports for the filter
  {
    for (auto *port : PipeWire::Manager::portsForNode(nodeId))
    {
      switch (port->direction())
      {
      case PipeWire::Direction::In:
        if (port->name() == "BAA_Filter_Input_L")
          inL = port;
        else if (port->name() == "BAA_Filter_Input_R")
          inR = port;
        break;

      case PipeWire::Direction::Out:
        if (port->name() == "BAA_Filter_Output_FL")
          outFL = port;
        else if (port->name() == "BAA_Filter_Output_FR")
          outFR = port;
        else if (port->name() == "BAA_Filter_Output_RL")
          outRL = port;
        else if (port->name() == "BAA_Filter_Output_RR")
          outRR = port;
        break;

      default:
        break;
      }
    }

    while (!inL || !inR || !outFL || !outFR || !outRL || !outRR)
    {
      auto &port = co_await waitForSignal(
        &PipeWire::Manager::instance(),
        &PipeWire::Manager::portAdded
      );

      if (port.nodeId() != nodeId)
        continue;

      switch (port.direction())
      {
      case PipeWire::Direction::In:
        if (port.name() == "BAA_Filter_Input_L")
          inL = &port;
        else if (port.name() == "BAA_Filter_Input_R")
          inR = &port;
        break;

      case PipeWire::Direction::Out:
        if (port.name() == "BAA_Filter_Output_FL")
          outFL = &port;
        else if (port.name() == "BAA_Filter_Output_FR")
          outFR = &port;
        else if (port.name() == "BAA_Filter_Output_RL")
          outRL = &port;
        else if (port.name() == "BAA_Filter_Output_RR")
          outRR = &port;
        break;

      default:
        break;
      }
    }
  }


  PipeWire::Port *btFL = nullptr;
  PipeWire::Port *btFR = nullptr;
  for (auto *port : PipeWire::Manager::portsForNode(bluetoothNode->id()))
  {
    if (port->name() == "output_FL")
    {
      btFL = port;
      continue;
    }
    if (port->name() == "output_FR")
    {
      btFR = port;
      continue;
    }
  }

  PipeWire::Port *dacFL = nullptr;
  PipeWire::Port *dacFR = nullptr;
  PipeWire::Port *dacRL = nullptr;
  PipeWire::Port *dacRR = nullptr;

  for (auto *port : PipeWire::Manager::portsForNode(dacNode->id()))
  {
    Log::debug(u"App::TEST"_s) << "  Port: " << port->name();

    if (port->name() == "playback_FL")
      dacFL = port;
    else if (port->name() == "playback_FR")
      dacFR = port;
    else if (port->name() == "playback_RL")
      dacRL = port;
    else if (port->name() == "playback_RR")
      dacRR = port;
  }

  while (!dacFL || !dacFR || !dacRL || !dacRR)
  {
    auto &port = co_await waitForSignal(
      &PipeWire::Manager::instance(),
      &PipeWire::Manager::portAdded
    );

    if (port.nodeId() != nodeId)
      continue;

    switch (port.direction())
    {
    case PipeWire::Direction::In:
      if (port.name() == "input_FL")
        dacFL = &port;
      else if (port.name() == "input_FR")
        dacFR = &port;
      else if (port.name() == "input_RL")
        dacRL = &port;
      else if (port.name() == "input_RR")
        dacRR = &port;

    default:
      break;
    }
  }

  if (!bluetoothNode || !dacNode)
    co_return;

  if (!btFL || !btFR)
    co_return;

  if (!dacFL || !dacFR || !dacRL || !dacRR)
    co_return;

  Log::debug(u"App::TEST"_s) << "Creating input links";
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    bluetoothNode->id(), btFL->id(),
    nodeId,              inL->id()
  ));
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    bluetoothNode->id(), btFR->id(),
    nodeId,              inR->id()
  ));

  Log::debug(u"App::TEST"_s) << "Creating output links";
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    nodeId,        outFR->id(),
    dacNode->id(), dacFR->id()
  ));
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    nodeId,        outFL->id(),
    dacNode->id(), dacFL->id()
  ));
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    nodeId,        outRR->id(),
    dacNode->id(), dacRR->id()
  ));
  links.emplace_back(createLink(
    PipeWire::Manager::context().core(),
    nodeId,        outRL->id(),
    dacNode->id(), dacRL->id()
  ));

  Log::debug(u"App::TEST"_s) << "Reached end of test";
}

App::App(int &ac, char **av, int flags) : QApplication{ ac, av, flags }
{
  parseArgs();

  loadTranslations();

  setupDBus();

  setupPipewire();

  testPipeWire();
  Log::debug(u"App::TEST"_s) << "Reached end of App::App().";
}










App::~App()
{
  DBus::Bluez::Manager::deinitialize();
  PipeWire::Manager::deinitialize();
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
  PipeWire::Manager::initialize();
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
