#pragma once

namespace PipeWire
{

  class Object;

  class ObjectAwaiter
  {
    using Conn = QMetaObject::Connection;

  public:
    explicit ObjectAwaiter(u32 id);

    bool await_ready() const;

    void await_suspend(std::coroutine_handle<> coroutine);

    Object &await_resume() const;

  private:
    u32  m_id;
    Conn m_connection;
  };

}
