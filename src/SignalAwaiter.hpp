#pragma once

#include <AbstractAwaiter.hpp>

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
class SignalAwaiter : public AbstractAwaiter<typename SignalResult<Args...>::Type>
{
  using ArgsTuple = std::tuple<Args...>;

  static constexpr size_t resultCount = sizeof...(Args);
  using ResultType = typename SignalResult<Args...>::Type;
  using Super = AbstractAwaiter<ResultType>;

public:
  using Signal = void (Sender::*)(Args...);

  SignalAwaiter(Sender *sender, Signal signal, Predicate predicate = {})
    : m_sender{ sender }, m_signal{ signal }, m_predicate{ std::move(predicate) }
  {}

  virtual ~SignalAwaiter() = default;

  virtual void arm() override
  {
    this->connect(
      m_sender, m_signal,
      [this](Args... args)
      {
        if (!m_predicate(args...))
          return;

        m_args.emplace(std::forward<Args>(args)...);
        this->resume();
      }
    );
  }

  virtual ResultType result() override
  {
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
    Super::timeout(duration);
    return *this;
  }

  template<class Rep, class Period, class U> requires (
    !std::is_void_v<ResultType> &&
    requires(Super &super, std::chrono::duration<Rep, Period> duration, U &&value) {
      super.timeout(duration, std::forward<U>(value));
    }
  )
  SignalAwaiter &timeout(std::chrono::duration<Rep, Period> duration, U &&defaultValue)
  {
    Super::timeout(duration, std::forward<U>(defaultValue));
    return *this;
  }

private:
  // User data
  Sender *m_sender;
  Signal m_signal;
  Predicate m_predicate;

  // Coroutine Metadata
  Local<ArgsTuple> m_args;
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
