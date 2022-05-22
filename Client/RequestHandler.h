#ifndef CLIENT_REQUESTHANDLER_H
#define CLIENT_REQUESTHANDLER_H

#include <iostream>
#include "boost/asio.hpp"
#include <memory>
#include <optional>

#include "Communication.h"
#include "HalperValidators.h"

class User;
class MessageU;

///Responsible for executing requests and deciphering responses and requests.
///Encrypts and decrypts content if necessary.
///Uses a communication class to transmit and receive communication with the server.

class RequestHandler: public std::enable_shared_from_this<RequestHandler>{
private:
    Communication _communication;
    std::shared_ptr<HalperValidators> _validators = std::make_shared<HalperValidators>();
    MessageU* _ptr_msgU;

    bool is_accept_response(uint16_t code);
    Protocol::Response::Registration registration_response_receive(uint32_t payload_size);
    std::list<User> users_list_receive(uint32_t payload_size);
    Protocol::Response::PublicKey public_key_receive(uint32_t payload_size);
    std::list<Message> waiting_message_receive(uint32_t payload_size);
    Protocol::Response::ConfirmSendMessage confirmation_sending_msg_receive(uint32_t payload_size);
    void handle_encrypt_send_message(Message* message);
    std::pair<size_t, std::string> handle_msg_content_received(uint32_t content_size);
    std::string handle_decrypt_received_message(Protocol::Response::WaitingMsgInfo* msg, const std::string& cipher_content);

public:
    boost::asio::ip::tcp::socket _socket;

    explicit RequestHandler(boost::asio::io_context& _io_context);
    std::any send_request(Protocol::Request::Header header, std::optional<std::any> = std::nullopt);
    std::any send_request(MessageU* ptr_msgU, Protocol::Request::Header header, std::optional<std::any> = std::nullopt);
    static std::shared_ptr<RequestHandler> get_ptr_connection(boost::asio::io_context& io_context);

    //A public functions get header and payload to allow users in this class to receive only the part of the response.
    Protocol::Response::Header get_header_response();
    std::any get_payload_response();

    friend class MessageU;
};
#endif //CLIENT_REQUESTHANDLER_H
