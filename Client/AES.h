#ifndef CLIENT_AES_H
#define CLIENT_AES_H


#include <iostream>
#include <string>
#include <aes.h>

///\Symmetric Encryption and Decryption class.
///\note To use the functions of the class, it is required to provide the content, and a symmetric key.
class AES{

private:
    typedef struct AES_KeyPair {
        uint8_t KEY[CryptoPP::AES::DEFAULT_KEYLENGTH]{};
        uint8_t IV[CryptoPP::AES::BLOCKSIZE]{};
        AES_KeyPair(std::string key) {
            memset(KEY, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
            memset(IV, 0x00, CryptoPP::AES::BLOCKSIZE);
            memcpy(KEY, key.data(), key.size());
        }
    }AES_KeyPair;

public:
    static std::string encrypt(const std::string& deciphered_content, const std::string& key);
    static std::string decrypt( std::string decrypted_content,  std::string key);
};
#endif //CLIENT_AES_H
