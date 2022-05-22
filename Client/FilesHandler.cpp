#include "FilesHandler.h"
#include "User.h"
#include "boost/filesystem.hpp"
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

void FilesHandler::create_directory(const std::string& directory_name){
    boost::filesystem::path root_path = std::string(directory_name);
    boost::filesystem::create_directories(root_path);
}

std::string FilesHandler::get_specific_info_from_user_file(const std::string& file_path, unsigned short line_number){
    std::string line;
    std::ifstream file(file_path, std::ios::out);
    if(!file.is_open()){
        throw MessageUException("Failed to open file.");
    }
    getline(file, line);
    if(line_number != NAME) { //If the specific line requested is the second line.
        getline(file, line);
        /*If the requested line is the third line, since this line of the user file contains the private key,
         which can be longer than one line, we will read this line using an iterator to read the all private key.*/
        if(line_number == PRIVATE_KEY)
            line = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    }
    file.close();
    return line;
}



std::string FilesHandler::get_content_file(const std::string& path){
    std::string content_file;
    try {
        std::ifstream file(path, std::ios::out | std::ios::binary);
        while (!file.eof()) {
            char chunk[CHUNK_SIZE] = { 0 };
            file.read(chunk, sizeof(chunk));
            content_file += chunk;
        }
        file.close();
    }catch(std::exception &e){
        throw MessageUException("Failed to open file.");
    }
    return content_file;
}

/// \return A path to a file where the content is saved.
///The file name is random characters.
std::string FilesHandler::write_content_to_file(const std::string& content) {
    std::string file_name = get_random_file_name();
    try {
        boost::filesystem::path root_path = std::string(FILES_FOLDER_NAME_RECEIVED);
        boost::filesystem::create_directories(root_path);
        std::ofstream file;
        file.open(std::string(FILES_FOLDER_NAME_RECEIVED) + "/" + file_name, std::ios::out | std::ios::binary);
        file.write(content.data(), content.size());
        file.close();
    }
    catch (std::exception& e) {
        throw MessageUException("Failed to receive file.");
    }
    return std::string(FILES_FOLDER_NAME_RECEIVED) + "/" + file_name;
}

std::pair<std::string, unsigned short> FilesHandler::get_port_and_ip(const std::string& path){
    std::ifstream file(path);
    std::string line_from_file;
    unsigned short port;
    getline(file, line_from_file);
    if(!line_from_file.empty()) {
        std::vector<std::string> info;
        boost::split(info, line_from_file, boost::is_any_of(":"));
        std::stringstream geek(info[1]);
        geek >> port;
        std::pair<std::string, unsigned short> port_and_ip(info[0], port);
        return port_and_ip;
    }
    else{
        throw MessageUException("Failed to read connection information file.");
    }
}

std::shared_ptr<User> FilesHandler::get_authenticate_user_details_from_file() {
    try {
        std::ifstream file(FILE_NAME_FOR_USER_DETAILS);
        std::string name, id;
        getline(file, name), getline(file, id);
        if(name.empty() || id.empty()) {
            throw MessageUException("User authentication failed.");
        }
        User user_details(name, id);
        return std::make_shared<User>(user_details);
    } catch (std::exception &e) {
        throw MessageUException("Unable to read user information from file.");
    }
}

void FilesHandler::write_user_details_to_file(const User& user, const std::string& key){
    try{
        std::ofstream file(FILE_NAME_FOR_USER_DETAILS, std::ios::out);
        file.write(user.get_name().data(), user.get_name().length());
        file.write("\n", sizeof(char));
        file.write(user.get_id().data(), user.get_id().length());
        file.write("\n", sizeof(char));
        file.write(key.data(), key.length());
    }catch(std::exception &e){
        throw MessageUException("Failed to create user information file.");
    }
}

std::string FilesHandler::get_random_file_name(){
    boost::uuids::random_generator generator;
    std::string new_file_name = boost::uuids::to_string(generator());
    boost::algorithm::erase_all(new_file_name, "-");
    return new_file_name;
}
