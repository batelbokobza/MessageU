#include "RequestHandler.h"
#include "MessageU.h"
#include "Protocol.h"
#include <utility>
#include "FilesHandler.h"
#include "User.h"
#include "Message.h"
#include "RSA.h"
#include "AES.h"
#include "boost/filesystem.hpp"
#include <filesystem>

using namespace Protocol;

RequestHandler::RequestHandler(boost::asio::io_context& _io_context) : 
    _socket(_io_context), _communication(&_socket), _ptr_msgU(nullptr){}

///\param *_io_context
///\return A smart pointer of the class.
std::shared_ptr<RequestHandler> RequestHandler::get_ptr_connection(boost::asio::io_context& _io_context) {
    std::shared_ptr<RequestHandler> shared_ptr;
    try{
        shared_ptr = std::make_shared<RequestHandler>(_io_context);
    }catch(std::exception &e){
        throw MessageUException("Connection failed.");
    }
    return shared_ptr;
}

///\brief  Used to send non-encryption requests from users to the server.
///\return Server response.
///\param first - Request::Header
///\param second - Optional payload of the type std::any
///\note  - To get the object returning from the function properly, you must use any_cast of the type you are expecting.
/// Otherwise, an unexpected response is possible.
std::any RequestHandler::send_request(Request::Header header, std::optional<std::any> payload) {
    _communication.send((char*)&header, sizeof(header));
    if (payload.has_value()) {
        if(payload->type() == typeid(Request::Registration)) {
            auto registration = std::any_cast<Request::Registration>(&payload.value());
            _communication.send((char*)registration, sizeof(Request::Registration));
        }
        else if(payload->type() == typeid(Request::PublicKey)) {
            auto id_to_public_key = std::any_cast<Request::PublicKey>(&payload.value());
            _communication.send((char*)id_to_public_key, sizeof(Request::PublicKey));
        }
    }
    return get_payload_response();
}

//Overloading function.
///\brief  Used to send request of the type of messages, of users to the server.
///\return Server response.
///\param first - Pointer of the MessageU class - for the purpose of encrypting and verifying certain requests.
///\param second - Request::Header
///\param third - Optional payload of the type std::any
///\note  - Without the use of a pointer parameter of the MessageU class an unexpected response is possible.
///\note  - To get the object returning from the function properly, you must use any_cast of the type you are expecting.
/// Otherwise, an unexpected response is possible.
std::any RequestHandler::send_request(MessageU* msgU, Request::Header header, std::optional<std::any> payload) {
    _ptr_msgU = msgU;   //If the request is to send a message / receive pending messages, the pointer will not be NULL.
    _communication.send((char*)&header, sizeof(header));
    if (payload.has_value()) {  //True if send message
        if (payload->type() == typeid(Message*)) {
            auto message = std::any_cast<Message*>(&payload.value());
            handle_encrypt_send_message(*message);
        }
    }
    return get_payload_response();
}

void RequestHandler::handle_encrypt_send_message(Message* message) {
    std::string cipher_content, key;
    Request::SendMessage msg_to_send{ message->get_target_id(), message->get_type() };
    if (message->get_type() != MessageType::SYMMETRIC_KEY_REQ) {
        if (message->get_type() == MessageType::SYMMETRIC_KEY_SEND) {
            key = _ptr_msgU->get_public_key_by_id(message->get_target_id());
            cipher_content = RSA::encrypt(message->get_content(), key);
        }
        else {
            //type content text of text file
            key = _ptr_msgU->get_symmetric_key_by_id(message->get_target_id());
            cipher_content = AES::encrypt(message->get_content(), key);
        }
        memcpy(msg_to_send.content, cipher_content.c_str(), cipher_content.length());
        msg_to_send.size = cipher_content.length();
    }
    _communication.send((char*)&msg_to_send, sizeof(msg_to_send));
}


///\brief  Receives and decodes the server response to the request.
///\return An object of any type that contains the server response.
///\note  To get the object returning from the function properly, you must use any_cast of the type you are expecting.
/// Otherwise, an unexpected response is possible.
std::any RequestHandler::get_payload_response(){
    Response::Header header = get_header_response();
    uint32_t payload_size = header.payload_size;
    std::any response;
    if(is_accept_response(static_cast<uint16_t>(header.code))){
        if(header.code == Response::Code::REGISTRATION_COMPLETED_SUCCESSFULLY){
            response = registration_response_receive(payload_size);
        }else if(header.code == Response::Code::USERS_LIST){
            response = users_list_receive(payload_size);
        }else if(header.code == Response::Code::PUBLIC_KEY){
            response = public_key_receive(payload_size);
        }else if(header.code == Response::Code::MESSAGE_TO_USER){
            response = confirmation_sending_msg_receive(payload_size);
        }else if(header.code == Response::Code::WAITING_MESSAGE){
            response = waiting_message_receive(payload_size);
        }
    }
    return response;
}

