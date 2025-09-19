#include "WebSocketHandler.h"
#include "Logs.h"
#include <iostream>
#include <functional>

WebSocketHandler::WebSocketHandler(Dat& settings)
    : settings_(settings), acceptor_(ioc_), running_(false) {
}

WebSocketHandler::~WebSocketHandler() {
    stop();
}

void WebSocketHandler::run() {
    std::string port_str = settings_.get("websocket_port");
    int port = std::stoi(port_str.empty() ? "8080" : port_str);

    LOG_WS("Starting WebSocket server on port {}", port);

    try {
        tcp::endpoint endpoint(tcp::v4(), static_cast<unsigned short>(port));
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(net::socket_base::max_listen_connections);

        running_ = true;
        do_accept();
        ioc_.run();
    } catch (const std::exception& e) {
        LOG_ERROR("WebSocket server error: {}", e.what());
    }
}

void WebSocketHandler::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::thread(&WebSocketHandler::do_session, this, std::move(socket)).detach();
            }
            if (running_) {
                do_accept();
            }
        });
}

void WebSocketHandler::stop() {
    if (running_) {
        LOG_WS("Stopping WebSocket server");
        running_ = false;
        ioc_.stop();
        beast::error_code ec;
        acceptor_.close(ec);
    }
}

void WebSocketHandler::do_session(tcp::socket socket) {
    try {
        websocket::stream<tcp::socket> ws(std::move(socket));

        // Accept the WebSocket handshake
        ws.accept();

        LOG_WS("WebSocket connection opened");

        for (;;) {
            beast::multi_buffer buffer;
            beast::error_code ec;

            // Read a message
            ws.read(buffer, ec);
            if (ec == websocket::error::closed) {
                LOG_WS("WebSocket connection closed");
                break;
            }
            if (ec) {
                LOG_ERROR("WebSocket read error: {}", ec.message());
                break;
            }

            std::string message = boost::beast::buffers_to_string(buffer.data());
            LOG_WS("Received message: {}", message);

            // Echo the message back
            ws.write(buffer.data(), ec);
            if (ec) {
                LOG_ERROR("WebSocket write error: {}", ec.message());
                break;
            }
            LOG_WS("Echoed message back");
        }
    } catch (const beast::system_error& se) {
        if (se.code() != websocket::error::closed) {
            LOG_ERROR("WebSocket session error: {}", se.code().message());
        }
    } catch (const std::exception& e) {
        LOG_ERROR("WebSocket session exception: {}", e.what());
    }
}