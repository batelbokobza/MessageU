#include "MessageU.h"
#include "Message.h"
#include "KeyManager.h"

using namespace Protocol;

MessageU::MessageU() = default;

MessageU::~MessageU() = default;

std::pair<User, std::string> MessageU::user_registration_request(std::string& user_name){ //100
    std::pair<User, std::string> new_user;
    Request::RSA_KeyPair key_pair = KeyManager::get_public_and_private_key();
    Request::Registration payload(user_name, key_pair.public_key);
    Request::Header header(EMPTY, CLIENT_VERSION, Request::Code::REGISTRATION, sizeof(Request::Registration));
    auto payload_res = TcpConnection().connect()->send_request(header, payload);
    if (payload_res.type() == typeid(Response::Registration)) {
        auto response = std::any_cast<Response::Registration>(payload_res);
        User user(user_name, response.client_id, key_pair.public_key);
        new_user.first = user;
        new_user.second = key_pair.private_key;
    }
       return new_user;
}

std::list<User> MessageU::users_list_request(const std::string& id){ //101
    std::string uid = from_hex_to_ascii(id);
    std::list<User> users_list;
    Request::Header header(uid, CLIENT_VERSION, Request::Code::USERS_LIST, EMPTY_PAYLOAD_SIZE);
    auto payload_res = TcpConnection().connect()->send_request(header);
    if(payload_res.type() == typeid(std::list<User>)) {
        users_list = std::any_cast<std::list<User>>(payload_res);
        update_users_list(users_list);
    }
    return _users_list;
}

/* Check if the usernames in the newly received list exist in the saved user list.
 * If the list is empty, we will fill it in the received list.
 * Otherwise, we will only add users whose names do not appear in the list.
 * The user name is unique, so there is no more than one customer with the same name.*/
void MessageU::update_users_list(const std::list<User>& new_list){
    if(_users_list.empty()){
        _users_list.assign(new_list.begin(), new_list.end());
    }else {
        for (const User &user : new_list) {
            if(get_user_id_by_name(user.get_name()).empty()){
                _users_list.emplace_back(user);
            }
        }
    }
}

/* We will update the key in the list of users according to the username.*/
Response::PublicKey MessageU::public_key_request(const std::string& uid, const std::string& tname){ //102
    std::string user_id = from_hex_to_ascii(uid);
    std::string target_id = get_user_id_by_name(tname);
    Request::Header header(user_id, CLIENT_VERSION, Request::Code::PUBLIC_KEY, target_id.length());
    Request::PublicKey id_to_payload(target_id);
    Response::PublicKey public_key_res;
    auto payload_res = TcpConnection().connect()->send_request(header, id_to_payload);
    if(payload_res.type() == typeid(Response::PublicKey)) {
        public_key_res = std::any_cast<Response::PublicKey>(payload_res);
        update_user_key_by_id(public_key_res.client_id, public_key_res.public_key, UPDATE_PUBLIC_KEY);
    }
    return public_key_res;
}


/* Check for update - If the type of message received is receiving a symmetric key of a user,
 * we will update the key in the list of users according to the username.*/
std::list<Message> MessageU::messages_list_request(const std::string& id){ //101
    users_list_request(id);
    std::string uid = from_hex_to_ascii(id);
    std::list<Message> messages_list;
    Request::Header header(uid, CLIENT_VERSION, Request::Code::WAITING_MESSAGES, EMPTY_PAYLOAD_SIZE);
    auto payload_res = TcpConnection().connect()->send_request(this, header);
    if(payload_res.type() == typeid(std::list<Message>)) {
        messages_list = std::any_cast<std::list<Message>>(payload_res);
        for(const Message& msg: messages_list){
            if(msg.get_type() == MessageType::SYMMETRIC_KEY_RECEIVED && is_accept_symmetric_key_message(msg.get_sender_id())){
                update_user_key_by_id(msg.get_sender_id(), msg.get_content(), UPDATE_SYMMETRIC_KEY);
            }
        }
    }
    return messages_list;
}


