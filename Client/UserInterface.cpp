#include "UserInterface.h"
#include <boost/algorithm/hex.hpp>
#include <filesystem>

using namespace Protocol;

UserInterface::UserInterface() = default;

[[noreturn]] void UserInterface::start() {
    while (true) {
        bool is_completed_successfully = false;
        while (!is_completed_successfully) {
            display_menu();
            if (process_request()) {
                is_completed_successfully = true;
            }
        }
    }
}

void UserInterface::display_menu(){
    std::cout << "\nMessageU client at your service." << "\n";
    for (auto& line : options_menu)
        std::cout << line << "\n";
}

int UserInterface::get_number_request(){
    std::string input = get_input_from_user("Please enter request number");
    if(!_req_validators->is_valid_number_request(input)){
        throw MessageUException("Invalid input number request.\n");
    }
    return stoi(input);
}

/// \brief   Handles user requests. If there is an error, an exception will be thrown.
///     Otherwise, the request will be sent and the response will be received.
/// \return         bool
bool UserInterface::process_request(){
        try {
            auto request_number = static_cast<unsigned short>(get_number_request()); //To make valid conversions.
            if (request_number == REGISTRATION_REQ){
                handle_request_registration();
            }else {
                _user = get_user_details();
                if (request_number == USERS_LIST_REQ) {
                    std::list<User> users_list = _msgU->users_list_request(_user->get_id());
                    print_users_list(users_list);
                } else if (request_number == PUBLIC_KEY_REQ) {
                    handle_request_public_key();
                } else if (request_number == WAITING_MESSAGES_REQ) {
                    std::list<Message> messages_list = _msgU->messages_list_request(_user->get_id());
                    view_waiting_message(messages_list);
                } else if (request_number == EXIT) {
                    exit(0);
                } else { //request for send message - symmetric key request / send symmetric key / text / file
                    handle_request_send_message(request_number);
                }
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    return true;
}


std::shared_ptr<User> UserInterface::get_user_details(){
    if(!_req_validators->is_user_registered()){
        throw MessageUException("Cannot be requested without registration. First, you need to register.");
    }
    return FilesHandler::get_authenticate_user_details_from_file();
}


void UserInterface::handle_request_registration() {
    if (_req_validators->is_user_registered()) {
        throw MessageUException("User is already registered.");
    }
    else {
        std::string user_name = get_input_from_user("Enter a username");
        if (check_input_name(user_name, true)) {
            std::pair<User, std::string> new_user = _msgU->user_registration_request(user_name);
            new_user.first.set_id(boost::algorithm::hex(new_user.first.get_id()));
            FilesHandler::write_user_details_to_file(new_user.first, new_user.second);
            std::cout << "\n* Registration completed successfully!" << "\n\n";
        }
    }
}

void UserInterface::handle_request_public_key() {
    std::string user_name_target = get_input_from_user("Enter a target username");
    if (check_input_name(user_name_target)) {
        Response::PublicKey public_key = _msgU->public_key_request(_user->get_id(), user_name_target);
        std::cout << "\n* " << user_name_target <<" public key was successfully received."<< "\n";
    }
}

void UserInterface::handle_request_send_message(unsigned short request_number){
    std::string user_name_target = get_input_from_user("Enter a target username");
    if(check_input_name(user_name_target)){
        if (_req_validators->is_possible_send_message(_msgU, request_number, user_name_target)) {
            Response::ConfirmSendMessage confirmation = handle_send_message(request_number, user_name_target);
            std::cout << "\n* The message to: " << user_name_target.c_str() << " - sent successfully." << "\n";
            std::cout << " Message ID is: " << confirmation.unique_msg_id << ".\n";
        }
    }
    }
}

Response::ConfirmSendMessage UserInterface::handle_send_message(unsigned short request_number, const std::string& tname) {
    std::string content;
    if (request_number == SEND_TEXT_MESSAGE_REQ || request_number == SEND_FILE_REQ) {
        content = request_number == SEND_FILE_REQ ? get_path_to_send_file() : get_message_content();
    }
    Response::ConfirmSendMessage confirmation = _msgU->send_message_request(request_number, _user->get_id(), tname, content);
    return confirmation;
}


bool UserInterface::check_input_name(const std::string& input_name, bool is_registration) {
    if (_req_validators->is_valid_user_name(input_name)) {
        if (!is_registration && !_req_validators->is_user_name_exist(_msgU, input_name)) {
            throw MessageUException("The username you entered does not exist.");
        }
    }
    return true;
}


void UserInterface::view_waiting_message(const std::list<Message>& messages_list){
    if(messages_list.empty()){
        std::cout << "\n* Sorry, You have no waiting messages." << "\n";
    }else{
        std::cout<< "\n* " << messages_list.size() <<" messages are waiting for you:"<<"\n\n";
        for(const Message& msg: messages_list){
            std::string content_type = msg.to_string(msg.get_type());
            std::string name = _msgU->get_user_name_by_id(msg.get_sender_id());
            std::cout << "\n* Message from: " << (name.empty()  ? "Unknown User" : name.c_str()) <<".\n";
            std::cout<< "  Content: \n  " << content_type << ".\n";
            if(msg.get_type() == MessageType::SEND_TEXT || msg.get_type() == MessageType::SEND_FILE){
                std::cout<<"   "<< msg.get_content() <<"\n\n";
            }
        }
    }
}

std::string UserInterface::get_message_content(){
    std::string message_content = get_input_from_user("Enter a text message content");
    return message_content;
}

std::string UserInterface::get_path_to_send_file(){
    std::string full_path = get_input_from_user("Enter a full path, with the file name you want to send");
    if(_req_validators->is_file_exist(full_path)){
        if(std::filesystem::file_size(full_path) > MAX_FILE_SIZE){
            throw MessageUException("File size is too large. The maximum size for sending a file is 4 GB.");
        }
        return full_path;
    }
    throw MessageUException("File not found.");
}


void UserInterface::print_users_list(std::list<User>& list){
    std::cout << "\n* The number of existing users in the list is " << list.size() << ".\n";
    std::cout << "\n               id                            name";
    std::cout << "\n--------------------------------\t    ----------------\n";
    for (const User& us : list){
        std::cout << std::setw( 14 ) << std::left << boost::algorithm::hex(us.get_id()) << "\t    " << us.get_name() << std::endl;
    }
}

std::string UserInterface::get_input_from_user(const std::string& message_to_user){
    std::string input;
    std::cout << message_to_user << ": " ;
    getline(std::cin, input);
    return input;
}
