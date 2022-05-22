#include "MessageUException.h"

MessageUException::MessageUException(const char *error) : _errorMessage(error){}
const char *MessageUException::what() const noexcept {
    return _errorMessage;
}