Response::ConfirmSendMessage MessageU::send_message_request(unsigned short request_number, const std::string& uid ,const std::string& tname, const std::string& content){
    Message* new_message;
    std::string uid_conversion = boost::algorithm::unhex(uid);
    std::string target_id = get_user_id_by_name(tname);
    if(request_number == GET_SYMMETRIC_KEY_REQ){
        new_message = new SymmetricKeyRequestMessage(uid_conversion, target_id);
    }else if(request_number == SEND_SYMMETRIC_KEY_REQ){
        std::string symmetric_key = KeyManager::get_new_symmetric_key();
        new_message = new SymmetricKeyMessage(uid_conversion, target_id, symmetric_key);
    }else if(request_number == SEND_TEXT_MESSAGE_REQ){
        new_message = new TextMessage(uid_conversion, target_id, content);
    }else {  // request for Send file.
        new_message = new FileMessage(uid_conversion, target_id, content);
    }
    Response::ConfirmSendMessage confirmation = handle_send_message(new_message);
    //delete new_message; todo: check if valid
    return confirmation;
}

Response::ConfirmSendMessage MessageU::handle_send_message(Message* message){ //103
    //The message object contains the sender user ID and the destination user ID.
    //When sending a message request to the server,
    // the user ID is sent in the header part and therefore it is missing from the size of the message sent.
    size_t size_buf =  sizeof(*message) - ID_LENGTH;
    Request::Header header(message->get_sender_id(), CLIENT_VERSION, Request::Code::SEND_MESSAGE, size_buf);
    Response::ConfirmSendMessage confirmation;
    auto payload_res = TcpConnection().connect()->send_request(this, header, message);
    if (payload_res.type() == typeid(Response::ConfirmSendMessage)) {
        confirmation = std::any_cast<Response::ConfirmSendMessage>(payload_res);
        if (message->get_type() == MessageType::SYMMETRIC_KEY_REQ) {
            _list_waiting_symmetric_key.emplace_back(message->get_target_id());
        }
        if (message->get_type() == MessageType::SYMMETRIC_KEY_SEND) {
            update_user_key_by_id(message->get_target_id(), message->get_content(), UPDATE_SYMMETRIC_KEY);
        }
    }
    return confirmation;
}

/*If a symmetric key message was received, we will check whether the user requested the user to send his symmetric key.
 * If not - we will not receive the message for security reasons, and returned false.
 * Otherwise, we will receive the message and delete the sender name from the list of users
 * to whom a symmetric key request was sent, and returned true.*/
bool MessageU::is_accept_symmetric_key_message(const std::string& sender){
    std::vector<std::string>::iterator it;
    it = std::find(_list_waiting_symmetric_key.begin(), _list_waiting_symmetric_key.end(), sender);
    if (it != _list_waiting_symmetric_key.end()){//If this is a response to a user's request for a symmetric key.
        _list_waiting_symmetric_key.erase(it);
        return true;
    }
    return false;
}

/*The function updates the keys of the users according to the response received to the request.
 *  The message is sent to an existing username only, so there is no need to update the users list.
 *  The function will return a false only if the update type is invalid. */
bool MessageU::update_user_key_by_id(const std::string& id, const std::string& key, unsigned short update_key_type){
    bool successfully_updated = false;
    for (User& user : _users_list) {
        if (user.get_id() == id) {
            if(update_key_type == UPDATE_SYMMETRIC_KEY) {
                user.set_symmetric_key(key);
                successfully_updated = true;
            }else if (update_key_type == UPDATE_PUBLIC_KEY) {
                user.set_public_key(key);
                successfully_updated = true;
            }
        }
    }
    return successfully_updated;
}

std::string MessageU::get_user_id_by_name(std::string name){
    name.resize(255);
    for (const User& user : _users_list){
        if (user.get_name() == name) {
            return user.get_id();
        }
    }
    return EMPTY;
}

std::string MessageU::get_public_key_by_id(const std::string& id){
    for (const User& user : _users_list){
        if (user.get_id() == id) {
            return user.get_public_key();
        }
    }
    return EMPTY;
}

std::string MessageU::get_symmetric_key_by_id(const std::string& id){
    for (const User& user : _users_list){
        if (user.get_id() == id) {
            return user.get_symmetric_key();
        }
    }
    return EMPTY;
}

std::string MessageU::get_user_name_by_id(std::string id){
    for (const User& user : _users_list){
        if (user.get_id() == id)
            return user.get_name();
    }
    return EMPTY;
}

std::string MessageU::from_hex_to_ascii(const std::string& uid) {
    std::string id_conversion = boost::algorithm::unhex(uid);
    return id_conversion;
}
