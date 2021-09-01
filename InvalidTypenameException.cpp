#include <stdexcept>

#pragma once
class InvalidTypenameException : public std::exception {
  static const char *message;

public:
  const char *what() const noexcept override { return message; }
};

const char *InvalidTypenameException::message = "Invalid typename!";
