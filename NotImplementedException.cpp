#include <stdexcept>

#pragma once
class NotImplementedException : public std::runtime_error {
public:
  NotImplementedException(std::string what) : std::runtime_error(what) {}
};