///\brief  Checks if the number obtained is valid, and if the response is a server error.
///\param code number to check
///\return bool
bool RequestHandler::is_accept_response(uint16_t code){
    //If the response number received from the server is unknown, we will not continue receiving the response.
    if(!_validators->is_valid_code_response(code)){
        throw MessageUException("Failed to receive response.(unexpected response received)");
    }else if(static_cast<Response::Code>(code) == Response::Code::GENERAL_ERROR){
            throw MessageUException("Server responded with an error.");
    }
    return true;
}

Response::Header RequestHandler::get_header_response(){
    Response::Header header{};
    _communication.receive((char*)&header, sizeof(header));
    return header;
}

Response::Registration RequestHandler::registration_response_receive(uint32_t payload_size){
    unsigned char uid_from_server[ID_LENGTH] = { 0 };
    _communication.receive((char*)uid_from_server, ID_LENGTH);
    Response::Registration res_registration(uid_from_server);
    return res_registration;
}

std::list<User> RequestHandler::users_list_receive(uint32_t payload_size){
    size_t size = payload_size, received;
    std::list<User> users_list;
    while(size > 0){
        Response::UserInfo user;
        received = _communication.receive((char*)&user, sizeof(user));
        if (received > 0) {
            users_list.emplace_back(User(std::string(user.name, user.name + MAX_NAME_LENGTH),
                std::string(user.id, user.id + ID_LENGTH)));
        }size -= received;
    }
    return users_list;
}

Response::PublicKey RequestHandler::public_key_receive(uint32_t payload_size){
    unsigned char id[ID_LENGTH] = { 0 };
    unsigned char pk[NUMBER_BYTES_TO_PUBLIC_KEY] = { 0 };
    _communication.receive((char*)id, ID_LENGTH);
    _communication.receive((char*)pk, NUMBER_BYTES_TO_PUBLIC_KEY);
    Response::PublicKey payload(id, pk);
    return payload;
}

Response::ConfirmSendMessage RequestHandler::confirmation_sending_msg_receive(uint32_t payload_size){
    Response::ConfirmSendMessage confirmation;
    _communication.receive((char*)&confirmation, sizeof(Response::ConfirmSendMessage));
    return confirmation;
}

std::list<Message> RequestHandler::waiting_message_receive(uint32_t payload_size) {
    size_t size = payload_size;
    std::list<Message> waiting_msgs_list;
    while (size > 0 && size <= payload_size) {
        Response::WaitingMsgInfo payload_msg;
        size_t received = _communication.receive((char *)&payload_msg, sizeof(payload_msg));
        if (received > 0) {
            if(payload_msg.type == MessageType::SYMMETRIC_KEY_REQ){
                //For a message received at the request of a symmetric key, there is no message content.
                waiting_msgs_list.emplace_back(Message(&payload_msg));
            }else{
                //For text, file, and symmetric key messages, we decrypt the content.(if there is content in the message)
                if(payload_msg.content_size > 0){
                    std::pair<size_t, std::string> received_info = handle_msg_content_received(payload_msg.content_size);
                    received += received_info.first;
                    std::string decrypted_content = handle_decrypt_received_message(&payload_msg, received_info.second);
                    waiting_msgs_list.emplace_back(Message(&payload_msg, decrypted_content));
                }
            }
            size -= received;
        }
    }
    return waiting_msgs_list;
}


std::pair<size_t, std::string> RequestHandler::handle_msg_content_received(uint32_t content_size){
    size_t received = 0, size = content_size;
    std::string content;
    while(size > 0 && size <= content_size){
        size_t receive_size = size > CHUNK_SIZE ? CHUNK_SIZE : size;
        uint8_t chunk_content[CHUNK_SIZE];
        received += _communication.receive((char *)&chunk_content, receive_size);
        if (received > 0) {
            content += std::string(chunk_content, chunk_content + receive_size);
        }
        size -= received;
    }
    std::pair<size_t, std::string> received_info(received, content);
    return received_info;
}



std::string RequestHandler::handle_decrypt_received_message(Response::WaitingMsgInfo* msg, const std::string& content){
    std::string decrypt_content, key;
    if(msg->type == MessageType::SYMMETRIC_KEY_RECEIVED){
        key = FilesHandler::get_specific_info_from_user_file("me.info", PRIVATE_KEY);
        decrypt_content = RSA::decrypt(content, key);
    }else{
        std::string id = std::string(msg->client_id, msg->client_id + ID_LENGTH);
        key = _ptr_msgU->get_symmetric_key_by_id(id);
        if(key.empty()){
            decrypt_content = "Failed to decrypt this message. There is no symmetric key for the sending user.";
        }else {
            if (msg->type == MessageType::TEXT_MESSAGE_RECEIVED || msg->type == MessageType::FILE_MESSAGE_RECEIVED) {
                decrypt_content = AES::decrypt(content, key);
            }if (msg->type == MessageType::FILE_MESSAGE_RECEIVED) {
                std::string id_str = std::string(msg->client_id, msg->client_id + ID_LENGTH);
                std::string file_path = FilesHandler::write_content_to_file(decrypt_content);
                decrypt_content = std::move(file_path);
            }
        }
    }
    return decrypt_content;
}
