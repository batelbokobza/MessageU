#ifndef CLIENT_KEYMANAGER_H
#define CLIENT_KEYMANAGER_H

#include <iostream>
#include <utility>
#include <string>
#include <rsa.h>
#include <aes.h>
#include <osrng.h>

#include "Protocol.h"
#include "Constants.h"
#include "AES.h"
#include "RSA.h"


///\brief The key manager. Public key, private key and symmetric key can be obtained from this class.
class KeyManager: public RSA, public AES {
public:
    static Protocol::Request::RSA_KeyPair get_key_pair_in_base64(unsigned int key_size = RSA_KEY_SIZE);
    static Protocol::Request::RSA_KeyPair get_public_and_private_key();
    static std::string get_new_symmetric_key();
    static std::string generate_symmetric_key(size_t size = CryptoPP::AES::DEFAULT_KEYLENGTH);
};

#endif //CLIENT_KEYMANAGER_H
