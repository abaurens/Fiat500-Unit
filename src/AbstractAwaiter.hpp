#pragma once

#include "TimeoutableTask.hpp"
#include <QObject>

template<class ResultType>
class AbstractAwaiter : public TimeoutableTask<ResultType>
{
  using Super = TimeoutableTask<ResultType>;

  using Conn = QMetaObject::Connection;
  template<class Rep, class Period>
  using duration = std::chrono::duration<Rep, Period>;

protected:
  virtual void arm() = 0;
  virtual void disarm() {};
  virtual ResultType result() = 0;
  virtual bool ready() const noexcept { return false; }

public:
  virtual ~AbstractAwaiter()
  {
    disarmInternal();
  }

  bool await_ready() const noexcept { return ready(); }

  void await_suspend(std::coroutine_handle<> coroutine)
  {
    m_coroutine = coroutine;
    arm();

    Super::armTimeout();
  }

  ResultType await_resume()
  {
    if (Super::timedOut())
      return Super::timeoutResult();

    return result();
  }

  void cancel() {} // reserved for possible later use

  template<class Rep, class Period>
  AbstractAwaiter &timeout(duration<Rep, Period> duration)
  {
    Super::timeout(duration);
    return *this;
  }

  template<class Rep, class Period, class U> requires (
    !std::is_void_v<ResultType> &&
    requires(Super &super, duration<Rep, Period> duration, U &&value) {
      super.timeout(duration, std::forward<U>(value));
    }
  )
  AbstractAwaiter &timeout(duration<Rep, Period> duration, U &&defaultValue)
  {
    Super::timeout(duration, std::forward<U>(defaultValue));
    return *this;
  }

protected:
  template<class... Args>
  QMetaObject::Connection connect(Args &&...args)
  {
    auto connection = QObject::connect(
      std::forward<Args>(args)...
    );

    m_connections.emplace_back(connection);
    return connection;
  }

  void resume()
  {
    auto coroutine = std::exchange(m_coroutine, {});

    if (!coroutine)
      return;

    disarmAll();
    coroutine.resume();
  }

private:
  void disarmAll()
  {
    disarmInternal();
    disarm();
  }

  void disarmInternal()
  {
    Super::disarmTimeout();

    for (const auto &connection : m_connections)
      QObject::disconnect(connection);
    m_connections.clear();
  }

  virtual void onTimeout() override
  {
    this->resume();
  }

private:
  std::coroutine_handle<> m_coroutine;
  std::vector<Conn> m_connections;
};
