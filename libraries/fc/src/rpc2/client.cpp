#include <fc/rpc2/client.hpp>
#include <fc/rpc/websocket_api.hpp>

namespace fc
{
namespace rpc2
{

client::~client()
{
    closed();
}

client::client(fc::http::websocket_connection& c)
    : _connection(c)
{
    _connection.on_message_handler(
        [&](const std::string& msg) { on_message(msg, true); });
    _connection.on_http_handler(
        [&](const std::string& msg) { return on_message(msg, false); });
    _connection.closed.connect([this]() { closed(); });
}

void client::closed()
{
    // TODO:
}

variant client::send_call(string method_name, variants args)
{
    auto request = _rpc_state.start_remote_call(method_name, {std::move(args)});
    _connection.send_message(fc::json::to_string(request));
    return _rpc_state.wait_for_response(*request.id);
}

std::string client::on_message(const std::string& message, bool send_message)
{
    try
    {
        if (send_message)
        {
            auto var = fc::json::from_string(message);
            auto reply = var.as<fc::rpc2::response>();
            _rpc_state.handle_reply(reply);
        }
    }
    catch (const fc::exception& e)
    {
        return e.to_detail_string();
    }
    return string();
}

} // namespace rpc
} // namespace fc
