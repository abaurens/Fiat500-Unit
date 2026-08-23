#include "pch.hpp" // IWYU pragma: keep

#include "Node.hpp"
#include "media/audio/Sink.hpp"
#include "media/video/Sink.hpp"
#include "media/audio/Source.hpp"
#include "media/video/Source.hpp"

#include <spa/utils/dict.h>

using namespace std::literals;

namespace PipeWire
{
  Scope<Node> Node::Create(uint32_t id, const spa_dict *props)
  {
    const spa_dict_item *item;

    MediaClass nodeClass;

    spa_dict_for_each(item, props)
    {
      if (item->key == "media.class"sv)
      {
        nodeClass = parseMediaClass(item->value);
        break;
      }
    }

    switch (nodeClass)
    {
    case MediaClass::AudioSink:
      return make_scope<Media::Audio::Sink>(id, props);

    case MediaClass::AudioSource:
      return make_scope<Media::Audio::Source>(id, props);

    case MediaClass::VideoSink:
      return make_scope<Media::Video::Sink>(id, props);

    case MediaClass::VideoSource:
      return make_scope<Media::Video::Source>(id, props);

    default:
      return Scope<Node>{ new Node(id, nodeClass, props) };
    }
  }

  Node::Node(uint32_t id, MediaClass mediaClass, std::nullptr_t, const spa_dict *props)
    : Object{ id, StaticType, props }, m_mediaClass{ mediaClass }
  {
    if (mediaClass != MediaClass::MidiBridge)
      return;

    //const spa_dict_item *item;
    //Log::debug(u"Node"_s).noquote() << *this << "created with properties:";
    //spa_dict_for_each(item, props)
    //{
    //  qDebug().nospace().noquote() << "  [" << item->key << "] = \"" << item->value << "\"";
    //}
  }

  Node::Node(uint32_t id, MediaClass mediaClass, const spa_dict *props)
    : Node{ id, mediaClass, nullptr, props }
  {
    auto &&classStr = propertyOr("media.class", "");
    if (classStr.isEmpty())
      return;

    MediaClass &&cls = parseMediaClass(classStr);
    Q_ASSERT(cls == mediaClass);
  }

  //bool Node::isBluetooth() const
  //{
  //  return device()->api() == "bluez5";
  //}

  QString Node::name() const
  {
    return property("node.name");
  }

  QString Node::nick() const
  {
    return propertyOr("node.nick", "");
  }

  QString Node::description() const
  {
    return propertyOr("node.description", "");
  }

  Node::MediaClass Node::mediaClass() const
  {
    return m_mediaClass;
  }

}
