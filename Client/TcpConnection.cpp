#include "TcpConnection.h"
#include "FilesHandler.h"
#include "RequestHandler.h"

TcpConnection::TcpConnection(){
    create_connection();
}

void TcpConnection::create_connection(){
    try {
        std::pair<std::string, unsigned short> address_and_port = FilesHandler::get_port_and_ip(FILE_NAME_FOR_NETWORK_DETAILS);
        _connection = RequestHandler::get_ptr_connection(_io_context);
        boost::asio::ip::tcp::endpoint _end_point(boost::asio::ip::address::from_string(address_and_port.first), address_and_port.second);
        boost::asio::connect(_connection->_socket, &_end_point);
    }
    catch (std::exception& e) {
        throw MessageUException("Failed connection with server.");
    }
}

std::shared_ptr<RequestHandler> TcpConnection::connect() const {
    return _connection;
}