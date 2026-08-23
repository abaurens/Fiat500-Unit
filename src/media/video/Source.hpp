#pragma once

#include "media/Source.hpp"
#include "pipewire/Node.hpp"

namespace Media::Video
{

  class Source : public Media::Source, public PipeWire::Node
  {
  public:
    Source(uint32_t id, const spa_dict *props = nullptr);
    virtual ~Source();

    virtual QString name() const override { return PipeWire::Node::name(); }

    virtual bool hasMetadata() const override;
    virtual bool canControlPlayback() const override;

    virtual quint32 state() override;
  };

}
