#include "Message.h"
#include "FilesHandler.h"
#include <utility>

using namespace Protocol;

Message::Message(std::string uid, std::string tid,  uint32_t msg_id, MessageType msg_type, uint32_t msg_size, std::string msg_content):
        _message_id(msg_id), _type(msg_type), _content_size(msg_size), _content(std::move(msg_content)),
        _sender_id(std::move(uid)), _target_id(std::move(tid)) {
}

Message::Message(std::string uid, std::string tid, MessageType msg_type, uint32_t msg_size, std::string msg_content):
        _type(msg_type), _content_size(msg_size), _content(std::move(msg_content)),
    _sender_id(std::move(uid)), _target_id(std::move(tid)) {
}

Message::Message(Response::WaitingMsgInfo* info, std::string msg_content):
        _message_id(info->message_id), _type(info->type), _content_size(info->content_size), _content(std::move(msg_content)) {
    _sender_id = info->client_id;
    _sender_id.resize(ID_LENGTH);
}

///\param  MessageType
///\return a string of the request type for message received.
std::string Message::to_string(MessageType msg_type_number){
    std::string type_str;
    if(msg_type_number == MessageType::SYMMETRIC_KEY_REQ){
        type_str = "Request for symmetric key";
    }else if(msg_type_number == MessageType::SYMMETRIC_KEY_SEND){
        type_str = "Symmetric key received";
    }else if(msg_type_number == MessageType::SEND_TEXT){
        type_str = "Text message";
    }else if(msg_type_number == MessageType::SEND_FILE){
        type_str = "Received file";
    }
    return type_str;
}


std::string Message::get_sender_id() const{
    return _sender_id;
}

void Message::set_sender_id(const std::string& id){
    _sender_id = id;
}

std::string Message::get_target_id() const{
    return _target_id;
}

void Message::set_target_id(const std::string& id){
    _target_id = id;
}

uint32_t Message::get_message_id() const{
    return _message_id;
}

void Message::set_message_id(uint32_t id){
    _message_id = id;
}

MessageType Message::get_type() const {
    return _type;
}

void Message::set_type(MessageType type) {
    _type = type;
}

uint32_t Message::get_content_size() const{
    return _content_size;
}

void Message::set_content_size(uint32_t size){
    _content_size = size;
}

std::string Message::get_content() const {
    return _content;
}

void Message::set_content(std::string msg_content){
    _content = msg_content;
}

SymmetricKeyRequestMessage::SymmetricKeyRequestMessage(const std::string& uid, const std::string& tid):
        Message(uid, tid, MessageType::SYMMETRIC_KEY_REQ, EMPTY_PAYLOAD_SIZE, EMPTY) {
}


SymmetricKeyMessage::SymmetricKeyMessage(const std::string& uid, const std::string& tid, const std::string& content):
        Message(uid, tid, MessageType::SYMMETRIC_KEY_SEND, content.length(), content) {
}

TextMessage::TextMessage(const std::string& uid, const std::string& tid, const std::string& content):
        Message(uid, tid, MessageType::SEND_TEXT, content.length(), content) {
}


FileMessage::FileMessage(const std::string& uid, const std::string& tid, const std::string& file_path):
        Message(uid, tid, MessageType::SEND_FILE, EMPTY_PAYLOAD_SIZE, EMPTY){
    std::string content_from_file = FilesHandler::get_content_file(file_path);
    _content_size = content_from_file.length();
    _content = std::move(content_from_file);
}