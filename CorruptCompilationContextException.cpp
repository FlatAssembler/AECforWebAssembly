#include "compilingContext.cpp"
#include <stdexcept>

#pragma once
class CorruptCompilationContextException : public std::exception {
  static const char *message;
  CompilationContext context;
  CorruptCompilationContextException() {}

public:
  CorruptCompilationContextException(CompilationContext context) {
    this->context = context;
  }
  const CompilationContext &getContext() const { return context; }
  const char *what() const noexcept override { return message; }
};

const char *CorruptCompilationContextException::message =
    "Corrupt compilation context!";
