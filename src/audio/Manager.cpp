#include "pch.hpp" // IWYU pragma: keep

#include "Manager.hpp"

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
