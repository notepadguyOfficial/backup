#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <map>
#include <string>
#include <fmt/core.h>
#include <filesystem>

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

        std::string formatted = fmt::format(
            "[{}:{}] [{}] {}[{}]{}\033[0m: {}",
            std::filesystem::path(file).filename().string(),  // file
            line,                                             // line
            func,                                             // function
            color,                                            // color start
            name,                                             // level name
            "\033[0m",                                        // color end
            msg                                               // message
        );

        auto it = LOG_LEVEL.find(level);
        if (it != LOG_LEVEL.end()) {
            logger->log(it->second, formatted);
        } else {
            logger->info(formatted);
        }
    }

    std::shared_ptr<spdlog::logger> getLogger();

    ~Logs();

private:
    std::shared_ptr<spdlog::logger> logger;
    std::map<std::string, spdlog::level::level_enum> LOG_LEVEL;
    std::map<std::string, std::string> LOG_LEVEL_NAME;
    std::map<std::string, std::string> LOG_LEVEL_COLOR;

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
