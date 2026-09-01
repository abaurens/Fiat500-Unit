#pragma once

template<class Promise>
struct AsyncPromiseBase
{
  struct FinalAwaiter
  {
    bool await_ready() const noexcept { return false; }

    void await_resume() const noexcept {}

    std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> handle) const noexcept
    {
      const auto continuation = handle.promise().continuation;

      if (continuation)
        return continuation;

      return std::noop_coroutine();
    }
  };

  std::suspend_never initial_suspend() noexcept { return {}; }
  FinalAwaiter final_suspend() noexcept { return {}; }

  void unhandled_exception() { std::terminate(); }

  std::coroutine_handle<> continuation {};
};

template<class T = void>
class AsyncTask;

template<class T>
class AsyncTask
{
public:
  struct promise_type;
  using Handle = std::coroutine_handle<promise_type>;

  explicit AsyncTask(Handle handle) : m_handle{ handle }
  {}

  AsyncTask(AsyncTask &&other) noexcept : m_handle{ std::exchange(other.m_handle, {}) }
  {}

  AsyncTask(const AsyncTask &) = delete;
  AsyncTask &operator=(const AsyncTask &) = delete;

  ~AsyncTask()
  {
    if (m_handle)
      m_handle.destroy();
  }

  bool await_ready() const noexcept { return m_handle.done(); }
  void await_suspend(std::coroutine_handle<> continuation) noexcept {
    m_handle.promise().continuation = continuation;
  }
  T await_resume() {
    return std::move(*m_handle.promise().m_value);
  }

  struct promise_type : AsyncPromiseBase<promise_type>
  {
    AsyncTask get_return_object() { return AsyncTask{ Handle::from_promise(*this) }; }

    template<class U>
    requires std::constructible_from<T, U&&>
    void return_value(U &&value)
    {
      m_value.emplace(std::forward<U>(value));
    }

    Local<T> m_value;
  };

private:
  Handle m_handle;
};

template<>
class AsyncTask<void>
{
public:
  struct promise_type;
  using Handle = std::coroutine_handle<promise_type>;

  explicit AsyncTask(Handle handle) : m_handle{ handle }
  {}

  AsyncTask(AsyncTask &&other) noexcept : m_handle{ std::exchange(other.m_handle, {}) }
  {}

  AsyncTask(const AsyncTask &) = delete;
  AsyncTask &operator=(const AsyncTask &) = delete;

  ~AsyncTask()
  {
    if (m_handle)
      m_handle.destroy();
  }

  bool await_ready() const noexcept { return m_handle.done(); }
  void await_suspend(std::coroutine_handle<> continuation) noexcept {
    m_handle.promise().continuation = continuation;
  }
  void await_resume() const noexcept  {}

  struct promise_type : AsyncPromiseBase<promise_type>
  {
    AsyncTask get_return_object() { return AsyncTask{ Handle::from_promise(*this) }; }

    void return_void() {}
  };

private:
  Handle m_handle;
};
