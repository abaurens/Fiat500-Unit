#pragma once

#include "AbstractManager.hpp"
#include "audio/Backend.hpp"

#include <QObject>

namespace Audio
{
  class Manager : public QObject, public ManagerType
  {
    Q_OBJECT

    MANAGER_OBJECT

  public:
    static Backend &backend() { return *instance().m_backend; }

  private:
    std::unique_ptr<Backend> m_backend;
  };

}
