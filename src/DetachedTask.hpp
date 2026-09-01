#pragma once

struct DetachedTask
{
  struct promise_type
  {
    DetachedTask get_return_object() { return {}; }

    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }

    void return_void() {}
    void unhandled_exception() { std::terminate(); }
  };
};
