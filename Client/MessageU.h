#ifndef CLIENT_MESSAGEU_H
#define CLIENT_MESSAGEU_H

#include <iostream>
#include "boost/filesystem.hpp"
#include "boost/asio.hpp"
#include <boost/algorithm/hex.hpp>

#include "User.h"
#include "RequestHandler.h"
#include "Constants.h"
#include "TcpConnection.h"

///\brief A class that links the user interface to the class that handles the sending of the request and receiving the response.
/*The MessageU class handles user choices,
 * creates the requests according to the parameters it receives in each function from the UserInterface class,
 * and receives responses according to these types of requests with the help of the RequestHandler class.*/

/* Note to the uses of the file - The user can ask other users for the symmetric key to communicate with them.
 * A malicious user may send a symmetric key without the user requesting it, so to prevent disclosure of information,
 * we accept symmetric keys that are accepted, only for users to whom a request has been sent.*/

class Message;

class MessageU{

private:
    std::vector<std::string> _list_waiting_symmetric_key;
    std::list<User> _users_list;
    std::string from_hex_to_ascii(const std::string& uid);
    void update_users_list(const std::list<User>& list);
    bool update_user_key_by_id(const std::string& id, const std::string& key, unsigned short update_type);
    bool is_accept_symmetric_key_message(const std::string& sender);
    Protocol::Response::ConfirmSendMessage handle_send_message(Message* message);

    
public:

    MessageU();
    ~MessageU();
    std::pair<User, std::string> user_registration_request(std::string& name);
    std::list<User> users_list_request(const std::string& uid);
    Protocol::Response::PublicKey public_key_request(const std::string& uid, const std::string& tname);
    std::list<Message> messages_list_request(const std::string& uid);

    std::string get_user_id_by_name(std::string name);
    std::string get_user_name_by_id(std::string uid);
    std::string get_public_key_by_id(const std::string& id);
    std::string get_symmetric_key_by_id(const std::string& uid);
    Protocol::Response::ConfirmSendMessage send_message_request(unsigned short request_number, const std::string& uid , const std::string& tname, const std::string& content = "");

};
#endif //CLIENT_MESSAGEU_H
