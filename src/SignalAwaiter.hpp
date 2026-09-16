#pragma once

#include <QObject>
#include <QTimer>

#include "TimeoutError.hpp"

template<class... Args>
struct SignalResult
{ using Type = std::tuple<Args...>; };

template<class T>
struct SignalResult<T>
{ using Type = T; };

template<>
struct SignalResult<>
{ using Type = void; };

template<class Sender, class Predicate, class... Args>
class SignalAwaiter
{
  using Conn = QMetaObject::Connection;
  using ArgsTuple = std::tuple<Args...>;

  static constexpr size_t resultCount = sizeof...(Args);

  template<size_t I>
  using ArgType = std::tuple_element_t<I, ArgsTuple>;
  using ResultTypes = std::tuple<void, ArgType<0>, ArgsTuple>;
  using ResultType = typename SignalResult<Args...>::Type;

public:
  using Signal = void (Sender::*)(Args...);

  SignalAwaiter(Sender *sender, Signal signal, Predicate predicate = {})
    : m_sender{ sender }, m_signal{ signal }, m_predicate{ std::move(predicate) }
  {}

  ~SignalAwaiter()
  {
    cancel();
  }

  void cancel()
  {
    // disarm the timeout
    if (m_timeoutCon)
      QObject::disconnect(m_timeoutCon);

    // disconnect the target signal
    if (m_targetCon)
      QObject::disconnect(m_targetCon);

    m_timer.stop();
  }

  bool await_ready() const noexcept { return false; }

  void await_suspend(std::coroutine_handle<> coroutine)
  {
    m_targetCon = QObject::connect(
      m_sender, m_signal,
      [this, coroutine](Args... args)
      {
        if (!m_predicate(args...))
          return;

        m_args.emplace(std::forward<Args>(args)...);
        cancel();
        coroutine.resume();
      }
    );

    if (m_timeout)
    {
      m_timer.setSingleShot(true);
      m_timer.setInterval(*m_timeout);

      m_timeoutCon = QObject::connect(
        &m_timer, &QTimer::timeout,
        [this, coroutine]
        {
          m_timedOut = true;

          cancel();
          coroutine.resume();
        }
      );

      m_timer.start();
    }
  }

  ResultType await_resume()
  {
    if (m_timedOut)
    {
      if constexpr (!std::is_void_v<ResultType>)
      {
        if (m_default)
          return std::forward<ResultType>(*m_default);
      }
      throw TimeoutError{};
    }

    if constexpr (resultCount == 1)
      return std::get<0>(std::move(*m_args));
    else if constexpr (resultCount > 1)
      return std::move(*m_args);
    else
      return;
  }

  template<class Rep, class Period>
  SignalAwaiter &timeout(std::chrono::duration<Rep, Period> duration)
  {
    m_timeout.emplace(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration)
    );

    return *this;
  }

  template<class Rep, class Period, class U> requires (
    !std::is_void_v<ResultType> &&
    requires(Local<ResultType> &local, U &&value) { local.emplace(std::forward<U>(value)); }
  )
  SignalAwaiter &timeout(std::chrono::duration<Rep, Period> duration, U &&defaultValue)
  {
    m_default.emplace(std::forward<U>(defaultValue));

    return timeout(duration);
  }

private:
  // User data
  Sender *m_sender;
  Signal m_signal;
  Predicate m_predicate;

  // Coroutine Metadata
  Conn m_targetCon;
  Conn m_timeoutCon;
  Local<ArgsTuple> m_args;

  // Timeout system
  Local<std::chrono::milliseconds> m_timeout;
  bool m_timedOut = false;
  QTimer m_timer;
  Local<ResultType> m_default; // Default value to be returned on timeout
};

template<class Sender, class... Args>
auto waitForSignal(Sender *sender, void (Sender::*signal)(Args...))
{
  struct AlwaysTrue
  {
    constexpr bool operator()(Args&...) const noexcept
    {
      return true;
    }
  };

  return SignalAwaiter<Sender, AlwaysTrue, Args...>{ sender, signal };
}

template<class Sender, class... Args, std::predicate<Args&...> Predicate>
auto waitForSignal(Sender *sender, void (Sender::*signal)(Args...), Predicate &&predicate)
{
  using Pred = std::decay_t<Predicate>;

  return SignalAwaiter<Sender, Pred, Args...>{ sender, signal, std::forward<Predicate>(predicate) };
}

//struct AwaitTest : public QObject
//{
//  Q_OBJECT
//
//signals:
//  void noArgs();
//  void oneArg(u32 value);
//  void multipleArgs(u32 value, const QString &text);
//};
