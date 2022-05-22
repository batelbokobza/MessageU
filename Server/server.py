import socket
import uuid
import threading
import message
import database
import user
import request
import response
import protocol


def is_valid_request_code(code, enum_type):
    for number in enum_type:
        if code == number:
            return True
    return False


def get_port(file_name):
    try:
        with open(file_name, "r") as file:
            return int(file.readline().strip())
    except (ValueError, FileNotFoundError):
        print("Cant read port from file.")


P = protocol


class MessageUServer(threading.Thread):

    def __init__(self, data_base, thread):
        threading.Thread.__init__(self)
        self.db = data_base
        self.user_sock = thread
        self.req = None

    def run(self):
        self.req = request.Request(self.user_sock)  # req contains header and payload
        request_status = self.process_request()  # The value will be true or false if the request received is valid.
        res = response.Response(self.user_sock, self.req, self.db)
        self.send_response(res, request_status, self.req.get_specific_header_field("CODE"))

    # if type request = users list or waiting messages, request is valid.
    def process_request(self):
        valid_request = False
        uid = self.req.get_specific_header_field("CLIENT_ID")
        code = self.req.get_specific_header_field("CODE")
        if is_valid_request_code(code, P.Request.Type):
            if code != P.Request.Type.REGISTRATION and not self.db.is_user_id_exist(uid):
                valid_request = False
            elif code == P.Request.Type.USERS_LIST or code == P.Request.Type.WAITING_MESSAGES:
                valid_request = True
            else:
                switch = {
                    P.Request.Type.REGISTRATION: self.user_registration,  # 100
                    P.Request.Type.PUBLIC_KEY: self.request_for_public_key,  # 102
                    P.Request.Type.SEND_MESSAGE: self.request_for_send_message,  # 103
                }
                status = switch.get(code, lambda: "")
                valid_request = status()
        return valid_request

    def user_registration(self):
        user_name_filed = self.req.get_specific_payload_field("NAME")
        if self.db.get_user_details_by_name(user_name_filed) is not None:
            return False
        new_user = user.User(uuid.uuid4().bytes, self.req.payload[0], self.req.payload[1])
        self.db.add_user(new_user.id, new_user.name, new_user.public_key, new_user.last_seen)
        return True

    def request_for_public_key(self):
        id_in_header = self.req.get_specific_header_field("CLIENT_ID")
        id_in_payload = self.req.payload
        return self.db.is_user_id_exist(id_in_header) and self.db.is_user_id_exist(id_in_payload)

    def request_for_send_message(self):
        source_id = self.req.get_specific_header_field("CLIENT_ID")
        target_id = self.req.get_specific_payload_field("CLIENT_ID")
        if not is_valid_request_code(self.req.get_specific_payload_field("MESSAGE_TYPE"), P.TypeMsg):
            return False
        return self.db.is_user_id_exist(source_id) and self.db.is_user_id_exist(target_id)

    def add_message_to_dataBase_and_send_confirmation(self, res):
        source_id = self.req.get_specific_header_field("CLIENT_ID")
        target_id = self.req.get_specific_payload_field("CLIENT_ID")
        message_type = self.req.get_specific_payload_field("MESSAGE_TYPE")
        message_content = ""
        if message_type != P.TypeMsg.SYMMETRIC_KEY_REQUEST:
            message_content = self.req.get_specific_payload_field("MESSAGE_CONTENT")
        msg_to_save = message.Message(target_id, source_id, message_type, message_content)
        self.db.add_message(msg_to_save)  # add to db table
        res.send_confirmation_of_message_sent(target_id, msg_to_save.id)

    def send_response(self, res, request_status, code):
        if not request_status:
            res.send_general_error()
        elif code == P.Request.Type.REGISTRATION:
            res.send_registration_successful()
        elif code == P.Request.Type.USERS_LIST:
            res.send_users_list()
        elif code == P.Request.Type.PUBLIC_KEY:
            res.send_public_key()
        elif code == P.Request.Type.SEND_MESSAGE:
            self.add_message_to_dataBase_and_send_confirmation(res)
        elif code == P.Request.Type.WAITING_MESSAGES:
            res.send_waiting_messages()


class Server:
    def __init__(self):
        # open socket
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.db = database.DataBase()
        self.prepare_database()

    def prepare_database(self):
        self.db.create_clients_table()
        self.db.create_message_table()

    def start_connection(self):
        server_port = get_port("port.info")
        try:
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind(('', int(server_port)))
            print("MessageU Chat connection created with port - ", server_port)
            self.prepare_messageU_server()
        except socket.error as socket_exception:
            print(socket_exception)
            self.socket.close()

    def prepare_messageU_server(self):
        while True:
            self.socket.listen(1)
            client_socket = self.socket.accept()[0]
            thread = MessageUServer(self.db, client_socket)
            thread.run()


if __name__ == '__main__':
    server = Server()
    server.start_connection()
