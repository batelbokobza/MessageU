#ifndef CLIENT_FILESHANDLER_H
#define CLIENT_FILESHANDLER_H

#include <iostream>
#include "boost/filesystem.hpp"
#include "boost/asio.hpp"
#include <boost/algorithm/string.hpp>
#include <vector>
#include "Constants.h"

#include "MessageUException.h"

/* The class has two functions that handle the user information file.
 * A function that reads the details from a file, and a function that writes the details to a file.
 * Functions work on a file with a constant name - FILE_NAME_FOR_USER_DETAILS,
 * And in the order of the lines given in the file.
 * You can to choose and edit the name of the file containing the user information, in the const file Constant.h*/
class User;
class FilesHandler {
public:
    FilesHandler() = default;
    static std::pair<std::string, unsigned short> get_port_and_ip(const std::string &path);
    static void create_directory(const std::string& directory_name);
    static std::string write_content_to_file(const std::string& content);
    static std::string get_content_file(const std::string& path);
    static std::shared_ptr<User> get_authenticate_user_details_from_file();
    static void write_user_details_to_file(const User& user, const std::string& key);
    static std::string get_random_file_name();
    static std::string get_specific_info_from_user_file(const std::string& file_path, unsigned short line_number);

};
#endif //CLIENT_FILESHANDLER_H
