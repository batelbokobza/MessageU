import random
import database
import protocol

P = protocol

LENGTH_MESSAGE_ID = 32
# Message structure in tuple:
MESSAGE_ID = 0
TARGET_USER = 1
SENDER_USER = 2
MESSAGE_TYPE = 3
MESSAGE_CONTENT = 4


class Message:

    # Constructor for received message
    def __init__(self, target_id, source_id, message_type, message_content=""):
        self.db = database.DataBase()
        self.id = self.get_unique_id_number()
        self.to_client = target_id
        self.from_client = source_id
        self.type = message_type
        self.content = message_content

    def __str__(self):
        return "message id: " + str(self.id) + ", to client: " + str(self.to_client) + ", from client: " + \
               str(self.from_client) + " type message: " + str(self.type) + " content message: " + str(self.content)

    def get_unique_id_number(self):
        unique_id = random.getrandbits(LENGTH_MESSAGE_ID)
        while self.db.is_message_id_exist(unique_id):
            unique_id = random.getrandbits(LENGTH_MESSAGE_ID)
        return unique_id

    def get_message_size(self, content):
        return P.Constant.CLIENT_ID_SIZE + P.Constant.MESSAGE_ID_SIZE + \
               P.Constant.MESSAGE_TYPE_SIZE + P.Constant.MESSAGE_CONTENT_SIZE + len(content)
