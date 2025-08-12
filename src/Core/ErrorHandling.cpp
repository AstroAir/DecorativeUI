// Core/ErrorHandling.cpp
#include "ErrorHandling.hpp"

namespace DeclarativeUI::Core::ErrorHandling {

// **Thread-local variable definition**
thread_local ErrorContext ErrorContextScope::current_context_;

}  // namespace DeclarativeUI::Core::ErrorHandling
