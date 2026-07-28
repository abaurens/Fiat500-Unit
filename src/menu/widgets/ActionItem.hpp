#pragma once

#include "menu/widgets/ButtonItem.hpp"

#include <QWidget>
#include <concepts>

class ActionItem : public ButtonItem
{
  Q_OBJECT

private:
  template<class T, class Ret, class... Args>
  using Slot = Ret (T::*)(Args...);

public:
  virtual ~ActionItem() = default;

  explicit ActionItem(const QString &text, Menu::Page *parent)
    : ButtonItem{ text, parent }
  {}

  template<class T, class Ret, class... Args>
  explicit ActionItem(const QString &text, T *caller, const Slot<T, Ret, Args...> slot, Menu::Page *parent)
    : ButtonItem{ text, parent }
  {
    setAction<T, Ret, Args...>(caller, slot);
  }

  template<std::invocable<> Act>
    requires std::is_same_v<std::invoke_result_t<Act>, void>
  explicit ActionItem(const QString &text, Act && act, Menu::Page *parent)
    : ButtonItem{ text, parent }
  {
    setAction<Act>(std::forward<Act>(act));
  }


  template<void (*Act)()>
  explicit ActionItem(const QString &text, Menu::Page *parent)
    : ButtonItem{ text, parent }
  {
    setAction<Act>();
  }

  // Setters
  template<class T, class Ret, class... Args>
  void setAction(T *caller, const Slot<T, Ret, Args...> slot)
  {
    unsetAction();
    connect(
      m_button, &QPushButton::clicked,
      caller,   slot
    );
  }

  template<std::invocable<> Act>
    requires std::is_same_v<std::invoke_result_t<Act>, void>
  void setAction(Act && act)
  {
    unsetAction();
    connect(m_button, &QPushButton::clicked, [act]() -> void { return act(); });
  }

  template<void (*Act)()>
  void setAction()
  {
    unsetAction();
    connect(m_button, &QPushButton::clicked, Act);
  }

private:
  void unsetAction();
};
