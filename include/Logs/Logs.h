#pragma once
#include "Console.h"
#include <boost/log/trivial.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <memory>
#include <map>
#include <string>
#include <fmt/core.h>
#include <filesystem>
#include <chrono>
#include <ctime>

class Logs {
public:
    static Logs& Instance();

    template <typename... Args>
    void log(const std::string& level,
             const char* file,
             int line,
             const char* func,
             fmt::format_string<Args...> fmtStr,
             Args&&... args)
    {
        std::string msg = fmt::format(fmt::runtime(fmtStr), std::forward<Args>(args)...);
        std::string name = level;
        auto itName = LOG_LEVEL_NAME.find(level);

        if(itName != LOG_LEVEL_NAME.end()) {
            name = itName->second;
        }
        else {
            name.clear();
            for (char c : level) name += std::toupper(c);
        }

        std::string color = "\033[0m";
        auto itColor = LOG_LEVEL_COLOR.find(name);
        if (itColor != LOG_LEVEL_COLOR.end()) {
            color = itColor->second;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);
        char time_str[20];
        std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);

        std::string formatted = fmt::format(
            "{}{}{} [{}:{}] {}[{}]{} {}[{}]{}: {}{}{}",
            this->timestamp_color_,                           // timestamp color
            time_str,                                         // timestamp
            "\033[0m",                                        // reset
            std::filesystem::path(file).filename().string(),  // file
            line,                                             // line
            this->func_color_,                                // func color
            func,                                             // function
            "\033[0m",                                        // reset
            color,                                            // level color
            name,                                             // level name
            "\033[0m",                                        // reset
            color,                                            // message color (same as level)
            msg,                                              // message
            "\033[0m"                                         // reset
        );

        auto sev = boost::log::trivial::info;
        auto it = LOG_LEVEL.find(level);
        if (it != LOG_LEVEL.end()) {
            sev = it->second;
        }
        BOOST_LOG_SEV(boost::log::trivial::logger::get(), sev) << formatted;
    }

    ~Logs();

private:
    std::map<std::string, boost::log::trivial::severity_level> LOG_LEVEL;
    std::map<std::string, std::string> LOG_LEVEL_NAME;
    std::map<std::string, std::string> LOG_LEVEL_COLOR;
    std::string timestamp_color_;
    std::string func_color_;
    std::string message_color_;

    Logs();
    Logs(const Logs&) = delete;
    Logs& operator=(const Logs&) = delete;
};

// 🔹 Macros
#define LOG_INFO(fmt, ...)     Logs::Instance().log("info", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)     Logs::Instance().log("warn", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)    Logs::Instance().log("error", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)    Logs::Instance().log("debug", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) Logs::Instance().log("critical", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ALERT(fmt, ...)    Logs::Instance().log("alert", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_DB(fmt, ...)       Logs::Instance().log("database", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_HTTP(fmt, ...)     Logs::Instance().log("http", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WS(fmt, ...)       Logs::Instance().log("websocket", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_VERBOSE(fmt, ...) Logs::Instance().log("verbose", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_THREAD(fmt, ...)  Logs::Instance().log("thread", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
