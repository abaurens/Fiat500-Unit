#pragma once

#include "media/Source.hpp"

namespace Media::Video
{

  class Source : public Media::Source
  {
  public:
    Source();
    virtual ~Source();

    //virtual bool hasMetadata() const override;
    //virtual bool canControlPlayback() const override;

    //virtual quint32 state() override;
  };

}
