import struct
import protocol
import message

P = protocol
Msg = message


class Response:

    def __init__(self, socket_connect, request, data_base):
        self.version = P.Constant.SERVER_VERSION_NUMBER
        self.connect = socket_connect
        self.req = request
        self.db = data_base

    def send_registration_successful(self):  # 1000
        user_name_filed = self.req.get_specific_payload_field("NAME")
        user = self.db.get_user_details_by_name(user_name_filed)
        self.send_header(P.Response.Type.REGISTRATION_SUCCESSFULLY, P.Constant.CLIENT_ID_SIZE)
        self.send_payload(P.Response.registration_successfully_format, (user.id,))

    def send_users_list(self):  # 1001
        users_list = self.get_users_list()
        info_user_size = (P.Constant.CLIENT_ID_SIZE + P.Constant.MAX_SIZE_NAME)
        self.send_header(P.Response.Type.USERS_LIST, (info_user_size * len(users_list)))
        for user in users_list:
            self.send_payload(P.Response.user_info_in_list_format, (user[0], user[1]))  # send user id and user name

    def get_users_list(self):
        requesting_user_id = self.req.get_specific_header_field("CLIENT_ID")
        users_list_from_db = self.db.get_users_list()
        users_list = []
        for user in users_list_from_db:
            if user[0] != requesting_user_id:
                users_list.append((user[0], user[1]))  # add user id and user name to list
        return users_list

    def send_public_key(self):  # 1002
        details = self.db.get_user_details_by_id(self.req.payload)  # The request payload contains the user id
        self.send_header(P.Response.Type.PUBLIC_KEY, P.Constant.CLIENT_ID_SIZE + P.Constant.PUBLIC_KEY_SIZE)
        self.send_payload(P.Response.public_key_format, (details.id, details.public_key))

    def send_confirmation_of_message_sent(self, target_user, msg_id):  # 1003
        self.send_header(P.Response.Type.CONFIRMATION_SEND_MESSAGE, P.Constant.CONFIRMATION_SEND_MESSAGE_SIZE)
        self.send_payload(P.Response.confirmation_send_message_format, (target_user, msg_id))

    # The information sent in message - the sender of the message, ID, type, content size and message content.
    def send_waiting_messages(self):  # 1004
        list_pending_message = self.get_waiting_message_list()
        payload_size = 0
        for msg in list_pending_message:
            payload_size += msg[1].get_message_size(msg[1].content)
        self.send_header(P.Response.Type.WAITING_MESSAGES, payload_size)
        for msg in list_pending_message:
            self.send_payload(P.Response.info_waiting_message_format,
                              (msg[1].from_client, msg[0], msg[1].type, len(msg[1].content)))
            self.send_payload(struct.Struct(str(len(msg[1].content)) + 's'), (msg[1].content,))
            # Delete a message that has already been sent.
            self.db.delete_message_by_message_id(msg[0])

    def get_waiting_message_list(self):
        user_id = self.req.get_specific_header_field("CLIENT_ID")  # user_id for receiving a messages sent to him
        msgs_from_db = self.db.get_all_user_messages(user_id)
        msgs_list = []
        for msg in msgs_from_db:
            # The target message should not be sent.
            # Setting the parameters of an object in the list - the sender of the message, type, and content message.
            msgs_list.append([msg[Msg.MESSAGE_ID], Msg.Message(None, msg[Msg.SENDER_USER], msg[Msg.MESSAGE_TYPE],
                                                               msg[Msg.MESSAGE_CONTENT])])
        return msgs_list

    def send_general_error(self):  # 9000
        self.send_header(P.Response.Type.GENERAL_ERROR, P.Constant.EMPTY_PAYLOAD_SIZE)

    def send_header(self, type_response, payload_size):
        response_header_data = (self.version, type_response, payload_size)
        self.connect.send(P.Response.header_format.pack(*response_header_data))

    def send_payload(self, struct_format, payload):
        self.connect.send(struct_format.pack(*payload))
