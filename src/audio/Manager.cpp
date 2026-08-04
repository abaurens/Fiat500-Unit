#include "Manager.hpp"
#include "Log.hpp"

namespace Audio
{
  Manager::Manager(QObject *owner) : ManagerType { owner }
  {}

  bool Manager::initializeInstance()
  {
    Log::debug(u"Manager"_s) << "Initializing";
    m_backend = Backend::create();

    return !!m_backend;
  }

  void Manager::deinitializeInstance()
  {
    Log::debug(u"Manager"_s) << "Deinitializing";
    m_backend.reset();
  }
}
