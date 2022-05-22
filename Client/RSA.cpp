#include "RSA.h"
#include <rsa.h>
#include <base64.h>
#include <osrng.h>
#include <utility>

///\brief Encrypts a text string by a public key.
///\param first - Encrypted content text
///\param second - Public key
///\return Decoded content text string
std::string RSA::encrypt(const std::string& content, const std::string& public_key){
	CryptoPP::AutoSeededRandomPool rng;
	std::string cipher_content;
	CryptoPP::RSA::PublicKey rsa_public_key = string_to_RSA_public_key(public_key);
	CryptoPP::RSAES_OAEP_SHA_Encryptor e(rsa_public_key);
	CryptoPP::StringSource ss(content, true, new CryptoPP::PK_EncryptorFilter(rng, e, new CryptoPP::StringSink(cipher_content)));
	return cipher_content;
}

///\brief Decrypts a text string by a public key.
///\param first - Decrypted content text
///\param second - Private key
///\return Decoded content text string
std::string RSA::decrypt(const std::string& content, const std::string& private_key){
	CryptoPP::AutoSeededRandomPool rng;
	std::string decrypted_content;
	CryptoPP::ByteQueue bytes;
	CryptoPP::StringSource(private_key, true, new CryptoPP::Base64Decoder).TransferTo(bytes);
	bytes.MessageEnd();
	CryptoPP::RSA::PrivateKey rsa_private_key;
	rsa_private_key.Load(bytes);
	CryptoPP::RSAES_OAEP_SHA_Decryptor d(rsa_private_key);
	CryptoPP::StringSource ss(content, true, new CryptoPP::PK_DecryptorFilter(rng, d, new CryptoPP::StringSink(decrypted_content)));
	return decrypted_content;
}

CryptoPP::RSA::PublicKey RSA::string_to_RSA_public_key(const std::string& public_key) {
    CryptoPP::StringSource as(public_key, true);
    CryptoPP::RSA::PublicKey pubKey;
    pubKey.Load(as);
    return pubKey;
}

///\brief Converts a public key to to a 160-byte string.
///\param const  CryptoPP::RSA::PublicKey&
///\return public key in type string
std::string RSA::create_public_key_160_bytes(const CryptoPP::RSA::PublicKey& public_key) {
    CryptoPP::ByteQueue bytes;
    std::string pk;
    CryptoPP::StringSink as(pk);
    public_key.Save(as);
    pk.resize(NUMBER_BYTES_TO_PUBLIC_KEY);
    return pk;
}

CryptoPP::RSA::PublicKey RSA::get_public_key_by_private_key(const std::string& private_key) {
	CryptoPP::ByteQueue bytes;
	CryptoPP::StringSource sr(private_key, true, new CryptoPP::Base64Decoder);
	sr.TransferTo(bytes);
	bytes.MessageEnd();
	CryptoPP::RSA::PrivateKey priv_key;
	priv_key.Load(bytes);
	CryptoPP::RSA::PublicKey pub_key(priv_key);
	return pub_key;
}
