#pragma once

#include "audio/Sink.hpp"
#include "audio/Source.hpp"

namespace Audio
{

  class Backend
  {
  public:
    static std::unique_ptr<Audio::Backend> create();

  public:
    virtual ~Backend() = default;

    virtual void run() = 0;

    //virtual QList<Sink*> sinks() = 0;
    //virtual QList<Source*> sources() = 0;
  };

}
