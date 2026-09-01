#include "ObjectAwaiter.hpp"
#include "media/Manager.hpp"

namespace PipeWire
{

  ObjectAwaiter::ObjectAwaiter(u32 id) : m_id{ id }
  {}

  bool ObjectAwaiter::await_ready() const
  {
    return Media::Manager::object(m_id) != nullptr;
  }

  void ObjectAwaiter::await_suspend(std::coroutine_handle<> coroutine)
  {
    m_connection = QObject::connect(
      &Media::Manager::instance(), &Media::Manager::objectAdded,
      &Media::Manager::instance(),
      [this, coroutine](PipeWire::Object &object)
      {
        if (object.id() != m_id)
          return;

        QObject::disconnect(m_connection);
        coroutine.resume();
      }
      );
  }

  PipeWire::Object &ObjectAwaiter::await_resume() const
  {
    return *Media::Manager::object(m_id);
  }

}
