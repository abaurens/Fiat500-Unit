#include "Sink.hpp"

#include <spa/utils/dict.h>

namespace Media::Audio
{

  Sink::Sink(uint32_t id, const spa_dict *props)
    : PipeWire::Node{ id, MediaClass::AudioSink, props }
  {
    Log::debug().nospace().noquote() << "Audio sink (" << id << ") created with properties:";

    const spa_dict_item *item;
    spa_dict_for_each(item, props)
    {
      qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    }
  }

  Sink::~Sink()
  {
    Log::debug().nospace().noquote() << "Audio sink (" << id() << ") destroyed";
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
