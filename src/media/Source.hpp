#pragma once

#include <QString>

namespace Media
{

  class Source
  {
  public:
    virtual ~Source() = default;

    virtual QString name() const = 0;
    virtual bool hasMetadata() const = 0;
    virtual bool canControlPlayback() const = 0;

    virtual quint32 state() = 0;
  };

}
