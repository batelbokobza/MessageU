#include "User.h"
#include "User.h"
#include "Constants.h"
#include <utility>


///Constructor
///\param first - User id
///\param second - User name
User::User(std::string name, std::string id): _name(std::move(name)), _id(std::move(id)){
}

///Constructor
///\param first - User id
///\param second - User name
///\param third - Public key for user
User::User(std::string name, std::string id, std::string key) : _name(std::move(name)), 
        _id(std::move(id)), _public_key(std::move(key)) {
}

std::string User::get_id() const {
    return _id;
}

std::string User::get_name() const {
    return _name;
}

std::string User::get_public_key() const {
    return _public_key;
}

std::string User::get_symmetric_key() const {
    return _symmetric_key;
}

void User::set_id(const std::string& id) {
     _id = id;
}

void User::set_name(const std::string& name) {
    _name = name;
}

void User::set_public_key(const std::string& public_key) {
     _public_key = public_key;
}
void User::set_symmetric_key(const std::string& symmetric_key) {
    _symmetric_key = symmetric_key;
}

