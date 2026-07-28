#pragma once

#include <QString>

namespace Audio
{

  class Sink
  {
  public:
    virtual const QString &name() const = 0;
    virtual unsigned channels() const = 0;

    virtual float volume() const = 0;
    virtual void setVolume(float) = 0;
  };

}
