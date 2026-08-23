#pragma once

namespace Media
{

  class Sink
  {
  public:

    virtual ~Sink() = default;

    virtual QString name() const = 0;
    virtual unsigned channels() const = 0;

    virtual float volume() const = 0;
    virtual void setVolume(float) = 0;
  };

}
