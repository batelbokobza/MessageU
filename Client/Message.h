#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include <iostream>
#include <utility>

#include "Protocol.h"
#include "Constants.h"

///\brief Produces an object that contains the properties of the message.
///These values can be get and updated using getter and setter functions.

class Message {
protected:
    
    std::string _sender_id;
    std::string _target_id;
    uint32_t _message_id{};
    Protocol::MessageType _type{};
    uint32_t _content_size{};
    std::string _content;

public:

    Message() = default;
    ~Message() = default;
    Message(std::string uid, std::string tid, Protocol::MessageType msg_type, uint32_t msg_size, std::string msg_content = "");

    Message(std::string uid, std::string tid, uint32_t msg_id, Protocol::MessageType msg_type, uint32_t msg_size,
                     std::string msg_content);

    Message(Protocol::Response::WaitingMsgInfo* info, std::string msg_content = "");

    static std::string to_string(Protocol::MessageType msg_type_number);// Returns a string of the request type.

    std::string get_sender_id() const;
    void set_sender_id(const std::string& id);
    std::string get_target_id() const;
    void set_target_id(const std::string& id);
    uint32_t get_message_id() const;
    void set_message_id(uint32_t id);
    Protocol::MessageType get_type() const;
    void set_type(Protocol::MessageType type);
    uint32_t get_content_size() const;
    void set_content_size(uint32_t size);
    std::string get_content() const;
    void set_content(std::string msg_content);

};


class SymmetricKeyRequestMessage: public Message{
public:
    explicit SymmetricKeyRequestMessage(const std::string& uid, const std::string& tid);
};

class SymmetricKeyMessage: public Message{
public:
    explicit SymmetricKeyMessage(const std::string& uid, const std::string& tid, const std::string& content);
};

class TextMessage: public Message{
public:
    explicit TextMessage(const std::string& uid, const std::string& tid, const std::string& content);
};

class FileMessage: public Message{
public:
    explicit FileMessage(const std::string& uid, const std::string& tid, const std::string& content);
};

#endif //CLIENT_MESSAGE_H
