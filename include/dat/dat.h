#ifndef DAT_H
#define DAT_H

#include <string>
#include <map>
#include <mutex>

class Dat {
private:
    std::map<std::string, std::string> settings;
    char xor_key;
    std::string filename_;
    std::mutex mtx_;
    bool auto_save_;
    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);
public:
    Dat(const std::string& filename);
    virtual ~Dat();
    bool LoadFromFile(const std::string& filename);
    bool SaveToFile(const std::string& filename);
    bool CreateDefaultFile(const std::string& filename);
    std::string get(const std::string& key);
    void set(const std::string& key, const std::string& value);
    std::string getFilename() const { return filename_; }
    void disableAutoSave() { auto_save_ = false; }
};

#endif