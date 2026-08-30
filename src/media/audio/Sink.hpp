#pragma once

#include "media/Sink.hpp"

namespace Media::Audio
{

  class Sink : public Media::Sink
  {
  public:
    Sink();
    virtual ~Sink();

    //virtual unsigned channels() const override;

    //virtual float volume() const override;
    //virtual void setVolume(float) override;
  };

}
