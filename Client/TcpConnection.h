#ifndef CLIENT_TCPCONNECTION_H
#define CLIENT_TCPCONNECTION_H

#include <iostream>
#include "Constants.h"
#include "boost/asio/io_context.hpp"

class RequestHandler;

class TcpConnection{
private:
    void create_connection();

public:
    boost::asio::io_context _io_context;
    std::shared_ptr<RequestHandler> _connection;

    TcpConnection();
    [[nodiscard]] std::shared_ptr<RequestHandler> connect() const;
};
#endif //CLIENT_TCPCONNECTION_H
