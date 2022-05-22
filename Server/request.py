import protocol

header_fields = {"CLIENT_ID": 0, "VERSION": 1, "CODE": 2, "PAYLOAD_SIZE": 3}

payload_fields = {"NAME": 0, "PUBLIC_KEY": 1, "CLIENT_ID": 0, "MESSAGE_TYPE": 1, "CONTENT_SIZE": 2,
                  "MESSAGE_CONTENT": 3}

P = protocol


class Request:

    def __init__(self, socket):
        self.sock = socket
        received_request = self.receive_request()
        self.header = received_request[0]
        self.payload = received_request[1] if len(received_request) > 1 else None

    def receive_request(self):
        header = self.get_header()
        code = header[2]
        if code == P.Request.Type.USERS_LIST or code == P.Request.Type.WAITING_MESSAGES:
            return [header]
        payload = self.get_payload(code)
        return [header, payload]

    def get_header(self):
        return P.Request.header_format.unpack(bytes(self.sock.recv(P.Request.header_format.size)))

    def get_payload(self, code):
        switch = {
            P.Request.Type.REGISTRATION: self.receive_payload_registration,  # 100
            P.Request.Type.PUBLIC_KEY: self.receive_payload_public_key,  # 102
            P.Request.Type.SEND_MESSAGE: self.receive_payload_send_message,  # 103
        }
        payload = switch.get(code, lambda: "Error: Request code does not exist!")
        return payload()

    def get_specific_header_field(self, filed):
        return self.header[header_fields.get(filed)]

    def get_specific_payload_field(self, filed):
        return self.payload[payload_fields.get(filed)]

    def receive_payload_registration(self):
        details = P.Request.register_format.unpack(bytes(self.sock.recv(P.Request.register_format.size)))
        # for null terminators
        return details[0].rstrip(bytearray('\x00'.encode())), details[1].rstrip(bytearray('\x00'.encode()))

    def receive_payload_public_key(self):
        user_id = P.Request.public_key_format.unpack(bytes(self.sock.recv(P.Request.public_key_format.size)))
        return user_id[0]

    def receive_payload_send_message(self):
        payload = P.Request.send_message_format.unpack(bytes(self.sock.recv(P.Request.send_message_format.size)))
        content_size = payload[2]
        content_data = b''
        while content_size > 0:
            data_chunk = self.sock.recv(content_size)
            if not data_chunk:
                break
            content_size -= len(data_chunk)
            content_data += data_chunk
            payload += (content_data,) if content_data else ''
        return payload
