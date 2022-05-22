#include "AES.h"
#include <modes.h>
#include <aes.h>
#include <filters.h>
#include <string>
#include <utility>


std::string AES::encrypt(const std::string& deciphered_content, const std::string& symmetric_key){
    AES_KeyPair pair(symmetric_key);
    std::string ciphered_content;
    CryptoPP::AES::Encryption aes_encryption(pair.KEY, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, pair.IV);
    CryptoPP::StreamTransformationFilter stf_encryptor(cbc_encryption, new CryptoPP::StringSink(ciphered_content));
    stf_encryptor.Put(reinterpret_cast<const unsigned char*>(deciphered_content.data()), deciphered_content.size());
    stf_encryptor.MessageEnd();
    return ciphered_content;
}

std::string AES::decrypt( std::string cipher_content,  std::string symmetric_key){
    AES_KeyPair pair(std::move(symmetric_key));
    std::string decrypted_content;
    CryptoPP::AES::Decryption aes_decryption(pair.KEY, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, pair.IV);
    CryptoPP::StreamTransformationFilter stf_decryptor(cbc_decryption, new CryptoPP::StringSink(decrypted_content));
    stf_decryptor.Put(reinterpret_cast<const unsigned char*>(cipher_content.data()), cipher_content.size());
    stf_decryptor.MessageEnd();
    return decrypted_content;
}
