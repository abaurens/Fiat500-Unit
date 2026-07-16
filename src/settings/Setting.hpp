#pragma once

#include <QObject>

#include <concepts>
#include <utility>

namespace Settings
{
  class ValueWatcher : public QObject
  {
    Q_OBJECT

  signals:
    void valueChanged();
  };

  template<class T>
  class Value : public ValueWatcher
  {
  public:
    template<std::convertible_to<T> G>
    Value(G &&value) : m_value{ std::forward<G>(value) }
    {}

    T &value() { return m_value; }
    const T &value() const { return m_value; }

    operator T&() { return value(); }
    operator const T&() const { return value(); }

    template<std::convertible_to<T> G>
    void set(G &&value)
    {
      m_value = std::forward<G>(value);
      emit valueChanged();
    }

    template<std::convertible_to<T> G>
    T &operator=(G &&value)
    {
      set(std::forward<G>(value));
      return m_value;
    }

    template<class S>
    friend S &operator<<(S &os, const Value &value)
    {
      os << value.m_value;
      return os;
    }

  private:
    T m_value;
  };
}
