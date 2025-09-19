#include "Logs.h"
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/expressions.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <filesystem>
#include <csignal>
#include <exception>
#include <regex>
#include <iostream>

std::string strip_ansi(const std::string& str) {
    std::regex ansi_regex("\033\\[[0-9;]*m");
    return std::regex_replace(str, ansi_regex, "");
}

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
        {"error", boost::log::trivial::error},
        {"warn", boost::log::trivial::warning},
        {"info", boost::log::trivial::info},
        {"http", boost::log::trivial::info},
        {"verbose", boost::log::trivial::trace},
        {"debug", boost::log::trivial::debug},
        {"critical", boost::log::trivial::fatal},
        {"alert", boost::log::trivial::fatal},
        {"database", boost::log::trivial::info},
        {"websocket", boost::log::trivial::info},
        {"thread", boost::log::trivial::info}
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
        {"ERROR",     "\033[38;5;196m"},   // bright red
        {"WARNING",   "\033[38;5;214m"},   // orange
        {"INFO",      "\033[38;5;40m"},    // bright green
        {"HTTP",      "\033[38;5;27m"},    // blue
        {"VERBOSE",   "\033[38;5;244m"},   // gray
        {"DEBUG",     "\033[38;5;15m"},    // white
        {"CRITICAL",  "\033[38;5;226m"},   // yellow
        {"ALERT",     "\033[38;5;226m"},   // yellow
        {"DATABASE",  "\033[38;5;51m"},    // cyan
        {"WEBSOCKET", "\033[38;5;201m"},   // magenta
        {"THREAD",    "\033[38;5;45m"}     // bright cyan
    };

    timestamp_color_ = "\033[38;5;250m"; // light gray
    func_color_ = "\033[38;5;39m";       // bright blue
    message_color_ = "\033[38;5;15m";    // white

    boost::log::add_common_attributes();
    boost::log::core::get()->remove_all_sinks();

    // Console sink
    auto console_backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
    console_backend->add_stream(boost::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){}));
    console_backend->auto_flush(true);
    auto console_sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>(console_backend);
    console_sink->set_formatter([](boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
        strm << "\033[2K\r" << rec[boost::log::expressions::smessage] << "\n";
    });
    boost::log::core::get()->add_sink(console_sink);

    // File sink
    auto file_backend = boost::make_shared<boost::log::sinks::text_file_backend>(
        boost::log::keywords::file_name = logDir + "/app.log",
        boost::log::keywords::rotation_size = 0,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_interval(boost::posix_time::hours(24)),
        boost::log::keywords::max_files = 14
    );
    auto file_sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>>(file_backend);
    file_sink->set_formatter([](boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
        strm << strip_ansi(rec[boost::log::expressions::smessage].get());
    });
    boost::log::core::get()->add_sink(file_sink);

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
    boost::log::core::get()->flush();
}
