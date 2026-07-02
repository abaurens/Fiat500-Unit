#pragma once

#include <memory>

template<class T>
using Scope = std::unique_ptr<T>;

template<class T, class... Args>
Scope<T> make_scope(Args &&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
