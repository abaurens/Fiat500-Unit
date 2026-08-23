#include "Source.hpp"

#include <spa/utils/dict.h>

namespace Media::Audio
{

  Source::Source(uint32_t id, const spa_dict *props)
    : PipeWire::Node{ id, MediaClass::AudioSource, props }
  {
    Log::debug().nospace().noquote() << "Audio Source (" << id << ") created with properties:";

    const spa_dict_item *item;
    spa_dict_for_each(item, props)
    {
      qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    }
  }

  Source::~Source()
  {
    Log::debug().nospace().noquote() << "Audio source (" << id() << ") destroyed";
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
