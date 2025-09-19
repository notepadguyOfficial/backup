#pragma once
#include <spdlog/sinks/sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/pattern_formatter.h>
#include <fmt/core.h>
#include <regex>
#include <memory>

inline std::string stripAnsi(const std::string& input) {
    static const std::regex ansiEscape("\033\\[[0-9;]*m");
    return std::regex_replace(input, ansiEscape, "");
}

template <typename Sink>
class color_stripping_sink : public spdlog::sinks::sink {
public:
    explicit color_stripping_sink(std::shared_ptr<Sink> wrapped_sink)
        : wrapped_sink_(std::move(wrapped_sink)) {
        base_formatter_ = std::make_unique<spdlog::pattern_formatter>();
    }

    void log(const spdlog::details::log_msg &msg) override {
        spdlog::memory_buf_t formatted;
        base_formatter_->format(msg, formatted);

        std::string clean_msg = stripAnsi(fmt::to_string(formatted));

        if (!clean_msg.empty() && clean_msg.back() == '\n') {
            clean_msg.pop_back();
        }

        spdlog::details::log_msg clean_msg_obj = msg;
        clean_msg_obj.payload = spdlog::string_view_t(clean_msg);

        wrapped_sink_->log(clean_msg_obj);
    }

    void flush() override {
        wrapped_sink_->flush();
    }

    void set_pattern(const std::string &pattern) override {
        auto new_formatter = std::make_unique<spdlog::pattern_formatter>(pattern);
        set_formatter(std::move(new_formatter));
    }

    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {
        base_formatter_ = std::move(sink_formatter);
        wrapped_sink_->set_formatter(base_formatter_->clone());
    }

private:
    std::shared_ptr<Sink> wrapped_sink_;
    std::unique_ptr<spdlog::formatter> base_formatter_;
};
