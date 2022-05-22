#ifndef CLIENT_CONSTANT_H
#define CLIENT_CONSTANT_H


enum possible_input_request{
    REGISTRATION_REQ = 1,
    USERS_LIST_REQ = 2,
    PUBLIC_KEY_REQ = 3,
    WAITING_MESSAGES_REQ = 4,
    SEND_TEXT_MESSAGE_REQ = 5,
    SEND_FILE_REQ = 50,
    GET_SYMMETRIC_KEY_REQ = 51,
    SEND_SYMMETRIC_KEY_REQ = 52,
    EXIT = 0,
};


/* The me file saves user information according to the data rows in enum.
 * If you want to change the contents of the file or the order of the lines in the file,
 * change the contents of the enum accordingly for a desired result.*/
enum line_number_info_about_user_from_file {
    NAME = 1,
    ID = 2,
    PRIVATE_KEY = 3
};


#define FILE_NAME_FOR_USER_DETAILS "me.info"
#define FILE_NAME_FOR_NETWORK_DETAILS "server.info"
#define TEMPORARY_FILE_FOLDER_NAME "%TMP%"
#define FILES_FOLDER_NAME_RECEIVED "ReceivedFiles//"
#define MAX_FILE_SIZE 9999
#define MAX_CHARACTER_SIZE 9999
#define ID_LENGTH 16
#define MAX_NAME_LENGTH 255
#define NUMBER_BYTES_TO_PUBLIC_KEY 160
#define SYMMETRIC_KEY_SIZE 128
#define EMPTY_PAYLOAD_SIZE 0
#define EMPTY ""
#define INFO_CLIENT(len_id, len_name) ((len_id) + (len_name))
#define CHUNK_SIZE 128

#define RANDOM_FILE_NAME_LENGTH 32

#define UPDATE_PUBLIC_KEY 8
#define UPDATE_SYMMETRIC_KEY 9


#define RSA_KEY_SIZE 1024

#endif //CLIENT_CONSTANT_H
