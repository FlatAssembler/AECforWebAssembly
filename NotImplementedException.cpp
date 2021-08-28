#include <stdexcept>

#pragma once
class NotImplementedException final : public std::runtime_error {
public:
  NotImplementedException(const std::string& what) noexcept : std::runtime_error(what) {}
};
