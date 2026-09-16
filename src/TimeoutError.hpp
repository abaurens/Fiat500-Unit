#pragma once

#include <stdexcept>

class TimeoutError : public std::runtime_error
{
public:
  TimeoutError() : std::runtime_error{ "Operation timed out" }
  {}
};
