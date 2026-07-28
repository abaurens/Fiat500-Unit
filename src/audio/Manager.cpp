#include "Manager.hpp"

namespace Audio
{
  Manager &Manager::instance()
  {
    static Manager g_manager;

    return g_manager;
  }

  Manager::Manager(QObject *parent) : QObject{ parent }
  {}

  bool Manager::initializeInstance()
  {
    m_backend = Backend::create();

    return !!m_backend;
  }
}
