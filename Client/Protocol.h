#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <utility>
#include "Constants.h"
#include <cstdint>
#include <memory>
#include <string>

class Message;

///\brief The namespace of the protocol.
///\brief Its contents contain a MessageType enum, for use in the message numbers for sending requests to the server,
///\brief and receiving responses to these requests, from the server.
///\AdditionalNamespaces
///\AdditionalNamespaces  Contains subspace names in scales with different names, to differentiate between responses and requests.
///\AdditionalNamespaces  You can use the namespace protocol for requests and namespaces for responses.
///\AdditionalNamespaces  The use will be made according to the keyword of the namespace - Protocol, followed by,
///\AdditionalNamespaces  the required namespace (Request / Response)
///\note To use the class, use the following include - Protocol.h
namespace Protocol{
    static constexpr uint8_t CLIENT_VERSION = 2;

    enum class MessageType: uint8_t{
        SYMMETRIC_KEY_REQ = 1,
        SYMMETRIC_KEY_SEND = 2,
        SYMMETRIC_KEY_RECEIVED = 2,
        TEXT_MESSAGE_RECEIVED = 3,
        SEND_TEXT = 3,
        FILE_MESSAGE_RECEIVED = 4,
        SEND_FILE = 4
    };
}


///\brief The namespace request belongs to the main namespace protocol.
///\Usage - Structures for sending types of requests to the server in accordance with the protocol.
namespace Protocol::Request{
    enum class Code : uint8_t{
        REGISTRATION = 100,
        USERS_LIST = 101,
        PUBLIC_KEY = 102,
        SEND_MESSAGE = 103,
        WAITING_MESSAGES = 104
    };


    struct Header{
        uint8_t client_id[ID_LENGTH] = { 0 };
        uint8_t version{};
        Code code{};
        uint32_t payload_size{};
        Header(const std::string& uid, uint8_t version, Code code, uint32_t payload_size):
                code(code), payload_size(payload_size), version(version){
            memcpy(client_id, uid.data(), ID_LENGTH);
        }
    };

    ///\brief Contains a constructor
    ///\param first - String that contains the user's ID.
    ///\param second - MessageType that contains the message type.
    struct SendMessage {
        uint8_t target_id[ID_LENGTH] = { 0 };
        MessageType type{0};
        uint32_t size{0};
        uint8_t content[MAX_CHARACTER_SIZE] = { 0 };
        SendMessage(const std::string& id, MessageType type) : type(type) {
            memcpy(target_id, id.data(), ID_LENGTH);
        }
        SendMessage() = default;
    };

    ///\brief Contains a constructor
    ///\param first - String that contains the user's name.
    ///\param second - String that contains the public key.
    struct Registration {
        uint8_t name[MAX_NAME_LENGTH] = {0};
        uint8_t public_key[NUMBER_BYTES_TO_PUBLIC_KEY] = {0};
        Registration(const std::string& client_name, const std::string& key){
            memcpy(name, client_name.data(), client_name.length() < MAX_NAME_LENGTH ? client_name.length() : MAX_NAME_LENGTH);
            memcpy(public_key, key.data(), key.length() < NUMBER_BYTES_TO_PUBLIC_KEY ? key.length() : NUMBER_BYTES_TO_PUBLIC_KEY); //todo: delete
        }
    };

    ///\brief Contains a constructor
    ///\param first - String that contains the user's ID.
    struct PublicKey {
        uint8_t client_id[ID_LENGTH] = {0};
        PublicKey() = default;
        PublicKey(const std::string& id){
            memcpy(client_id, id.data(), ID_LENGTH);
        }
    };

    struct RSA_KeyPair {
        std::string public_key;
        std::string private_key;
    };
}

///\brief The namespace response belongs to the main namespace protocol.
///\Usage - Structures for receiving the types of responses from the server according to the protocol.

namespace Protocol::Response{
    enum class Code : uint16_t{
        REGISTRATION_COMPLETED_SUCCESSFULLY = 1000,
        USERS_LIST= 1001,
        PUBLIC_KEY= 1002,
        MESSAGE_TO_USER= 1003,
        WAITING_MESSAGE = 1004,
        GENERAL_ERROR = 9000
    };

    struct Header {
        uint8_t version;
        Code code;
        uint32_t payload_size;
    };

    struct UserInfo{
        uint8_t id[ID_LENGTH] = { 0 };
        uint8_t name[MAX_NAME_LENGTH] = { 0 };
        UserInfo() = default;
    };

    ///\brief Contains a constructor
    ///\param first - Array of characters that contains the user's ID.
    ///\note String-type struct fields.
    struct Registration {
        std::string client_id;
        Registration(unsigned char id[]){
            client_id = std::string(id, id + ID_LENGTH);
        }
    };


    ///\brief Contains a constructor
    ///\param first - Array of characters that contains the user's ID.
    ///\param second - Array of characters contains the public key.
    ///\note String-type struct fields.
    struct PublicKey {
        std::string client_id;
        std::string public_key;
        PublicKey(unsigned char id[], unsigned char key[]){
            client_id = std::string(id, id + ID_LENGTH);
            public_key = std::string(key, key + NUMBER_BYTES_TO_PUBLIC_KEY);
        }
        PublicKey() = default;
    };


    struct ConfirmSendMessage {
        uint8_t target_id[ID_LENGTH] = {0};
        uint32_t unique_msg_id{0};
    };

    struct WaitingMsgInfo {
        char client_id[ID_LENGTH] = { 0 };
        uint32_t message_id{0};
        MessageType type{0};
        uint32_t content_size{0};
    };
}

#endif //CLIENT_PROTOCOL_H
