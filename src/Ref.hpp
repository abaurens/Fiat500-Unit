#pragma once

#include <memory>

template<class T>
using Ref = std::shared_ptr<T>;

template<class T, class... Args>
Ref<T> make_ref(Args &&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }
