#include "dat.h"
#include "Logs.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

const char MASTER_KEY = 0xAA;

std::string EncryptWithKey(const std::string& data, char key) {
    std::string result = data;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] ^= key;
    }
    return result;
}

Dat::Dat(const std::string& filename) {
    filename_ = filename;
    xor_key = 0xFF;
    if (!LoadFromFile(filename)) {
        CreateDefaultFile(filename);
    }
    // Set auto_save after loading/creating
    auto_save_ = (get("auto_save") != "false");
    LOG_INFO("Dat initialization completed");
}

Dat::~Dat() {
    if (auto_save_) {
        SaveToFile(filename_);
    }
}

std::string Dat::encrypt(const std::string& data) {
    return EncryptWithKey(data, xor_key);
}

std::string Dat::decrypt(const std::string& data) {
    // XOR is symmetric
    return EncryptWithKey(data, xor_key);
}

bool Dat::LoadFromFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        LOG_INFO("Settings file not found: {}", filename);
        return false;
    }
    // Read encrypted key (1 byte)
    char encrypted_key;
    file.read(&encrypted_key, 1);
    if (!file) {
        LOG_ERROR("Failed to read encrypted key from {}", filename);
        return false;
    }
    // Decrypt key with master key
    xor_key = encrypted_key ^ MASTER_KEY;
    // Read the rest as encrypted settings
    std::string encrypted_settings((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string decrypted_settings = decrypt(encrypted_settings);
    std::istringstream iss(decrypted_settings);
    std::string line;
    settings.clear();
    while (std::getline(iss, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            settings[key] = value;
        }
    }
    LOG_INFO("Loaded settings from {}", filename);
    return true;
}

bool Dat::SaveToFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx_);
    // Update auto_save setting
    settings["auto_save"] = auto_save_ ? "true" : "false";
    std::ostringstream oss;
    for (const auto& pair : settings) {
        oss << pair.first << "=" << pair.second << "\n";
    }
    std::string data = oss.str();
    std::string encrypted_settings = encrypt(data);
    // Encrypt the XOR key with master key
    char encrypted_key = xor_key ^ MASTER_KEY;
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        LOG_ERROR("Failed to save file: {}", filename);
        return false;
    }
    // Write encrypted key (1 byte)
    file.write(&encrypted_key, 1);
    // Write encrypted settings
    file.write(encrypted_settings.c_str(), encrypted_settings.size());
    if (!file) {
        LOG_ERROR("Failed to write to file: {}", filename);
        return false;
    }
    LOG_INFO("Saved settings to {}", filename);
    return true;
}

bool Dat::CreateDefaultFile(const std::string& filename) {
    // Set default settings
    settings["version"] = "1.0";
    settings["debug"] = "true";
    settings["auto_save"] = "true";
    settings["websocket_port"] = "8080";
    // Save settings to create a base file
    bool result = SaveToFile(filename);
    if (result) {
        LOG_INFO("Created default file: {}", filename);
    }
    return result;
}

std::string Dat::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = settings.find(key);
    return it != settings.end() ? it->second : "";
}

void Dat::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx_);
    settings[key] = value;
}