#include "KeyManager.h"
#include "RSA.h"
#include <modes.h>
#include <immintrin.h>
#include <osrng.h>
#include <base64.h>

using namespace Protocol;

Request::RSA_KeyPair KeyManager::get_public_and_private_key() {
    return get_key_pair_in_base64();
}

///\param optional size key. default size = 1024
Request::RSA_KeyPair KeyManager::get_key_pair_in_base64(unsigned int key_size) {
    Request::RSA_KeyPair key_pair;
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::RSA::PrivateKey private_key;
    private_key.GenerateRandomWithKeySize(rng, key_size);
    CryptoPP::Base64Encoder private_key_sink(new CryptoPP::StringSink(key_pair.private_key));
    private_key.DEREncode(private_key_sink);
    private_key_sink.MessageEnd();
    CryptoPP::RSA::PublicKey public_key(private_key);
    CryptoPP::Base64Encoder public_key_sink(new CryptoPP::StringSink(key_pair.public_key));
    public_key.DEREncode(public_key_sink);
    public_key_sink.MessageEnd();
    key_pair.public_key = create_public_key_160_bytes(public_key); //from RSA class
    return key_pair;
}

std::string KeyManager::get_new_symmetric_key() {
    std::string key = generate_symmetric_key();
    return key;
}

///\param optional size key. default size = CryptoPP::AES::DEFAULT_KEYLENGTH
std::string KeyManager::generate_symmetric_key(size_t size) {
    uint8_t res[CryptoPP::AES::DEFAULT_KEYLENGTH] = { 0 };
    for (size_t i = 0; i < size; i += 4) {
        _rdrand32_step(reinterpret_cast<unsigned int*>(&res[i]));
    }
    std::string key = std::string((char*)res, CryptoPP::AES::DEFAULT_KEYLENGTH);
    return key;
}