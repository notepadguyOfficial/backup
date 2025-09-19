#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <string>
#include "dat.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSocketHandler {
public:
    WebSocketHandler(Dat& settings);
    ~WebSocketHandler();

    void run();
    void stop();

private:
    Dat& settings_;
    net::io_context ioc_;
    tcp::acceptor acceptor_;
    std::thread ws_thread_;
    bool running_;

    void do_accept();
    void do_session(tcp::socket socket);
};

#endif