#include "Logs.h"
#include "ColorStrippingSink.h"
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/pattern_formatter.h>
#include <filesystem>
#include <csignal>
#include <exception>

Logs& Logs::Instance() {
    static Logs instance;
    return instance;
}

Logs::Logs() {
    namespace fs = std::filesystem;
    std::string logDir = "Logs";
    if (!fs::exists(logDir)) {
        fs::create_directories(logDir);
    }

    LOG_LEVEL = {
        {"error", spdlog::level::err},
        {"warn", spdlog::level::warn},
        {"info", spdlog::level::info},
        {"http", spdlog::level::info},
        {"verbose", spdlog::level::debug},
        {"debug", spdlog::level::debug},
        {"critical", spdlog::level::critical},
        {"alert", spdlog::level::critical},
        {"database", spdlog::level::info},
        {"websocket", spdlog::level::info},
        {"thread", spdlog::level::info}
    };

    LOG_LEVEL_NAME = {
        {"error",     "ERROR"},
        {"warn",      "WARNING"},
        {"info",      "INFO"},
        {"http",      "HTTP"},
        {"verbose",   "VERBOSE"},
        {"debug",     "DEBUG"},
        {"critical",  "CRITICAL"},
        {"alert",     "ALERT"},
        {"database",  "DATABASE"},
        {"websocket", "WEBSOCKET"},
        {"thread",    "THREAD"}
    };

    LOG_LEVEL_COLOR = {
        {"ERROR",     "\033[1;31m"},   // bright red
        {"WARNING",   "\033[1;35m"},   // magenta
        {"INFO",      "\033[1;32m"},   // green
        {"HTTP",      "\033[1;34m"},   // blue
        {"VERBOSE",   "\033[1;90m"},   // gray
        {"DEBUG",     "\033[1;37m"},   // white
        {"CRITICAL",  "\033[1;33m"},   // yellow
        {"ALERT",     "\033[1;33m"},   // yellow
        {"DATABASE",  "\033[1;36m"},   // cyan
        {"WEBSOCKET", "\033[1;95m"},    // magenta bright
        {"THREAD",    "\033[1;96m"}    // cyan bright
    };

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%Y-%m-%d %H:%M:%S %v");

    auto rawDailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        logDir + "/app.log", 0, 0, true, 14
    );
    auto dailySink = std::make_shared<color_stripping_sink<spdlog::sinks::daily_file_sink_mt>>(rawDailySink);
    dailySink->set_pattern("%Y-%m-%d %H:%M:%S %v");

    auto distSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    distSink->add_sink(consoleSink);
    distSink->add_sink(dailySink);

    logger = std::make_shared<spdlog::logger>("multi_sink", distSink);
    logger->flush_on(spdlog::level::info);
    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::register_logger(logger);
    logger->set_level(spdlog::level::debug);

    std::set_terminate([]() {
        try {
            std::rethrow_exception(std::current_exception());
        }
        catch(const std::exception& exception) {
            LOG_CRITICAL("Uncaught Exception: {}", exception.what());
        }
        catch(...) {
            LOG_CRITICAL("Uncaught unknown exception");
        }
        std::_Exit(1);
    });

    auto handler = std::signal(SIGSEGV, [](int sig) {
        LOG_CRITICAL("Segmentation fault (signal {})", sig);
        std::_Exit(sig);
    });

    std::signal(SIGSEGV, handler);
    std::signal(SIGINT, handler);
    std::signal(SIGTERM, handler);
}

Logs::~Logs() {
    if (logger) {
        logger->flush();
    }
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> Logs::getLogger() {
    return logger;
}
