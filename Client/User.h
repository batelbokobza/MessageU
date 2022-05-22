#ifndef CLIENT_USER_H
#define CLIENT_USER_H

#include <iostream>
#include <list>
#include <fstream>
#include <boost/algorithm/hex.hpp>

///\brief Produces an object that contains the user attributes.
///These values can be get and updated using getter and setter functions.
class User{
private:
    std::string _name;
    std::string _id;
    std::string _public_key;
    std::string _symmetric_key;

public:
    explicit User() = default;
    explicit User(std::string name, std::string id);
    explicit User(std::string name, std::string id, std::string key);

    ~User() = default;

    std::string get_name() const;
    std::string get_id() const;
    std::string get_public_key() const;
    std::string get_symmetric_key() const;
    void set_id(const std::string& id);
    void set_name(const std::string& name);
    void set_public_key(const std::string& key);
    void set_symmetric_key(const std::string& key);
};
#endif //CLIENT_USER_H
