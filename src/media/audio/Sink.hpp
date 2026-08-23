#pragma once

#include "media/Sink.hpp"
#include "pipewire/Node.hpp"

namespace Media::Audio
{

  class Sink : public Media::Sink, public PipeWire::Node
  {
  public:
    Sink(uint32_t id, const spa_dict *props = nullptr);

    virtual ~Sink();

    virtual QString name() const override { return PipeWire::Node::name(); }

    virtual unsigned channels() const override;

    virtual float volume() const override;
    virtual void setVolume(float) override;
  };

}
