#ifndef CLIENT_HALPERVALIDATORS_H
#define CLIENT_HALPERVALIDATORS_H

#include "boost/filesystem.hpp"
#include <boost/algorithm/cxx11/one_of.hpp>
#include "boost/asio.hpp"
#include <iostream>
#include <memory>

#include "MessageUException.h"
#include "Constants.h"
#include "Protocol.h"

class MessageU;

/* Contains functions for checking the correctness of requests received
 * from the user and responses received from the server.*/
class HalperValidators{

    static const unsigned int several_requests = 9;
    unsigned int possible_request_number[several_requests] = {0, 1, 2, 3, 4, 5, 50, 51, 52};
    unsigned int possible_response_code[several_requests] = {1000, 1001, 1002, 1003, 1004, 9000};

public:
    HalperValidators();
    bool is_valid_number_request(const std::string& number);
    bool is_valid_name(const std::string& name);
    bool is_valid_user_name(const std::string& name);
    bool is_file_exist(std::string file_path);
    bool is_user_registered();
    bool is_possible_send_message(const std::shared_ptr<MessageU>& ptr, unsigned short number, const std::string& name);
    bool is_user_name_exist(const std::shared_ptr<MessageU>& ptr, const std::string& name);
    bool is_valid_code_response(uint16_t number);
};
#endif //CLIENT_HALPERVALIDATORS_H
