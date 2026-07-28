#pragma once

#include <QString>

namespace Audio
{

  class Source
  {
  public:
    virtual const QString &name() const = 0;
    virtual bool hasMetadata() const = 0;
    virtual bool canControlPlayback() const = 0;

    virtual quint32 state() = 0;
  };

}
