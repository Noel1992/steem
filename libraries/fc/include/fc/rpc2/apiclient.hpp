#ifndef _STEEM_JSON_RPC_2_API_CLIENT_
#define _STEEM_JSON_RPC_2_API_CLIENT_

#include <string>
#include <fc/variant.hpp>
#include <fc/rpc2/client.hpp>

namespace fc
{

namespace http
{
class websocket_client;
}

namespace rpc2
{

class apiclient
{
public:
    apiclient(const std::string& server_address);
    ~apiclient();

public:
    fc::variant call(const std::string& action, const std::vector<string>& args);

private:
    std::string _server_address;
    client* _client;
    http::websocket_client* _websocket_client;
};

} // namespace rpc2
} // namespace fc

#endif // _STEEM_JSON_RPC_2_API_CLIENT_
