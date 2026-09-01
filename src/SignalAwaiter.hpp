#pragma once

#include <QObject>

template<class Sender, class... Args>
class SignalAwaiter
{
  using Conn = QMetaObject::Connection;
  using ArgsTuple = std::tuple<Args...>;

public:
  using Signal = void (Sender::*)(Args...);

  SignalAwaiter(const Sender *sender, Signal signal)
    : m_sender{ sender } , m_signal{ signal }
  {}

  bool await_ready() const noexcept { return false; }

  void await_suspend(std::coroutine_handle<> coroutine)
  {
    m_connection = QObject::connect(
      m_sender, m_signal,
      m_sender,
      [this, coroutine](Args... args)
      {
        m_args.emplace(std::forward<Args>(args)...);

        QObject::disconnect(m_connection);

        coroutine.resume();
      }
    );
  }

  decltype(auto) await_resume()
  {
    if constexpr (sizeof...(Args) == 1)
      return std::get<0>(std::move(*m_args));
    else if constexpr (sizeof...(Args) > 1)
      return std::move(*m_args);
    else
      return;
  }

private:
  const Sender *m_sender;
  Signal m_signal;

  Conn m_connection;
  Local<ArgsTuple> m_args;
};

template<class Sender, class... Args>
auto waitForSignal(const Sender *sender, void (Sender::*signal)(Args...))
{
  return SignalAwaiter<Sender, Args...>{ sender, signal };
}


struct AwaitTest : public QObject
{
  Q_OBJECT

signals:
  void noArgs();
  void oneArg(u32 value);
  void multipleArgs(u32 value, const QString &text);
};
