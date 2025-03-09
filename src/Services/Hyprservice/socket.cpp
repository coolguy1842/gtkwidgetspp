#include <fmt/format.h>
#include <glibmm/miscutils.h>

#include <Services/HyprService.hpp>

void Services::Hypr::Service::loadSocketPaths() {
    std::string runtimeDir = Glib::getenv("XDG_RUNTIME_DIR");
    std::string signature  = Glib::getenv("HYPRLAND_INSTANCE_SIGNATURE");

    if(runtimeDir.empty() || signature.empty()) {
        throw new std::runtime_error("Error getting path for hyprland socket.");
    }

    _dispatchSocketPath = fmt::format("{}/hypr/{}/.socket.sock", runtimeDir, signature);
    _eventSocketPath    = fmt::format("{}/hypr/{}/.socket2.sock", runtimeDir, signature);
}

Glib::RefPtr<Gio::SocketConnection> Services::Hypr::Service::connect(SocketType type) {
    switch(type) {
    case DISPATCH: return _socketClient->connect(_dispatchSocketAddress);
    default:       return _socketClient->connect(_eventSocketAddress);
    }
}

std::pair<Glib::RefPtr<Gio::SocketConnection>, Glib::RefPtr<Gio::DataInputStream>> Services::Hypr::Service::stream(SocketType type, std::string msg) {
    Glib::RefPtr<Gio::SocketConnection> socket = connect(type);
    if(!msg.empty()) {
        socket->get_output_stream()->write(msg);
    }

    return std::make_pair(socket, Gio::DataInputStream::create(socket->get_input_stream()));
}

std::string Services::Hypr::Service::message(std::string msg) {
    auto pair = stream(DISPATCH);
    pair.first->get_output_stream()->write(msg);

    std::string out;
    pair.second->read_upto(out, "\x04");
    return out;
}

void Services::Hypr::Service::watchStream(const Glib::RefPtr<Gio::DataInputStream>& stream) {
    stream->read_line_async(
        [stream, this](const Glib::RefPtr<Gio::AsyncResult>& res) {
            std::string event;
            stream->read_line_finish_utf8(res, event);

            this->onEvent(event);
            this->watchStream(stream);
        },
        nullptr
    );
}
