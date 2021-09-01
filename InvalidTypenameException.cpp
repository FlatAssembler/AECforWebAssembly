#include <stdexcept>

#pragma once
class InvalidTypenameException : public std::exception {
  static const char *message;

public:
  const char *what() const override { return message; }
};

const char *InvalidTypenameException::message = "Invalid typename!";
