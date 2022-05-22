import sqlite3
from sqlite3 import Error
import user

EMPTY = 0
FILE_NAME_FOR_DATA_BASE = 'server.db'


class DataBase:

    def __init__(self):
        self.connect = sqlite3.connect(FILE_NAME_FOR_DATA_BASE)
        self.connect.text_factory = bytes
        self.cursor = self.connect.cursor()

    def create_clients_table(self):
        try:
            self.cursor.execute("CREATE TABLE IF NOT EXISTS Clients(ClientId BINARY(16) NOT NULL PRIMARY KEY, "
                                "Name varchar(255) NOT NULL, PublicKey BINARY(160) NOT NULL,"
                                "LastSeen DATATIMEOFFSET(1) NOT NULL)")
        except Error:
            print(Error)

    def create_message_table(self):
        try:
            self.cursor.execute("CREATE TABLE IF NOT EXISTS Message(MessageId BINARY(4) NOT NULL PRIMARY KEY, "
                                "ToClient BINARY(16) NOT NULL, FromClient BINARY(16) NOT NULL,"
                                "Type BINARY(1) NOT NULL, Content BLOB)")
        except Error:
            print(Error)

    def add_user(self, user_id, user_name, public_key, last_seen):
        data = (user_id, user_name, public_key, last_seen)
        self.cursor.execute("INSERT INTO Clients VALUES(?, ?, ?, ?)", data)
        self.connect.commit()

    def add_message(self, msg):
        data = (msg.id, msg.to_client, msg.from_client, msg.type, msg.content)
        self.connect.execute("INSERT INTO Message VALUES(?, ?, ?, ?, ?)", data)
        self.connect.commit()

    def get_all_user_messages(self, user_id):
        return self.cursor.execute("SELECT * FROM Message WHERE ToClient = ?", (user_id,)).fetchall()

    def delete_user(self, uid):
        self.cursor.execute("DELETE FROM Clients WHERE ClientId = ?", (uid,))

    def delete_all_user_messages(self, uid):
        self.cursor.execute("DELETE FROM Message WHERE ToClient = ?", (uid,)).fetchall()

    def delete_message_by_message_id(self, unique_id_message):
        self.cursor.execute("DELETE FROM Message WHERE MessageId = ?", (unique_id_message,)).fetchone()

    def get_user_details_by_name(self, user_name):
        user_details = self.cursor.execute("SELECT * FROM Clients WHERE Name = ?", (user_name,)).fetchone()
        return user.User(*user_details) if user_details is not None else None

    def get_user_details_by_id(self, user_id):
        user_details = self.cursor.execute("SELECT * FROM Clients WHERE ClientId = ?", (user_id,)).fetchone()
        return user.User(*user_details) if user_details is not None else None

    def is_user_id_exist(self, user_id):
        return len(self.cursor.execute("SELECT * FROM Clients WHERE ClientId = ?", (user_id,)).fetchall()) != EMPTY

    def get_users_list(self):
        return self.cursor.execute("SELECT * FROM Clients").fetchall()

    def is_message_id_exist(self, msg_id):
        is_exist = self.cursor.execute("SELECT * FROM Message WHERE MessageId = ?", (msg_id,)).fetchone()
        return is_exist is not None  # The function returned true if the length of parameter is_exist != EMPTY
