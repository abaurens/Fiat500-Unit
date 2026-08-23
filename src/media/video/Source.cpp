#include "Source.hpp"

namespace Media::Video
{

  Source::Source(uint32_t id, const spa_dict *props)
    : PipeWire::Node{ id, MediaClass::VideoSource, props }
  {
    // Log::debug().nospace().noquote() << "Video Source (" << id << ") created";
  }

  Source::~Source()
  {
    // Log::debug().nospace().noquote() << "Video source (" << id() << ") destroyed";
  }

  bool Source::hasMetadata() const
  {
    return false;
  }

  bool Source::canControlPlayback() const
  {
    return false;
  }

  quint32 Source::state()
  {
    return 0;
  }
}
