#pragma once

namespace PipeWire
{

  template<class T, auto Deleter>
  class Handle
  {
  public:
    using HandleType = T;

    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;

    Handle(Handle &&) = delete;
    Handle &operator=(Handle &&) = delete;

  protected:
    explicit Handle(T *handle) : m_handle{ handle, Deleter }
    {}

    T *handle() const { return m_handle.get(); }

  private:
    Scope<T, decltype(Deleter)> m_handle;
  };

  template <typename T>
  concept DerivedFromHandle = requires(const T& obj) {
    // Un lambda qui n'accepte QUE Handle<U, V> (et ses dérivés grâce à la conversion implicite)
    []<class U, auto V>(const Handle<U, V>&){ }(obj);
  };
}
