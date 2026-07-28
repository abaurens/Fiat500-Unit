#pragma once

#include "audio/Backend.hpp"

#include <QObject>

namespace Audio
{
  class Manager : public QObject
  {
    Q_OBJECT

  public:
    static Manager &instance();

    static bool initialize() { return instance().initializeInstance(); }
    static Backend &backend() { return *instance().m_backend; }

  public:
    explicit Manager(QObject *parent = nullptr);

  private:
    bool initializeInstance();

  private:
    std::unique_ptr<Backend> m_backend;
  };

}
