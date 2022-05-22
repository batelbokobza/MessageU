#ifndef CLIENTENCRYPT_RSA_H
#define CLIENTENCRYPT_RSA_H

#include <iostream>
#include <rsa.h>

#include "Constants.h"

///\Asymmetric Encryption and Decryption class.
class RSA{

public:

    static std::string encrypt(const std::string& content, const std::string& key);
    static std::string decrypt(const std::string& content, const std::string& key);

    static std::string create_public_key_160_bytes(const CryptoPP::RSA::PublicKey& public_key);
    static CryptoPP::RSA::PublicKey get_public_key_by_private_key(const std::string& private_key);
    static CryptoPP::RSA::PublicKey string_to_RSA_public_key(const std::string& publicKey);

};
#endif //CLIENTENCRYPT_RSA_H