#include "Communication.h"
#include "MessageUException.h"

Communication::Communication(boost::asio::ip::tcp::socket* socket): _socket(socket){}

///\param first - char*
///\param second - size_t
///\note In the event of an error in sending the information, an exception is thrown.
void Communication::send(char* buffer, size_t size){
    try {
        _socket->send(boost::asio::buffer(buffer, size));
    }catch(std::exception &e){
        throw MessageUException("Failed to send request.");
    }
}

///\param first - char*
///\param second - size_t
///\rethrn The size of the content received in bytes
///\note In the event of an error in receiving the information, an exception is thrown.
int Communication::receive(char* buffer, size_t size){
    try {
        size_t bytes_sent = _socket->receive(boost::asio::buffer(buffer, size));
        return bytes_sent;
    }catch(std::exception &e){
        throw MessageUException("Failed to receive response.");
    }
}
