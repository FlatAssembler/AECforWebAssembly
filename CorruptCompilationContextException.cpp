#include <stdexcept>

#pragma once
class CorruptCompilationContextException : public std::exception {
  static const char *message;

public:
  const char *what() const override { return message; }
};

const char *CorruptCompilationContextException::message =
    "Corrupt compilation context!";
