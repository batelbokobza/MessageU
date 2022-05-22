#ifndef CLIENT_USERINTERFACE_H
#define CLIENT_USERINTERFACE_H

#include <iostream>
#include "boost/asio.hpp"
#include <memory>
#include <string>

#include "User.h"
#include "Message.h"
#include "MessageU.h"
#include "HalperValidators.h"
#include "FilesHandler.h"
#include "Protocol.h"

///\brief Responsible for managing communication with the user.
///     The class presents the requests that can be made, and their response.
class UserInterface {

    std::vector <std::string> options_menu{"1) Register", "2) Request for clients list", "3) Request for public key",
                                           "4) Request for waiting messages", "5) Send a text message",
                                           "50) Send a request for a file", "51) Send a request for symmetric key",
                                           "52) Send your symmetric key", "0) Exit "};

private:
    std::shared_ptr<User> _user = std::make_shared<User>();
    std::shared_ptr<MessageU> _msgU = std::make_shared<MessageU>();
    std::shared_ptr<HalperValidators> _req_validators = std::make_shared<HalperValidators>();

    void display_menu();
    int get_number_request();
    std::string get_input_from_user(const std::string& message_to_user);
    std::shared_ptr<User> get_user_details();
    std::string get_message_content();
    Protocol::Response::ConfirmSendMessage handle_send_message(unsigned short request_number, const std::string& target_id);
    void handle_request_public_key();
    void handle_request_send_message(unsigned short request_number);
    void handle_request_registration();
    std::string get_path_to_send_file();
    bool process_request();
    void print_users_list(std::list<User>& list);
    void view_waiting_message(const std::list<Message>& list);
    bool check_input_name(const std::string& input, bool is_registration = false);

public:

    ~UserInterface() = default;
    UserInterface();
    [[noreturn]] void start();
};
#endif //CLIENT_USERINTERFACE_H
