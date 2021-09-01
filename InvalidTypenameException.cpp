#include <stdexcept>

#pragma once
class InvalidTypenameException : public std::exception {
  static const char *message;

public:
public
  const char *what() { return message; }
};

const char *InvalidTypenameException::message = "Invalid typename!";
