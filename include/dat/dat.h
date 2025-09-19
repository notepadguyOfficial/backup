#ifndef DAT_H
#define DAT_H

#include <string>
#include <map>

class Dat {
private:
    std::map<std::string, std::string> settings;
    char xor_key;
    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);
public:
    Dat(const std::string& filename);
    virtual ~Dat();
    bool LOAD_FROM_FILE(const std::string& filename);
    bool SAVE_TO_FILE(const std::string& filename);
    bool CREATE_DEFAULT_FILE(const std::string& filename);
    std::string get(const std::string& key);
    void set(const std::string& key, const std::string& value);
};

#endif