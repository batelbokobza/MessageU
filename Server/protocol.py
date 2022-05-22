import struct
from enum import IntEnum


class Constant(object):
    CLIENT_ID_SIZE = 16
    MESSAGE_ID_SIZE = 4
    MESSAGE_TYPE_SIZE = 1
    MESSAGE_CONTENT_SIZE = 4
    PUBLIC_KEY_SIZE = 160
    MAX_SIZE_NAME = 255
    CONFIRMATION_SEND_MESSAGE_SIZE = CLIENT_ID_SIZE + MESSAGE_ID_SIZE
    SERVER_VERSION_NUMBER = 2
    EMPTY_PAYLOAD_SIZE = 0


class Request(object):
    class Type(IntEnum):
        REGISTRATION = 100
        USERS_LIST = 101
        PUBLIC_KEY = 102
        SEND_MESSAGE = 103
        WAITING_MESSAGES = 104

    header_format = struct.Struct('16s B B I')
    register_format = struct.Struct('255s 160s')
    public_key_format = struct.Struct('16s')
    send_message_format = struct.Struct('16s B I')


class TypeMsg(IntEnum):
    SYMMETRIC_KEY_REQUEST = 1
    SEND_SYMMETRIC_KEY = 2
    SEND_TEXT_MESSAGE = 3
    SEND_FILE = 4


class Response(object):
    class Type(IntEnum):
        REGISTRATION_SUCCESSFULLY = 1000
        USERS_LIST = 1001
        PUBLIC_KEY = 1002
        CONFIRMATION_SEND_MESSAGE = 1003
        WAITING_MESSAGES = 1004
        GENERAL_ERROR = 9000

    header_format = struct.Struct('B H I')
    registration_successfully_format = struct.Struct('16s')
    user_info_in_list_format = struct.Struct('16s 255s')
    public_key_format = struct.Struct('16s 160s')
    confirmation_send_message_format = struct.Struct('16s I')
    info_waiting_message_format = struct.Struct('16s I B I')
