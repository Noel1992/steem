#include <fc/rpc2/apiclient.hpp>

namespace fc
{
namespace rpc2
{

apiclient::apiclient(const std::string& server_address)
    : _server_address(server_address)
{
    _websocket_client = new http::websocket_client();
    auto conn = _websocket_client->connect(_server_address);
    _client = new client(*conn);
}

apiclient::~apiclient()
{
    delete _client;
    delete _websocket_client;
}

fc::variant apiclient::call(const std::string& action, const std::vector<string>& args)
{
    variants vars;
    size_t arg_size = args.size();
    for (size_t i = 0; i < arg_size; ++i)
    {
        vars.push_back(args[i]);
    }
    return _client->send_call(action, vars);
}

} // namespace rpc2
} // namespace fc