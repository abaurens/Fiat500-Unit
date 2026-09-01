#pragma once

#include <memory>

template<class T, class Deleter = std::default_delete<T>>
using Scope = std::unique_ptr<T, Deleter>;

template<class T, class... Args>
Scope<T> makeScope(Args &&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
