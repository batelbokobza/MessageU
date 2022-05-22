import uuid
from datetime import datetime


class User:

    def __init__(self, user_id=uuid.uuid4().bytes, user_name="", public_key="", last_seen=datetime.now()):
        self.id = user_id
        self.name = user_name
        self.public_key = public_key
        self.last_seen = last_seen

    def __str__(self):
        return "id: " + str(self.id) + ", name: " + str(self.name) + ", publicKey: " + str(self.public_key) + \
               " last_seen: " + str(self.last_seen)
