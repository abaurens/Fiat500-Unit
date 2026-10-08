#pragma once

#include "TimeoutError.hpp"

#include <QObject>
#include <QTimer>

template<class ResultType = void>
class TimeoutableTask
{
  using Conn = QMetaObject::Connection;
  template<class Rep, class Period>
  using duration = std::chrono::duration<Rep, Period>;
  using milliseconds = std::chrono::milliseconds;

  struct TaskTimer : public QTimer
  {
    TimeoutableTask *owner = nullptr;
  };

protected:
  virtual void onTimeout() = 0;

public:
  TimeoutableTask() = default;

  TimeoutableTask(TimeoutableTask &&other) noexcept
    : m_armed     { std::exchange(other.m_armed, false) },
      m_timer     { std::move(other.m_timer)            },
      connection  { std::exchange(other.connection, {}) },
      m_triggered { other.m_triggered                   },
      m_timeout   { std::move(other.m_timeout)          },
      m_default   { std::move(other.m_default)          }
  {
    if (m_timer)
      m_timer->owner = this;
  }

  TimeoutableTask(const TimeoutableTask &) = delete;
  TimeoutableTask &operator=(const TimeoutableTask &) = delete;

  virtual ~TimeoutableTask()
  {
    disarmTimeout();
  }

  template<class Rep, class Period>
  TimeoutableTask &timeout(duration<Rep, Period> duration)
  {
    m_timeout.emplace(
      std::chrono::duration_cast<milliseconds>(duration)
    );

    return *this;
  }

  template<class Rep, class Period, class U> requires (
    !std::is_void_v<ResultType> &&
    requires(Local<ResultType> &local, U &&value) { local.emplace(std::forward<U>(value)); }
  )
  TimeoutableTask &timeout(duration<Rep, Period> duration, U &&defaultValue)
  {
    m_default.emplace(std::forward<U>(defaultValue));

    return timeout(duration);
  }

protected:
  bool timedOut() const { return m_triggered; }

  void armTimeout()
  {
    if (isSet() == false)
      return;

    Q_ASSERT(!m_timer);
    Q_ASSERT(isArmed() == false);

    m_timer = makeScope<TaskTimer>();
    m_timer->owner = this;
    auto *timer = m_timer.get();

    m_timer->setSingleShot(true);
    m_timer->setInterval(*m_timeout);
    connection = m_timer->callOnTimeout(
      [timer] { timer->owner->trigger(); }
    );
    m_timer->start();

    m_armed = true;
  }

  void disarmTimeout()
  {
    disarmInternal();
  }

  ResultType timeoutResult()
  {
    if constexpr (!std::is_void_v<ResultType>)
    {
      if (m_default)
        return std::forward<ResultType>(*m_default);
    }

    throw TimeoutError{};
  }

private:
  void trigger()
  {
    m_triggered = true;
    disarmInternal<true>();

    onTimeout();
  }

  template<bool Async = false>
  void disarmInternal()
  {
    QObject::disconnect(connection);
    connection = {};

    m_armed = false;

    if constexpr (Async)
    {
      Q_ASSERT(m_timer);
      m_timer.release()->deleteLater();
    }
    else
    {
      if (m_timer)
      {
        m_timer->stop();
        m_timer.reset();
      }
    }
  }

  bool isSet() const { return static_cast<bool>(m_timeout); }
  bool isArmed() const { return m_armed; /*static_cast<bool>(m_timer);*/ }

private:
  bool m_armed = false;
  Scope<TaskTimer> m_timer = nullptr;
  Conn connection{};

  bool m_triggered = false;
  Local<milliseconds> m_timeout{};
  Local<ResultType> m_default{};
};
