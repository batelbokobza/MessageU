#include <string>
#include <iostream>
#include <exception>
#include <utility>

#ifndef CLIENT_MESSAGEUEXCEPTION_H
#define CLIENT_MESSAGEUEXCEPTION_H

class MessageUException : public std::exception {

private:
    const char *_errorMessage;

public:
    explicit MessageUException(const char *error);
    const char *what() const noexcept override;
};

#endif //CLIENT_MESSAGEUEXCEPTION_H
