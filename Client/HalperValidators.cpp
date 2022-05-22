#include "HalperValidators.h"
#include "boost/filesystem.hpp"
#include <filesystem>
#include "MessageU.h"

using namespace Protocol;

HalperValidators::HalperValidators() = default;

bool HalperValidators::is_valid_number_request(const std::string& number) {
    try {
        int num = stoi(number);
        if ((boost::algorithm::one_of_equal(possible_request_number, num))) {
            return true;
        }
    } catch (std::exception &e) {
        return false;
    }
    return true;
}

bool HalperValidators::is_valid_code_response(uint16_t number){
    return boost::algorithm::one_of_equal(possible_response_code, number);
}

bool HalperValidators::is_user_registered() {
    return (is_file_exist(FILE_NAME_FOR_USER_DETAILS) && !std::filesystem::is_empty(FILE_NAME_FOR_USER_DETAILS));
}

bool HalperValidators::is_file_exist(std::string file_path){
    return boost::filesystem::exists(file_path);
}

bool HalperValidators::is_valid_user_name(const std::string& name) {
    if(name.length() > MAX_NAME_LENGTH || !is_valid_name(name)) {
        throw MessageUException("Username can contain up to 255 characters. Characters and numbers only.");
    }else if (name.empty()) {
        throw MessageUException("Username can not be empty.");
    }else{
        return true;
    }
}

bool HalperValidators::is_valid_name(const std::string& name){
    if(name.length() <= MAX_NAME_LENGTH) {
        for(char c : name){
            if((c != '-') && (c != ' ') &&  (c != '_') && !std::isalpha(c) && !std::isdigit(c))
                return false;
        }
    }
    return true;
}

///\note The function receives in the first parameter a pointer to the MessageU class.
/// Without this pointer, there may be an unexpected response when using the function.
bool HalperValidators::is_user_name_exist(const std::shared_ptr<MessageU>& msgU_ptr, const std::string& name){
   return !msgU_ptr->get_user_id_by_name(name).empty();
}

/* To send a text message or a file, a symmetric key is required to encrypt the content.
 * To send a message with a symmetric key, a public key of the recipient of the message is required to encrypt the sent key.*/
///\note The function receives in the first parameter a pointer to the MessageU class.
/// Without this pointer, there may be an unexpected response when using the function.
bool HalperValidators::is_possible_send_message(const std::shared_ptr<MessageU>& msgU_ptr, unsigned short request_number, const std::string& name){
    std::string id = msgU_ptr->get_user_id_by_name(name);
    if(request_number == SEND_TEXT_MESSAGE_REQ || request_number == SEND_FILE_REQ){
        if(msgU_ptr->get_symmetric_key_by_id(id).empty()){
            throw MessageUException("The message could not be sent. Required to get symmetric key with this client.");
        }
    }else if(request_number == SEND_SYMMETRIC_KEY_REQ){
        if(msgU_ptr->get_public_key_by_id(id).empty()){
            throw MessageUException("The message could not be sent. Required to get public key with this client.");
        }
    }
    return true;
}

