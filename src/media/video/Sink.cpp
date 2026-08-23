#include "Sink.hpp"

namespace Media::Video
{

  Sink::Sink(uint32_t id, const spa_dict *props)
    : PipeWire::Node{ id, MediaClass::VideoSink, props }
  {
    //Log::debug().nospace().noquote() << "Video sink (" << id << ") created";
  }

  Sink::~Sink()
  {
    //Log::debug().nospace().noquote() << "Video sink (" << id() << ") destroyed";
  }

  unsigned Sink::channels() const
  {
    return 2;
  }

  float Sink::volume() const
  {
    return 0.0f;
  }

  void Sink::setVolume(float volume)
  {
    ;
  }
}
