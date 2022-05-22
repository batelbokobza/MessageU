#ifndef CLIENT_COMMUNICATION_H
#define CLIENT_COMMUNICATION_H

#include "boost/asio.hpp"
#include <iostream>
#include <memory>

class Communication{
private:
    boost::asio::ip::tcp::socket* _socket;

public:
    explicit Communication(boost::asio::ip::tcp::socket* socket);
    void send(char* buffer, size_t size);
    int receive(char* buffer, size_t size);

};

#endif //CLIENT_COMMUNICATION_H
